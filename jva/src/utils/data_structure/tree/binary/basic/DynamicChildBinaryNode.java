package utils.data_structure.tree.binary.basic;

import java.util.function.BiConsumer;
import java.util.function.Consumer;
import java.util.function.Function;

public class DynamicChildBinaryNode<T, U, V extends Node<T, U>> implements Node<T, U> {

    private Node<T, U> left;
    private Node<T, U> right;
    private T nodeObject;
    private final Consumer<V> childrenChanged;

    public DynamicChildBinaryNode(Consumer<V> childrenChanged) {
        this.childrenChanged = childrenChanged;
    }

    public T getValue() {
        return nodeObject;
    }
    public void setValue(T nodeObject) {
        this.nodeObject = nodeObject;
    }

    public Node<T, U> getLeft() {
        return left;
    }

    public Node<T, U> getRight() {
        return right;
    }

    public void setLeft(Node<T, U> node) {
        left = node;
        childrenChanged.accept((V)left);
    }

    public void setRight(Node<T, U> node) {
        right = node;
        childrenChanged.accept((V)right);
    }

    @Override
    public void accept(Function<T, Boolean> nodeValidator, Function<U, Boolean> leafValidator, Consumer<U> visitor) {
        if(nodeValidator.apply(getValue())) {
            left.accept(nodeValidator, leafValidator, visitor);
            right.accept(nodeValidator, leafValidator, visitor);
        }
    }
}
