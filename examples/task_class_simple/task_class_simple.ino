#include <TaskManager.h>
#include "Blink.h"
#include "Speak.h"

void setup() {
    delay(2000);
    Tasks.add<Blink>()->startFps(1.);
    Tasks.add<Speak>("speak")->startIntervalSec(0.5);
}

void loop() {
    Tasks.update();
}
