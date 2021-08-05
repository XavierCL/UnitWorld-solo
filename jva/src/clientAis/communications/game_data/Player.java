package clientAis.communications.game_data;

import net.minidev.json.JSONObject;

public class Player {

    public final String id;

    public Player(JSONObject data) {
        this.id = data.getAsString("i");
    }
}
