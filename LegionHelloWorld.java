import org.legion.Runtime;
import org.legion.TaskFunction;
import org.legion.Context;
import org.legion.Runtime;
import org.legion.Future;
import org.legion.TaskLauncher;

public class LegionHelloWorld {

  static final int TOP_ID   = 1;
  static final int HELLO_ID = 2;

  static class MyTask implements TaskFunction {
    public void task(Context ctx, Runtime rt) {
      System.out.println("Hello world, from sub-task");
    }
  }

  public static void main(String[] args) {

    Runtime.register_task(HELLO_ID, new MyTask());

    Runtime.register_task(TOP_ID, new TaskFunction() {
      public void task(Context ctx, Runtime rt) {
        TaskLauncher launcher = new TaskLauncher(HELLO_ID);
        Future result = rt.executeTask(ctx, launcher);
        result.waitOnComplete();
      }
    });

    Runtime.set_top_level_task_id(TOP_ID);
    Runtime.start(args);
  }
}