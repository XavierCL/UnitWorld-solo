package utils.state_machine;

public interface Stopable<I> {
    void stop(I input);
}
