package clientAis.implementations;

import clientAis.communications.ServerCommander;
import clientAis.communications.game_data.GameState;
import utils.data_structure.tupple.Tuple2;
import utils.math.vector.Vector2;

import java.util.Set;
import java.util.function.Consumer;
import java.util.stream.Collectors;

public class GoMiddle implements Bot {

    @Override
    public Consumer<ServerCommander> exec(Tuple2<GameState, String> input) {
        return serverCommander -> {
            final Set<String> mySinguities = input.value1.singuities.stream()
                    .filter(singuity -> singuity.playerId.equals(input.value2))
                    .map(singuity -> singuity.id)
                    .collect(Collectors.toSet());
            serverCommander.moveUnitsToPosition(mySinguities, new Vector2(2500, 2500));
        };
    }
}
