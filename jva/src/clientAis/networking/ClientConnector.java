package clientAis.networking;

import java.io.IOException;
import java.net.Socket;
import java.util.function.Consumer;

public class ClientConnector {

    public ClientConnector(String serverIp, int serverPort, Consumer<CommunicationHandler> clientConnectedCallBack) {
        try {
            final Socket socket = new Socket(serverIp, serverPort);
            final ConnectionCredentials serverCredentials = new ConnectionCredentials(serverIp, serverPort);
            clientConnectedCallBack.accept(new CommunicationHandler(socket, serverCredentials));
        }
        catch (IOException ioException) {
            ioException.printStackTrace();
        }
    }
}
