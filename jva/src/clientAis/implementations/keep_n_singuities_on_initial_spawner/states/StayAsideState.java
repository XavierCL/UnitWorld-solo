package clientAis.implementations.keep_n_singuities_on_initial_spawner.states;

import clientAis.communications.ServerCommander;
import clientAis.dynamic_data.DataPacket;
import utils.math.vector.Vector2;
import utils.state_machine.State;
import utils.unit_world.singuity_state_machine.SinguityStateMachine;

import java.util.Optional;
import java.util.function.Consumer;

public class StayAsideState implements State<DataPacket, Consumer<ServerCommander>> {

    private Optional<SinguityStateMachine> singuityStateMachineOpt = Optional.empty();

    public StayAsideState() {
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
                        serverCommander -> serverCommander.moveUnitsToPosition(singuityStateMachine.singuities, new Vector2(3000, 3000)))
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
