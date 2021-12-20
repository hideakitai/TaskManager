#pragma once
#ifndef ARDUINO_TASK_MANAGER_TASK_EMPTY_H
#define ARDUINO_TASK_MANAGER_TASK_EMPTY_H

#include "TaskBase.h"

namespace arduino {
namespace task {

    class TaskEmpty : public Base {
        std::function<void(Base*)> func;

    public:
        TaskEmpty(const String& name) : Base(name) {}
        virtual ~TaskEmpty() {}
        virtual void update() override {
            if (func) func(this);
        }

        void add_update_func(const std::function<void(Base*)> f) {
            func = f;
        }
    };

}  // namespace task
}  // namespace arduino

#endif  // ARDUINO_TASK_MANAGER_TASK_EMPTY_H
