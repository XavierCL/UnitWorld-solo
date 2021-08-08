package utils.data_structure.bounding_area_hierarchy.query_radius;

import utils.data_structure.bounding_area_hierarchy.AxisAlignedBoundingBox;
import utils.data_structure.tree.binary.query_radius.BNQueryRadius;
import utils.math.vector.Vector2;

import java.util.List;
import java.util.Map;
import java.util.concurrent.atomic.AtomicInteger;
import java.util.concurrent.atomic.AtomicLong;
import java.util.function.Function;
import java.util.stream.Collectors;

class AABBBNQueryRadius<T> extends BNQueryRadius<AxisAlignedBoundingBox, T> {

    private final Map<Long, T> mortonMap;
    private final Function<T, Vector2> leafObjectPositionMapper;

    public AABBBNQueryRadius(List<Long> mortonCode, Map<Long, T> mortonMap, Function<T, Vector2> leafObjectPositionMapper) {
        super();
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
            super.setRight(new AABBBNQueryRadius<>(rightCodes, mortonMap, leafObjectPositionMapper));
        }
        else if(rightCodes.size() == 1) {
            super.setRight(new AABBLNQueryRadius<>(mortonMap.get(rightCodes.get(0)), leafObjectPositionMapper));
        }
        if(leftCodes.size() > 1) {
            super.setLeft(new AABBBNQueryRadius<>(leftCodes, mortonMap, leafObjectPositionMapper));
        }
        else if(leftCodes.size() == 1) {
            super.setLeft(new AABBLNQueryRadius<>(mortonMap.get(leftCodes.get(0)), leafObjectPositionMapper));
        }

        return new AxisAlignedBoundingBox(super.getRight().getValue(), super.getLeft().getValue());
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
