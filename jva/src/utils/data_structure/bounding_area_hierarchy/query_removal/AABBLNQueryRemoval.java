package utils.data_structure.bounding_area_hierarchy.query_removal;

import utils.data_structure.bounding_area_hierarchy.AxisAlignedBoundingBox;
import utils.data_structure.tree.binary.query_removal.LNQueryRemoval;
import utils.math.vector.Vector2;

import java.util.function.Function;

class AABBLNQueryRemoval<T> extends LNQueryRemoval<AxisAlignedBoundingBox, T> {

    public AABBLNQueryRemoval(final T leafObject, Function<T, Vector2> positionMapper) {
        super(new AxisAlignedBoundingBox(positionMapper.apply(leafObject), positionMapper.apply(leafObject)), leafObject);
    }
}
