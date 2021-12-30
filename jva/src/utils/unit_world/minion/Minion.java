package utils.unit_world.minion;

import clientAis.communications.ServerCommander;
import clientAis.dynamic_data.DataPacket;
import utils.data_structure.tupple.Tuple2;
import utils.state_machine.Behaviour;
import utils.state_machine.StateMachine;

import java.util.HashSet;
import java.util.Optional;
import java.util.Set;
import java.util.function.Consumer;
import java.util.stream.IntStream;

public class Minion implements Behaviour<DataPacket, Consumer<ServerCommander>> {

    public final Set<String> singuities;
    private final StateMachine<Tuple2<DataPacket, Minion>, Consumer<ServerCommander>> stateMachine;

    public Minion(MinionState initialState) {
        this.singuities = new HashSet<>();
        this.stateMachine = new StateMachine<>(initialState);
    }

    @Override
    public Consumer<ServerCommander> exec(DataPacket input) {
        singuities.retainAll(input.ownedSinguities);
        return stateMachine.exec(new Tuple2<>(input, this));
    }

    public Minion split(final MinionState minionState, final int amountOfSinguities) {
        final Minion minion = new Minion(minionState);

        minion.singuities.addAll(splitNFirstSinguities(amountOfSinguities));

        return minion;
    }

    private Set<String> splitNFirstSinguities(final int amountOfSinguities) {
        final Set<String> newSinguitySet = new HashSet<>();
        final int maxSplitAmount = singuities.size();

        IntStream.range(0, amountOfSinguities)
                .filter(i -> i < maxSplitAmount)
                .forEach(i -> {
            final Optional<String> singuityIdOpt = singuities.stream().findFirst();
            singuityIdOpt.ifPresent(singuityId -> {
                newSinguitySet.add(singuityId);
                singuities.remove(singuityId);
            });
        });

        return newSinguitySet;
    }

    public String getRunningStateName() {
        return stateMachine.getRunningStateName();
    }
}
