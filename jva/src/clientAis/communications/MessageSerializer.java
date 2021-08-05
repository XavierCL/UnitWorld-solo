package clientAis.communications;

import net.minidev.json.JSONObject;
import net.minidev.json.JSONValue;

import java.util.Arrays;
import java.util.List;
import java.util.stream.Collectors;

public class MessageSerializer {

    private String remainingDeserializationBuffer;

    public MessageSerializer() {
        this.remainingDeserializationBuffer = "";
    }

    public String serialize(List<JSONObject> dataList) {
        return "\\n" + dataList.stream()
                .map(JSONObject::toString)
                .collect(Collectors.joining());
    }

    public List<JSONObject> deserialize(String rawData) {
        remainingDeserializationBuffer += rawData;
        String[] rawDataPackets = remainingDeserializationBuffer.split("\\\\n", -1);

        if(rawDataPackets.length > 0) {
            if (rawDataPackets[rawDataPackets.length - 1].equals("\0")) {
                remainingDeserializationBuffer = "";
            } else {
                remainingDeserializationBuffer = rawDataPackets[rawDataPackets.length - 1];
            }
        }

        rawDataPackets = Arrays.copyOfRange(rawDataPackets, 0, rawDataPackets.length-1);

        return Arrays.stream(rawDataPackets)
                .map(string -> (JSONObject) JSONValue.parse(string))
                .collect(Collectors.toList());
    }
}
