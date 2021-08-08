package clientAis.implementations.mindless_chase;

import clientAis.communications.ServerCommander;
import clientAis.dynamic_data.DataPacket;
import clientAis.implementations.Bot;
import utils.data_structure.cluster.DataCluster;
import utils.math.vector.Vector2;

import java.util.Comparator;
import java.util.List;
import java.util.function.Consumer;
import java.util.stream.Collectors;

public class MindlessChase implements Bot {

    @Override
    public Consumer<ServerCommander> exec(DataPacket input) {
        Vector2 stupidFirstSinguityPosition = input.singuityIdMap.get(input.ownedClusters.stream()
                .max(Comparator.comparingInt(cluster -> cluster.elements.size())).get()
                .elements.stream()
                .findFirst().get())
                .position;

        final List<DataCluster<String>> sortedAdverseClusters = input.adverseClusters.stream()
                .sorted(Comparator.comparingDouble(cluster -> input.singuityIdMap.get(cluster.elements.stream().findFirst().get()).position.minus(stupidFirstSinguityPosition).magnitudeSquared()))
                .collect(Collectors.toList());

        final Vector2 closestSpawnerPosition = input.adverseSpawners.stream()
                .map(input.spawnerIdMap::get)
                .min(Comparator.comparingDouble(spawner -> spawner.position.minus(stupidFirstSinguityPosition).magnitudeSquared()))
                .map(spawner -> spawner.position)
                .orElse(stupidFirstSinguityPosition);

        final Vector2 singuityDestination;
        if(closestSpawnerPosition.minus(stupidFirstSinguityPosition).magnitudeSquared() < 60 * 60) {
            singuityDestination = closestSpawnerPosition;
        }
        else if(!sortedAdverseClusters.isEmpty()) {
            final DataCluster<String> targetCluster = sortedAdverseClusters.get(0);
            singuityDestination = input.singuityIdMap.get(targetCluster.elements.stream().findFirst().get()).position;
        }
        else {
            singuityDestination = stupidFirstSinguityPosition;
        }


        return serverCommander -> {
            serverCommander.moveUnitsToPosition(input.ownedSinguities, singuityDestination);
        };
    }
}
