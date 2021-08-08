package utils.data_structure.tree.binary.basic;

import java.util.function.Consumer;
import java.util.function.Function;

public class DynamicChildLeafNode<T, U> implements Node<T, U> {

    private T nodeObject;
    private final U leafObject;

    private final Runnable visitedBehaviour;

    public DynamicChildLeafNode(T nodeObject, U leafObject, Runnable visitedBehaviour) {
        this.nodeObject = nodeObject;
        this.leafObject = leafObject;
        this.visitedBehaviour = visitedBehaviour;
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
            visitedBehaviour.run();
        }
    }
}
