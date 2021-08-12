package utils.state_machine;

public interface State<I, O> extends Behaviour<I, O>, Transition<I, O>, Debuggable<I>, Startable<I>, Stopable<I>, Nameable {}
