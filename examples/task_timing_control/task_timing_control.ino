#include <TaskManager.h>

void setup()
{
    Serial.begin(115200);
    delay(2000);

    // task framerate is 1 and repeat forever
    Tasks.framerate("task1", 1, [&]{
        Serial.print("framerate task1: frame = ");
        Serial.println(Tasks.frame("task1"));
    });

    Tasks.framerate("task2", 1, [&]{
        Serial.print("framerate task2: frame = ");
        Serial.println(Tasks.frame("task2"));
    });

    Serial.print("task start: now = ");
    Serial.println(millis());
}

void loop()
{
    Tasks.update();

    // reset task timer to 0 second in 5 seconds
    static uint32_t prev_ms = millis();
    if (millis() > prev_ms + 5000) {
        Tasks.setTime("task2", 0);
        prev_ms = millis();
    }
}
