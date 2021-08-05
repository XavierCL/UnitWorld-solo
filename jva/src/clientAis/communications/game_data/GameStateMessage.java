package clientAis.communications.game_data;

import net.minidev.json.JSONObject;

public class GameStateMessage {

    public final GameState gameState;
    public final Player currentPlayer;

    public GameStateMessage(JSONObject data) {
        this.gameState = new GameState((JSONObject) data.get("c"));
        this.currentPlayer = new Player((JSONObject) data.get("u"));
    }
}
