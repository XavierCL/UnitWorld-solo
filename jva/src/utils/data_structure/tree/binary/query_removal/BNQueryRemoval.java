package utils.data_structure.tree.binary.query_removal;

import utils.data_structure.tree.binary.basic.BinaryNode;
import utils.data_structure.tree.binary.basic.DynamicChildBinaryNode;
import utils.data_structure.tree.binary.basic.Node;
import utils.data_structure.tree.binary.basic.ParentedNode;

import java.util.Optional;
import java.util.function.Consumer;
import java.util.function.Function;

public class BNQueryRemoval<T, U> implements
        ParentedNode<T, U>,
        BinaryNode<T, U, Node<T, U>> {

    public Optional<BNQueryRemoval<T, U>> parent;

    public DynamicChildBinaryNode<T, U, ParentedNode<T, U>> binaryNode;

    public BNQueryRemoval() {
        this.parent = Optional.empty();
        this.binaryNode = new DynamicChildBinaryNode<>(child -> child.setParent(this));
    }

    public T getValue() {
        return binaryNode.getValue();
    }
    public void setValue(T nodeObject) {
        this.binaryNode.setValue(nodeObject);
    }

    @Override
    public void accept(Function<T, Boolean> nodeValidator, Function<U, Boolean> leafValidator, Consumer<U> visitor) {
        binaryNode.accept(nodeValidator, leafValidator, visitor);
    }

    @Override
    public Optional<BNQueryRemoval<T, U>> getParent() {
        return parent;
    }

    @Override
    public void setParent(BNQueryRemoval<T, U> node) {
        parent = Optional.of(node);
    }

    @Override
    public Node<T, U> getLeft() {
        return binaryNode.getLeft();
    }

    @Override
    public Node<T, U> getRight() {
        return binaryNode.getRight();
    }

    @Override
    public void setLeft(Node<T, U> node) {
        binaryNode.setLeft(node);
    }

    @Override
    public void setRight(Node<T, U> node) {
        binaryNode.setRight(node);
    }
}
