package clientAis.implementations.basic_minion_wielder;

import clientAis.communications.ServerCommander;
import clientAis.dynamic_data.DataPacket;
import clientAis.implementations.Bot;
import clientAis.implementations.basic_minion_wielder.states.DefendClosestSpawner;
import utils.unit_world.minion.Minion;
import utils.unit_world.minion.MinionWielder;

import java.util.HashSet;
import java.util.Set;
import java.util.function.Consumer;
import java.util.stream.Collectors;

public class BasicMinionWielder implements Bot, MinionWielder {

    private final Set<Minion> minions;
    private final Set<Minion> minionsToAdd;
    private final Set<Minion> minionsToRemove;

    // TODO: Check whether an enemy cluster is moving or not. If it's moving, check where it's most likely headed to (probably a spawner. Which one?)
    //       If it's not moving, and it's on an enemy spawner, it's probably defending. If it's not moving, and on an ally spawner, then it's probably attacking.
    //       If it's not moving, and it's in the middle of nowhere, then it's probably wasting its time? Bait? Forming a strong cluster? Gathering more singuities before attack?

    public BasicMinionWielder() {
        this.minions = new HashSet<>();
        this.minionsToAdd = new HashSet<>();
        this.minionsToRemove = new HashSet<>();
        addMinion(new Minion(new DefendClosestSpawner(this)));
    }

    @Override
    public Consumer<ServerCommander> exec(DataPacket input) {
        final Set<Consumer<ServerCommander>> commandList = minions.stream()
                .map(minion -> minion.exec(input))
                .collect(Collectors.toSet());

        updateMinionList();

        return serverCommander -> commandList.forEach(serverCommanderConsumer ->
                serverCommanderConsumer.accept(serverCommander));
    }

    private void updateMinionList() {
        minions.addAll(minionsToAdd);
        minions.stream()
                .filter(minion -> minions.size() > 1)
                .filter(minion -> minion.singuities.size() == 0)
                .forEach(minionsToRemove::add);
        minions.removeAll(minionsToRemove);
        minionsToRemove.forEach(minion ->
                DataPacket.singuityResourceHandler.free(minion.singuities));

        minionsToAdd.clear();
        minionsToRemove.clear();
    }

    @Override
    public void addMinion(Minion minion) {
        minionsToAdd.add(minion);
    }

    @Override
    public void removeMinion(Minion minion) {
        minionsToRemove.add(minion);
    }

    @Override
    public Set<Minion> getMinions() {
        return minions;
    }
}
