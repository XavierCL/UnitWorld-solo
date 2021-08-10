package utils.minion;

import clientAis.communications.ServerCommander;
import clientAis.dynamic_data.DataPacket;
import utils.data_structure.tupple.Tuple2;
import utils.state_machine.State;

import java.util.function.Consumer;

public abstract class MinionState implements State<Tuple2<DataPacket, Minion>, Consumer<ServerCommander>> {

    private final MinionWielder minionWielder;

    public MinionState(MinionWielder minionWielder) {
        this.minionWielder = minionWielder;
    }

    public MinionWielder getMinionWielder() {
        return minionWielder;
    }
}
