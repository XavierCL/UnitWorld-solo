package clientAis.implementations;

import clientAis.communications.ServerCommander;
import clientAis.communications.game_data.GameState;
import clientAis.communications.game_data.Singuity;
import clientAis.communications.game_data.Spawner;
import utils.data_structure.tupple.Tuple2;
import utils.math.vector.Vector2;

import java.util.*;
import java.util.concurrent.atomic.AtomicReference;
import java.util.function.Consumer;
import java.util.stream.Collectors;
import java.util.stream.DoubleStream;

public class ClosestSpawner implements Bot {

    @Override
    public Consumer<ServerCommander> exec(Tuple2<GameState, String> input) {
        final List<Singuity> playerSinguities = input.value1.singuities.stream()
                .filter(singuity -> singuity.playerId.equals(input.value2))
                .collect(Collectors.toList());
        final List<Spawner> playerSpawners = input.value1.spawners.stream()
                .filter(spawner -> spawner.allegence.map(spawnerAllegence ->
                        spawnerAllegence.isClaimed && spawnerAllegence.playerId.equals(input.value2)).orElse(false))
                .collect(Collectors.toList());
        final List<Spawner> desiredSpawners = new ArrayList<>(input.value1.spawners);
        desiredSpawners.removeAll(playerSpawners);

        Optional<Spawner> closestSpawnerOpt = Optional.empty();
        Optional<Double> smallestDistanceOpt = Optional.empty();
        for(Spawner playerSpawner: playerSpawners) {
            for(Spawner desiredSpawner: desiredSpawners) {
                if(!closestSpawnerOpt.isPresent()) {
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

        final List<Consumer<ServerCommander>> commandList = new ArrayList<>();
        closestSpawnerOpt.ifPresent(closestSpawner -> commandList.add(serverCommander -> {
                final Set<String> playerSinguityIds = playerSinguities.stream()
                        .map(singuity -> singuity.id)
                        .collect(Collectors.toSet());
                final String spawnerId = closestSpawner.id;
                serverCommander.moveUnitsToSpawner(playerSinguityIds, spawnerId);
        }));

        return serverCommander -> commandList.forEach(serverCommanderConsumer -> serverCommanderConsumer.accept(serverCommander));
    }
}
