#include <TaskManager.h>
#include "Speak.h"

void setup() {
    Serial.begin(115200);
    delay(2000);

    Tasks.add<Speak>("Main")
        // Add subtasks without duration
        ->then<Speak>("Sub1", [&](TaskRef<Speak> task) {
            task->number(1);  // configure subtasks by lambda
        })
        // You can mix the subtasks with duration 3[sec]
        ->then<Speak>("Sub2", 3, [&](TaskRef<Speak> task) {
            task->number(2);
        })
        // You should explicitly call nextSubTask() to exit the subtask without duration
        ->then<Speak>("Sub3", [&](TaskRef<Speak> task) {
            task->number(3);
        });

    Tasks["Main"]->startFps(1.);
}

void loop() {
    Tasks.update();

    while (Serial.available()) {
        char c = Serial.read();

        if (c == 'n') {
            if (!Tasks["Main"]->nextSubTask()) {
                Serial.println("No next task -> restart Main task");
                Tasks["Main"]->restart();
            }
        }
    }
}
