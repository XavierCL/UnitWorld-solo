package clientAis.implementations.Dummy;

import clientAis.communications.ServerCommander;
import clientAis.dynamic_data.DataPacket;
import clientAis.implementations.Bot;

import java.util.function.Consumer;

public class Dummy implements Bot {

    @Override
    public Consumer<ServerCommander> exec(DataPacket input) {
        return serverCommander -> {};
    }
}
