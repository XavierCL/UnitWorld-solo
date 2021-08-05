package utils.data_structure.tree.binary;

import java.util.Optional;
import java.util.function.Consumer;
import java.util.function.Function;

public interface Node<T, U> {

    Optional<U> accept(Function<T, Boolean> nodeValidator, Function<U, Boolean> leafValidator, Consumer<U> visitor);
    T getValue();
}
