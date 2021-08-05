package clientAis.communications.game_data;

import net.minidev.json.JSONObject;

public class SpawnerAllegence {

    public final boolean isClaimed;
    public final float healthPoints;
    public final String playerId;

    public SpawnerAllegence(JSONObject data) {
        this.isClaimed = data.getAsString("i").equals("true");
        this.healthPoints = data.getAsNumber("h").floatValue();
        this.playerId = data.getAsString("p");
    }
}
