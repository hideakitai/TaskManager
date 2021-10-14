#pragma once
#ifndef ARDUINO_TASK_MANAGER_TASK_EMPTY_H
#define ARDUINO_TASK_MANAGER_TASK_EMPTY_H

#include "TaskBase.h"

namespace arduino {
namespace task {

    class TaskEmpty : public Base {
    public:
        TaskEmpty(const String& name) : Base(name) {}
        virtual ~TaskEmpty() {}
        virtual void update() override {}
    };

}  // namespace task
}  // namespace arduino

#endif  // ARDUINO_TASK_MANAGER_TASK_EMPTY_H
