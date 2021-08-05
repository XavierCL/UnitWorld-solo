package utils.timer;

import java.util.TimerTask;

public class LambdaTimerTaskHelper extends TimerTask {

    private final Runnable runnable;

    public LambdaTimerTaskHelper(Runnable runnable) {
        super();
        this.runnable = runnable;
    }

    @Override
    public void run() {
        runnable.run();
    }
}
