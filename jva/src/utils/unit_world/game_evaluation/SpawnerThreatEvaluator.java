package utils.unit_world.game_evaluation;

import clientAis.communications.game_data.Singuity;
import clientAis.communications.game_data.Spawner;
import utils.data_structure.cluster.DataCluster;
import utils.math.vector.Vector2;

import java.util.HashMap;
import java.util.Map;
import java.util.Optional;
import java.util.Set;
import java.util.concurrent.atomic.AtomicReference;

public class SpawnerThreatEvaluator {

    public static Map<String, Double> evaluateThreatLevels(final Set<Spawner> ownedSpawners, final Set<DataCluster<Singuity>> adverseClusters) {
        final Map<String, Double> threatLevelMap = new HashMap<>();

        ownedSpawners.forEach(spawner -> {
            final AtomicReference<Double> threat = new AtomicReference<>(0d);
            adverseClusters.forEach(adverseCluster -> {
                final Optional<Vector2> centerOfMassOpt = Vector2.centerOfMass(adverseCluster.elements, singuity -> singuity.position);
                centerOfMassOpt.ifPresent(centerOfMass -> {
                    final Double clusterThreat = adverseCluster.elements.size() / spawner.position.minus(centerOfMass).magnitudeSquared();
                    threat.set(threat.get() + clusterThreat);
                });
            });
            threatLevelMap.put(spawner.id, threat.get());
        });

        return threatLevelMap;
    }

    public static Map<String, Double> evaluateThreatRatios(final Set<Spawner> ownedSpawners, final Set<DataCluster<Singuity>> adverseClusters) {
        final Map<String, Double> threatLevels = evaluateThreatLevels(ownedSpawners, adverseClusters);
        final double totalThreat = threatLevels.values().stream()
                .reduce(Double::sum)
                .orElse(0d);

        final Map<String, Double> threatRatios = new HashMap<>(threatLevels);
        threatRatios.replaceAll((spawnerId, threatLevel) -> threatLevel / totalThreat);

        return threatRatios;
    }
}
