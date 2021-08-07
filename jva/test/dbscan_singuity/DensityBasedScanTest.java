package dbscan_singuity;

import clientAis.communications.game_data.Singuity;
import org.junit.jupiter.api.Test;
import utils.data_structure.cluster.DataCluster;
import utils.data_structure.cluster.DensityBasedScan;
import utils.math.vector.Vector2;

import java.util.ArrayList;
import java.util.List;
import java.util.Set;
import java.util.concurrent.atomic.AtomicReference;
import java.util.stream.IntStream;

public class DensityBasedScanTest {

    @Test
    public void ScanRunsAndDoesn_tCrash() {
        List<Singuity> singuities = new ArrayList<>();
        singuities.add(new Singuity(new Vector2(200, 300), "0"));

        DensityBasedScan<Singuity, String> densityBasedScan = new DensityBasedScan<>(singuities,
                singuity -> singuity.position,
                singuity -> singuity.id);
        Set<DataCluster<String>> dataClusters = densityBasedScan.findClusters(10);

        assert(dataClusters.stream().findFirst().get().elements.size() == 1);
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

        singuities.add(new Singuity(new Vector2(8200, 2300), "8"));
        singuities.add(new Singuity(new Vector2(8210, 2250), "9"));
        singuities.add(new Singuity(new Vector2(8119, 2200), "10"));
        singuities.add(new Singuity(new Vector2(8300, 2500), "11"));

        DensityBasedScan<Singuity, String> densityBasedScan = new DensityBasedScan<>(singuities,
                singuity -> singuity.position,
                singuity -> singuity.id);
        Set<DataCluster<String>> dataClusters = densityBasedScan.findClusters(400);

        assert(dataClusters.size() == 3);
        dataClusters.forEach(stringDataCluster -> {
            assert(stringDataCluster.elements.size() == 4);
        });
    }

    @Test
    public void A1500QueriesForPerformanceAnalysis() {
        List<Singuity> singuities = new ArrayList<>();

        IntStream.range(0, 1500).forEach(i -> {
            singuities.add(new Singuity(new Vector2(Math.random()*10000, Math.random()*10000), String.valueOf(i)));
        });

        DensityBasedScan<Singuity, String> densityBasedScan = new DensityBasedScan<>(singuities,
                singuity -> singuity.position,
                singuity -> singuity.id);

        long x1 = System.currentTimeMillis();
        AtomicReference<Set<DataCluster<String>>> atomicReference = new AtomicReference<>();
        IntStream.range(0, 1000).forEach(i -> {
            atomicReference.set(densityBasedScan.findClusters(1000));
        });
        long x2 = System.currentTimeMillis();

        assert(true);
    }
}
