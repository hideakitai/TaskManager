#include <TaskManager.h>

void setup()
{
    Serial.begin(115200);
    delay(2000);

    // task framerate is 1 and repeat forever
    Tasks.framerate(1, []{
        Serial.print("framerate forever task: now = ");
        Serial.println(millis());
    });

    // task framerate is 2 and 10 times only
    Tasks.framerate(2, 10, []{
        Serial.print("framerate limited task: now = ");
        Serial.println(millis());
    });

    Serial.print("task start: now = ");
    Serial.println(millis());
}

void loop()
{
    Tasks.update();
}
