package utils.data_structure.cluster_singuity;

import clientAis.communications.game_data.Singuity;
import utils.data_structure.bah_singuity.BoundingAreaHierarchy;
import utils.shape.Circle;

import java.util.*;

public class SinguityDensityBasedScan {

    private final BoundingAreaHierarchy boundingAreaHierarchy;
    private final List<Singuity> singuities;

    public SinguityDensityBasedScan(final List<Singuity> singuities) {
        boundingAreaHierarchy = new BoundingAreaHierarchy(singuities);
        this.singuities = singuities;
    }

    public List<SinguityCluster> findClusters(final double radiusSize) {
        final List<SinguityCluster> singuityClusters = new ArrayList<>();
        final Map<Singuity, List<Singuity>> singuityScanMap = new HashMap<>();
        singuities.forEach(singuity -> {
            List<Singuity> nearEnoughSinguities = boundingAreaHierarchy.query(new Circle(singuity.position, radiusSize));
            singuityScanMap.put(singuity, nearEnoughSinguities);
        });
        final Set<Singuity> remainingSinguities = new HashSet<>(singuityScanMap.keySet());

        while(!remainingSinguities.isEmpty()) {
            final Singuity firstValidSinguity = remainingSinguities.stream()
                    .findFirst()
                    .get();
            singuityClusters.add(new SinguityCluster(new HashSet<>()));
            final SinguityCluster lastCluster = singuityClusters.get(singuityClusters.size()-1);

            fillCluster(lastCluster, firstValidSinguity, remainingSinguities, singuityScanMap);
        }

        return singuityClusters;
    }

    private void fillCluster(final SinguityCluster cluster, final Singuity singuityToAdd, final Set<Singuity> remainingSinguities, final Map<Singuity, List<Singuity>> singuityScanMap) {
        cluster.singuityIds.add(singuityToAdd.id);
        remainingSinguities.remove(singuityToAdd);
        singuityScanMap.get(singuityToAdd).forEach(singuity -> {
            if(remainingSinguities.contains(singuity)) {
                fillCluster(cluster, singuity, remainingSinguities, singuityScanMap);
            }
        });
    }
}
