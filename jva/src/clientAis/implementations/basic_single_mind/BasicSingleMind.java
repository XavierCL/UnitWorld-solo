package clientAis.implementations.basic_single_mind;

import clientAis.communications.ServerCommander;
import clientAis.dynamic_data.DataPacket;
import clientAis.implementations.Bot;
import clientAis.implementations.basic_single_mind.states.ChallengeClosestSpawner;
import utils.unit_world.minion.Minion;
import utils.unit_world.minion.MinionWielder;

import java.util.ArrayList;
import java.util.HashSet;
import java.util.List;
import java.util.Set;
import java.util.function.Consumer;
import java.util.stream.Collectors;

public class BasicSingleMind implements Bot, MinionWielder {

    private final Minion minion;

    public BasicSingleMind() {
        this.minion = new Minion(new ChallengeClosestSpawner(this));
    }

    @Override
    public Consumer<ServerCommander> exec(DataPacket input) {
        Set<String> renegadeSinguities = findRenegades(input);
        handleRenegades(renegadeSinguities);

        Consumer<ServerCommander> command = minion.exec(input);

        final List<Consumer<ServerCommander>> commandList = new ArrayList<>();
        commandList.add(command);

        return serverCommander -> commandList.forEach(serverCommanderConsumer -> serverCommanderConsumer.accept(serverCommander));
    }

    private Set<String> findRenegades(DataPacket input) {
        final Set<String> assignedSinguities = new HashSet<>(minion.singuities);

        return input.ownedSinguities.stream()
                .filter(id -> !assignedSinguities.contains(id))
                .collect(Collectors.toSet());
    }

    private void handleRenegades(Set<String> renegadeSinguities) {
        minion.singuities.addAll(renegadeSinguities);
    }

    @Override
    public void addMinion(Minion ignored) {}

    @Override
    public void removeMinion(Minion ignored) {}

    @Override
    public Set<Minion> getMinions() {
        return new HashSet<>();
    }
}
