package utils.data_structure.tree.binary.query_removal;

import java.util.Optional;
import java.util.concurrent.atomic.AtomicBoolean;
import java.util.function.Consumer;
import java.util.function.Function;

public class BNQueryRemoval<T, U> implements NodeQueryRemoval<T, U> {

    public Optional<BNQueryRemoval<T, U>> parent;

    public NodeQueryRemoval<T, U> left;
    public NodeQueryRemoval<T, U> right;

    private T nodeObject;

    public BNQueryRemoval() {
        this.parent = Optional.empty();
    }

    public T getValue() {
        return nodeObject;
    }
    public void setValue(T nodeObject) {
        this.nodeObject = nodeObject;
    }

    @Override
    public void accept(Function<T, Boolean> nodeValidator, Function<U, Boolean> leafValidator, Consumer<U> visitor) {
        if(nodeValidator.apply(getValue())) {
            left.accept(nodeValidator, leafValidator, visitor);
            right.accept(nodeValidator, leafValidator, visitor);
        }
    }

    @Override
    public Optional<BNQueryRemoval<T, U>> getParent() {
        return parent;
    }

    @Override
    public void setParent(BNQueryRemoval<T, U> node) {
        parent = Optional.of(node);
    }
}
