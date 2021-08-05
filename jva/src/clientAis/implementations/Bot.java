package clientAis.implementations;

import clientAis.communications.ServerCommander;
import clientAis.communications.game_data.GameState;
import utils.data_structure.tupple.Tuple2;
import utils.state_machine.Behaviour;

import java.util.function.Consumer;
import java.util.function.Supplier;

public interface Bot extends Behaviour<Tuple2<GameState, String>, Consumer<ServerCommander>> {

}
