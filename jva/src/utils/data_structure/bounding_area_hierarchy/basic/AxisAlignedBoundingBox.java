package utils.data_structure.bounding_area_hierarchy.basic;

import utils.math.vector.Vector2;

public class AxisAlignedBoundingBox {

    public Vector2 upperLeft;
    public Vector2 lowerRight;

    public AxisAlignedBoundingBox(Vector2 upperLeft, Vector2 lowerRight) {
        this.upperLeft =  upperLeft;
        this.lowerRight = lowerRight;
    }

    public AxisAlignedBoundingBox(AxisAlignedBoundingBox aabb1, AxisAlignedBoundingBox aabb2) {
        double smallestX = Math.min(aabb1.upperLeft.x,  aabb2.upperLeft.x);
        double biggestX =  Math.max(aabb1.lowerRight.x, aabb2.lowerRight.x);
        double smallestY = Math.min(aabb1.upperLeft.y,  aabb2.upperLeft.y);
        double biggestY =  Math.max(aabb1.lowerRight.y, aabb2.lowerRight.y);

        this.upperLeft =  new Vector2(smallestX, smallestY);
        this.lowerRight = new Vector2(biggestX,  biggestY);
    }

    public Boolean collidesWith(AxisAlignedBoundingBox that) {
        return     this.upperLeft.x < that.lowerRight.x
                && that.upperLeft.x < this.lowerRight.x
                && this.upperLeft.y < that.lowerRight.y
                && that.upperLeft.y < this.lowerRight.y;
    }
}
