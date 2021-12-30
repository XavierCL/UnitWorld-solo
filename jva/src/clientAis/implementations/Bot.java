package clientAis.implementations;

import clientAis.communications.ServerCommander;
import clientAis.dynamic_data.DataPacket;
import utils.state_machine.Behaviour;

import java.util.function.Consumer;

public interface Bot extends Behaviour<DataPacket, Consumer<ServerCommander>> {}
