from typing import Tuple

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

        oldClusterToNewCluster = np.ones(np.max(lastClusters) + 1, np.int32) * -1
        nextNewClusterId = np.max(singuityClusterIds) + 1

        for oldClusterId in range(np.max(lastClusters) + 1):
            oldClusterNewIds = singuityClusterIds[lastClusters == oldClusterId]
            oldClusterNewIds = oldClusterNewIds[oldClusterNewIds != -1]

            # In case no units in the old cluster are in a new cluster, create a new cluster for those
            if len(oldClusterNewIds) == 0:
                oldClusterToNewCluster[oldClusterId] = nextNewClusterId
                nextNewClusterId += 1
                continue

            uniqueNewClusters, counts = np.unique(oldClusterNewIds, return_counts=True)
            maxCountIndex = np.argmax(counts)
            modeNewCluster = uniqueNewClusters[maxCountIndex]
            oldClusterToNewCluster[oldClusterId] = modeNewCluster

        postClusters = assign(singuityClusterIds, lastClusters[lastClusters != -1], oldClusterToNewCluster[lastClusters[lastClusters != -1]])

        _, mapping = toNaturals(postClusters[postClusters != -1], returnMapping=True)
        reducedPostClusters = np.where(postClusters != -1, -1 if len(mapping) == 0 else mapping[postClusters], -1)

        uniqueLastClusters = np.unique(lastClusters[lastClusters != -1])
        oldClusterMapping = assign(
            np.ones(0 if len(uniqueLastClusters) == 0 else np.max(uniqueLastClusters) + 1, dtype=np.int32) * -1, uniqueLastClusters,
            mapping[oldClusterToNewCluster[uniqueLastClusters]]
            )

        self.lastIds = ids
        self.lastClusters = reducedPostClusters

        return oldClusterMapping, self.lastClusters
