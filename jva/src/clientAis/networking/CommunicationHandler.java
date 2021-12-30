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
    private BufferedReader reader;
    private final PrintWriter writer;

    public CommunicationHandler(Socket socket, ConnectionCredentials serverCredentials) {
        this.socket = socket;
        this.serverCredentials = serverCredentials;
        this.isClosed = false;
        this.bufferData = new char[BUFFER_SIZE];
        try {
            this.reader = new BufferedReader(new InputStreamReader(socket.getInputStream()));
            this.writer = new PrintWriter(socket.getOutputStream(), true);
        }
        catch (IOException ioException) {
            throw new RuntimeException(ioException);
        }
    }

    @Override
    public void close() {
        isClosed = true;
    }

    public void send(String communicationData) {
        raiseExceptionIfDisconnectedFromServer("send");
        writer.write(communicationData);
        writer.flush();
    }

    public String receive() {
        raiseExceptionIfDisconnectedFromServer("receive");
        try {
            final int lengthRead = reader.read(bufferData);
            if(lengthRead == -1) {
                close();
                throw new RuntimeException("Remote socket has been shutdown while receiving from it");
            }
            final char[] copiedBufferData = Arrays.copyOfRange(bufferData, 0, lengthRead);
            return new String(copiedBufferData);
        }
        catch (IOException ioException) {
            ioException.printStackTrace();
            close();
            if(socket.isClosed()) {
                throw new RuntimeException("Remote socket has been shutdown while receiving from it");
            }
            try {
                reader = new BufferedReader(new InputStreamReader(socket.getInputStream()));
            }
            catch (IOException ioException2) {
                ioException2.printStackTrace();
                throw new RuntimeException("Remote socket has been shutdown while receiving from it");
            }
            return receive();
        }
    }

    private void raiseExceptionIfDisconnectedFromServer(String nameOfAction) {
        if(isClosed) {
            throw new RuntimeException("Cannot " + nameOfAction + " data through a disconnected socket.");
        }
    }
}
