package clientAis.implementations.relentless_attacker;

import clientAis.communications.ServerCommander;
import clientAis.communications.game_data.Singuity;
import clientAis.dynamic_data.DataPacket;
import clientAis.implementations.Bot;
import utils.data_structure.cluster.DataCluster;
import utils.math.vector.Vector2;
import utils.unit_world.cluster.ClusterTrajectoryEstimator;

import java.util.Optional;
import java.util.function.Consumer;
import java.util.function.Function;

public class RelentlessAttacker implements Bot {

    @Override
    public Consumer<ServerCommander> exec(DataPacket input) {
        final Optional<DataCluster<String>> firstAdverseClusterOpt = input.adverseClusters.stream().findFirst();
        if(firstAdverseClusterOpt.isEmpty()) {
            return serverCommander -> {};
        }

        final Function<Integer, Optional<Vector2>> adverseClusterDestinationFunction =
                ClusterTrajectoryEstimator.estimateCurrentTrajectory(firstAdverseClusterOpt.get(), input);

        final Optional<Vector2> averageSinguityPositionOpt = Vector2.centerOfMass(input.ownedSinguities, id -> input.singuityIdMap.get(id).position);
        final Function<Integer, Optional<Vector2>> allyClusterDestinationFunction =
                frame -> adverseClusterDestinationFunction.apply(frame)
                        .flatMap(vector -> averageSinguityPositionOpt.map(averageSinguityPosition ->
                                averageSinguityPosition.plus(
                                        vector.minus(averageSinguityPosition)
                                        .scaledToMagnitude(frame * Singuity.SINGUITY_MAX_SPEED))));

        double bestDistance = Double.MAX_VALUE;
        int bestFrameCount = -1;
        for(int i = 0; i < 600; i++) {
            final Optional<Vector2> adversePositionOpt = adverseClusterDestinationFunction.apply(i);
            final Optional<Vector2> allyPositionOpt = allyClusterDestinationFunction.apply(i);
            if(adversePositionOpt.isPresent() && allyPositionOpt.isPresent()) {
                final double distance = adversePositionOpt.get().minus(allyPositionOpt.get()).magnitude();
                if(bestDistance > distance) {
                    bestDistance = distance;
                    bestFrameCount = i;
                }
            }
        }

        final Optional<Vector2> destinationOpt = allyClusterDestinationFunction.apply(bestFrameCount);

        return destinationOpt.<Consumer<ServerCommander>>map(vector2 -> serverCommander -> {
            serverCommander.moveUnitsToPosition(input.ownedSinguities, vector2);
        }).orElseGet(() -> serverCommander -> {});

    }
}
