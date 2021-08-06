package utils.data_structure.bah_singuity;

import clientAis.communications.game_data.Singuity;
import utils.data_structure.tree.binary.BinaryNode;

import java.util.List;
import java.util.Map;
import java.util.concurrent.atomic.AtomicInteger;
import java.util.concurrent.atomic.AtomicLong;
import java.util.stream.Collectors;

class AABBSinguityBinaryNode extends BinaryNode<AxisAlignedBoundingBox, Singuity> {

    private final Map<Long, Singuity> mortonSinguityMap;

    public AABBSinguityBinaryNode(List<Long> mortonCode, Map<Long, Singuity> mortonSinguityMap) {
        this.mortonSinguityMap = mortonSinguityMap;
        super.setValue(buildBinaryHierarchy(mortonCode));
    }

    private AxisAlignedBoundingBox buildBinaryHierarchy(List<Long> mortonCodes) {
        final int biggestVaryingBitNumber = biggestVaryingBitNumber(mortonCodes);

        List<Long> rightCodes = mortonCodes.stream()
                .filter(value -> (value & (1L << biggestVaryingBitNumber)) > 0)
                .collect(Collectors.toList());
        List<Long> leftCodes = mortonCodes.stream()
                .filter(value -> (value & (1L << biggestVaryingBitNumber)) == 0)
                .collect(Collectors.toList());

        if(rightCodes.size() > 1) {
            super.right = new AABBSinguityBinaryNode(rightCodes, mortonSinguityMap);
        }
        else if(rightCodes.size() == 1) {
            super.right = new AABBSinguityLeafNode(mortonSinguityMap.get(rightCodes.get(0)));
        }
        if(leftCodes.size() > 1) {
            super.left = new AABBSinguityBinaryNode(leftCodes, mortonSinguityMap);
        }
        else if(leftCodes.size() == 1) {
            super.left = new AABBSinguityLeafNode(mortonSinguityMap.get(leftCodes.get(0)));
        }

        return new AxisAlignedBoundingBox(right.getValue(), left.getValue());
    }

    private int biggestVaryingBitNumber(List<Long> values) {
        AtomicInteger biggestBitNumber = new AtomicInteger(0);
        AtomicLong lastValue = new AtomicLong(values.get(values.size()-1));

        values.forEach(value -> {
            final long difference = lastValue.get() ^ value;
            final int bitNumberOfDifference = biggestBitNumber(difference);
            if(biggestBitNumber.get() < bitNumberOfDifference) {
                biggestBitNumber.set(bitNumberOfDifference);
                lastValue.set(value);
            }
        });

        return biggestBitNumber.get();
    }

    private int biggestBitNumber(long value) {
        long shiftedValue = value;
        int shiftedAmount = 0;

        while(shiftedValue > 1) {
            shiftedValue >>= 1;
            shiftedAmount++;
        }

        return shiftedAmount;
    }
}
