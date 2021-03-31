#include <TaskManager.h>
#include "EmptyTask.h"

void setup() {
    Serial.begin(115200);
    delay(2000);

    Tasks.add<EmptyTask>("empty")->startFps(1.);
}

void loop() {
    Tasks.update();
}
