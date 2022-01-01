package utils.unit_world.singuity_state_machine;

import clientAis.communications.ServerCommander;
import clientAis.dynamic_data.DataPacket;
import utils.state_machine.StateMachine;

import java.util.Optional;
import java.util.Set;
import java.util.function.Consumer;

public class SinguityStateMachine {

    public final StateMachine<DataPacket, Consumer<ServerCommander>> stateMachine;
    public final Set<String> singuities;
    public final Optional<String> spawnerOpt;

    public SinguityStateMachine(
            final StateMachine<DataPacket, Consumer<ServerCommander>> stateMachine,
            final Set<String> singuities,
            final String spawner) {
        this.stateMachine = stateMachine;
        this.singuities = singuities;
        this.spawnerOpt = Optional.ofNullable(spawner);
    }

    public SinguityStateMachine(
            final StateMachine<DataPacket, Consumer<ServerCommander>> stateMachine,
            final Set<String> singuities) {
        this(stateMachine, singuities, null);
    }
}
