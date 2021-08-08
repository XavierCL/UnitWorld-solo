package utils.data_structure.tree.binary.basic;

import java.util.function.Consumer;
import java.util.function.Function;

public interface BinaryNode<T, U, V extends Node<T, U>> extends Node<T, U> {

    Node<T, U> getLeft();
    Node<T, U> getRight();
    void setLeft(V node);
    void setRight(V node);
}
