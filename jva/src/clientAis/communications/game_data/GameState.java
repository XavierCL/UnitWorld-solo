package clientAis.communications.game_data;

import net.minidev.json.JSONArray;
import net.minidev.json.JSONObject;

import java.util.List;
import java.util.stream.Collectors;

public class GameState {

    public final List<Player> players;
    public final List<Singuity> singuities;
    public final List<Spawner> spawners;
    public final int frameCount;

    public GameState(JSONObject data) {
        this.players = ((JSONArray) data.get("p")).stream()
                .map(object -> new Player((JSONObject) object))
                .collect(Collectors.toList());
        this.singuities = ((JSONArray) data.get("s")).stream()
                .map(object -> new Singuity((JSONObject) object))
                .collect(Collectors.toList());
        this.spawners = ((JSONArray) data.get("a")).stream()
                .map(object -> new Spawner((JSONObject) object))
                .collect(Collectors.toList());
        this.frameCount = data.getAsNumber("f").intValue();
    }
}
