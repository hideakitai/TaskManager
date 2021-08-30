#pragma once
#ifndef SPEAK_H  // change depending on your class
#define SPEAK_H  // change depending on your class

#include <TaskManager.h>

class Speak : public Task::Base {
    int num {0};

public:
    Speak(const String& name)
    : Base(name) {}

    virtual ~Speak() {}

    // You can set paramters like builder pattern
    Speak* number(const int n) {
        num = n;
        return this;
    }

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

    // optional (you can remove this method)
    virtual void exit() override {
        Serial.print("Task ");
        Serial.print(getName());
        Serial.println(" exit()");
    }
};

#endif  // SPEAK_H
