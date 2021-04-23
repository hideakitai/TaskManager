#pragma once
#ifndef MY_EMPTY_TASK_H
#define MY_EMPTY_TASK_H

#include <TaskManager.h>

class EmptyTask : public Task::Base {
public:
    EmptyTask(const String& name)
    : Task::Base(name) {
    }

    virtual ~EmptyTask() {
    }

    // optional (you can remove this method)
    // virtual void begin() override {
    // }

    // optional (you can remove this method)
    // virtual void enter() override {
    // }

    virtual void update() override {
    }

    // optional (you can remove this method)
    // virtual void exit() override {
    // }

    // optional (you can remove this method)
    // virtual void idle() override {
    // }

    // optional (you can remove this method)
    // virtual void reset() override {
    // }
};

#endif  // MY_EMPTY_TASK_H
