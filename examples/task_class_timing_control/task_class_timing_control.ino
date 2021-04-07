#include <TaskManager.h>
#include "SpeakEvent.h"

void setup() {
    Serial.begin(115200);
    delay(2000);

    Serial.println("add tasks");
    Serial.println("start task1");

    Tasks.add<SpeakEvent>("task1")->startFps(1.);
    Tasks.add<SpeakEvent>("task2")->startInterval(0.5);

    // you can also start by:
    // Tasks.startFps("task1", 1.);       // start all task at once
    // Tasks.startInterval("task2", 0.5); // start "task2"

    Serial.print("all tasks are ");
    Serial.println(Tasks.size());
    Serial.print("active tasks are ");
    Serial.println(Tasks.getActiveTaskSize());
    Serial.print("auto erase : ");
    Serial.println(Tasks.isAutoErase() ? "true" : "false");

    Serial.println("main loop start");
}

void loop() {
    Tasks.update();

    if (Tasks.frame("task1") >= 5 && !Tasks.isRunning("task2")) {
        Serial.print("all tasks are ");
        Serial.println(Tasks.size());
        Serial.print("active tasks are ");
        Serial.println(Tasks.getActiveTaskSize());

        Serial.println("start task2");
        Tasks.start("task2");
    }

    if (Tasks.frame("task1") >= 10) {
        Serial.print("all tasks are ");
        Serial.println(Tasks.size());
        Serial.print("active tasks are ");
        Serial.println(Tasks.getActiveTaskSize());

        Serial.println("stop task1");
        Tasks.stop("task1");
    }

    if (Tasks.frame("task2") >= 15) {
        Serial.print("all tasks are ");
        Serial.println(Tasks.size());
        Serial.print("active tasks are ");
        Serial.println(Tasks.getActiveTaskSize());

        Serial.println("stop task2");
        Tasks.stop("task2");
    }

    if (millis() > 16000 && !Tasks.empty()) {
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

    if (millis() > 17000) {
        Serial.println("end");
        delay(1000);
    }
}
