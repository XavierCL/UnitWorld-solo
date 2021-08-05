package clientAis.communications.game_data;

import net.minidev.json.JSONObject;
import utils.math.vector.Vector2;

import java.util.Optional;
import java.util.concurrent.atomic.AtomicReference;

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

    public MobileDestination(Vector2 position) {
        this.pointDestination = Optional.of(position);
        this.allegedSpawnerDestination = Optional.empty();
        this.spawnerDestination = Optional.empty();
    }

    public MobileDestination(AllegedSpawnerDestination allegedSpawnerDestination) {
        this.pointDestination = Optional.empty();
        this.allegedSpawnerDestination = Optional.of(allegedSpawnerDestination);
        this.spawnerDestination = Optional.empty();
    }

    public MobileDestination(String spawnerId) {
        this.pointDestination = Optional.empty();
        this.allegedSpawnerDestination = Optional.empty();
        this.spawnerDestination = Optional.of(spawnerId);
    }

    public String serializeToJson() {
        final JSONObject mobileDestinationJson = new JSONObject();

        pointDestination.ifPresent(point -> {
            mobileDestinationJson.put("x", point.x);
            mobileDestinationJson.put("y", point.y);
        });
        allegedSpawnerDestination.ifPresent(spawnerDestination -> {
            final JSONObject allegenceJson = new JSONObject();
            if(spawnerDestination.spawnerAllegence.isPresent()) {
                allegenceJson.put("i", spawnerDestination.spawnerAllegence.get().isClaimed);
                allegenceJson.put("h", spawnerDestination.spawnerAllegence.get().healthPoints);
                allegenceJson.put("p", spawnerDestination.spawnerAllegence.get().playerId);
            }
            else {
                allegenceJson.put("n", "");
            }

            final JSONObject spawnerJson = new JSONObject();
            spawnerJson.put("i", spawnerDestination.spawnerId);
            spawnerJson.put("a", allegenceJson);
            mobileDestinationJson.put("s", spawnerJson);
        });
        spawnerDestination.ifPresent(destination -> {
            mobileDestinationJson.put("u", spawnerDestination.get());
        });

        return mobileDestinationJson.toString();
    }
}
