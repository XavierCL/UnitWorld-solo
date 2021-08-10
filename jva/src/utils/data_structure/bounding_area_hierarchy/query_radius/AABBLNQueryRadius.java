package utils.data_structure.bounding_area_hierarchy.query_radius;

import utils.data_structure.bounding_area_hierarchy.AxisAlignedBoundingBox;
import utils.data_structure.tree.binary.query_radius.LNQueryRadius;
import utils.math.vector.Vector2;

import java.util.function.Function;

class AABBLNQueryRadius<T> extends LNQueryRadius<AxisAlignedBoundingBox, T> {

    public AABBLNQueryRadius(final T leafObject, Function<T, Vector2> positionMapper) {
        super(new AxisAlignedBoundingBox(positionMapper.apply(leafObject), positionMapper.apply(leafObject)), leafObject);
    }
}