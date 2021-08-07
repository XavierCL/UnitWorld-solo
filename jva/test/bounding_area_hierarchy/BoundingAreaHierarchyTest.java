package bounding_area_hierarchy;

import clientAis.communications.game_data.Singuity;
import org.junit.jupiter.api.Test;
import utils.data_structure.bounding_area_hierarchy.basic.BoundingAreaHierarchy;
import utils.math.vector.Vector2;
import utils.shape.Circle;

import java.util.ArrayList;
import java.util.List;
import java.util.concurrent.atomic.AtomicReference;
import java.util.stream.IntStream;

public class BoundingAreaHierarchyTest {

    @Test
    public void SingleSinguityInCenterWithBigEnoughQueryArea() {
        final List<Singuity> singuities = new ArrayList<>();
        singuities.add(new Singuity(new Vector2(0, 0)));

        final BoundingAreaHierarchy<Singuity> boundingAreaHierarchy = new BoundingAreaHierarchy<>(singuities, singuity -> singuity.position);

        final Circle queryArea = new Circle(new Vector2(30, 40), 100);
        List<Singuity> inboundSinguities = boundingAreaHierarchy.query(queryArea);

        assert(singuities.get(0) == inboundSinguities.get(0));
    }

    @Test
    public void SingleSinguityInCenterWithTooSmallQueryArea() {
        final List<Singuity> singuities = new ArrayList<>();
        singuities.add(new Singuity(new Vector2(0, 0)));

        final BoundingAreaHierarchy<Singuity> boundingAreaHierarchy = new BoundingAreaHierarchy<>(singuities, singuity -> singuity.position);

        final Circle queryArea = new Circle(new Vector2(30, 40), 10);
        List<Singuity> inboundSinguities = boundingAreaHierarchy.query(queryArea);

        assert(inboundSinguities.isEmpty());
    }

    @Test
    public void TwoSinguitiesNearCenterWithOnlyOneOfThemInQueryCircle() {
        final List<Singuity> singuities = new ArrayList<>();
        Singuity inboundSinguity = new Singuity(new Vector2(0, 0));
        singuities.add(inboundSinguity);
        singuities.add(new Singuity(new Vector2(1000, 1000)));

        final BoundingAreaHierarchy<Singuity> boundingAreaHierarchy = new BoundingAreaHierarchy<>(singuities, singuity -> singuity.position);

        final Circle queryArea = new Circle(new Vector2(10, 10), 20);
        List<Singuity> inboundSinguities = boundingAreaHierarchy.query(queryArea);

        assert(inboundSinguities.get(0) == inboundSinguity);
        assert(inboundSinguities.size() == 1);
    }

    @Test
    public void ThreeSinguitiesNearCenterWithOnlyOneOfThemInQueryCircle() {
        final List<Singuity> singuities = new ArrayList<>();
        Singuity inboundSinguity = new Singuity(new Vector2(0, 0));
        singuities.add(inboundSinguity);
        singuities.add(new Singuity(new Vector2(1000, 1000)));
        singuities.add(new Singuity(new Vector2(-1000, -1000)));

        final BoundingAreaHierarchy<Singuity> boundingAreaHierarchy = new BoundingAreaHierarchy<>(singuities, singuity -> singuity.position);

        final Circle queryArea = new Circle(new Vector2(10, 10), 20);
        List<Singuity> inboundSinguities = boundingAreaHierarchy.query(queryArea);

        assert(inboundSinguities.get(0) == inboundSinguity);
        assert(inboundSinguities.size() == 1);
    }

    @Test
    public void FiveSinguitiesOverTenAreCorrectlyQueriedInCircle() {
        final List<Singuity> singuities = new ArrayList<>();
        singuities.add(new Singuity(new Vector2(0, 0)));
        singuities.add(new Singuity(new Vector2(400, 30)));
        singuities.add(new Singuity(new Vector2(-20, 5000)));
        singuities.add(new Singuity(new Vector2(300, 2644)));
        singuities.add(new Singuity(new Vector2(642, 34)));

        final List<Singuity> inQueryZoneSinguities = new ArrayList<>();
        inQueryZoneSinguities.add(new Singuity(new Vector2(4000, 3300)));
        inQueryZoneSinguities.add(new Singuity(new Vector2(4100, 3200)));
        inQueryZoneSinguities.add(new Singuity(new Vector2(3900, 3000)));
        inQueryZoneSinguities.add(new Singuity(new Vector2(4250, 3100)));
        inQueryZoneSinguities.add(new Singuity(new Vector2(4050, 3600)));

        singuities.addAll(inQueryZoneSinguities);

        final BoundingAreaHierarchy<Singuity> boundingAreaHierarchy = new BoundingAreaHierarchy<>(singuities, singuity -> singuity.position);

        final Circle queryArea = new Circle(new Vector2(4000, 3300), 10000);
        List<Singuity> inboundSinguities = boundingAreaHierarchy.query(queryArea);

        assert(inboundSinguities.containsAll(inQueryZoneSinguities));
    }

    @Test
    public void AHundredThousandRandomQueriesJustToSeeHowLongItTakes() {
        final List<Singuity> singuities = new ArrayList<>(100000);

        IntStream.range(0, 100000).forEach(i -> {
            singuities.add(new Singuity(new Vector2(Math.random()*10000, Math.random()*10000)));
        });

        final BoundingAreaHierarchy<Singuity> boundingAreaHierarchy = new BoundingAreaHierarchy<>(singuities, singuity -> singuity.position);

        final Circle queryArea = new Circle(new Vector2(4000, 3300), 1000);

        long x1 = System.currentTimeMillis();

        AtomicReference<List<Singuity>> atomicReference = new AtomicReference<>();
        IntStream.range(0, 1000).forEach(j -> {
            atomicReference.set(boundingAreaHierarchy.query(queryArea));
        });

        long x2 = System.currentTimeMillis();

        assert(true);
    }

}
