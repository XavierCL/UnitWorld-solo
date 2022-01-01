package clientAis.dynamic_data;

import clientAis.communications.game_data.GameState;
import clientAis.communications.game_data.Singuity;
import clientAis.communications.game_data.Spawner;
import utils.data_structure.cluster.DataCluster;
import utils.data_structure.cluster.DensityBasedScan;
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
        this.newOwnedSinguities = new HashSet<>(ownedSinguities);
        previousInputOpt.ifPresent(dataPacket -> this.newOwnedSinguities.removeAll(dataPacket.ownedSinguities));
        // dead owned
        this.deadOwnedSinguities = new HashSet<>(previousInputOpt.map(dataPacket -> dataPacket.ownedSinguities)
                .orElse(new HashSet<>()));
        previousInputOpt.ifPresent(dataPacket -> this.deadOwnedSinguities.removeAll(ownedSinguities));
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
        this.newOwnedSpawners = new HashSet<>(ownedSpawners);
        previousInputOpt.ifPresent(dataPacket -> this.ownedSpawners.removeAll(dataPacket.ownedSpawners));
        // dead owned
        this.deadOwnedSpawners = new HashSet<>(previousInputOpt.map(dataPacket -> dataPacket.ownedSpawners).orElse(new HashSet<>()));
        previousInputOpt.ifPresent(dataPacket -> this.deadOwnedSpawners.removeAll(dataPacket.ownedSpawners));
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
        //previousInput.ifPresent(this::computeClusterIds);
    }
}
