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
            task->number(1)->setDurationSec(3);
        })
        ->subtask<Speak>("Sub2", [&](TaskRef<Speak> task) {
            task->number(2)->setDurationSec(3);
        })
        ->subtask<Speak>("Sub3", [&](TaskRef<Speak> task) {
            task->number(3)->setDurationSec(3);
        });

    // You can also choose whether to loop
    bool b_loop = true;
    Tasks["Main"]->startFpsForSec(1., 12, b_loop);
}

void loop() {
    Tasks.update();
}
