package utils.data_structure.tree.binary.query_removal;

import utils.data_structure.tree.binary.basic.DynamicChildLeafNode;
import utils.data_structure.tree.binary.basic.LeafNode;
import utils.data_structure.tree.binary.basic.ParentedNode;

import java.util.Optional;
import java.util.function.Consumer;
import java.util.function.Function;

public class LNQueryRemoval<T, U> implements
        ParentedNode<T, U>,
        LeafNode<T, U> {

    public Optional<BNQueryRemoval<T, U>> parent;

    public final DynamicChildLeafNode<T, U> leafNode;

    public LNQueryRemoval(T nodeObject, U leafObject) {
        this.parent = Optional.empty();
        this.leafNode = new DynamicChildLeafNode<>(nodeObject, leafObject, () -> {
            parent.ifPresent(parent -> parent.getParent().ifPresent(grandParent -> {
                if(grandParent.getLeft() == parent) {
                    grandParent.setLeft(parent.getLeft() == this ? parent.getRight() : parent.getLeft());
                }
                else {
                    grandParent.setRight(parent.getLeft() == this ? parent.getRight() : parent.getLeft());
                }
            }));
        });
    }

    public T getValue() {
        return leafNode.getValue();
    }
    public void setValue(T nodeObject) {
        this.leafNode.setValue(nodeObject);
    }

    @Override
    public void accept(Function<T, Boolean> nodeValidator, Function<U, Boolean> leafValidator, Consumer<U> visitor) {
        leafNode.accept(nodeValidator, leafValidator, visitor);
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
