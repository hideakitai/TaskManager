#include <TaskManager.h>
#include "Speak.h"

void setup() {
    delay(2000);
    Tasks.add<Speak>("speak")
        ->number(123)  // you can set required parameter like this
        ->startIntervalSec(0.5);
}

void loop() {
    Tasks.update();
}
