package utils.data_structure.bounding_area_hierarchy;

import utils.data_structure.morton_encoding.MortonMapper;
import utils.data_structure.tree.binary.Node;
import utils.math.vector.Vector2;
import utils.shape.Circle;

import java.util.*;
import java.util.function.Function;
import java.util.stream.Collectors;

public class BoundingAreaHierarchy<T> {

    private final Optional<Node<AxisAlignedBoundingBox, T>> root;
    private final Function<T, Vector2> leafObjectPositionMapper;

    public BoundingAreaHierarchy(List<T> elements, Function<T, Vector2> leafObjectPositionMapper) {
        this.leafObjectPositionMapper = leafObjectPositionMapper;

        final Map<Long, T> mortonEncodedSinguities = MortonMapper.mapElements(elements, leafObjectPositionMapper);
        final List<Long> sortedMortonCodes = new ArrayList<>(mortonEncodedSinguities.keySet())
                .stream()
                .sorted(Comparator.comparingLong((key -> key)))
                .collect(Collectors.toList());


        if(sortedMortonCodes.size() > 1) {
            this.root = Optional.of(new AABBBinaryNode<T>(sortedMortonCodes, mortonEncodedSinguities, leafObjectPositionMapper));
        }
        else if(sortedMortonCodes.size() == 1) {
            this.root = Optional.of(new AABBLeafNode<>(elements.get(0), leafObjectPositionMapper));
        }
        else {
            this.root = Optional.empty();
        }
    }

    public List<T> query(final Circle collidingRegion) {
        final List<T> collidingSinguities = new ArrayList<>();
        final AxisAlignedBoundingBox regionAabb = collidingRegion.toAabb();

        root.ifPresent(node -> node.accept(
                regionAabb::collidesWith,
                singuity -> collidingRegion.contains(leafObjectPositionMapper.apply(singuity)),
                collidingSinguities::add));

        return collidingSinguities;
    }
}
