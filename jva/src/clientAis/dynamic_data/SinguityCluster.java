package clientAis.dynamic_data;

import clientAis.communications.game_data.Singuity;

import java.util.List;
import java.util.stream.Collectors;

public class SinguityCluster {

    private final List<String> cluster;

    public SinguityCluster(List<Singuity> signuityGroup) {
        this.cluster = signuityGroup.stream().map(singuity -> singuity.id).collect(Collectors.toList());
    }
}
