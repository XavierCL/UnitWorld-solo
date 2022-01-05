package clientAis.implementations.attack_first_enemy_spawner_with_n_singuities.states;

import clientAis.communications.ServerCommander;
import clientAis.dynamic_data.DataPacket;
import utils.state_machine.State;
import utils.unit_world.singuity_state_machine.SinguityStateMachine;

import java.util.Optional;
import java.util.function.Consumer;

public class InitialDefenderState implements State<DataPacket, Consumer<ServerCommander>> {

    private Optional<SinguityStateMachine> singuityStateMachineOpt = Optional.empty();

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
        if(singuityStateMachineOpt.isPresent()) {
            if(singuityStateMachineOpt.get().spawnerOpt.isPresent()) {
                return serverCommander -> serverCommander.moveUnitsToPosition(
                        singuityStateMachineOpt.get().singuities,
                        input.spawnerIdMap.get(singuityStateMachineOpt.get().spawnerOpt.get()).position);
            }
        }

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
