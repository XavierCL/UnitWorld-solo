package utils.data_structure.cluster;

public class ClusterBijectionCounter<T> {

    public final DataCluster<T> currentCluster;
    public final DataCluster<T> previousCluster;
    public int recurrence;

    public ClusterBijectionCounter(DataCluster<T> currentCluster, DataCluster<T> previousCluster) {
        this.currentCluster = currentCluster;
        this.previousCluster = previousCluster;
        this.recurrence = 1;
    }

    public void increment() {
        recurrence++;
    }
}
