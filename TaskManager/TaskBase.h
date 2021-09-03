#pragma once
#ifndef ARDUINO_TASK_MANAGER_TASK_BASE_H
#define ARDUINO_TASK_MANAGER_TASK_BASE_H

#include <Arduino.h>
#include "util/PollingTimer/FrameRateCounter.h"

namespace arduino {
namespace task {

    enum class SubTaskMode : uint8_t {
        NA,
        PARALLEL,
        SYNC,
        SEQUENCE
    };

    class Manager;
    class TaskEmpty;

    class Base : public FrameRateCounter {
        friend class Manager;

    protected:
        String name;
        bool b_auto_erase {false};

        // for SubTask
        Vec<Ref<Base>> subtasks;
        SubTaskMode mode {SubTaskMode::NA};
        // only for SubTaskMode::SEQUENCE
        uint8_t subtask_index {0};

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
        Base* subtask(const std::function<void(Ref<TaskType>)>& setup) {
            return subtask("", setup);
        }
        template <typename TaskType>
        Base* subtask(const String& name, const std::function<void(Ref<TaskType>)>& setup) {
            if ((mode != SubTaskMode::NA) && (mode != SubTaskMode::PARALLEL)) {
                LOG_ERROR("All subtask should be same mode (should be added by same method)");
                return nullptr;
            }
            setSubTaskMode(SubTaskMode::PARALLEL);
            Ref<TaskType> t = std::make_shared<TaskType>(name);
            subtasks.emplace_back(t);
            t->begin();
            setup(t);
            return this;
        }

        template <typename TaskType>
        Base* sync(const std::function<void(Ref<TaskType>)>& setup) {
            return sync("", setup);
        }
        template <typename TaskType>
        Base* sync(const String& name, const std::function<void(Ref<TaskType>)>& setup) {
            if ((mode != SubTaskMode::NA) && (mode != SubTaskMode::SYNC)) {
                LOG_ERROR("All subtask should be same mode (should be added by same method)");
                return nullptr;
            }
            setSubTaskMode(SubTaskMode::SYNC);
            Ref<TaskType> t = std::make_shared<TaskType>(name);
            subtasks.emplace_back(t);
            t->begin();
            setup(t);
            return this;
        }

        template <typename TaskType>
        Base* then(const std::function<void(Ref<TaskType>)>& setup) {
            return then("", 0, setup);
        }
        template <typename TaskType>
        Base* then(const String& name, const std::function<void(Ref<TaskType>)>& setup) {
            return then(name, 0, setup);
        }
        template <typename TaskType>
        Base* then(const double sec, const std::function<void(Ref<TaskType>)>& setup) {
            return then("", sec, setup);
        }
        template <typename TaskType>
        Base* then(const String& name, const double sec, const std::function<void(Ref<TaskType>)>& setup) {
            if ((mode != SubTaskMode::NA) && (mode != SubTaskMode::SEQUENCE)) {
                LOG_ERROR("All subtask should be same mode (should be added by same method)");
                return nullptr;
            }
            setSubTaskMode(SubTaskMode::SEQUENCE);
            Ref<TaskType> t = std::make_shared<TaskType>(name);
            subtasks.emplace_back(t);
            t->setDurationSec(sec);
            t->begin();
            setup(t);
            return this;
        }

