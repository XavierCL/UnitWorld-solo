package clientAis.communications.game_data;

import net.minidev.json.JSONObject;

public class GameStateMessage {

    public final GameState gameState;
    public final String currentPlayer;

    public GameStateMessage(JSONObject data) {
        this.gameState = new GameState((JSONObject) data.get("c"));
        this.currentPlayer = data.getAsString("u");
    }
}
