#include <TaskManager.h>
#include "Speak.h"

void setup() {
    Serial.begin(115200);
    delay(2000);

    Tasks.add<Speak>("Main")
        // Just iadd subtasks by subtask() method
        // This is completely same as usual tasks other than
        // it can be controlled only while parent task is running
        ->subtask<Speak>("Sub1", [&](TaskRef<Speak> task) {
            task->number(1);  // configure subtasks by lambda
            task->setAutoErase(true);
        })
        ->subtask<Speak>("Sub2", [&](TaskRef<Speak> task) {
            task->number(2);
            task->setAutoErase(false);
        })
        ->subtask<Speak>("Sub3", [&](TaskRef<Speak> task) {
            task->number(3);
            task->setAutoErase(false);
        });

    Tasks["Main"]->startFps(1.);
    (*Tasks["Main"])["Sub1"]->startFps(1.);
}

void loop() {
    Tasks.update();

    if (Tasks["Main"]->existsSubTask("Sub1")) {
        auto st = (*Tasks["Main"])["Sub1"];
        if (st->isRunning() && (st->frame() >= 3.)) {
            st->stop();
            if (Tasks["Main"]->existsSubTask("Sub2"))
                (*Tasks["Main"])["Sub2"]->startFps(1.);
        }
    }

    if (Tasks["Main"]->existsSubTask("Sub2")) {
        auto st = (*Tasks["Main"])["Sub2"];
        if (st->isRunning() && (st->frame() >= 3.)) {
            st->stop();
            if (Tasks["Main"]->existsSubTask("Sub3"))
                (*Tasks["Main"])["Sub3"]->startFps(1.);
        }
    }

    if (Tasks["Main"]->existsSubTask("Sub3")) {
        auto st = (*Tasks["Main"])["Sub3"];
        if (st->isRunning() && (st->frame() >= 3.)) {
            st->stop();
            if (Tasks["Main"]->existsSubTask("Sub1"))
                (*Tasks["Main"])["Sub1"]->startFps(1.);
        }
    }
}
