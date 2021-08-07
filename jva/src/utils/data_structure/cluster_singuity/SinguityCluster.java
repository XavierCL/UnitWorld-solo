package utils.data_structure.cluster_singuity;

import clientAis.communications.game_data.Singuity;

import java.util.Set;

public class SinguityCluster {

    public final Set<String> singuityIds;

    public SinguityCluster(Set<String> signuityGroup) {
        this.singuityIds = signuityGroup;
    }
}
