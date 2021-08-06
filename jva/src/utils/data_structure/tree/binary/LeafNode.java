package utils.data_structure.tree.binary;

import java.util.Optional;
import java.util.function.Consumer;
import java.util.function.Function;

public class LeafNode<T, U> implements Node<T, U> {

    private T nodeObject;
    private final U leafObject;

    public LeafNode(T nodeObject, U leafObject) {
        this.nodeObject = nodeObject;
        this.leafObject = leafObject;
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
        }
    }
}
