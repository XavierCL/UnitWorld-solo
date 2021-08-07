package utils.data_structure.bah_singuity;

import clientAis.communications.game_data.Singuity;
import utils.data_structure.morton_encoding.MortonMapper;
import utils.data_structure.tree.binary.Node;
import utils.shape.Circle;

import java.util.*;
import java.util.stream.Collectors;

public class BoundingAreaHierarchy {

    private final Optional<Node<AxisAlignedBoundingBox, Singuity>> root;

    public BoundingAreaHierarchy(List<Singuity> singuities) {

        final Map<Long, Singuity> mortonEncodedSinguities = MortonMapper.mapSinguities(singuities);
        final List<Long> sortedMortonCodes = new ArrayList<>(mortonEncodedSinguities.keySet())
                .stream()
                .sorted(Comparator.comparingLong((key -> key)))
                .collect(Collectors.toList());


        if(sortedMortonCodes.size() > 1) {
            this.root = Optional.of(new AABBSinguityBinaryNode(sortedMortonCodes, mortonEncodedSinguities));
        }
        else if(sortedMortonCodes.size() == 1) {
            this.root = Optional.of(new AABBSinguityLeafNode(singuities.get(0)));
        }
        else {
            this.root = Optional.empty();
        }
    }

    public List<Singuity> query(final Circle collidingRegion) {
        final List<Singuity> collidingSinguities = new ArrayList<>();
        final AxisAlignedBoundingBox regionAabb = collidingRegion.toAabb();

        root.ifPresent(node -> node.accept(
                regionAabb::collidesWith,
                singuity -> collidingRegion.contains(singuity.position),
                collidingSinguities::add));

        return collidingSinguities;
    }
}
