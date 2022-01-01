package utils.unit_world.game_data_resources;

import java.util.function.Function;
import java.util.Set;

public interface MemoryResource<T> {
    Set<T> alloc(int amount);
    Set<T> alloc(int amount, Function<T, Double> scoreFunction);
    void free(Set<T> ptrSet);

    void actualize(Set<T> ptrSet);
    Set<T> peekRemaining();
}