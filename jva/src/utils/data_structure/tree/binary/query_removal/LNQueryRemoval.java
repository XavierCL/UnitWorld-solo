package utils.data_structure.tree.binary.query_removal;

import java.util.Optional;
import java.util.function.Consumer;
import java.util.function.Function;

public class LNQueryRemoval<T, U> implements NodeQueryRemoval<T, U> {

    public Optional<BNQueryRemoval<T, U>> parent;

    private T nodeObject;
    private final U leafObject;

    public LNQueryRemoval(T nodeObject, U leafObject) {
        this.nodeObject = nodeObject;
        this.leafObject = leafObject;
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
        if(leafValidator.apply(leafObject)) {
            visitor.accept(leafObject);
            parent.ifPresent(parent -> parent.getParent().ifPresent(grandParent -> {
                if(grandParent.left == parent) {
                    grandParent.left = parent.left == this ? parent.right : parent.left;
                    grandParent.left.setParent(grandParent);
                }
                else {
                    grandParent.right = parent.left == this ? parent.right : parent.left;
                    grandParent.right.setParent(grandParent);
                }
            }));
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
