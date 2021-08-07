package utils.data_structure.cluster;

import utils.data_structure.bounding_area_hierarchy.query_removal.BAHQueryRemoval;
import utils.math.vector.Vector2;
import utils.shape.Circle;

import java.util.*;
import java.util.function.Function;

public class DensityBasedScan<T, ID> {

    private final BAHQueryRemoval<T> bahQueryRemoval;
    private final List<T> elements;
    private final Function<T, Vector2> elementPositionMapper;
    private final Function<T, ID> elementIdMapper;

    private double scanRadiusSize;

    public DensityBasedScan(final List<T> elements, Function<T, Vector2> elementPositionMapper, Function<T, ID> elementIdMapper) {
        bahQueryRemoval = new BAHQueryRemoval<>(elements, elementPositionMapper);
        this.elements = elements;
        this.elementPositionMapper = elementPositionMapper;
        this.elementIdMapper = elementIdMapper;
    }

    public Set<DataCluster<ID>> findClusters(final double radiusSize) {
        this.scanRadiusSize = radiusSize;
        final List<DataCluster<ID>> dataClusters = new ArrayList<>();
        final Set<T> remainingSinguities = new LinkedHashSet<>(elements);

        while(!remainingSinguities.isEmpty()) {
            final T firstValid = remainingSinguities.stream()
                    .findFirst()
                    .get();
            dataClusters.add(new DataCluster<>(new HashSet<>()));
            final DataCluster<ID> lastCluster = dataClusters.get(dataClusters.size()-1);

            fillCluster(lastCluster, firstValid, remainingSinguities);
        }

        return new HashSet<>(dataClusters);
    }

    private void fillCluster(final DataCluster<ID> cluster, final T elementToAdd, final Set<T> remainingElements) {
        final Set<T> remainingElementsToAddInCluster = new LinkedHashSet<>();
        remainingElementsToAddInCluster.add(elementToAdd);

        while(!remainingElementsToAddInCluster.isEmpty()) {
            final T firstElement = remainingElementsToAddInCluster.stream().findFirst().get();
            cluster.elements.add(elementIdMapper.apply(firstElement));
            final List<T> nearEnoughSinguities = bahQueryRemoval.query(new Circle(elementPositionMapper.apply(firstElement), scanRadiusSize));
            nearEnoughSinguities.stream()
                    .filter(remainingElements::contains)
                    .forEach(remainingElementsToAddInCluster::add);

            remainingElements.remove(firstElement);
            remainingElementsToAddInCluster.remove(firstElement);
        }
    }

    /*
    private void fillCluster(final DataCluster<ID> cluster, final T elementToAdd, final Set<T> remainingElements, final Map<T, List<T>> elementScanMap) {
        cluster.elements.add(elementIdMapper.apply(elementToAdd));
        remainingElements.remove(elementToAdd);
        elementScanMap.get(elementToAdd).forEach(element -> {
            if(remainingElements.contains(element)) {
                fillCluster(cluster, element, remainingElements, elementScanMap);
            }
        });
    }
    */
}
