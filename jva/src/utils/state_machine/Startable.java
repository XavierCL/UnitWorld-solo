package utils.state_machine;

public interface Startable<I> {
    void start(I input);
}
