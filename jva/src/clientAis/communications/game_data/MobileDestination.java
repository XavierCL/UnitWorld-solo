package clientAis.communications.game_data;

import net.minidev.json.JSONObject;
import utils.math.vector.Vector2;

import java.util.Optional;

public class MobileDestination {

    public final Optional<Vector2> pointDestination;
    public final Optional<AllegedSpawnerDestination> allegedSpawnerDestination;
    public final Optional<String> spawnerDestination;

    public MobileDestination(JSONObject data) {
        Optional<Vector2> pointDestination = Optional.empty();
        Optional<AllegedSpawnerDestination> allegedSpawnerDestination = Optional.empty();
        Optional<String> spawnerDestination = Optional.empty();

        if(data.containsKey("p")) {
            pointDestination = Optional.of(new Vector2((JSONObject) data.get("p")));
        }
        else if(data.containsKey("s")) {
            allegedSpawnerDestination = Optional.of(new AllegedSpawnerDestination((JSONObject) data.get("s")));
        }
        else { // if(data.containsKey("i")) {
            spawnerDestination = Optional.of(data.getAsString("i"));
        }

        this.pointDestination = pointDestination;
        this.allegedSpawnerDestination = allegedSpawnerDestination;
        this.spawnerDestination = spawnerDestination;
    }
}
