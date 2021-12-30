package utils.unit_world.game_data_resources.spawner;

import utils.unit_world.game_data_resources.MemoryResource;

import java.util.Comparator;
import java.util.HashSet;
import java.util.List;
import java.util.Set;
import java.util.function.Function;
import java.util.stream.Collectors;
import java.util.stream.IntStream;

public class OwnedSpawnerResourceHandler implements MemoryResource<String> {

    private final Set<String> availableOwnedSpawnerIds;
    private final Set<String> freeOwnedSpawnerIds;

    public OwnedSpawnerResourceHandler() {
        this.availableOwnedSpawnerIds = new HashSet<>();
        this.freeOwnedSpawnerIds = new HashSet<>();
    }

    @Override
    public Set<String> alloc(final int amount) {
        return alloc(amount, id -> 0d);
    }

    @Override
    public Set<String> alloc(final int amount, final Function<String, Double> scoreFunction) {
        final List<String> sortedOwnedSpawnerIds = freeOwnedSpawnerIds.stream()
                .sorted(Comparator.comparingDouble(scoreFunction::apply))
                .collect(Collectors.toList());

        final Set<String> allocatedOwnedSpawnerIds = new HashSet<>();
        IntStream.range(0, amount)
                .filter(i -> i < freeOwnedSpawnerIds.size())
                .forEach(i -> allocatedOwnedSpawnerIds.add(sortedOwnedSpawnerIds.get(i)));

        freeOwnedSpawnerIds.removeAll(allocatedOwnedSpawnerIds);

        return allocatedOwnedSpawnerIds;
    }

    @Override
    public void free(final Set<String> ptrSet) {
        final Set<String> availableElements = new HashSet<>(ptrSet);
        availableElements.retainAll(availableOwnedSpawnerIds);
        freeOwnedSpawnerIds.addAll(availableElements);
    }

    @Override
    public void actualize(final Set<String> ptrSet) {
        final Set<String> newElements = new HashSet<>(ptrSet);
        newElements.removeAll(availableOwnedSpawnerIds);
        freeOwnedSpawnerIds.addAll(newElements);
        availableOwnedSpawnerIds.addAll(newElements);

        availableOwnedSpawnerIds.retainAll(ptrSet);
        freeOwnedSpawnerIds.retainAll(ptrSet);
    }

    @Override
    public Set<String> peekRemaining() {
        return new HashSet<>(freeOwnedSpawnerIds);
    }
}
