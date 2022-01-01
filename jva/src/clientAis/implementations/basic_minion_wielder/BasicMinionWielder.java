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