package utils.data_structure.tree.binary;

import java.util.Optional;
import java.util.function.Consumer;
import java.util.function.Function;

public class BinaryNode<T, U> implements Node<T, U> {

    public Node<T, U> left;
    public Node<T, U> right;

    private final T nodeObject;

    public BinaryNode(T nodeObject) {
        this.nodeObject = nodeObject;
    }

    public T getValue() {
        return nodeObject;
    }

    @Override
    public Optional<U> accept(Function<T, Boolean> nodeValidator, Function<U, Boolean> leafValidator, Consumer<U> visitor) {
        if(nodeValidator.apply(getValue())) {
            left.accept(nodeValidator, leafValidator, visitor)
                    .ifPresent(visitor);
            right.accept(nodeValidator, leafValidator, visitor)
                    .ifPresent(visitor);
        }

        return Optional.empty();
    }
}
