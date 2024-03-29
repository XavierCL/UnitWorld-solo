package clientAis.communications.game_data;

import net.minidev.json.JSONObject;
import utils.math.vector.Vector2;

import java.util.Optional;

public class Singuity {

    public static final double SINGUITY_MAX_SPEED = 4;
    public static final double SINGUITY_MAX_ACCELERATION = 0.08;

    public final String id;
    public final String playerId;
    public final Vector2 position;
    public final Vector2 velocity;
    public final Optional<MobileDestination> destination;
    public final float healthPoints;
    public final int lastShootFrame;

    public Singuity(JSONObject data) {
        this.id = data.getAsString("i");
        this.playerId = data.getAsString("p");
        this.position = new Vector2((JSONObject) data.get("o"));
        this.velocity = new Vector2((JSONObject) data.get("s"));
        this.destination = data.getAsString("d").equals("n") ? Optional.empty() : Optional.of(new MobileDestination((JSONObject) data.get("d")));
        this.healthPoints = data.getAsNumber("h").floatValue();
        this.lastShootFrame = data.getAsNumber("l").intValue();
    }

    public Singuity(Vector2 position) {
        this.id = "";
        this.playerId = "";
        this.position = position;
        this.velocity = new Vector2();
        this.destination = Optional.empty();
        this.healthPoints = 0;
        this.lastShootFrame = 0;
    }

    public Singuity(Vector2 position, String id) {
        this.id = id;
        this.playerId = "";
        this.position = position;
        this.velocity = new Vector2();
        this.destination = Optional.empty();
        this.healthPoints = 0;
        this.lastShootFrame = 0;
    }
}
