package clientAis.implementations;

import clientAis.communications.ServerCommander;
import clientAis.communications.game_data.GameState;
import utils.data_structure.tupple.Tuple2;

import java.util.function.Consumer;

public class SinguityGrouping implements Bot {

    @Override
    public Consumer<ServerCommander> exec(Tuple2<GameState, String> input) {
        return null;
    }
}
