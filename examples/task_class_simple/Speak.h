#pragma once
#ifndef SPEAK_H  // change depending on your class
#define SPEAK_H  // change depending on your class

#include <TaskManager.h>

class Speak : public Task::Base {
public:
    Speak(const String& name)
    : Base(name) {
        Serial.begin(115200);
    }

    virtual ~Speak() {}

    // optional (you can remove this method)
    virtual void begin() override {
        Serial.print("Task ");
        Serial.print(getName());
        Serial.println(" begin()");
    }

    // optional (you can remove this method)
    virtual void enter() override {
        Serial.print("Task ");
        Serial.print(getName());
        Serial.println(" enter()");
    }

    virtual void update() override {
        Serial.print("Task ");
        Serial.print(getName());
        Serial.print(" update() at frame = ");
        Serial.print(frame());
        Serial.print(", time = ");
        Serial.println(millis());
    }
};

#endif  // SPEAK_H
