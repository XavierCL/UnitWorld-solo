package clientAis.dynamic_data;

import clientAis.communications.game_data.GameState;
import clientAis.communications.game_data.Singuity;
import clientAis.communications.game_data.Spawner;
import utils.data_structure.cluster.DataCluster;
import utils.data_structure.cluster.DensityBasedScan;
import utils.game_data_resources.SinguityResourceHandler;

import java.util.*;
import java.util.stream.Collectors;

public class DataPacket {

    public static final double DENSITY_SCAN_RADIUS = 20;

    public static final SinguityResourceHandler singuityResourceHandler = new SinguityResourceHandler();

    public final GameState gameState;
    public final String playerId;

    public final Map<String, Singuity> singuityIdMap;
    public final Map<String, Spawner> spawnerIdMap;

    public final Set<String> ownedSinguities;
    public final Set<String> adverseSinguities;
    public final Set<String> allSinguities;

    public final Set<String> ownedSpawners;
    public final Set<String> adverseSpawners;
    public final Set<String> allSpawners;
    public final Set<String> freeSpawners;
    public final Set<String> attackableSpawners;

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
        gameState.singuities
                .stream()
                .filter(Objects::nonNull)
                .forEach(singuity -> singuityIdMap.put(singuity.id, singuity));
        this.spawnerIdMap = new HashMap<>();
        gameState.spawners.forEach(spawner -> spawnerIdMap.put(spawner.id, spawner));

        // setup singuities
        this.ownedSinguities = gameState.singuities.stream()
                .filter(Objects::nonNull)
                .filter(singuity -> singuity.playerId.equals(currentPlayerId))
                .map(singuity -> singuity.id)
                .collect(Collectors.toSet());
        this.allSinguities = gameState.singuities.stream()
                .filter(Objects::nonNull)
                .map(singuity -> singuity.id)
                .collect(Collectors.toSet());
        this.adverseSinguities = allSinguities.stream()
                .filter(Objects::nonNull)
                .filter(singuity -> !ownedSinguities.contains(singuity))
                .collect(Collectors.toSet());

        // singuity resource handler
        DataPacket.singuityResourceHandler.actualize(ownedSinguities);

        // setup spawners
        this.ownedSpawners = gameState.spawners.stream()
                .filter(spawner -> spawner.allegence.map(spawnerAllegence ->
                spawnerAllegence.isClaimed && spawnerAllegence.playerId.equals(currentPlayerId))
                        .orElse(false))
                .map(spawner -> spawner.id)
                .collect(Collectors.toSet());
        this.allSpawners = gameState.spawners.stream()
                .map(spawner -> spawner.id)
                .collect(Collectors.toSet());
        this.adverseSpawners = gameState.spawners.stream()
                .filter(spawner -> spawner.allegence.map(spawnerAllegence ->
                        spawnerAllegence.isClaimed && !spawnerAllegence.playerId.equals(currentPlayerId))
                        .orElse(false))
                .map(spawner -> spawner.id)
                .collect(Collectors.toSet());
        this.freeSpawners = allSpawners.stream()
                .filter(spawner -> !ownedSpawners.contains(spawner)
                        && !adverseSpawners.contains(spawner))
                .collect(Collectors.toSet());
        this.attackableSpawners = new HashSet<>();
        attackableSpawners.addAll(adverseSpawners);
        attackableSpawners.addAll(freeSpawners);


        // setup clusters
        final DensityBasedScan<Singuity, String> ownedSinguityDensityScanner = new DensityBasedScan<>(ownedSinguities.stream()
                .map(singuityIdMap::get)
                .collect(Collectors.toList()),
                singuity -> singuity.position,
                singuity -> singuity.id);
        this.ownedClusters = ownedSinguityDensityScanner.query(DENSITY_SCAN_RADIUS);
        final DensityBasedScan<Singuity, String> adverseSinguityDensityScanner = new DensityBasedScan<>(adverseSinguities.stream()
                .map(singuityIdMap::get)
                .collect(Collectors.toList()),
                singuity -> singuity.position,
                singuity -> singuity.id);
        this.adverseClusters = adverseSinguityDensityScanner.query(DENSITY_SCAN_RADIUS);
        this.allClusters = new HashSet<>(ownedClusters);
        allClusters.addAll(adverseClusters);
        //previousInput.ifPresent(this::computeClusterIds);
    }

    /*
    private void computeClusterIds(DataPacket previousInput) {
        Map<DataCluster<String>, DataCluster<String>> potentialClusterBijections = new HashMap<>();
        Map<String, DataCluster<String>> singuityIdToClusterMap = new HashMap<>();
        Map<String, DataCluster<String>> singuityIdToPreviousClusterMap = new HashMap<>();

        // current map
        allSinguities.forEach(id -> allClusters.stream()
                .filter(cluster -> cluster.elements.contains(id))
                .forEach(cluster -> singuityIdToClusterMap.put(id, cluster)));

        // previous map
        previousInput.allSinguities.forEach(id -> previousInput.allClusters.stream()
                .filter(cluster -> cluster.elements.contains(id))
                .forEach(cluster -> singuityIdToPreviousClusterMap.put(id, cluster)));

        // map between clusters
        singuityIdToClusterMap.keySet().forEach(currentId -> {
            final DataCluster<String> currentCluster = singuityIdToClusterMap.get(currentId);
            final DataCluster<String> previousCluster = singuityIdToPreviousClusterMap.get(currentId);
            if(previousCluster != null) {
                potentialClusterBijections.put(currentCluster, previousCluster);
            }
        });
    }*/
}
