package clientAis.implementations.threat_level_defender.states;

import clientAis.communications.ServerCommander;
import clientAis.communications.game_data.Singuity;
import clientAis.communications.game_data.Spawner;
import clientAis.dynamic_data.DataPacket;
import utils.data_structure.cluster.DataCluster;
import utils.data_structure.tupple.Tuple2;
import utils.unit_world.game_evaluation.SpawnerThreatEvaluator;
import utils.unit_world.minion.Minion;
import utils.unit_world.minion.MinionState;
import utils.unit_world.minion.MinionWielder;

import java.util.*;
import java.util.function.Consumer;
import java.util.stream.Collectors;
import java.util.stream.IntStream;

public class DefendClosestSpawner extends MinionState {

    private Optional<Spawner> spawnerToDefend;

    public DefendClosestSpawner(MinionWielder minionWielder) {
        super(minionWielder);
    }

    @Override
    public void start(final Tuple2<DataPacket, Minion> input) {
        input.value2.singuities.addAll(DataPacket.singuityResourceHandler.alloc(150));
        spawnerToDefend = DataPacket.ownedSpawnerResourceHandler.alloc(1).stream().map(input.value1.spawnerIdMap::get).findFirst();
    }

    @Override
    public void stop(final Tuple2<DataPacket, Minion> input) {

    }

    @Override
    public Consumer<ServerCommander> exec(final Tuple2<DataPacket, Minion> input) {
        input.value2.singuities.addAll(DataPacket.singuityResourceHandler.alloc(10));

        final Set<Spawner> ownedSpawners = input.value1.ownedSpawners.stream()
                .map(input.value1.spawnerIdMap::get)
                .collect(Collectors.toSet());
        final Set<DataCluster<Singuity>> adverseCluster = input.value1.adverseClusters.stream()
                .map(cluster -> new DataCluster<>(cluster.elements.stream()
                        .map(input.value1.singuityIdMap::get)
                        .collect(Collectors.toSet())))
                .collect(Collectors.toSet());
        final Map<String, Double> spawnerRatios = SpawnerThreatEvaluator.evaluateThreatRatios(ownedSpawners, adverseCluster);

        final Map<String, Integer> singuityAmountOnEverySpawner = new HashMap<>();
        spawnerRatios.forEach((spawnerId, ratio) -> {
            singuityAmountOnEverySpawner.put(spawnerId, (int) (ratio * input.value2.singuities.size()));
        });

        final Set<Consumer<ServerCommander>> serverCommands = new HashSet<>();

        final Set<Singuity> singuitiesCopy = input.value2.singuities.stream().map(input.value1.singuityIdMap::get).collect(Collectors.toSet());
        singuityAmountOnEverySpawner.forEach((spawnerId, singuityAmount) -> {
            final Set<Singuity> selectedSinguities = new HashSet<>();
            IntStream.range(0, singuityAmountOnEverySpawner.get(spawnerId)).forEach(i -> {
                final Singuity SelectedSinguity = singuitiesCopy.stream().findFirst().get();
                selectedSinguities.add(SelectedSinguity);
                singuitiesCopy.remove(SelectedSinguity);
            });
            serverCommands.add(serverCommander -> {
                serverCommander.moveUnitsToPosition(selectedSinguities.stream().map(singuity -> singuity.id).collect(Collectors.toSet()), input.value1.spawnerIdMap.get(spawnerId).position);
            });
        });
        return serverCommander -> {
            serverCommands.forEach(command -> command.accept(serverCommander));
        };
    }

    @Override
    public MinionState next(final Tuple2<DataPacket, Minion> input) {
        if(input.value2.singuities.size() > (100 + (20 * input.value1.ownedSpawners.size()))
                && input.value1.freeSpawners.size() > 0) {
            final MinionState challengeClosestSpawner = new ChallengeClosestSpawner(getMinionWielder());
            final Minion splitted = input.value2.split(challengeClosestSpawner, 100);
            getMinionWielder().addMinion(splitted);
        }
        return this;
    }

    @Override
    public void debug(final Tuple2<DataPacket, Minion> input) {

    }

    @Override
    public String getName() {
        return "defend";
    }
}