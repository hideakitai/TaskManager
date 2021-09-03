#include <TaskManager.h>
#include "Speak.h"

void setup() {
    Serial.begin(115200);
    delay(2000);

    Tasks.add<Speak>("Main")
        // Add subtasks with duration 3[sec]
        ->then<Speak>("Sub1", 3, [&](TaskRef<Speak> subtask) {
            subtask->number(1);  // configure subtasks inside of lambda
        })
        ->then<Speak>("Sub2", 3, [&](TaskRef<Speak> subtask) {
            subtask->number(2);
        })
        ->then<Speak>("Sub3", 3, [&](TaskRef<Speak> subtask) {
            subtask->number(3);
        });

    // You can also choose whether to loop
    bool b_loop = true;
    Tasks["Main"]->startFpsForSec(1., 12, b_loop);
}

void loop() {
    Tasks.update();
}
