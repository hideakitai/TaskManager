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
        bool b_prev_running {false};

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
        virtual void reset() {};

        bool hasEnter() const { return !b_prev_running && isRunning(); }
        bool hasExit() const { return b_prev_running && !isRunning(); }

        const String& getName() const { return name; }

    protected:
        void callUpdate() {
            if (hasEnter()) {
                b_prev_running = true;
                enter();
            }

            if (isRunning()) update();

            if (hasExit()) {
                b_prev_running = false;
                exit();
            }
        }
    };

}  // namespace task
}  // namespace arduino

#endif  // ARDUINO_TASK_MANAGER_TASK_BASE_H
