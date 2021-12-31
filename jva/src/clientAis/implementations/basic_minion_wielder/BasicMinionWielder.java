package clientAis.implementations.basic_minion_wielder;

import clientAis.communications.ServerCommander;
import clientAis.communications.game_data.Singuity;
import clientAis.communications.game_data.Spawner;
import clientAis.dynamic_data.DataPacket;
import clientAis.implementations.Bot;
import clientAis.implementations.basic_minion_wielder.states.DefendClosestSpawner;
import utils.data_structure.cluster.DataCluster;
import utils.math.vector.Vector2;
import utils.unit_world.minion.Minion;
import utils.unit_world.minion.MinionWielder;

import java.util.*;
import java.util.concurrent.atomic.AtomicReference;
import java.util.function.Consumer;
import java.util.stream.Collectors;

public class BasicMinionWielder implements Bot, MinionWielder {

    private final Set<Minion> minions;
    private final Set<Minion> minionsToAdd;
    private final Set<Minion> minionsToRemove;

    // TODO: Check whether an enemy cluster is moving or not. If it's moving, check where it's most likely headed to (probably a spawner. Which one?)
    //       If it's not moving, and it's on an enemy spawner, it's probably defending. If it's not moving, and on an ally spawner, then it's probably attacking.
    //       If it's not moving, and it's in the middle of nowhere, then it's probably wasting its time? Bait? Forming a strong cluster? Gathering more singuities before attack?

    public BasicMinionWielder() {
        this.minions = new HashSet<>();
        this.minionsToAdd = new HashSet<>();
        this.minionsToRemove = new HashSet<>();
        addMinion(new Minion(new DefendClosestSpawner(this)));
    }

    @Override
    public Consumer<ServerCommander> exec(DataPacket input) {
        final Set<Consumer<ServerCommander>> commandList = minions.stream()
                .map(minion -> minion.exec(input))
                .collect(Collectors.toSet());

        updateMinionList();

        return serverCommander -> commandList.forEach(serverCommanderConsumer ->
                serverCommanderConsumer.accept(serverCommander));
    }

    private void updateMinionList() {
        minions.addAll(minionsToAdd);
        minions.stream()
                .filter(minion -> minions.size() > 1)
                .filter(minion -> minion.singuities.size() == 0)
                .forEach(minionsToRemove::add);
        minions.removeAll(minionsToRemove);
        minionsToRemove.forEach(minion ->
                DataPacket.singuityResourceHandler.free(minion.singuities));

        minionsToAdd.clear();
        minionsToRemove.clear();
    }

    @Override
    public void addMinion(Minion minion) {
        minionsToAdd.add(minion);
    }

    @Override
    public void removeMinion(Minion minion) {
        minionsToRemove.add(minion);
    }

    @Override
    public Set<Minion> getMinions() {
        return minions;
    }


    // TODO: instead of using distances to evaluate threat level, we should predict where every enemy cluster is going,
    //       and towards which spawners they are most likely headed
    private Map<String, Double> evaluateThreatLevels(final Set<Spawner> ownedSpawners, final Set<DataCluster<Singuity>> adverseClusters) {
        final Map<String, Double> threatLevelMap = new HashMap<>();

        ownedSpawners.forEach(spawner -> {
            final AtomicReference<Double> threat = new AtomicReference<>(0d);
            adverseClusters.forEach(adverseCluster -> {
                final Optional<Vector2> centerOfMassOpt = Vector2.centerOfMass(adverseCluster.elements, singuity -> singuity.position);
                centerOfMassOpt.ifPresent(centerOfMass -> {
                    final Double clusterThreat = adverseCluster.elements.size() / spawner.position.minus(centerOfMass).magnitudeSquared();
                    threat.set(threat.get() + clusterThreat);
                });
            });
            threatLevelMap.put(spawner.id, threat.get());
        });

        return threatLevelMap;
    }
}
