package clientAis.implementations.basic_single_mind.states;

import clientAis.communications.ServerCommander;
import clientAis.communications.game_data.Singuity;
import clientAis.communications.game_data.Spawner;
import clientAis.dynamic_data.DataPacket;
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

public class AttackEnemySpawner extends MinionState {

    private final AtomicReference<Optional<String>> closestSpawnerOpt = new AtomicReference<>(Optional.empty());

    public AttackEnemySpawner(MinionWielder minionWielder) {
        super(minionWielder);
    }

    @Override
    public void start(final Tuple2<DataPacket, Minion> input) {
        final Set<Singuity> singuities = input.value2.singuities.stream()
                .map(id -> input.value1.singuityIdMap.get(id)).collect(Collectors.toSet());
        final Set<Spawner> attackableSpawners = input.value1.adverseSpawners.stream()
                .map(id -> input.value1.spawnerIdMap.get(id)).collect(Collectors.toSet());
        final Optional<Vector2> centerOfMassOpt = computeCenterOfMass(singuities);
        centerOfMassOpt.ifPresent(centerOfMass -> {
            closestSpawnerOpt.set(attackableSpawners.stream()
                    .min(Comparator.comparingDouble(spawner -> spawner.position.minus(centerOfMass).magnitudeSquared()))
                    .map(spawner -> spawner.id));
        });
    }

    @Override
    public void stop(final Tuple2<DataPacket, Minion> input) {

    }

    @Override
    public Consumer<ServerCommander> exec(final Tuple2<DataPacket, Minion> input) {

        return serverCommander -> closestSpawnerOpt.get().ifPresent(closestSpawner ->
                serverCommander.moveUnitsToPosition(input.value2.singuities, input.value1.spawnerIdMap.get(closestSpawner).position));
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
        if(closestSpawnerOpt.get().isPresent()) {
            if(input.value1.freeSpawners.contains(closestSpawnerOpt.get().get())) {
                return new DefendClosestSpawner(getMinionWielder());
            }
        }
        if(input.value2.singuities.size() < 50) {
            return new DefendClosestSpawner(getMinionWielder());
        }
        return this;
    }

    @Override
    public void debug(final Tuple2<DataPacket, Minion> input) {

    }

    @Override
    public String getName() {
        return "attack";
    }
}
