package utils.data_structure.cluster;

import utils.data_structure.bounding_area_hierarchy.BoundingAreaHierarchy;
import utils.math.vector.Vector2;
import utils.shape.Circle;

import java.util.*;
import java.util.function.Function;

public class DensityBasedScan<T, ID> {

    private final BoundingAreaHierarchy<T> boundingAreaHierarchy;
    private final List<T> elements;
    private final Function<T, Vector2> elementPositionMapper;
    private final Function<T, ID> elementIdMapper;

    public DensityBasedScan(final List<T> elements, Function<T, Vector2> elementPositionMapper, Function<T, ID> elementIdMapper) {
        boundingAreaHierarchy = new BoundingAreaHierarchy<>(elements, elementPositionMapper);
        this.elements = elements;
        this.elementPositionMapper = elementPositionMapper;
        this.elementIdMapper = elementIdMapper;
    }

    public Set<DataCluster<ID>> findClusters(final double radiusSize) {
        final List<DataCluster<ID>> dataClusters = new ArrayList<>();
        final Map<T, List<T>> ScanMap = new HashMap<>();
        elements.forEach(element -> {
            List<T> nearEnoughSinguities = boundingAreaHierarchy.query(new Circle(elementPositionMapper.apply(element), radiusSize));
            ScanMap.put(element, nearEnoughSinguities);
        });
        final Set<T> remainingSinguities = new HashSet<>(ScanMap.keySet());

        while(!remainingSinguities.isEmpty()) {
            final T firstValid = remainingSinguities.stream()
                    .findFirst()
                    .get();
            dataClusters.add(new DataCluster<>(new HashSet<>()));
            final DataCluster<ID> lastCluster = dataClusters.get(dataClusters.size()-1);

            fillCluster(lastCluster, firstValid, remainingSinguities, ScanMap);
        }

        return new HashSet<>(dataClusters);
    }

    private void fillCluster(final DataCluster<ID> cluster, final T elementToAdd, final Set<T> remainingElements, final Map<T, List<T>> elementScanMap) {
        cluster.elements.add(elementIdMapper.apply(elementToAdd));
        remainingElements.remove(elementToAdd);
        elementScanMap.get(elementToAdd).forEach(element -> {
            if(remainingElements.contains(element)) {
                fillCluster(cluster, element, remainingElements, elementScanMap);
            }
        });
    }
}
