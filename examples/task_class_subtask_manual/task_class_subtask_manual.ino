#include <TaskManager.h>
#include "Speak.h"

void setup() {
    Serial.begin(115200);
    delay(2000);

    Tasks.add<Speak>("Main")
        // You can set SubTaskMode (default: SubTaskMode::SYNC)
        ->setSubTaskMode(SubTaskMode::SEQUENCE)

        // Add subtasks
        ->subtask<Speak>("Sub1", [&](TaskRef<Speak> task) {
            // configure subtasks by lambda
            task->number(1);
        })
        ->subtask<Speak>("Sub2", [&](TaskRef<Speak> task) {
            task->number(2);
        })
        ->subtask<Speak>("Sub3", [&](TaskRef<Speak> task) {
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
