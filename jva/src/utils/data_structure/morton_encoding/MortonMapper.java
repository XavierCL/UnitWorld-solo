package utils.data_structure.morton_encoding;

import clientAis.communications.game_data.Singuity;
import utils.math.vector.Vector2;
import utils.math.vector.Vector2Int;

import java.util.HashMap;
import java.util.List;
import java.util.Map;

public class MortonMapper {

    public static Map<Long, Singuity> mapSinguities(List<Singuity> singuities){
        final Map<Long, Singuity> mortonMap = new HashMap<>();
        singuities.forEach(singuity -> {
            final Vector2Int transformedPositionForMortonEncoding = singuity.position
                    .plus(new Vector2(7500, 7500))
                    .scaled(100)
                    .toVector2Int();
            mortonMap.put(MortonEncoder2D.encode(transformedPositionForMortonEncoding), singuity);
        });
        return mortonMap;
    }
}
