package clientAis.dynamic_data;

import clientAis.communications.game_data.GameState;
import clientAis.communications.game_data.Singuity;
import clientAis.communications.game_data.Spawner;
import utils.data_structure.cluster_singuity.SinguityCluster;
import utils.data_structure.cluster_singuity.SinguityDensityBasedScan;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.stream.Collectors;

public class DataPacket {

    public static final double DENSITY_SCAN_RADIUS = 20;

    public final GameState gameState;

    public final Map<String, Singuity> singuityIdMap;
    public final Map<String, Spawner> spawnerIdMap;

    public final List<String> ownedSinguities;
    public final List<String> adverseSinguities;
    public final List<String> allSinguities;

    public final List<String> ownedSpawners;
    public final List<String> adverseSpawners;
    public final List<String> allSpawners;
    public final List<String> freeSpawners;

    public final List<SinguityCluster> ownedClusters;
    public final List<SinguityCluster> adverseClusters;

    public DataPacket(final GameState gameState, final String currentPlayerId) {
        this.gameState = gameState;

        this.singuityIdMap = new HashMap<>();
        gameState.singuities.forEach(singuity -> singuityIdMap.put(singuity.id, singuity));
        this.spawnerIdMap = new HashMap<>();
        gameState.spawners.forEach(spawner -> spawnerIdMap.put(spawner.id, spawner));

        this.ownedSinguities = gameState.singuities.stream()
                .filter(singuity -> singuity.playerId.equals(currentPlayerId))
                .map(singuity -> singuity.id)
                .collect(Collectors.toList());
        this.allSinguities = gameState.singuities.stream()
                .map(singuity -> singuity.id)
                .collect(Collectors.toList());
        this.adverseSinguities = allSinguities.stream()
                .filter(singuity -> !ownedSinguities.contains(singuity))
                .collect(Collectors.toList());

        this.ownedSpawners = gameState.spawners.stream()
                .filter(spawner -> spawner.allegence.map(spawnerAllegence ->
                spawnerAllegence.isClaimed && spawnerAllegence.playerId.equals(currentPlayerId))
                        .orElse(false))
                .map(spawner -> spawner.id)
                .collect(Collectors.toList());
        this.allSpawners = gameState.spawners.stream()
                .map(spawner -> spawner.id)
                .collect(Collectors.toList());
        this.adverseSpawners = gameState.spawners.stream()
                .filter(spawner -> spawner.allegence.map(spawnerAllegence ->
                        spawnerAllegence.isClaimed && !spawnerAllegence.playerId.equals(currentPlayerId))
                        .orElse(false))
                .map(spawner -> spawner.id)
                .collect(Collectors.toList());
        this.freeSpawners = allSpawners.stream()
                .filter(spawner -> !ownedSpawners.contains(spawner)
                        && !adverseSpawners.contains(spawner))
                .collect(Collectors.toList());

        SinguityDensityBasedScan ownedSinguityDensityScanner = new SinguityDensityBasedScan(ownedSinguities.stream()
                .map(singuityIdMap::get)
                .collect(Collectors.toList()));
        this.ownedClusters = ownedSinguityDensityScanner.findClusters(DENSITY_SCAN_RADIUS);

        SinguityDensityBasedScan adverseSinguityDensityScanner = new SinguityDensityBasedScan(adverseSinguities.stream()
                .map(singuityIdMap::get)
                .collect(Collectors.toList()));
        this.adverseClusters = ownedSinguityDensityScanner.findClusters(DENSITY_SCAN_RADIUS);
    }
}
