package utils.data_structure.collection;

import java.util.*;
import java.util.function.Function;
import java.util.stream.Collectors;
import java.util.stream.IntStream;

public class Discriminator<T> {

    private final Collection<T> collection;

    public Discriminator(final Collection<T> collection) {
        this.collection = collection;
    }

    public Collection<T> discriminate(final int amount, final Function<T, Double> scoreFunction) {
        final List<T> sortedElements = collection.stream()
                .sorted(Comparator.comparingDouble(scoreFunction::apply))
                .collect(Collectors.toList());
        Collections.reverse(sortedElements);

        final Collection<T> filteredElements = new HashSet<>();
        IntStream.range(0, amount)
                .filter(i -> i < collection.size())
                .forEach(i -> filteredElements.add(sortedElements.get(i)));

        return filteredElements;
    }
}
