package utils.unit_world.game_data_resources.singuity;

import utils.unit_world.game_data_resources.MemoryResource;

import java.util.*;
import java.util.function.*;
import java.util.stream.*;

public class SinguityResourceHandler implements MemoryResource<String> {

    private final Set<String> availableSinguityIds;
    private final Set<String> freeSinguityIds;

    public SinguityResourceHandler() {
        this.availableSinguityIds = new HashSet<>();
        this.freeSinguityIds = new HashSet<>();
    }

    @Override
    public Set<String> alloc(final int amount) {
        return alloc(amount, id -> 0d);
    }

    @Override
    public Set<String> alloc(final int amount, final Function<String, Double> scoreFunction) {
        final List<String> sortedSinguityIds = freeSinguityIds.stream()
                .sorted(Comparator.comparingDouble(scoreFunction::apply))
                .collect(Collectors.toList());

        final Set<String> allocatedSinguityIds = new HashSet<>();
        IntStream.range(0, amount)
                .filter(i -> i < freeSinguityIds.size())
                .forEach(i -> allocatedSinguityIds.add(sortedSinguityIds.get(i)));

        freeSinguityIds.removeAll(allocatedSinguityIds);

        return allocatedSinguityIds;
    }

    @Override
    public void free(final Set<String> ptrSet) {
        final Set<String> availableElements = new HashSet<>(ptrSet);
        availableElements.retainAll(availableSinguityIds);
        freeSinguityIds.addAll(availableElements);
    }

    @Override
    public void actualize(final Set<String> ptrSet) {
        final Set<String> newElements = new HashSet<>(ptrSet);
        newElements.removeAll(availableSinguityIds);
        freeSinguityIds.addAll(newElements);
        availableSinguityIds.addAll(newElements);

        availableSinguityIds.retainAll(ptrSet);
        freeSinguityIds.retainAll(ptrSet);
    }

    @Override
    public Set<String> peekRemaining() {
        return new HashSet<>(freeSinguityIds);
    }
}
