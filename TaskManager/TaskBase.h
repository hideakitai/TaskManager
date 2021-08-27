#pragma once
#ifndef ARDUINO_TASK_MANAGER_TASK_BASE_H
#define ARDUINO_TASK_MANAGER_TASK_BASE_H

#include <Arduino.h>
#include "util/PollingTimer/FrameRateCounter.h"

namespace arduino {
namespace task {

    class Manager;

    class Base : public FrameRateCounter {
        friend class Manager;

    protected:
        String name;
        bool b_auto_erase {false};

    public:
        Base(const String& name)
        : FrameRateCounter(), name(name) {};
        Base(const Base&) = default;
        Base& operator=(const Base&) = default;
        Base(Base&&) = default;
        Base& operator=(Base&&) = default;
        virtual ~Base() = default;

        virtual void begin() {};
        virtual void enter() {};
        virtual void update() = 0;
        virtual void exit() {};
        virtual void idle() {};
        virtual void reset() {};

        bool hasEnter() const { return this->hasStarted(); }
        bool hasExit() const { return this->hasStopped(); }

        Base* setAutoErase(const bool b) {
            b_auto_erase = b;
            return this;
        }
        bool isAutoErase() const { return b_auto_erase; }

        const String& getName() const { return name; }
    };

}  // namespace task
}  // namespace arduino

#endif  // ARDUINO_TASK_MANAGER_TASK_BASE_H
