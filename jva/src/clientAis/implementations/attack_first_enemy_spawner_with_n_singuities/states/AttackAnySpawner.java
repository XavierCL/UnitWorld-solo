package clientAis.implementations.attack_first_enemy_spawner_with_n_singuities.states;

import clientAis.communications.ServerCommander;
import clientAis.dynamic_data.DataPacket;
import utils.state_machine.State;
import utils.unit_world.singuity_state_machine.SinguityStateMachine;

import java.util.Optional;
import java.util.function.Consumer;

public class AttackAnySpawner implements State<DataPacket, Consumer<ServerCommander>> {

    private Optional<SinguityStateMachine> singuityStateMachineOpt = Optional.empty();
    private final String idOfSpawnerToCapture;

    public AttackAnySpawner(final String idOfSpawnerToCapture) {
        this.idOfSpawnerToCapture = idOfSpawnerToCapture;
    }

    public void linkSinguityMachine(SinguityStateMachine singuityStateMachine) {
        singuityStateMachineOpt = Optional.of(singuityStateMachine);
    }

    @Override
    public void start(DataPacket input) {

    }

    @Override
    public void stop(DataPacket input) {

    }

    @Override
    public Consumer<ServerCommander> exec(DataPacket input) {
        return singuityStateMachineOpt.<Consumer<ServerCommander>>map(singuityStateMachine ->
                        serverCommander -> serverCommander.moveUnitsToPosition(singuityStateMachine.singuities, input.spawnerIdMap.get(idOfSpawnerToCapture).position))
                .orElseGet(() -> serverCommander -> {});

    }

    @Override
    public State<DataPacket, Consumer<ServerCommander>> next(DataPacket input) {
        return this;
    }

    @Override
    public void debug(DataPacket input) {

    }

    @Override
    public String getName() {
        return "capture spawner";
    }
}
