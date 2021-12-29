from typing import Any, List, Tuple, TypeVar

import numpy as np
from sklearn.cluster import DBSCAN

from utils.arrays import assign, toNaturals

class Clusterer:
    def __init__(self):
        self.lastIds = np.empty(0)
        self.lastClusters = np.empty(0)

    # Returns [old cluster mapping, singuity clusters]
    def cluster(self, ids: np.ndarray, positions: np.ndarray) -> Tuple[np.ndarray, np.ndarray]:
        if len(ids) == 0:
            self.lastIds = ids
            self.lastClusters = np.empty(0, dtype=np.int32)
            return np.empty(0, dtype=np.int32), np.empty(0, dtype=np.int32)

        idByIndex = {id: index for index, id in enumerate(ids)}
        oldIdMapping = np.array([idByIndex.get(id, -1) for id in self.lastIds], dtype=np.int32)
        lastClusters = assign(np.ones(len(ids), dtype=np.int32) * -1, oldIdMapping[oldIdMapping != -1], self.lastClusters[oldIdMapping != -1])
        singuityClusterIds = DBSCAN(eps=100, min_samples=25).fit_predict(positions)

        newClusterToOldCluster = np.ones(np.max(singuityClusterIds) + 1, np.int32) * -1

        for clusterId in range(np.max(lastClusters) + 1):
            oldClusterNewIds = singuityClusterIds[lastClusters == clusterId]
            oldClusterNewIds = oldClusterNewIds[oldClusterNewIds != -1]

            # In case old cluster is no more, don't compute it
            if len(oldClusterNewIds) == 0:
                continue

            uniqueNewClusters, counts = np.unique(oldClusterNewIds, return_counts=True)
            maxCountIndex = np.argmax(counts)
            modeNewCluster = uniqueNewClusters[maxCountIndex]
            newClusterToOldCluster[modeNewCluster] = clusterId

        newClusterToOldCluster[newClusterToOldCluster == -1] = np.arange(np.count_nonzero(newClusterToOldCluster == -1)) + np.max(lastClusters) + 1
        postClusters = assign(np.where(singuityClusterIds != -1, -1 if len(newClusterToOldCluster) == 0 else newClusterToOldCluster[singuityClusterIds], -1), lastClusters != -1, lastClusters[lastClusters != -1])

        _, mapping = toNaturals(postClusters[postClusters != -1], returnMapping=True)
        reducedPostClusters = np.where(postClusters != -1, mapping[postClusters], -1)

        uniqueLastClusters = np.unique(lastClusters[lastClusters != -1])
        oldClusterMapping = assign(np.ones(0 if len(uniqueLastClusters) == 0 else np.max(uniqueLastClusters) + 1, dtype=np.int32) * -1, uniqueLastClusters, mapping[uniqueLastClusters])

        self.lastIds = ids
        self.lastClusters = reducedPostClusters

        if np.min(self.lastClusters) > 0:
            temp1 = 0

        return oldClusterMapping, self.lastClusters
