package clientAis.communications;

import clientAis.communications.game_data.GameStateMessage;
import clientAis.networking.CommunicationHandler;
import net.minidev.json.JSONObject;

import java.io.Closeable;
import java.util.List;

public class ServerReceiver implements Runnable, Closeable {

    private final CommunicationHandler communicationHandler;
    private final GameManager gameManager;
    private final MessageSerializer messageSerializer;

    private Thread loopThread;
    private int lastReceivedTimeStamp;

    public ServerReceiver(CommunicationHandler communicationHandler, GameManager gameManager, MessageSerializer messageSerializer) {
        this.communicationHandler = communicationHandler;
        this.gameManager = gameManager;
        this.messageSerializer = messageSerializer;
    }

    public void startAsync() {
        loopThread = new Thread(this);
        loopThread.start();
    }

    @Override
    public void close() {
        communicationHandler.close();
        try {
            loopThread.join();
        }
        catch (InterruptedException e) {
            e.printStackTrace();
        }
    }

    @Override
    public void run() {
        while(!communicationHandler.isClosed) {
            final String rawMessage = communicationHandler.receive();
            final List<JSONObject> deserializedMessage = messageSerializer.deserialize(rawMessage);

            for(JSONObject message: deserializedMessage) {
                final int messageTimeStamp = message.getAsNumber("timestamp").intValue();
                if(messageTimeStamp > lastReceivedTimeStamp) {
                    lastReceivedTimeStamp = messageTimeStamp;
                    final GameStateMessage
                }
            }
        }
    }
}
