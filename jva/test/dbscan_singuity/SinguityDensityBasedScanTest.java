package dbscan_singuity;

import clientAis.communications.game_data.Singuity;
import org.junit.jupiter.api.Test;
import utils.data_structure.cluster_singuity.SinguityCluster;
import utils.data_structure.cluster_singuity.SinguityDensityBasedScan;
import utils.math.vector.Vector2;

import java.util.ArrayList;
import java.util.List;
import java.util.concurrent.atomic.AtomicReference;
import java.util.stream.IntStream;

public class SinguityDensityBasedScanTest {

    @Test
    public void ScanRunsAndDoesn_tCrash() {
        List<Singuity> singuities = new ArrayList<>();
        singuities.add(new Singuity(new Vector2(200, 300), "0"));

        SinguityDensityBasedScan singuityDensityBasedScan = new SinguityDensityBasedScan(singuities);
        List<SinguityCluster> singuityClusters = singuityDensityBasedScan.findClusters(10);

        assert(singuityClusters.get(0).singuityIds.size() == 1);
    }

    @Test
    public void ScanDetectsTwoClustersAndTheirSizesAreValid() {
        List<Singuity> singuities = new ArrayList<>();
        singuities.add(new Singuity(new Vector2(203, 300), "0"));
        singuities.add(new Singuity(new Vector2(217, 250), "1"));
        singuities.add(new Singuity(new Vector2(113, 200), "2"));
        singuities.add(new Singuity(new Vector2(304, 500), "3"));

        singuities.add(new Singuity(new Vector2(2200, 2300), "4"));
        singuities.add(new Singuity(new Vector2(2210, 2250), "5"));
        singuities.add(new Singuity(new Vector2(2119, 2200), "6"));
        singuities.add(new Singuity(new Vector2(2300, 2500), "7"));

        SinguityDensityBasedScan singuityDensityBasedScan = new SinguityDensityBasedScan(singuities);
        List<SinguityCluster> singuityClusters = singuityDensityBasedScan.findClusters(400);

        assert(singuityClusters.size() == 2);
        assert(singuityClusters.get(0).singuityIds.size() == 4);
        assert(singuityClusters.get(1).singuityIds.size() == 4);
    }

    @Test
    public void A1500QueriesForPerformanceAnalysis() {
        List<Singuity> singuities = new ArrayList<>();

        IntStream.range(0, 1500).forEach(i -> {
            singuities.add(new Singuity(new Vector2(Math.random()*10000, Math.random()*10000), String.valueOf(i)));
        });

        SinguityDensityBasedScan singuityDensityBasedScan = new SinguityDensityBasedScan(singuities);

        long x1 = System.currentTimeMillis();
        AtomicReference<List<SinguityCluster>> atomicReference = new AtomicReference<>();
        IntStream.range(0, 1000).forEach(i -> {
            atomicReference.set(singuityDensityBasedScan.findClusters(1000));
        });
        long x2 = System.currentTimeMillis();

        assert(true);
    }
}
