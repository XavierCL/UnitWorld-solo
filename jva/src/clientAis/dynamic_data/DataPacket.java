package clientAis.dynamic_data;

import clientAis.communications.game_data.GameState;
import clientAis.communications.game_data.Singuity;
import clientAis.communications.game_data.Spawner;
import utils.data_structure.cluster.DataCluster;
import utils.data_structure.cluster.DensityBasedScan;
import utils.data_structure.tupple.Tuple3;
import utils.unit_world.game_data_resources.MemoryResource;
import utils.unit_world.game_data_resources.ResourceHandler;

import java.util.*;
import java.util.stream.Collectors;

public class DataPacket {

    public static final double DENSITY_SCAN_RADIUS = 20;

    public static final MemoryResource<String> singuityResourceHandler = new ResourceHandler<>();
    public static final MemoryResource<String> ownedSpawnerResourceHandler = new ResourceHandler<>();

    public final GameState gameState;
    public final String playerId;

    public final Map<String, Singuity> singuityIdMap;
    public final Map<String, Spawner> spawnerIdMap;

    public final Set<String> ownedSinguities;
    public final Set<String> adverseSinguities;
    public final Set<String> allSinguities;
    public final Set<String> newOwnedSinguities;
    public final Set<String> deadOwnedSinguities;

    public final Set<String> ownedSpawners;
    public final Set<String> newOwnedSpawners;
    public final Set<String> deadOwnedSpawners;
    public final Set<String> adverseSpawners;
    public final Set<String> allSpawners;
    public final Set<String> freeSpawners;
    public final Set<String> attackableSpawners;

    public final Map<String, DataCluster<String>> clusterIdMap;

    public final Set<DataCluster<String>> ownedClusters;
    public final Set<DataCluster<String>> adverseClusters;
    public final Set<DataCluster<String>> allClusters;

    public Optional<DataPacket> previousInput;

    public DataPacket(final GameState gameState, final String currentPlayerId, Optional<DataPacket> previousInputOpt) {
        this.gameState = gameState;
        this.playerId = currentPlayerId;

        // save previous state and clear the 2nd previous state to avoid going out of memory
        this.previousInput = previousInputOpt;
        previousInput.ifPresent(previousInput -> {
            previousInput.previousInput = Optional.empty();
        });

        // setup id maps
        this.singuityIdMap = new HashMap<>();
        gameState.singuities.stream()
                .filter(Objects::nonNull)
                .forEach(singuity -> singuityIdMap.put(singuity.id, singuity));
        this.spawnerIdMap = new HashMap<>();
        gameState.spawners.forEach(spawner -> spawnerIdMap.put(spawner.id, spawner));


        // setup singuities
        // all
        this.allSinguities = gameState.singuities.stream()
                .filter(Objects::nonNull)
                .map(singuity -> singuity.id)
                .collect(Collectors.toSet());
        // owned
        this.ownedSinguities = gameState.singuities.stream()
                .filter(Objects::nonNull)
                .filter(singuity -> singuity.playerId.equals(currentPlayerId))
                .map(singuity -> singuity.id)
                .collect(Collectors.toSet());
        // new owned
        this.newOwnedSinguities = new HashSet<>();
        previousInputOpt.ifPresentOrElse(previousInput -> ownedSinguities.stream()
                .filter(ownedSinguity -> !previousInput.ownedSinguities.contains(ownedSinguity))
                .forEach(newOwnedSinguities::add), () -> newOwnedSinguities.addAll(ownedSinguities));
        // dead owned
        this.deadOwnedSinguities = new HashSet<>();
        previousInputOpt.ifPresent(previousInput -> previousInput.ownedSinguities.stream()
                .filter(previousOwnedSinguities -> !ownedSinguities.contains(previousOwnedSinguities))
                .forEach(deadOwnedSinguities::add));
        // adverse
        this.adverseSinguities = allSinguities.stream()
                .filter(Objects::nonNull)
                .filter(singuity -> !ownedSinguities.contains(singuity))
                .collect(Collectors.toSet());

        // singuity resource handler
        DataPacket.singuityResourceHandler.actualize(ownedSinguities);

        // setup spawners
        // all
        this.allSpawners = gameState.spawners.stream()
                .map(spawner -> spawner.id)
                .collect(Collectors.toSet());
        // owned
        this.ownedSpawners = gameState.spawners.stream()
                .filter(spawner -> spawner.allegence.map(spawnerAllegence ->
                spawnerAllegence.isClaimed && spawnerAllegence.playerId.equals(currentPlayerId))
                        .orElse(false))
                .map(spawner -> spawner.id)
                .collect(Collectors.toSet());
        // new owned
        this.newOwnedSpawners = new HashSet<>();
        previousInputOpt.ifPresentOrElse(previousInput -> ownedSpawners.stream()
                .filter(ownedSpawner -> !previousInput.ownedSpawners.contains(ownedSpawner))
                .forEach(newOwnedSpawners::add), () -> newOwnedSpawners.addAll(ownedSpawners));
        // dead owned
        this.deadOwnedSpawners = new HashSet<>();
        previousInputOpt.ifPresent(previousInput -> previousInput.ownedSpawners.stream()
                .filter(previousOwnedSpawner -> !ownedSpawners.contains(previousOwnedSpawner))
                .forEach(deadOwnedSpawners::add));
        // adverse
        this.adverseSpawners = gameState.spawners.stream()
                .filter(spawner -> spawner.allegence.map(spawnerAllegence ->
                        spawnerAllegence.isClaimed && !spawnerAllegence.playerId.equals(currentPlayerId))
                        .orElse(false))
                .map(spawner -> spawner.id)
                .collect(Collectors.toSet());
        // free
        this.freeSpawners = allSpawners.stream()
                .filter(spawner -> !ownedSpawners.contains(spawner)
                        && !adverseSpawners.contains(spawner))
                .collect(Collectors.toSet());
        // attackable
        this.attackableSpawners = new HashSet<>();
        this.attackableSpawners.addAll(adverseSpawners);
        this.attackableSpawners.addAll(freeSpawners);

        // owned spawner resource handler
        DataPacket.ownedSpawnerResourceHandler.actualize(ownedSpawners);

        // setup clusters
        final DensityBasedScan<String> ownedSinguityDensityScanner = new DensityBasedScan<>(
                new ArrayList<>(ownedSinguities),
                id -> singuityIdMap.get(id).position);
        this.ownedClusters = ownedSinguityDensityScanner.query(DENSITY_SCAN_RADIUS);
        final DensityBasedScan<String> adverseSinguityDensityScanner = new DensityBasedScan<>(
                new ArrayList<>(adverseSinguities),
                id -> singuityIdMap.get(id).position);
        this.adverseClusters = adverseSinguityDensityScanner.query(DENSITY_SCAN_RADIUS);
        this.allClusters = new HashSet<>(ownedClusters);
        allClusters.addAll(adverseClusters);
        previousInput.ifPresentOrElse(previousInput -> updateClusterIds(allClusters, previousInput.allClusters), () -> initClusterIds(allClusters));

        this.clusterIdMap = new HashMap<>();
        allClusters.forEach(cluster -> clusterIdMap.put(cluster.id, cluster));
    }

