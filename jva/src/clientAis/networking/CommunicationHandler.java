package clientAis.networking;

import java.io.*;
import java.net.Socket;
import java.util.Arrays;
import java.util.stream.IntStream;

public class CommunicationHandler implements Closeable {

    public boolean isClosed;

    private static final int BUFFER_SIZE = 65535;

    private final Socket socket;
    private final ConnectionCredentials serverCredentials;
    private final char[] bufferData;

    public CommunicationHandler(Socket socket, ConnectionCredentials serverCredentials) {
        this.socket = socket;
        this.serverCredentials = serverCredentials;
        this.isClosed = false;
        this.bufferData = new char[BUFFER_SIZE];
    }

    @Override
    public void close() {
        isClosed = true;
    }

    public void send(String communicationData) {
        raiseExceptionIfDisconnectedFromServer("send");
        try {
            final PrintWriter writer = new PrintWriter(socket.getOutputStream(), true);
            writer.print(communicationData);
        }
        catch (IOException ioException) {
            ioException.printStackTrace();
            close();
            throw new RuntimeException("Remote socket has been shutdown while sending to it");
        }

    }

    public String receive() {
        raiseExceptionIfDisconnectedFromServer("receive");
        try {
            final BufferedReader reader = new BufferedReader(new InputStreamReader(socket.getInputStream()));
            final int lengthRead = reader.read(bufferData);
            final char[] copiedBufferData = Arrays.copyOfRange(bufferData, 0, lengthRead);
            return new String(copiedBufferData);
        }
        catch (IOException ioException) {
            ioException.printStackTrace();
            close();
            throw new RuntimeException("Remote socket has been shutdown while receiving from it");
        }

    }

    private void raiseExceptionIfDisconnectedFromServer(String nameOfAction) {
        if(isClosed) {
            throw new RuntimeException("Cannot " + nameOfAction + " data through a disconnected socket.");
        }
    }
}
