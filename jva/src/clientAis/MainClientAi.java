package clientAis;


import clientAis.cli.CommandLineParser;
import clientAis.communications.MessageSerializer;
import clientAis.communications.ServerCommander;
import clientAis.communications.ServerReceiver;
import clientAis.communications.game_data.GameState;
import clientAis.dynamic_data.DataPacket;
import clientAis.games.GameManager;
import clientAis.implementations.Bot;
import clientAis.implementations.basic_minion_wielder.BasicMinionWielder;
import clientAis.implementations.basic_single_mind.BasicSingleMind;
import clientAis.implementations.challenge_defense.ChallengeDefense;
import clientAis.implementations.closest_spawner.ClosestSpawner;
import clientAis.implementations.dummy.Dummy;
import clientAis.implementations.go_middle.GoMiddle;
import clientAis.implementations.mindless_chase.MindlessChase;
import clientAis.implementations.multidefense.MultiDefense;
import clientAis.implementations.relentless_attacker.RelentlessAttacker;
import clientAis.implementations.threat_level_defender.ThreatLevelDefender;
import clientAis.networking.ClientConnector;
import utils.timer.LambdaTimerTaskHelper;

import java.util.HashMap;
import java.util.Map;
import java.util.Optional;
import java.util.Timer;
import java.util.concurrent.atomic.AtomicInteger;
import java.util.function.Consumer;

public class MainClientAi {

    public static final String DEFAULT_SERVER_IP = "127.0.0.1";
    public static final String DEFAULT_SERVER_PORT = "52124";
    public static final String DEFAULT_AI_NAME = MultiDefense.class.getName();
    public static final double SECOND_BETWEEN_AI_FRAME = 0.1;
    public static final long REFRESH_PERIOD_MS = (int)(SECOND_BETWEEN_AI_FRAME*1000);

    private static final Map<String, Bot> BOT_IMPLEMENTATIONS = new HashMap<>();
    static {
        addBotImplementation(new Dummy());
        addBotImplementation(new GoMiddle());
        addBotImplementation(new ClosestSpawner());
        addBotImplementation(new MindlessChase());
        addBotImplementation(new ChallengeDefense());
        addBotImplementation(new BasicSingleMind());
        addBotImplementation(new BasicMinionWielder());
        addBotImplementation(new ThreatLevelDefender());

        addBotImplementation(new RelentlessAttacker());
        addBotImplementation(new MultiDefense());
    }

    private static Optional<DataPacket> previousInputOpt = Optional.empty();

    private static void addBotImplementation(Bot bot) {
        BOT_IMPLEMENTATIONS.put(bot.getClass().getName(), bot);
    }

    public static void main(String[] args) {
        final CommandLineParser commandLineParser = new CommandLineParser(args);

        final String serverIp = commandLineParser.findArgument("serverIp")
                .orElse(DEFAULT_SERVER_IP);
        final String serverPortString = commandLineParser.findArgument("serverPort")
                .orElse(DEFAULT_SERVER_PORT);
        final int serverPort = Integer.parseInt(serverPortString);
        final String aiName = commandLineParser.findArgument("aiName")
                .orElse(DEFAULT_AI_NAME);

        ClientConnector.connect(serverIp, serverPort, communicationHandler -> {
            System.out.println("Connected to server at " + serverIp + ":" + serverPort);

            final MessageSerializer messageSerializer = new MessageSerializer();
            final ServerCommander serverCommander = new ServerCommander(communicationHandler, messageSerializer);

            final GameManager gameManager = new GameManager();

            final ServerReceiver serverReceiver = new ServerReceiver(communicationHandler, gameManager, messageSerializer);
            serverReceiver.startAsync();

            System.out.println("Started games evaluation");

            final Bot bot = BOT_IMPLEMENTATIONS.get(aiName);

            AtomicInteger lastAiGameStateVersion = new AtomicInteger(-1);

            Timer timer = new Timer();
            timer.scheduleAtFixedRate(new LambdaTimerTaskHelper(() -> {
                if(communicationHandler.isClosed) {
                    timer.cancel();
                }

                gameManager.gameState.ifPresent(gameState -> gameManager.currentPlayerId.ifPresent(currentPlayerId -> {
                    if(gameState.frameCount > lastAiGameStateVersion.get()) {
                        lastAiGameStateVersion.set(gameManager.gameState.get().frameCount);
                    }
                    final double timeBeforeRunningBot = System.nanoTime()/1000000.0;
                    runBot(bot, currentPlayerId, gameState, serverCommander);
                    final double timeAfterRunningBot = System.nanoTime()/1000000.0;
                    final double deltaTime = timeAfterRunningBot - timeBeforeRunningBot;
                    System.out.println("Frame " + gameState.frameCount + " took " + String.format("%,.3f", deltaTime) + "ms. CPU " + (int)(100*deltaTime/REFRESH_PERIOD_MS) + "%.");

                }));
            }), 0, REFRESH_PERIOD_MS);
        });
    }

    private static void runBot(
            final Bot bot,
            final String currentPlayerId,
            final GameState gameState,
            final ServerCommander serverCommander) {
        final DataPacket input = new DataPacket(gameState, currentPlayerId, previousInputOpt);
        final Consumer<ServerCommander> consumer = bot.exec(input);
        previousInputOpt = Optional.of(input);
        consumer.accept(serverCommander);
    }
}
