package clientAis.implementations.basic_single_mind.states;

import clientAis.communications.ServerCommander;
import clientAis.communications.game_data.Singuity;
import clientAis.communications.game_data.Spawner;
import clientAis.dynamic_data.DataPacket;
import utils.data_structure.cluster.DataCluster;
import utils.data_structure.tupple.Tuple2;
import utils.math.vector.Vector2;
import utils.unit_world.minion.Minion;
import utils.unit_world.minion.MinionState;
import utils.unit_world.minion.MinionWielder;
import utils.state_machine.State;

import java.util.Comparator;
import java.util.Objects;
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
        final Set<Singuity> singuities = input.value2.singuities.stream()
                .map(id -> input.value1.singuityIdMap.get(id)).collect(Collectors.toSet());
        final Set<Spawner> spawners = input.value1.ownedSpawners.stream()
                .map(id -> input.value1.spawnerIdMap.get(id)).collect(Collectors.toSet());
        final Optional<DataCluster<String>> biggestThreat = input.value1.adverseClusters.stream()
                .reduce((c1, c2) -> c1.elements.size() > c2.elements.size() ? c1 : c2);

        Optional<Vector2> threateningPosition = computeCenterOfMass(singuities);
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

    private Optional<Vector2> computeCenterOfMass(final Set<Singuity> singuities) {
        return singuities.stream()
                .filter(Objects::nonNull)
                .map(singuity -> singuity.position)
                .reduce(Vector2::plus)
                .map(v -> v.scaled(1.0/singuities.size()));
    }

    @Override
    public State<Tuple2<DataPacket, Minion>, Consumer<ServerCommander>> next(final Tuple2<DataPacket, Minion> input) {
        if(input.value2.singuities.size() > (100 + (50 * input.value1.ownedSpawners.size()))) {
            return new ChallengeClosestSpawner(getMinionWielder());
        }
        // attack if enemy made a mistake
        if(input.value2.singuities.size() > input.value1.adverseSinguities.size()*4
                && input.value2.singuities.size() > 150) {
            return new AttackEnemySpawner(getMinionWielder());
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
