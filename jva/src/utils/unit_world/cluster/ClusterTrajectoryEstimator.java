package utils.unit_world.cluster;

import clientAis.communications.game_data.Singuity;
import clientAis.dynamic_data.DataPacket;
import utils.data_structure.cluster.DataCluster;
import utils.math.vector.Vector2;

import java.util.Optional;
import java.util.function.Function;

public class ClusterTrajectoryEstimator {

    public static Function<Integer, Optional<Vector2>> estimateCurrentTrajectory(final DataCluster<String> singuityCluster, final DataPacket input) {
        final Optional<Vector2> averagePositionOpt = Vector2.centerOfMass(singuityCluster.elements, s -> input.singuityIdMap.get(s).position);
        final Optional<Vector2> averageVelocityOpt = Vector2.centerOfMass(singuityCluster.elements, s -> input.singuityIdMap.get(s).velocity);

        if(averagePositionOpt.isPresent() && averageVelocityOpt.isPresent()) {
            final Vector2 averagePosition = averagePositionOpt.get();
            final Vector2 averageVelocity = averageVelocityOpt.get();
            return frameCount -> Optional.of(averagePosition.plus(averageVelocity.scaled(frameCount)));
        }

        return frameCount -> Optional.empty();
    }

    public static Optional<Double> estimateFramesToTravelTo(final DataCluster<String> singuityCluster, final Vector2 destination, final DataPacket input) {
        final Optional<Vector2> averagePositionOpt = Vector2.centerOfMass(singuityCluster.elements, s -> input.singuityIdMap.get(s).position);

        return averagePositionOpt.map(vector2 -> destination.minus(vector2).magnitude() / Singuity.SINGUITY_MAX_SPEED);
    }
}
