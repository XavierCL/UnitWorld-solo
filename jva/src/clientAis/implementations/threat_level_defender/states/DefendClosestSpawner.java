package clientAis.implementations.basic_minion_wielder.states;

import clientAis.communications.ServerCommander;
import clientAis.communications.game_data.Singuity;
import clientAis.communications.game_data.Spawner;
import clientAis.dynamic_data.DataPacket;
import utils.data_structure.cluster.DataCluster;
import utils.data_structure.tupple.Tuple2;
import utils.math.vector.Vector2;
import utils.minion.Minion;
import utils.minion.MinionState;
import utils.minion.MinionWielder;

import java.util.Comparator;
import java.util.Optional;
import java.util.Set;
import java.util.concurrent.atomic.AtomicReference;
import java.util.function.Consumer;
import java.util.stream.Collectors;

public class DefendClosestSpawner extends MinionState {

    public DefendClosestSpawner(MinionWielder minionWielder) {
        super(minionWielder);
    }

    @Override
    public void start(final Tuple2<DataPacket, Minion> input) {

    }

    @Override
    public void stop(final Tuple2<DataPacket, Minion> input) {

    }

    @Override
    public Consumer<ServerCommander> exec(final Tuple2<DataPacket, Minion> input) {
        input.value2.singuities.addAll(DataPacket.singuityResourceHandler.alloc(100));

        final Set<Singuity> singuities = input.value2.singuities.stream()
                .map(id -> input.value1.singuityIdMap.get(id)).collect(Collectors.toSet());
        final Set<Spawner> spawners = input.value1.ownedSpawners.stream()
                .map(id -> input.value1.spawnerIdMap.get(id)).collect(Collectors.toSet());
        final Optional<DataCluster<String>> biggestThreat = input.value1.adverseClusters.stream()
                .reduce((c1, c2) -> c1.elements.size() > c2.elements.size() ? c1 : c2);

        Optional<Vector2> threateningPosition = Vector2.centerOfMass(singuities, singuity -> singuity.position);
        if(biggestThreat.isPresent()) {
            threateningPosition = Vector2.centerOfMass(biggestThreat.get().elements.stream().map(input.value1.singuityIdMap::get).collect(Collectors.toSet()), s -> s.position);
        }
        final AtomicReference<Optional<Spawner>> closestSpawnerOpt = new AtomicReference<>(Optional.empty());

        threateningPosition.ifPresent(centerOfMass -> {
            closestSpawnerOpt.set(spawners.stream()
                    .min(Comparator.comparingDouble(spawner -> spawner.position.minus(centerOfMass).magnitudeSquared())));
        });

        return serverCommander -> closestSpawnerOpt.get().ifPresent(closestSpawner ->
                serverCommander.moveUnitsToPosition(input.value2.singuities, closestSpawner.position));
    }

    @Override
    public MinionState next(final Tuple2<DataPacket, Minion> input) {
        if(input.value2.singuities.size() > (100 + (20 * input.value1.ownedSpawners.size()))
                && input.value1.freeSpawners.size() > 0) {
            final MinionState challengeClosestSpawner = new ChallengeClosestSpawner(getMinionWielder());
            final Minion splitted = input.value2.split(challengeClosestSpawner, 100);
            getMinionWielder().addMinion(splitted);
        }
        // attack if enemy made a mistake
        if(input.value2.singuities.size() > input.value1.adverseSinguities.size()*3.5
                && input.value2.singuities.size() > 150) {
            final MinionState challengeClosestSpawner = new AttackEnemySpawner(getMinionWielder());
            final Minion splitted = input.value2.split(challengeClosestSpawner, input.value2.singuities.size()-10);
            getMinionWielder().addMinion(splitted);
        }
        return this;
    }

    @Override
    public void debug(final Tuple2<DataPacket, Minion> input) {

    }
}