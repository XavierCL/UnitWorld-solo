package clientAis.implementations.singuity_grouping;

import clientAis.communications.ServerCommander;
import clientAis.dynamic_data.DataPacket;
import clientAis.implementations.Bot;

import java.util.function.Consumer;

public class SinguityGrouping implements Bot {

    @Override
    public Consumer<ServerCommander> exec(DataPacket input) {


        return serverCommander -> {

        };
    }
}
