#pragma once
#ifndef SPEAK_EVENT_H  // change depending on your class
#define SPEAK_EVENT_H  // change depending on your class

#include <TaskManager.h>

class SpeakEvent : public Task::Base {
public:
    SpeakEvent(const String& name)
    : Task::Base(name) {
        Serial.print("Task ");
        Serial.print(name);
        Serial.println(" has created");
    }

    virtual ~SpeakEvent() {
        Serial.print("Task ");
        Serial.print(getName());
        Serial.println(" will be destructed");
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
        Serial.println(frame());
    }

    // optional (you can remove this method)
    virtual void exit() override {
        Serial.print("Task ");
        Serial.print(getName());
        Serial.println(" exit()");
    }

    // optional (you can remove this method)
    virtual void idle() override {
        // Serial.print("Task ");
        // Serial.print(getName());
        // Serial.println(" idle()");
    }

    // optional (you can remove this method)
    virtual void reset() override {
        Serial.print("Task ");
        Serial.print(getName());
        Serial.println(" reset()");
    }
};

#endif  // SPEAK_EVENT_H
