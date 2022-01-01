package clientAis.implementations.multidefense;

import clientAis.communications.ServerCommander;
import clientAis.communications.game_data.Singuity;
import clientAis.communications.game_data.Spawner;
import clientAis.dynamic_data.DataPacket;
import clientAis.implementations.Bot;
import clientAis.implementations.multidefense.states.InitialDefenderState;
import utils.state_machine.StateMachine;
import utils.unit_world.singuity_state_machine.SinguityStateMachine;

import java.util.HashSet;
import java.util.Optional;
import java.util.Set;
import java.util.function.BinaryOperator;
import java.util.function.Consumer;
import java.util.stream.Collectors;

public class MultiDefense implements Bot {

    public final Set<SinguityStateMachine> singuityStateMachines;

    public MultiDefense() {
        this.singuityStateMachines = new HashSet<>();
    }

    @Override
    public Consumer<ServerCommander> exec(DataPacket input) {
        makeSureEveryOwnedSpawnerIsHandledWithAStateMachine(input);
        assingNewSinguitiesToTheRightSpawners(input);

        return serverCommander -> singuityStateMachines.stream()
                .map(singuityStateMachine -> singuityStateMachine.stateMachine)
                .map(stateMachine -> stateMachine.exec(input))
                .forEach(serverCommanderConsumer -> serverCommanderConsumer.accept(serverCommander));
    }

    private void makeSureEveryOwnedSpawnerIsHandledWithAStateMachine(final DataPacket input) {
        input.newOwnedSpawners.forEach(this::addDefenderStateMachine);
        input.deadOwnedSpawners.forEach(this::removeDefenderStateMachine);
    }

    private void assingNewSinguitiesToTheRightSpawners(final DataPacket input) {
        input.newOwnedSinguities.forEach(singuityId -> {
            final Singuity singuity = input.singuityIdMap.get(singuityId);
            final Optional<SinguityStateMachine> singuityStateMachineOpt = singuityStateMachines.stream()
                    .filter(singuityStateMachine -> singuityStateMachine.spawnerOpt.isPresent())
                    .reduce(bestSuitedSinguityStateMachineForAGivenSinguity(singuity, input));
            singuityStateMachineOpt.ifPresent(singuityStateMachine -> singuityStateMachine.singuities.add(singuityId));
        });
    }

    private BinaryOperator<SinguityStateMachine> bestSuitedSinguityStateMachineForAGivenSinguity(
            final Singuity singuity,
            final DataPacket input) {
        return (singuityStateMachine1, singuityStateMachine2) -> {
            // we assume the spawners are present
            final Spawner firstSpawner = input.spawnerIdMap.get(singuityStateMachine1.spawnerOpt.get());
            final Spawner SecondSpawner = input.spawnerIdMap.get(singuityStateMachine2.spawnerOpt.get());
            final double distanceSquaredFromFirstMachineSpawner = singuity.position.distanceSquared(firstSpawner.position);
            final double distanceSquaredFromSecondMachineSpawner = singuity.position.distanceSquared(SecondSpawner.position);

            if(distanceSquaredFromFirstMachineSpawner < distanceSquaredFromSecondMachineSpawner) {
                return singuityStateMachine1;
            }
            return singuityStateMachine2;
        };
    }

    private void addDefenderStateMachine(final String spawner) {
        final Optional<SinguityStateMachine> singuityStateMachineOpt = singuityStateMachines.stream()
                .filter(singuityStateMachine -> singuityStateMachine.spawnerOpt.isPresent())
                .filter(singuityStateMachine -> singuityStateMachine.spawnerOpt.get().equals(spawner))
                .findFirst();
        if(singuityStateMachineOpt.isEmpty()) {
            // "complicated stuff" to have a double-sided reference between the state and its state machine
            final InitialDefenderState initialDefenderState = new InitialDefenderState();
            final SinguityStateMachine singuityStateMachine = new SinguityStateMachine(
                    new StateMachine<>(initialDefenderState),
                    new HashSet<>(),
                    spawner);
            // that's where the "double-sided reference" is updated for the state
            initialDefenderState.linkSinguityMachine(singuityStateMachine);
            singuityStateMachines.add(singuityStateMachine);
        }
    }

    private void removeDefenderStateMachine(final String spawner) {
        // Using a set here is indeed redundant, but we handle the general case by doing so instead of
        // assuming that the uuids just work as intended.
        final Set<SinguityStateMachine> singuityStateMachineSet = singuityStateMachines.stream()
                .filter(singuityStateMachine -> singuityStateMachine.spawnerOpt.isPresent())
                .filter(singuityStateMachine -> singuityStateMachine.spawnerOpt.get().equals(spawner))
                .collect(Collectors.toSet());
        singuityStateMachineSet.forEach(singuityStateMachines::remove);
    }
}
