#pragma once
#ifndef ARDUINO_TASK_MANAGER_TASK_BASE_H
#define ARDUINO_TASK_MANAGER_TASK_BASE_H

#include <Arduino.h>
#include "util/PollingTimer/FrameRateCounter.h"

namespace arduino {
namespace task {

    enum class SubTaskMode : uint8_t { NA, PARALLEL, SYNC, SEQUENCE };

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
        size_t subtask_index {0};  // only for SubTaskMode::SEQUENCE

    public:
        Base(const String& name) : FrameRateCounter(), name(name) {};
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

        virtual void stop() override {
            FrameRateCounter::stop();
            for (auto& st : subtasks) st->stop();
            subtask_index = 0;
        }

        virtual void restart() override {
            this->update();  // only in restart()
            for (auto& st : subtasks) {
                if (st->isRunning()) st->update();
            }
            this->stop();
            if (hasExit()) exit_recursive();
            FrameRateCounter::restart();
            if (hasEnter()) enter_recursive();
            releaseEventTrigger();  // disable hasExit()
        }

        virtual void clear() override {
            stop();
            subtasks.clear();
            mode = SubTaskMode::NA;
            subtask_index = 0;
            FrameRateCounter::clear();
        }

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
                if (t->getName() == name) return true;
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
        void begin_recursive() {
            this->begin();
            for (auto& st : subtasks) {
                st->begin();
            }
        }

        void enter_recursive() {
            this->enter();

            int64_t us = usec64();
            switch (getSubTaskMode()) {
                case SubTaskMode::SYNC: {
                    for (auto& st : subtasks) {
                        st->startIntervalFromForSec(getIntervalSec(), getOffsetSec(), getDurationSec());
                        st->setTimeUsec64(us);
                        st->enter();
                    }
                    break;
                }
                case SubTaskMode::SEQUENCE: {
                    startSubTask(0);
                    break;
                }
                default: {
                    break;
                }
            }
        }

        void update_recursive() {
            if (isRunning()) {
                if (hasEnter()) {
                    releaseEventTrigger();  // disable hasExit()
                    enter_recursive();
                }

                if (FrameRateCounter::update()) {
                    this->update();
                }

                if (hasSubTasks()) {
                    switch (getSubTaskMode()) {
                        case SubTaskMode::PARALLEL: {
                            // same procedure with main task
                            // but its behavior is allowd only when the parent task is running
                            auto it = subtasks.begin();
                            while (it != subtasks.end()) {
                                (*it)->update_recursive();
                                if ((*it)->isStopping() && (*it)->isAutoErase()) {
                                    it = subtasks.erase(it);
                                } else {
                                    ++it;
                                }
                            }
                        }
                        case SubTaskMode::SYNC: {
                            // update all subtasks
                            for (auto& st : subtasks) {
                                if (st->FrameRateCounter::update()) {
                                    st->update();
                                }
                            }
                            break;
                        }
                        case SubTaskMode::SEQUENCE: {
                            // update subtasks one by one
                            const size_t idx = getSubTaskIndex();
                            auto st = subtasks[idx];
                            if (st->FrameRateCounter::update()) {
                                st->update();
                            }
                            // for duration ends
                            if (st->hasExit()) {
                                st->releaseEventTrigger();  // disable hasExit()
                                st->exit();
                                if (idx + 1 < numSubTasks()) proceedToNextSubTask();
                            }
                            break;
                        }
                        default: {
                            break;
                        }
                    }
                }
            } else {
                idle_recursive();
            }
            // for external trigger
            if (hasExit()) {
                releaseEventTrigger();  // disable hasExit()
                exit_recursive();
            }
        }

        void exit_recursive() {
            if (hasSubTasks()) {
                switch (getSubTaskMode()) {
                    case SubTaskMode::PARALLEL: {
                        for (auto& st : subtasks) {
                            if (st->isRunning()) st->stop();
                            if (st->hasExit()) {
                                st->releaseEventTrigger();  // disable hasExit()
                                st->exit();
                            }
                        }
                        // if auto erase is enabled, erase it
                        auto it = subtasks.begin();
                        while (it != subtasks.end()) {
                            if ((*it)->isStopping() && (*it)->isAutoErase()) {
                                it = subtasks.erase(it);
                            } else {
                                ++it;
                            }
                        }
                        break;
                    }
                    case SubTaskMode::SYNC: {
                        for (auto& st : subtasks) {
                            if (st->isRunning()) st->stop();
                            if (st->hasExit()) {
                                st->releaseEventTrigger();  // disable hasExit()
                                st->exit();
                            }
                        }
                        break;
                    }
                    case SubTaskMode::SEQUENCE: {
                        // exit active subtask
                        auto st = subtasks[getSubTaskIndex()];
                        if (st->isRunning()) {
                            st->stop();
                        }
                        if (st->hasExit()) {
                            st->releaseEventTrigger();  // disable hasExit()
                            st->exit();
                        }
                        break;
                    }
                    default: {
                        break;
                    }
                }
            }
            subtask_index = 0;
            this->exit();
        }

        void idle_recursive() {
            this->idle();
            for (auto& st : subtasks) {
                st->idle();
            }
        }

        void reset_recursive() {
            for (auto& st : subtasks) {
                st->reset();
            }
            this->reset();
            subtask_index = 0;
        }

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
                if (hasFixedSubTaskDuration()) st->setTimeUsec64(us - getCurrentDurationSecSum() * 1000000);

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
                }
                if (st->hasExit()) {
                    st->releaseEventTrigger();  // disable hasExit()
                    st->exit();
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
                    for (size_t i = 0; i < subtask_index; ++i) d += subtasks[i]->getDurationSec();
                    return d;
                }
            }
            return 0.;
        }
    };

}  // namespace task
}  // namespace arduino

#endif  // ARDUINO_TASK_MANAGER_TASK_BASE_H
