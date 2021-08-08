package clientAis.implementations.go_middle;

import clientAis.communications.ServerCommander;
import clientAis.dynamic_data.DataPacket;
import clientAis.implementations.Bot;
import utils.math.vector.Vector2;

import java.util.HashSet;
import java.util.function.Consumer;

public class GoMiddle implements Bot {

    @Override
    public Consumer<ServerCommander> exec(DataPacket input) {
        return serverCommander -> serverCommander.moveUnitsToPosition(new HashSet<>(input.ownedSinguities), new Vector2(2500, 2500));
    }
}
