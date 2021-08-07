package utils.data_structure.tree.binary.query_removal;

import java.util.Optional;
import java.util.function.Consumer;
import java.util.function.Function;

public interface NodeQueryRemoval<T, U> {

    void accept(Function<T, Boolean> nodeValidator, Function<U, Boolean> leafValidator, Consumer<U> visitor);
    Optional<BNQueryRemoval<T, U>> getParent();
    void setParent(BNQueryRemoval<T, U> node);
    T getValue();
    void setValue(T nodeObject);
}
