package clientAis.implementations.threat_level_defender.states;

import clientAis.communications.ServerCommander;
import clientAis.communications.game_data.Spawner;
import clientAis.dynamic_data.DataPacket;
import utils.data_structure.cluster.DataCluster;
import utils.data_structure.tupple.Tuple2;
import utils.unit_world.game_evaluation.SpawnerThreatEvaluator;
import utils.unit_world.minion.Minion;
import utils.unit_world.minion.MinionState;
import utils.unit_world.minion.MinionWielder;

import java.util.Map;
import java.util.Optional;
import java.util.function.Consumer;
import java.util.stream.Collectors;

public class DefendClosestSpawner extends MinionState {

    private Optional<Spawner> spawnerToDefend;

    public DefendClosestSpawner(MinionWielder minionWielder) {
        super(minionWielder);
    }

    @Override
    public void start(final Tuple2<DataPacket, Minion> input) {
        input.value2.singuities.addAll(DataPacket.singuityResourceHandler.alloc(150));
        spawnerToDefend = Optional.empty();
        spawnerToDefend = DataPacket.ownedSpawnerResourceHandler.alloc(1).stream().map(input.value1.spawnerIdMap::get).findFirst();
    }

    @Override
    public void stop(final Tuple2<DataPacket, Minion> input) {

    }

    @Override
    public Consumer<ServerCommander> exec(final Tuple2<DataPacket, Minion> input) {
        if(spawnerToDefend.isEmpty()) {
            spawnerToDefend = DataPacket.ownedSpawnerResourceHandler.alloc(1).stream().map(input.value1.spawnerIdMap::get).findAny();
        }
        if(spawnerToDefend.isPresent()) {
            final Spawner spawner = spawnerToDefend.get();
            if(input.value1.spawnerIdMap.get(spawner.id) == null) {
                spawnerToDefend = Optional.empty();
            }
            else {
                final Map<String, Double> threatRatios = SpawnerThreatEvaluator.evaluateThreatRatios(
                        input.value1.ownedSpawners.stream()
                                .map(input.value1.spawnerIdMap::get)
                                .collect(Collectors.toSet()),
                        input.value1.adverseClusters.stream().map(cluster ->
                                new DataCluster<>(cluster.elements.stream()
                                        .map(input.value1.singuityIdMap::get)
                                        .collect(Collectors.toSet())))
                                .collect(Collectors.toSet()));

                spawnerToDefend.ifPresent(spawnerToDefend -> {
                    final double singuityRatio = threatRatios.get(spawnerToDefend.id);
                    if(input.value2.singuities.size() < input.value1.ownedSinguities.size()*singuityRatio) {
                        final int difference = (int)(input.value1.ownedSinguities.size()*singuityRatio - input.value2.singuities.size());
                        input.value2.singuities.addAll(DataPacket.singuityResourceHandler.alloc(difference));
                    }
                    else if(input.value2.singuities.size() > input.value1.ownedSinguities.size()*singuityRatio) {
                        final int difference = (int)(input.value2.singuities.size() - input.value1.ownedSinguities.size()*singuityRatio);
                        final Minion voidMinion = input.value2.split(new VoidState(getMinionWielder()), difference);
                        DataPacket.singuityResourceHandler.free(voidMinion.singuities);
                    }
                });
            }
        }

        return serverCommander ->
                spawnerToDefend.ifPresent(spawnerToDefend ->
                        serverCommander.moveUnitsToPosition(input.value2.singuities, spawnerToDefend.position));
    }

    @Override
    public MinionState next(final Tuple2<DataPacket, Minion> input) {
        if(spawnerToDefend.isEmpty()) {
            getMinionWielder().removeMinion(input.value2);
        }
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