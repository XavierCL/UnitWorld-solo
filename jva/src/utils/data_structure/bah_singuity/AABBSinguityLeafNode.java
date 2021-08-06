package utils.data_structure.bah_singuity;

import clientAis.communications.game_data.Singuity;
import utils.data_structure.tree.binary.LeafNode;

class AABBSinguityLeafNode extends LeafNode<AxisAlignedBoundingBox, Singuity> {

    public AABBSinguityLeafNode(final Singuity leafObject) {
        super(new AxisAlignedBoundingBox(leafObject.position, leafObject.position), leafObject);
    }
}
