package utils.data_structure.optionnal;

import java.util.Optional;

public class OptionalWrapper {

    public static <U> Optional<U> wrap(U u) {
        if(u == null) {
            return Optional.empty();
        }
        return Optional.of(u);
    }
}
