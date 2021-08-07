package clientAis.implementations.closest_spawner;

import clientAis.communications.ServerCommander;
import clientAis.communications.game_data.Spawner;
import clientAis.dynamic_data.DataPacket;
import clientAis.implementations.Bot;

import java.util.*;
import java.util.function.Consumer;
import java.util.stream.Collectors;

public class ClosestSpawner implements Bot {

    @Override
    public Consumer<ServerCommander> exec(DataPacket input) {
        Optional<Spawner> closestSpawnerOpt = Optional.empty();
        Optional<Double> smallestDistanceOpt = Optional.empty();
        for(Spawner playerSpawner: input.ownedSpawners.stream()
                .map(input.spawnerIdMap::get)
                .collect(Collectors.toList())) {
            for(Spawner desiredSpawner: input.attackableSpawners.stream()
                    .map(input.spawnerIdMap::get)
                    .collect(Collectors.toList())) {
                if(closestSpawnerOpt.isEmpty()) {
                    closestSpawnerOpt = Optional.of(desiredSpawner);
                    smallestDistanceOpt = Optional.of(playerSpawner.position.minus(desiredSpawner.position).magnitudeSquared());
                    continue;
                }
                double distanceSquared = playerSpawner.position.minus(desiredSpawner.position).magnitudeSquared();
                if(distanceSquared < smallestDistanceOpt.get()) {
                    closestSpawnerOpt = Optional.of(desiredSpawner);
                    smallestDistanceOpt = Optional.of(distanceSquared);
                }
            }
        }

        System.out.println(input.adverseClusters.size());

        final List<Consumer<ServerCommander>> commandList = new ArrayList<>();
        closestSpawnerOpt.ifPresent(closestSpawner -> commandList.add(serverCommander -> {
                final Set<String> playerSinguityIds = new HashSet<>(input.ownedSinguities);
                final String spawnerId = closestSpawner.id;
                serverCommander.moveUnitsToSpawner(playerSinguityIds, spawnerId);
        }));

        return serverCommander -> commandList.forEach(serverCommanderConsumer -> serverCommanderConsumer.accept(serverCommander));
    }
}
