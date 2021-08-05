package clientAis.communications.game_data;

import net.minidev.json.JSONObject;
import utils.math.vector.Vector2;

import java.util.Optional;

public class Singuity {

    public final int id;
    public final int playerId;
    public final Vector2 position;
    public final Vector2 velocity;
    public final Optional<MobileDestination> destination;
    public final float healthPoints;
    public final int lastShootFrame;

    public Singuity(JSONObject data) {
        this.id = data.getAsNumber("i").intValue();
        this.playerId = data.getAsNumber("p").intValue();
        this.position = new Vector2((JSONObject) data.get("o"));
        this.velocity = new Vector2((JSONObject) data.get("s"));
        this.destination = data.getAsString("d").equals("n") ? Optional.empty() : Optional.of(new MobileDestination((JSONObject) data.get("d")));
        this.healthPoints = data.getAsNumber("h").floatValue();
        this.lastShootFrame = data.getAsNumber("l").intValue();
    }
}
