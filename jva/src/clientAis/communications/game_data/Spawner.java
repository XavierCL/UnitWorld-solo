package clientAis.communications.game_data;

import net.minidev.json.JSONObject;
import utils.math.vector.Vector2;

import java.util.Optional;

public class Spawner {

    public final String id;
    public final Vector2 position;
    public final Optional<SpawnerAllegence> allegence;
    public final Optional<MobileDestination> rally;
    public final int lastSpawnFrame;
    public final int totalSpawnCount;
    public final int lastClaimFrameCount;

    public Spawner(JSONObject data) {
        this.id = data.getAsString("i");
        this.position = new Vector2((JSONObject) data.get("p"));
        this.allegence = data.getAsString("a").length() == 0 ? Optional.empty() : Optional.of(new SpawnerAllegence((JSONObject) data.get("a")));
        this.rally = data.getAsString("r").length() == 0 ? Optional.empty() : Optional.of(new MobileDestination((JSONObject) data.get("r")));
        this.lastSpawnFrame = data.getAsNumber("l").intValue();
        this.totalSpawnCount = data.getAsNumber("t").intValue();
        this.lastClaimFrameCount = data.getAsNumber("c").intValue();
    }
}
