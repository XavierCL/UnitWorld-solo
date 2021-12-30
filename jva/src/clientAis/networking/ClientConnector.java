package clientAis.networking;

import java.io.IOException;
import java.net.Socket;
import java.util.function.Consumer;

public class ClientConnector {

    public static final long CONNECTION_TIMEOUT = 10000;

    public static void connect(String serverIp, int serverPort, Consumer<CommunicationHandler> clientConnectedCallBack) {
        long timeoutTime = System.currentTimeMillis() + CONNECTION_TIMEOUT;
        Socket socket = null;
        while(socket == null && System.currentTimeMillis() < timeoutTime) {
            try {
                socket = new Socket(serverIp, serverPort);
                final ConnectionCredentials serverCredentials = new ConnectionCredentials(serverIp, serverPort);
                clientConnectedCallBack.accept(new CommunicationHandler(socket, serverCredentials));
            }
            catch (IOException ignored) {}
        }
        if(System.currentTimeMillis() >= timeoutTime) {
            throw new RuntimeException("java client couldn't connect to server");
        }
    }
}
