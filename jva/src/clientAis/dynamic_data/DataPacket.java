package clientAis.dynamic_data;

import clientAis.communications.game_data.GameState;
import clientAis.communications.game_data.Singuity;
import clientAis.communications.game_data.Spawner;
import utils.data_structure.cluster.DataCluster;
import utils.data_structure.cluster.DensityBasedScan;

import java.util.*;
import java.util.stream.Collectors;

public class DataPacket {

    public static final double DENSITY_SCAN_RADIUS = 20;

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
        gameState.singuities.forEach(singuity -> singuityIdMap.put(singuity.id, singuity));
        this.spawnerIdMap = new HashMap<>();
        gameState.spawners.forEach(spawner -> spawnerIdMap.put(spawner.id, spawner));

        // setup singuities
        this.ownedSinguities = gameState.singuities.stream()
                .filter(singuity -> singuity.playerId.equals(currentPlayerId))
                .map(singuity -> singuity.id)
                .collect(Collectors.toSet());
        this.allSinguities = gameState.singuities.stream()
                .map(singuity -> singuity.id)
                .collect(Collectors.toSet());
        this.adverseSinguities = allSinguities.stream()
                .filter(singuity -> !ownedSinguities.contains(singuity))
                .collect(Collectors.toSet());

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
        this.ownedClusters = ownedSinguityDensityScanner.findClusters(DENSITY_SCAN_RADIUS);
        final DensityBasedScan<Singuity, String> adverseSinguityDensityScanner = new DensityBasedScan<>(adverseSinguities.stream()
                .map(singuityIdMap::get)
                .collect(Collectors.toList()),
                singuity -> singuity.position,
                singuity -> singuity.id);
        this.adverseClusters = adverseSinguityDensityScanner.findClusters(DENSITY_SCAN_RADIUS);
    }
}
