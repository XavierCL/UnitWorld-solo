package clientAis.implementations.keep_n_singuities_on_initial_spawner;

import clientAis.communications.ServerCommander;
import clientAis.communications.game_data.Singuity;
import clientAis.communications.game_data.Spawner;
import clientAis.dynamic_data.DataPacket;
import clientAis.implementations.Bot;
import clientAis.implementations.keep_n_singuities_on_initial_spawner.states.InitialDefenderState;
import clientAis.implementations.keep_n_singuities_on_initial_spawner.states.StayAsideState;
import utils.state_machine.StateMachine;
import utils.unit_world.singuity_state_machine.SinguityStateMachine;

import java.util.HashSet;
import java.util.List;
import java.util.Optional;
import java.util.Set;
import java.util.function.BinaryOperator;
import java.util.function.Consumer;
import java.util.stream.Collectors;

public class KeepNSinguitiesOnInitialSpawner implements Bot {

    public final Set<SinguityStateMachine> singuityStateMachines;
    public final Set<SinguityStateMachine> singuityStateMachinesToRemove;

    public KeepNSinguitiesOnInitialSpawner() {
        this.singuityStateMachines = new HashSet<>();
        this.singuityStateMachinesToRemove = new HashSet<>();
    }

    @Override
    public Consumer<ServerCommander> exec(final DataPacket input) {
        singuityStateMachines.removeAll(singuityStateMachinesToRemove);
        singuityStateMachinesToRemove.clear();

        updateInitialSpawnerStates(input);
        updateSinguitiesForTheRunningStates(input);
        updateNonDefendingStates(input);

        return executeBotMachines(input);
    }

    private void updateInitialSpawnerStates(final DataPacket input) {
        input.newOwnedSpawners.forEach(this::addDefenderStateMachine);
        input.deadOwnedSpawners.forEach(this::removeDefenderStateMachine);
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

    private void updateSinguitiesForTheRunningStates(final DataPacket input) {
        assingNewSinguitiesToTheRightSpawners(input.newOwnedSinguities, input);
        removeSinguitiesFromRunningStates(input.deadOwnedSinguities);
    }

    public void assingNewSinguitiesToTheRightSpawners(final Set<String> singuitiesToAdd, final DataPacket input) {
        singuitiesToAdd.forEach(singuityId -> {
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

    private void removeSinguitiesFromRunningStates(final Set<String> singuitiesToRemove) {
        singuityStateMachines.forEach(singuityStateMachine -> singuityStateMachine.singuities.removeAll(singuitiesToRemove));
    }

    private void updateNonDefendingStates(final DataPacket input) {
        final Optional<Set<String>> defendingSinguitiesOpt = getDefendingSinguitiesOpt();
        defendingSinguitiesOpt.ifPresent(defendingSinguities -> {
            if(defendingSinguities.size() >= 50) {
                final String singuityToRemove = defendingSinguities.stream().findFirst().get();
                // updating singuities when creating the new state
                singuityStateMachines.forEach(singuityStateMachine -> singuityStateMachine.singuities.remove(singuityToRemove));
                final StayAsideState state = new StayAsideState();
                final SinguityStateMachine singuityStateMachine = new SinguityStateMachine(
                        new StateMachine<>(state),
                        new HashSet<>(List.of(singuityToRemove)));
                state.linkSinguityMachine(singuityStateMachine);
                singuityStateMachines.add(singuityStateMachine);
            }
        });
    }

    private Optional<Set<String>> getDefendingSinguitiesOpt() {
        return singuityStateMachines.stream()
                .filter(singuityStateMachine -> singuityStateMachine.spawnerOpt.isPresent())
                .map(singuityStateMachine -> singuityStateMachine.singuities)
                .reduce((singuities1, singuities2) -> {
                    final Set<String> combined = new HashSet<>(singuities1);
                    combined.addAll(singuities2);
                    return combined;
                });
    }

    private Consumer<ServerCommander> executeBotMachines(final DataPacket input) {
        return serverCommander -> singuityStateMachines.stream()
                .map(singuityStateMachine -> singuityStateMachine.stateMachine)
                .map(stateMachine -> stateMachine.exec(input))
                .forEach(serverCommanderConsumer -> serverCommanderConsumer.accept(serverCommander));
    }

    public void removeStateMachine(final SinguityStateMachine singuityStateMachine) {
        singuityStateMachinesToRemove.add(singuityStateMachine);
    }
}
