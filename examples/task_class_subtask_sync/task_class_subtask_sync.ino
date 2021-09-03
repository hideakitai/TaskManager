#include <TaskManager.h>
#include "Speak.h"

void setup() {
    Serial.begin(115200);
    delay(2000);

    Tasks.add<Speak>("Main")
        ->sync<Speak>("Sub1", [&](TaskRef<Speak> subtask) {
            subtask->number(1);  // configure subtasks inside of lambda
        })
        ->sync<Speak>("Sub2", [&](TaskRef<Speak> subtask) {
            subtask->number(2);
        })
        ->sync<Speak>("Sub3", [&](TaskRef<Speak> subtask) {
            subtask->number(3);
        });

    // You can also choose whether to loop
    bool b_loop = true;
    Tasks["Main"]->startFpsForSec(1., 12, b_loop);
}

void loop() {
    Tasks.update();
}
