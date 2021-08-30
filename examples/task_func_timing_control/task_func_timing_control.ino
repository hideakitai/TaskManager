#include <TaskManager.h>

void setup() {
    Serial.begin(115200);
    delay(2000);

    // you should name your tasks to handle them
    // task framerate is 1 and repeat forever
    Tasks.add("task1", [&] {
             Serial.print("framerate task1: frame = ");
             Serial.print(Tasks["task1"]->frame());
             Serial.print(", time = ");
             Serial.println(Tasks["task1"]->msec());
         })
        ->startFps(1);

    // task framerate is 2 and repeat forever
    Tasks.add("task2", [&] {
             Serial.print("framerate task2: frame = ");
             Serial.print(Tasks["task2"]->frame());
             Serial.print(", time = ");
             Serial.println(Tasks["task2"]->msec());
         })
        ->startFps(1);

    Serial.print("task start: now = ");
    Serial.println(millis());
}

void loop() {
    Tasks.update();

    if (Tasks["task1"]->frame() >= 5) {
        Tasks["task1"]->stop();
    }
    if (Tasks["task2"]->frame() >= 10) {
        Tasks["task2"]->stop();
    }
    if (Tasks.getActiveTaskSize() == 0) {
        Tasks["task1"]->start();                // start() runs the task with same setting
        Tasks["task2"]->startIntervalSec(1.5);  // change interval to 1.5sec and start task
    }
}
