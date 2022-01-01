package utils.data_structure.bounding_area_hierarchy.query_removal;

import utils.data_structure.bounding_area_hierarchy.AxisAlignedBoundingBox;
import utils.data_structure.bounding_area_hierarchy.basic.Queryable;
import utils.data_structure.morton_encoding.MortonMapper;
import utils.data_structure.tree.binary.basic.ParentedNode;
import utils.math.vector.Vector2;
import utils.shape.Circle;

import java.util.*;
import java.util.function.Function;
import java.util.stream.Collectors;

public class BAHQueryRemoval<T> implements Queryable<Circle, List<T>> {

    protected final Optional<ParentedNode<AxisAlignedBoundingBox, T>> root;
    protected final Function<T, Vector2> leafObjectPositionMapper;

    public BAHQueryRemoval(List<T> elements, Function<T, Vector2> leafObjectPositionMapper) {
        this.leafObjectPositionMapper = leafObjectPositionMapper;

        final Map<Long, T> mortonEncodedElement = MortonMapper.mapElements(elements, leafObjectPositionMapper);
        final List<Long> sortedMortonCodes = new ArrayList<>(mortonEncodedElement.keySet())
                .stream()
                .sorted(Comparator.comparingLong((key -> key)))
                .collect(Collectors.toList());


        if(sortedMortonCodes.size() > 1) {
            this.root = Optional.of(new AABBBNQueryRemoval<>(sortedMortonCodes, mortonEncodedElement, leafObjectPositionMapper));
        }
        else if(sortedMortonCodes.size() == 1) {
            this.root = Optional.of(new AABBLNQueryRemoval<>(elements.get(0), leafObjectPositionMapper));
        }
        else {
            this.root = Optional.empty();
        }
    }

    @Override
    public List<T> query(final Circle collidingRegion) {
        final List<T> collidingElements = new ArrayList<>();
        final AxisAlignedBoundingBox regionAabb = collidingRegion.toAabb();

        root.ifPresent(node -> node.accept(
                regionAabb::collidesWith,
                element -> collidingRegion.contains(leafObjectPositionMapper.apply(element)),
                collidingElements::add));

        return collidingElements;
    }
}