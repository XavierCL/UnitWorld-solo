package clientAis.implementations.multidefense.states;

import clientAis.communications.ServerCommander;
import clientAis.dynamic_data.DataPacket;
import utils.state_machine.State;
import utils.unit_world.singuity_state_machine.SinguityStateMachine;

import java.util.function.Consumer;

public class InitialDefenderState implements State<DataPacket, Consumer<ServerCommander>> {


    public void linkSinguityMachine(SinguityStateMachine singuityStateMachine) {
        // TODO: implement private variable
    }

    @Override
    public void start(DataPacket input) {

    }

    @Override
    public void stop(DataPacket input) {

    }

    @Override
    public Consumer<ServerCommander> exec(DataPacket input) {
        return serverCommander -> {};
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
        return "initial defender";
    }
}
