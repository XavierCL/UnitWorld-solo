package clientAis;

import clientAis.cli.CommandLineParser;
import clientAis.communications.MessageSerializer;
import clientAis.communications.ServerCommander;
import clientAis.communications.ServerReceiver;
import clientAis.games.GameManager;
import clientAis.implementations.Bot;
import clientAis.implementations.Dummy;
import clientAis.networking.ClientConnector;
import utils.data_structure.tupple.Tuple2;
import utils.timer.LambdaTimerTaskHelper;

import java.util.*;
import java.util.concurrent.atomic.AtomicInteger;
import java.util.function.Consumer;

public class MainClientAi {

    public static final String DEFAULT_SERVER_IP = "127.0.0.1";
    public static final String DEFAULT_SERVER_PORT = "52124";
    public static final String DEFAULT_AI_NAME = Dummy.class.getName();
    public static final double SECOND_BETWEEN_AI_FRAME = 0.5;
    public static final long REFRESH_PERIOD_MS = (int)(1/SECOND_BETWEEN_AI_FRAME);

    private static final Map<String, Bot> BOT_IMPLEMENTATIONS = new HashMap<>();
    static {
        BOT_IMPLEMENTATIONS.put(Dummy.class.getName(), new Dummy());
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

        new ClientConnector(serverIp, serverPort, communicationHandler -> {
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

                gameManager.gameState.ifPresent(gameState -> {
                    if(gameState.frameCount > lastAiGameStateVersion.get()) {
                        lastAiGameStateVersion.set(gameManager.gameState.get().frameCount);
                    }
                    Consumer<ServerCommander> consumer = bot.exec(new Tuple2<>(gameState, gameManager.currentPlayerId.get()));
                    consumer.accept(serverCommander);
                });
            }), 0, REFRESH_PERIOD_MS);
        });

    }
}