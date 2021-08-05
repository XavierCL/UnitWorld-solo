package clientAis.games;

import clientAis.communications.game_data.GameState;

import java.util.Optional;

public class GameManager {

    public Optional<String> currentPlayerId;
    public Optional<GameState> gameState;

    public GameManager() {
        this.currentPlayerId = Optional.empty();
        this.gameState = Optional.empty();
    }

    public void setCurrentPlayerId(String currentPlayerId) {
        this.currentPlayerId = Optional.of(currentPlayerId);
    }

    public void setNextCompleteGameState(GameState gameState) {
        this.gameState = Optional.of(gameState);
    }
}
