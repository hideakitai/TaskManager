#include <TaskManager.h>

void setup() {
    Serial.begin(115200);
    delay(2000);

    // you should name your tasks to handle them
    // task framerate is 1 and repeat forever
    Tasks.add("task1", [&] {
             Serial.print("framerate task1: frame = ");
             Serial.print(Tasks.frame("task1"));
             Serial.print(", time = ");
             Serial.println(Tasks.msec("task1"));
         })
        ->startFps(1);

    // task framerate is 2 and repeat forever
    Tasks.add("task2", [&] {
             Serial.print("framerate task2: frame = ");
             Serial.print(Tasks.frame("task2"));
             Serial.print(", time = ");
             Serial.println(Tasks.msec("task2"));
         })
        ->startFps(1);

    Serial.print("task start: now = ");
    Serial.println(millis());
}

void loop() {
    Tasks.update();

    if (Tasks.frame("task1") >= 5) {
        Tasks.stop("task1");
    }
    if (Tasks.frame("task2") >= 10) {
        Tasks.stop("task2");
    }
    if (Tasks.getActiveTaskSize() == 0) {
        Tasks.start("task1");               // start() runs the task with same setting
        Tasks.startInterval("task2", 1.5);  // change interval to 1.5sec and start task
    }
}
