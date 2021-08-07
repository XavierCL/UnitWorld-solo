package utils.data_structure.tree.binary.basic;

import java.util.function.Consumer;
import java.util.function.Function;

public class BinaryNode<T, U> implements Node<T, U> {

    public Node<T, U> left;
    public Node<T, U> right;

    private T nodeObject;

    public BinaryNode() {}

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
}
