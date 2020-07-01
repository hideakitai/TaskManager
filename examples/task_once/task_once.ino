#include <TaskManager.h>

void setup()
{
    Serial.begin(115200);
    delay(2000);

    // task is executed only once after 5000[ms]
    Tasks.once(5000, []{
        Serial.print("once task: now = ");
        Serial.println(millis());
    });

    Serial.print("task start: now = ");
    Serial.println(millis());
}

void loop()
{
    Tasks.update();
}
