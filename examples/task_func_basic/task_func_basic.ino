#include <TaskManager.h>

void my_task() {
    Serial.print("framerate forever task: now = ");
    Serial.println(millis());
}

void setup() {
    Serial.begin(115200);
    delay(2000);

    // called in every update() and repeat forever (default)
    // start() starts the task without changing previous (default) setting
    Tasks.add([] {
             Serial.print("forever task called every update(): now = ");
             Serial.println(millis());
         })
        ->start();

    // task framerate is 1 and repeat forever
    // Tasks.add([]() {
    //          Serial.print("framerate forever task: now = ");
    //          Serial.println(millis());
    //      })
    //     ->startFps(1);

    // task framerate is 1 and repeat forever (pre-defined function)
    // Tasks.add(my_task)->startFps(1);

    // task framerate is 2 and 10 times only
    // Tasks.add([] {
    //          Serial.print("framerate limited task: now = ");
    //          Serial.println(millis());
    //      })
    //     ->startFpsFor(2, 10);

    // task interval is 1.0[sec] and repeat forever
    // Tasks.add([] {
    //          Serial.print("interval forever task: now = ");
    //          Serial.println(millis());
    //      })
    //     ->startInterval(1.0);

    // task interval is 0.5[sec] and 10 times only
    // Tasks.add([] {
    //          Serial.print("interval limited task: now = ");
    //          Serial.println(millis());
    //      })
    //     ->startIntervalFor(0.5, 10);

    // task is executed only once after 5.0[ms]
    // Tasks.add([] {
    //          Serial.print("once task: now = ");
    //          Serial.println(millis());
    //      })
    //     ->startOnceAfter(5.0);

    Serial.print("task start: now = ");
    Serial.println(millis());
}

void loop() {
    Tasks.update();
}
