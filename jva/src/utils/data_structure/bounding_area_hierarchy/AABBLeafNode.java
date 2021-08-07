package utils.data_structure.bounding_area_hierarchy;

import utils.data_structure.tree.binary.LeafNode;
import utils.math.vector.Vector2;

import java.util.function.Function;

class AABBLeafNode<T> extends LeafNode<AxisAlignedBoundingBox, T> {

    public AABBLeafNode(final T leafObject, Function<T, Vector2> positionMapper) {
        super(new AxisAlignedBoundingBox(positionMapper.apply(leafObject), positionMapper.apply(leafObject)), leafObject);
    }
}