        Base* hold(const double sec) {
            return then<TaskEmpty>("", sec, [](Ref<TaskEmpty>) {});
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

        bool existsSubTask(const String& name) const {
            for (auto& t : subtasks)
                if (t->getName() == name)
                    return true;
            return false;
        }

        template <typename TaskType = Base>
        Ref<TaskType> getSubTaskByName(const String& name) const {
            for (auto& t : subtasks)
                if (t->getName() == name)
#if ARX_HAVE_LIBSTDCPLUSPLUS >= 201103L  // Have libstdc++11
                    return std::static_pointer_cast<TaskType>(t);
#else
                    return (Ref<TaskType>)t;
#endif
            LOG_ERROR("No task found named", name);
            return nullptr;
        }

        template <typename TaskType = Base>
        Ref<TaskType> getSubTaskByIndex(const size_t i) const {
            if (i >= subtasks.size()) {
                LOG_ERROR("Task index is out of bound:", i, "should be <", subtasks.size());
                return nullptr;
            }

#if ARX_HAVE_LIBSTDCPLUSPLUS >= 201103L  // Have libstdc++11
            return std::static_pointer_cast<TaskType>(subtasks[i]);
#else
            return (Ref<TaskType>)subtasks[i];
#endif
        }

        template <typename TaskType = Base>
        Ref<TaskType> operator[](const String& name) const {
            return getSubTaskByName(name);
        }

        template <typename TaskType = Base>
        Ref<TaskType> operator[](const size_t i) const {
            return getSubTaskByIndex(i);
        }

        // ========== only for SubTaskMode::SEQUENCE ==========

        bool nextSubTask() {
            if (mode == SubTaskMode::SEQUENCE) {
                if (!hasFixedSubTaskDuration())
                    return nextSubTaskImpl();
                else {
                    LOG_ERROR("Couldn't run next subtask: At least one subtask should NOT have duration");
                    return false;
                }
            } else {
                LOG_ERROR("Couldn't run next subtask: SubTaskMode should be SEQUENCE");
                return false;
            }
        }

    private:
        bool startSubTask(const size_t idx) {
            if (idx < numSubTasks()) {
                int64_t us = this->usec64();
                subtask_index = idx;
                auto st = subtasks[idx];
                double interval_sec = st->hasInterval() ? st->getIntervalSec() : getIntervalSec();
                double offset_sec = st->hasOffset() ? st->getOffsetSec() : getOffsetSec();
                double duration_sec = st->hasDuration() ? st->getDurationSec() : getDurationSec();
                st->startIntervalFromForSec(interval_sec, offset_sec, duration_sec);

                // compensate the time difference of main task and sub tasks
                if (hasFixedSubTaskDuration())
                    st->setTimeUsec64(us - getCurrentDurationSecSum() * 1000000);

                st->enter();
                return true;
            } else {
                LOG_ERROR("Couldn't run next subtask: index", idx, "should <", numSubTasks());
                return false;
            }
        }

        bool nextSubTaskImpl() {
            if (mode == SubTaskMode::SEQUENCE) {
                auto st = subtasks[subtask_index];
                if (st->isRunning()) {
                    st->stop();
                    if (st->hasExit()) {
                        subtasks[subtask_index]->exit();
                    }
                }
                if (subtask_index + 1 < subtasks.size())
                    return startSubTask(subtask_index + 1);
                else {
                    LOG_WARN("No more subtasks : index", subtask_index, "size", numSubTasks());
                    return false;
                }
            } else {
                LOG_ERROR("Couldn't run next subtask: SubTaskMode should be SEQUENCE");
                return false;
            }
        }

        bool proceedToNextSubTask() {
            if (mode == SubTaskMode::SEQUENCE) {
                return nextSubTaskImpl();
            } else {
                LOG_ERROR("Couldn't run next subtask: SubTaskMode should be SEQUENCE");
                return false;
            }
        }

        bool hasFixedSubTaskDuration() const {
            for (const auto& st : subtasks)
                if (!st->hasDuration()) return false;
            return true;
        }

        double getCurrentDurationSec() const {
            if (mode == SubTaskMode::SEQUENCE)
                return subtasks[subtask_index]->getDurationSec();
            else
                return 0.;
        }

        double getCurrentDurationSecSum() const {
            if (mode == SubTaskMode::SEQUENCE) {
                if (hasFixedSubTaskDuration()) {
                    double d = 0.;
                    for (size_t i = 0; i < subtask_index; ++i)
                        d += subtasks[i]->getDurationSec();
                    return d;
                }
            }
            return 0.;
        }
    };

}  // namespace task
}  // namespace arduino

#endif  // ARDUINO_TASK_MANAGER_TASK_BASE_H
