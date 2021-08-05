package clientAis.communications;

import clientAis.communications.game_data.MobileDestination;
import clientAis.networking.CommunicationHandler;
import net.minidev.json.JSONArray;
import net.minidev.json.JSONObject;
import utils.math.vector.Vector2;

import java.util.Set;

public class ServerCommander {

    private final CommunicationHandler serverCommunicator;
    private final MessageSerializer messageSerializer;

    public ServerCommander(CommunicationHandler serverCommunicator, MessageSerializer messageSerializer) {
        this.serverCommunicator = serverCommunicator;
        this.messageSerializer = messageSerializer;
    }

    public void moveUnitsToPosition(Set<String> singuityIds, Vector2 position) {
        final JSONObject jsonPacket = new JSONObject();
        jsonPacket.put("type", "move-units-to-position");
        final JSONObject jsonData = new JSONObject();
        final JSONArray singuityIdsJson = new JSONArray();
        singuityIdsJson.addAll(singuityIds);
        jsonData.put("m", singuityIdsJson);
        final JSONObject jsonPosition = new JSONObject();
        jsonPosition.put("x", position.x);
        jsonPosition.put("y", position.y);
        jsonData.put("d", jsonPosition);
        jsonPacket.put("data", jsonData);
        jsonPacket.put("timestamp", System.currentTimeMillis());

        serverCommunicator.send(jsonPacket + "\\n");
    }

    public void moveUnitsToSpawner(Set<String> singuityIds, String spawnerId) {
        final JSONObject jsonPacket = new JSONObject();
        jsonPacket.put("type", "move-units-to-spawner");
        final JSONObject jsonData = new JSONObject();
        final JSONArray singuityIdsJson = new JSONArray();
        singuityIdsJson.addAll(singuityIds);
        jsonData.put("m", singuityIdsJson);
        jsonData.put("s", spawnerId);
        jsonPacket.put("data", jsonData);
        jsonPacket.put("timestamp", System.currentTimeMillis());

        serverCommunicator.send(jsonPacket + "\\n");
    }

    public void setSpawnersRally(Set<String> spawnersId, MobileDestination mobileDestination) {
        final JSONObject jsonPacket = new JSONObject();
        jsonPacket.put("type", "set-spawners-rally");
        final JSONObject jsonData = new JSONObject();
        final JSONArray spawnersIdJson = new JSONArray();
        spawnersIdJson.addAll(spawnersId);
        jsonData.put("s", spawnersIdJson);
        jsonData.put("d", mobileDestination.serializeToJson());
        jsonPacket.put("data", jsonData);
        jsonPacket.put("timestamp", System.currentTimeMillis());

        serverCommunicator.send(jsonPacket + "\\n");
    }
}
