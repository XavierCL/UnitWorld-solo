package utils.data_structure.bounding_area_hierarchy.query_removal;

import utils.data_structure.bounding_area_hierarchy.basic.AxisAlignedBoundingBox;
import utils.data_structure.tree.binary.query_removal.BNQueryRemoval;
import utils.math.vector.Vector2;

import java.util.List;
import java.util.Map;
import java.util.concurrent.atomic.AtomicInteger;
import java.util.concurrent.atomic.AtomicLong;
import java.util.function.Function;
import java.util.stream.Collectors;

class AABBBNQueryRemoval<T> extends BNQueryRemoval<AxisAlignedBoundingBox, T> {

    private final Map<Long, T> mortonMap;
    private final Function<T, Vector2> leafObjectPositionMapper;

    public AABBBNQueryRemoval(List<Long> mortonCode, Map<Long, T> mortonMap, Function<T, Vector2> leafObjectPositionMapper) {
        this.mortonMap = mortonMap;
        this.leafObjectPositionMapper = leafObjectPositionMapper;
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
            super.right = new AABBBNQueryRemoval<>(rightCodes, mortonMap, leafObjectPositionMapper);
            super.right.setParent(this);
        }
        else if(rightCodes.size() == 1) {
            super.right = new AABBLNQueryRemoval<T>(mortonMap.get(rightCodes.get(0)), leafObjectPositionMapper);
            super.right.setParent(this);
        }
        if(leftCodes.size() > 1) {
            super.left = new AABBBNQueryRemoval<>(leftCodes, mortonMap, leafObjectPositionMapper);
            super.left.setParent(this);
        }
        else if(leftCodes.size() == 1) {
            super.left = new AABBLNQueryRemoval<>(mortonMap.get(leftCodes.get(0)), leafObjectPositionMapper);
            super.left.setParent(this);
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
