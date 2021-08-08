package utils.data_structure.tree.binary.basic;

import utils.data_structure.tree.binary.basic.Node;
import utils.data_structure.tree.binary.query_removal.BNQueryRemoval;

import java.util.Optional;

public interface ParentedNode<T, U> extends Node<T, U> {

    Optional<BNQueryRemoval<T, U>> getParent();
    void setParent(BNQueryRemoval<T, U> node);
}
