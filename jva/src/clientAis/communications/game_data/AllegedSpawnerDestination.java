package clientAis.communications.game_data;

import net.minidev.json.JSONObject;

import java.util.Optional;

public class AllegedSpawnerDestination {

    public final String spawnerId;
    public final Optional<SpawnerAllegence> spawnerAllegence;

    public AllegedSpawnerDestination(JSONObject data) {
        this.spawnerId = data.getAsString("i");
        this.spawnerAllegence = data.getAsString("a").equals("n") ? Optional.empty() : Optional.of(new SpawnerAllegence((JSONObject) data.get("a")));
    }
}
