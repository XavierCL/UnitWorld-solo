package utils.unit_world.minion;

import java.util.Set;

public interface MinionWielder {

    void addMinion(final Minion minion);
    void removeMinion(final Minion minion);
    Set<Minion> getMinions();
}
