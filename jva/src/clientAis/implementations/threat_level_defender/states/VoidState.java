package clientAis.implementations.threat_level_defender.states;

import clientAis.communications.ServerCommander;
import clientAis.dynamic_data.DataPacket;
import utils.data_structure.tupple.Tuple2;
import utils.state_machine.State;
import utils.unit_world.minion.Minion;
import utils.unit_world.minion.MinionState;
import utils.unit_world.minion.MinionWielder;

import java.util.function.Consumer;

public class VoidState extends MinionState {

    public VoidState(MinionWielder minionWielder) {
        super(minionWielder);
    }

    @Override
    public void start(Tuple2<DataPacket, Minion> input) {}

    @Override
    public void stop(Tuple2<DataPacket, Minion> input) {}

    @Override
    public Consumer<ServerCommander> exec(Tuple2<DataPacket, Minion> input) {
        return serverCommander -> {};
    }

    @Override
    public State<Tuple2<DataPacket, Minion>, Consumer<ServerCommander>> next(Tuple2<DataPacket, Minion> input) {
        return this;
    }

    @Override
    public void debug(Tuple2<DataPacket, Minion> input) {}

    @Override
    public String getName() {
        return "void";
    }
}
