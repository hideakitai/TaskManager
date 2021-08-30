#include <TaskManager.h>
#include "SpeakEvent.h"

void setup() {
    Serial.begin(115200);
    delay(2000);

    Serial.println("add tasks");
    Serial.println("start task1");

    Tasks.add<SpeakEvent>("task1")->startFps(2);
    Tasks.add<SpeakEvent>("task2");

    // you can also start by:
    // Tasks.startFps(1.);                     // start all task at once
    // Tasks["task2"]->startIntervalSec(0.5);  // start "task2"

    Serial.print("all tasks are ");
    Serial.println(Tasks.size());
    Serial.print("active tasks are ");
    Serial.println(Tasks.getActiveTaskSize());

    Serial.println("main loop start");
}

void loop() {
    Tasks.update();

    if (Tasks["task1"]->frame() >= 5 && !Tasks["task2"]->isRunning()) {
        Serial.print("all tasks are ");
        Serial.println(Tasks.size());
        Serial.print("active tasks are ");
        Serial.println(Tasks.getActiveTaskSize());

        Serial.println("start task2");
        Tasks["task2"]->startFps(2);
    }

    if (Tasks["task1"]->frame() >= 10) {
        Serial.print("all tasks are ");
        Serial.println(Tasks.size());
        Serial.print("active tasks are ");
        Serial.println(Tasks.getActiveTaskSize());

        Serial.println("stop task1");
        Tasks["task1"]->stop();
    }

    if (Tasks["task2"]->frame() >= 7) {
        Serial.print("all tasks are ");
        Serial.println(Tasks.size());
        Serial.print("active tasks are ");
        Serial.println(Tasks.getActiveTaskSize());

        Serial.println("stop task2");
        Tasks["task2"]->stop();
    }

    if (millis() > 10000 && !Tasks.empty()) {
        Serial.print("all tasks are ");
        Serial.println(Tasks.size());
        Serial.print("active tasks are ");
        Serial.println(Tasks.getActiveTaskSize());

        Serial.println("erase task1");
        Tasks.erase("task1");
        Serial.println("erase task2");
        Tasks.erase("task2");

        Serial.print("all tasks are ");
        Serial.println(Tasks.size());
        Serial.print("active tasks are ");
        Serial.println(Tasks.getActiveTaskSize());
    }

    if (Tasks.empty()) {
        Serial.println("end");
        while (true)
            ;
    }
}
