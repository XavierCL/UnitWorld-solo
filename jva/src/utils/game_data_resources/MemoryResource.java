package utils.game_data_resources;

import java.util.function.Function;
import java.util.Set;

public interface MemoryResource<PointerType> {
    Set<PointerType> alloc(int amount);
    Set<PointerType> alloc(int amount,  Function<String, Double> scoreFunction);
    void free(Set<PointerType> ptrSet);

    void actualize(Set<PointerType> ptrSet);
    Set<PointerType> peekRemaining();
}