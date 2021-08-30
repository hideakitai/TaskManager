#pragma once
#ifndef ARDUINO_TASK_MANAGER_TASK_BASE_H
#define ARDUINO_TASK_MANAGER_TASK_BASE_H

#include <Arduino.h>
#include "util/PollingTimer/FrameRateCounter.h"

namespace arduino {
namespace task {

    enum class SubTaskMode : uint8_t {
        SYNC,
        SEQUENCE
    };

    class Manager;

    class Base : public FrameRateCounter {
        friend class Manager;

    protected:
        String name;
        bool b_auto_erase {false};

        // for SubTask
        Vec<Ref<Base>> subtasks;
        SubTaskMode mode {SubTaskMode::SYNC};
        // only for SubTaskMode::SEQUENCE
        uint8_t subtask_index {0};
        Vec<double> subtask_durations;

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

        bool hasEnter() const {
            return this->hasStarted();
        }
        bool hasExit() const {
            return this->hasStopped();
        }

        Base* setAutoErase(const bool b) {
            b_auto_erase = b;
            return this;
        }
        bool isAutoErase() const {
            return b_auto_erase;
        }

        const String& getName() const {
            return name;
        }

        // =========== for SubTask ==========

        template <typename TaskType>
        Base* subtask(const String& name, std::function<void(Ref<TaskType>)> setup) {
            Ref<TaskType> t = std::make_shared<TaskType>(name);
            subtasks.emplace_back(t);
            t->begin();
            setup(t);
            subtask_durations.emplace_back(t->getDurationUsec());
            return this;
        }

        Vec<Ref<Base>>& getSubTasks() {
            return subtasks;
        }
        const Vec<Ref<Base>>& getSubTasks() const {
            return subtasks;
        }

        Base* setSubTaskIndex(const size_t i) {
            subtask_index = i;
            return this;
        }
        size_t getSubTaskIndex() const {
            if (mode == SubTaskMode::SEQUENCE)
                return subtask_index;
            else
                return 0;
        }

        Base* setSubTaskMode(const SubTaskMode m) {
            mode = m;
            return this;
        }
        SubTaskMode getSubTaskMode() const {
            return mode;
        }

        bool hasSubTasks() const {
            return !subtasks.empty();
        }

        size_t numSubTasks() const {
            return subtasks.size();
        }

        // ========== only for SubTaskMode::SEQUENCE ==========

        double getCurrentDurationSec() const {
            if (mode == SubTaskMode::SEQUENCE)
                return subtasks[subtask_index]->getDurationSec();
            else
                return 0.;
        }

        double getCurrentDurationSecSum() const {
            if (mode == SubTaskMode::SEQUENCE) {
                double d = 0.;
                for (size_t i = 0; i < subtask_index; ++i)
                    d += subtasks[i]->getDurationSec();
                return d;
            } else {
                return 0.;
            }
        }
    };

}  // namespace task
}  // namespace arduino

#endif  // ARDUINO_TASK_MANAGER_TASK_BASE_H