    private void initClusterIds(final Set<DataCluster<String>> singuityClusters) {
        singuityClusters.forEach(singuityCluster -> singuityCluster.id = UUID.randomUUID().toString());
    }

    private void updateClusterIds(final Set<DataCluster<String>> singuityClusters, final Set<DataCluster<String>> previousSinguityClusters) {
        final Set<String> availableClusterIds = previousSinguityClusters.stream()
                .map(cluster -> cluster.id)
                .collect(Collectors.toSet());
        var referenceCounterObjects = generateClustersReferenceCount(previousSinguityClusters).stream()
                .sorted(Comparator.comparingInt(clusterReferenceCount -> clusterReferenceCount.value3))
                .collect(Collectors.toList());
        Collections.reverse(referenceCounterObjects);
        referenceCounterObjects.forEach(referenceCounterObject -> {
            if(availableClusterIds.contains(referenceCounterObject.value2)) {   // if the reference hasn't been taken yet
                referenceCounterObject.value1.id = referenceCounterObject.value2;   // set the id of the cluster
                availableClusterIds.remove(referenceCounterObject.value2);          // "take" the reference so we can't use it twice
            }
        });

        // generate new uuids for the somewhat "new" clusters that have no ids yet
        singuityClusters.stream()
                .filter(singuityCluster -> singuityCluster.id.equals(""))
                .forEach(singuityCluster -> singuityCluster.id = UUID.randomUUID().toString());
    }

    private Set<Tuple3<DataCluster<String>, String, Integer>> generateClustersReferenceCount(
            final Set<DataCluster<String>> previousSinguityClusters) {
        // <cluster that needs its id to be set, one such possible id, amount of times this possible id was referenced>
        final Set<Tuple3<DataCluster<String>, String, Integer>> clustersReferenceCount = new HashSet<>();

        allSinguities.forEach(singuity -> {
            previousSinguityClusters.forEach(previousCluster -> {
                if(previousCluster.elements.contains(singuity)) {
                    final Optional<Tuple3<DataCluster<String>, String, Integer>> clusterReferenceCountOpt = clustersReferenceCount.stream()
                            .filter(clusterReferenceCount -> clusterReferenceCount.value2.equals(previousCluster.id))
                            .findFirst();
                    clusterReferenceCountOpt.ifPresentOrElse(clusterReferenceCount -> clusterReferenceCount.value3++,
                            () -> clustersReferenceCount.add(new Tuple3<>(
                                    findClusterInWhichSinguityBelongs(singuity),
                                    previousCluster.id,
                                    1)));
                }
            });
        });

        return clustersReferenceCount;
    }

    private DataCluster<String> findClusterInWhichSinguityBelongs(final String singuity) {
        return allClusters.stream()
                .filter(cluster -> cluster.elements.contains(singuity))
                .findFirst().get();
    }
}
