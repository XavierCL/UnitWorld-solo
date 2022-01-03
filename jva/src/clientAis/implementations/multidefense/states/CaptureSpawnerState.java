package clientAis.implementations.multidefense.states;

import clientAis.communications.ServerCommander;
import clientAis.dynamic_data.DataPacket;
import clientAis.implementations.multidefense.MultiDefense;
import utils.state_machine.State;
import utils.unit_world.singuity_state_machine.SinguityStateMachine;

import java.util.Optional;
import java.util.function.Consumer;

public class CaptureSpawnerState implements State<DataPacket, Consumer<ServerCommander>> {

    private Optional<SinguityStateMachine> singuityStateMachineOpt = Optional.empty();
    private Optional<MultiDefense> multiDefenseOpt = Optional.empty();
    private final String idOfSpawnerToCapture;

    public CaptureSpawnerState(final String idOfSpawnerToCapture) {
        this.idOfSpawnerToCapture = idOfSpawnerToCapture;
    }

    public void linkSinguityMachine(SinguityStateMachine singuityStateMachine) {
        singuityStateMachineOpt = Optional.of(singuityStateMachine);
    }

    public void linkMultiDefenseBot(final MultiDefense multiDefense) {
        multiDefenseOpt = Optional.of(multiDefense);
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
                        serverCommander -> serverCommander.moveUnitsToSpawner(singuityStateMachine.singuities, idOfSpawnerToCapture))
                .orElseGet(() -> serverCommander -> {});

    }

    @Override
    public State<DataPacket, Consumer<ServerCommander>> next(DataPacket input) {
        if(!input.freeSpawners.contains(idOfSpawnerToCapture)) {
            multiDefenseOpt.ifPresent(multiDefense -> singuityStateMachineOpt.ifPresent(singuityStateMachine -> {
                multiDefense.assingNewSinguitiesToTheRightSpawners(singuityStateMachine.singuities, input);
                multiDefense.removeStateMachine(singuityStateMachine);
            }));
        }
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
