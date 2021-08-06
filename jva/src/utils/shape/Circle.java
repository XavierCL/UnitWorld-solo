package utils.shape;

import utils.data_structure.bah_singuity.AxisAlignedBoundingBox;
import utils.math.vector.Vector2;

public class Circle {

    public final Vector2 center;
    public final double radii;

    public Circle(Vector2 center, double radii) {
        this.center = center;
        this.radii = radii;
    }

    public AxisAlignedBoundingBox toAabb() {
        return new AxisAlignedBoundingBox(
                new Vector2(center.x - radii, center.y - radii),
                new Vector2(center.x + radii, center.y + radii));
    }

    public Boolean contains(Vector2 position) {
        return center.minus(position).magnitudeSquared() < radii * radii;
    }
}
