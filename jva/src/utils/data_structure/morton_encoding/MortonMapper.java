package utils.data_structure.morton_encoding;

import clientAis.communications.game_data.Singuity;
import utils.math.vector.Vector2;
import utils.math.vector.Vector2Int;

import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.function.Function;

public class MortonMapper {

    public static <T> Map<Long, T> mapElements(List<T> elements, Function<T, Vector2> objectPositionMapper){
        final Map<Long, T> mortonMap = new HashMap<>(elements.size());

        elements.forEach(element -> {
            final Vector2Int transformedPositionForMortonEncoding = objectPositionMapper.apply(element)
                    .plus(new Vector2(7500, 7500))
                    .scaled(100)
                    .toVector2Int();
            mortonMap.put(MortonEncoder2D.encode(transformedPositionForMortonEncoding), element);
        });

        return mortonMap;
    }
}
