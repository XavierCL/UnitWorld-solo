package dbscan_singuity;

import clientAis.communications.game_data.Singuity;
import org.junit.jupiter.api.Test;
import utils.data_structure.cluster.DataCluster;
import utils.data_structure.cluster.DensityBasedScan;
import utils.math.vector.Vector2;

import java.util.*;
import java.util.concurrent.atomic.AtomicReference;
import java.util.stream.Collectors;
import java.util.stream.IntStream;

public class DensityBasedScanTest {

    @Test
    public void ScanRunsAndDoesn_tCrash() {
        List<Singuity> singuities = new ArrayList<>();
        Map<String, Singuity> singuityMap = new HashMap<>();
        singuities.add(new Singuity(new Vector2(200, 300), "0"));
        singuityMap.put(singuities.get(0).id, singuities.get(0));

        DensityBasedScan<String> densityBasedScan = new DensityBasedScan<>(
                singuities.stream().map(singuity -> singuity.id).collect(Collectors.toList()),
                id -> singuityMap.get(id).position);
        Set<DataCluster<String>> dataClusters = densityBasedScan.query(10d);

        assert(dataClusters.stream().findFirst().get().elements.size() == 1);
    }

    @Test
    public void ScanDetectsTwoClustersAndTheirSizesAreValid() {
        List<Singuity> singuities = new ArrayList<>();
        Map<String, Singuity> singuityMap = new HashMap<>();
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

        for(final Singuity singuity: singuities) {
            singuityMap.put(singuity.id, singuity);
        }

        DensityBasedScan<String> densityBasedScan = new DensityBasedScan<>(
                singuities.stream().map(singuity -> singuity.id).collect(Collectors.toList()),
                id -> singuityMap.get(id).position);
        Set<DataCluster<String>> dataClusters = densityBasedScan.query(400d);

        assert(dataClusters.size() == 3);
        dataClusters.forEach(stringDataCluster -> {
            assert(stringDataCluster.elements.size() == 4);
        });
    }

    @Test
    public void A1500QueriesForPerformanceAnalysis() {
        List<Singuity> singuities = new ArrayList<>();
        Map<String, Singuity> singuityMap = new HashMap<>();

        IntStream.range(0, 1500).forEach(i -> {
            singuities.add(new Singuity(new Vector2(Math.random()*10000, Math.random()*10000), String.valueOf(i)));
        });

        for(final Singuity singuity: singuities) {
            singuityMap.put(singuity.id, singuity);
        }

        DensityBasedScan<String> densityBasedScan = new DensityBasedScan<>(
                singuities.stream().map(singuity -> singuity.id).collect(Collectors.toList()),
                id -> singuityMap.get(id).position);

        long x1 = System.currentTimeMillis();
        AtomicReference<Set<DataCluster<String>>> atomicReference = new AtomicReference<>();
        IntStream.range(0, 1000).forEach(i -> {
            atomicReference.set(densityBasedScan.query(1000d));
        });
        long x2 = System.currentTimeMillis();

        assert(true);
    }
}
