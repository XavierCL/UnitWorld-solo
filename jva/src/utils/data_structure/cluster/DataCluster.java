package utils.data_structure.cluster;

import java.util.Set;

public class DataCluster<T> {

    public final Set<T> elements;
    public String id;

    public DataCluster(Set<T> elements) {
        this.elements = elements;
        this.id = "";
    }
}
