package utils.unit_world.game_data_resources;

import java.util.Comparator;
import java.util.HashSet;
import java.util.List;
import java.util.Set;
import java.util.function.Function;
import java.util.stream.Collectors;
import java.util.stream.IntStream;

public class ResourceHandler<T> implements MemoryResource<T> {

    private final Set<T> availableIds;
    private final Set<T> freeIds;

    public ResourceHandler() {
        this.availableIds = new HashSet<>();
        this.freeIds = new HashSet<>();
    }

    @Override
    public Set<T> alloc(final int amount) {
        return alloc(amount, id -> 0d);
    }

    @Override
    public Set<T> alloc(final int amount, final Function<T, Double> scoreFunction) {
        final List<T> sortedIds = freeIds.stream()
                .sorted(Comparator.comparingDouble(scoreFunction::apply))
                .collect(Collectors.toList());

        final Set<T> allocatedIds = new HashSet<>();
        IntStream.range(0, amount)
                .filter(i -> i < freeIds.size())
                .forEach(i -> allocatedIds.add(sortedIds.get(i)));

        freeIds.removeAll(allocatedIds);

        return allocatedIds;
    }

    @Override
    public void free(final Set<T> ptrSet) {
        final Set<T> availableElements = new HashSet<>(ptrSet);
        availableElements.retainAll(availableIds);
        freeIds.addAll(availableElements);
    }

    @Override
    public void actualize(final Set<T> ptrSet) {
        final Set<T> newElements = new HashSet<>(ptrSet);
        newElements.removeAll(availableIds);
        freeIds.addAll(newElements);
        availableIds.addAll(newElements);

        availableIds.retainAll(ptrSet);
        freeIds.retainAll(ptrSet);
    }

    @Override
    public Set<T> peekRemaining() {
        return new HashSet<>(freeIds);
    }
}
