#pragma once
#ifndef BLINK_H  // change depending on your class
#define BLINK_H  // change depending on your class

#include <Arduino.h>
#include <TaskManager.h>

#ifndef LED_BUILTIN
#define LED_BUILTIN 13  // <- change to your own led pin
#endif

class Blink : public Task::Base {
    bool b;

public:
    Blink(const String& name)
    : Task::Base(name)
    , b(false) {
        pinMode(LED_BUILTIN, OUTPUT);
        digitalWrite(LED_BUILTIN, LOW);
    }

    virtual ~Blink() {}

    virtual void update() override {
        digitalWrite(13, b);
        b = !b;
    }
};

#endif  // BLINK_H
