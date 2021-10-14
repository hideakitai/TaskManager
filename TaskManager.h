#pragma once
#ifndef ARDUINO_TASK_MANAGER_H
#define ARDUINO_TASK_MANAGER_H

#include <Arduino.h>

#include "TaskManager/util/ArxContainer/ArxContainer.h"
#include "TaskManager/util/ArxSmartPtr/ArxSmartPtr.h"
#include "TaskManager/util/TeensyDirtySTLErrorSolution/TeensyDirtySTLErrorSolution.h"
#include "TaskManager/util/DebugLog/DebugLog.h"

#ifdef TASKMANAGER_DEBUGLOG_ENABLE
#include "TaskManager/util/DebugLog/DebugLogEnable.h"
#else
#include "TaskManager/util/DebugLog/DebugLogDisable.h"
#endif

#if ARX_HAVE_LIBSTDCPLUSPLUS >= 201103L  // Have libstdc++11
#include <algorithm>
#include <iterator>
#endif

namespace arduino {
namespace task {

    template <typename T>
    using Ref = std::shared_ptr<T>;
    using Func = std::function<void(void)>;

#if ARX_HAVE_LIBSTDCPLUSPLUS >= 201103L  // Have libstdc++11
    template <typename T>
    using Vec = std::vector<T>;
    using namespace std;
#else
    template <typename T>
    using Vec = arx::vector<T>;
    using namespace arx;
#endif

}  // namespace task
}  // namespace arduino

#include "TaskManager/TaskBase.h"
#include "TaskManager/TaskEmpty.h"

namespace arduino {
namespace task {

    using TaskList = Vec<Ref<Base>>;

    class Manager {
        Manager() {}
        Manager(const Manager&) = delete;
        Manager& operator=(const Manager&) = delete;

        TaskList tasks;

    public:
        static Manager& get() {
            static Manager m;
            return m;
        }

        Ref<TaskEmpty> add(const Func& task) {
            return add("", task);
        }

        Ref<TaskEmpty> add(const String& name, const Func& task) {
            Ref<TaskEmpty> t = std::make_shared<TaskEmpty>(name);
            t->onUpdate(task);
            tasks.emplace_back(t);
            t->begin_recursive();
            return t;
        }

        template <typename TaskType>
        Ref<TaskType> add() {
            return add<TaskType>("");
        }

        template <typename TaskType>
        Ref<TaskType> add(const String& name) {
            Ref<TaskType> t = std::make_shared<TaskType>(name);
            tasks.emplace_back(t);
            t->begin_recursive();
            return t;
        }

        void update() {
            auto it = tasks.begin();
            while (it != tasks.end()) {
                (*it)->update_recursive();
                if ((*it)->isStopping() && (*it)->isAutoErase()) {
                    it = tasks.erase(it);
                } else {
                    ++it;
                }
            }
        }
        void update(const String& name) {
            auto task = getTaskByName(name);
            if (task) {
                task->update_recursive();
                if (task->isStopping() && task->isAutoErase()) {
                    erase(name);
                }
            }
        }
        void update(const size_t idx) {
            auto task = getTaskByIndex(idx);
            if (task) {
                task->update_recursive();
                if (task->isStopping() && task->isAutoErase()) {
                    erase(idx);
                }
            }
        }

        void reset() {
            for (auto& t : tasks) t->reset_recursive();
        }
        bool reset(const String& name) {
            auto t = getTaskByName(name);
            if (t) {
                t->reset_recursive();
                return true;
            }
            return false;
        }
        bool reset(const size_t idx) {
            auto t = getTaskByIndex(idx);
            if (t) {
                t->reset_recursive();
                return true;
            }
            return false;
        }

        bool erase(const String& name) {
#if ARX_HAVE_LIBSTDCPLUSPLUS >= 201103L  // Have libstdc++11
            auto results =
                std::remove_if(tasks.begin(), tasks.end(), [&](const Ref<Base>& t) { return (t->getName() == name); });
            auto it = tasks.erase(results, tasks.end());
            return it != tasks.end();
#else
            auto it = tasks.begin();
            while (it != tasks.end()) {
                if ((*it)->getName() == name) {
                    it = tasks.erase(it);
                    return true;
                } else
                    ++it;
            }
            return false;
#endif
        }
        bool erase(const size_t idx) {
            if (idx >= tasks.size()) return false;
            auto it = tasks.begin() + idx;
            tasks.erase(it);
            return true;
        }

        void clear() {
            tasks.clear();
        }

        bool empty() const {
            return tasks.size() == 0;
        }

        size_t size() const {
            return tasks.size();
        }

        bool exists(const String& name) const {
            for (auto& t : tasks)
                if (t->getName() == name) return true;
            return false;
        }

        size_t getActiveTaskSize() const {
            size_t i = 0;
            for (const auto& t : tasks) {
                if (t->isRunning()) ++i;
            }
            return i;
        }

        void setAutoErase(const bool b) {
            for (auto& t : tasks) {
                t->setAutoErase(b);
            }
        }

        template <typename TaskType = Base>
        Ref<TaskType> getTaskByName(const String& name) const {
            for (auto& t : tasks)
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
        Ref<TaskType> getTaskByIndex(const size_t i) const {
            if (i >= tasks.size()) {
                LOG_ERROR("Task index is out of bound:", i, "should be <", tasks.size());
                return nullptr;
            }

#if ARX_HAVE_LIBSTDCPLUSPLUS >= 201103L  // Have libstdc++11
            return std::static_pointer_cast<TaskType>(tasks[i]);
#else
            return (Ref<TaskType>)tasks[i];
#endif
        }

        template <typename TaskType = Base>
        Ref<TaskType> operator[](const String& name) const {
            return getTaskByName(name);
        }

        template <typename TaskType = Base>
        Ref<TaskType> operator[](const size_t i) const {
            return getTaskByIndex(i);
        }

        // ========== Task method wrappers ==========

        void start() {
            for (auto& t : tasks) t->start();
        }

        void startFromSec(const double from_sec) {
            for (auto& t : tasks) t->startFromSec(from_sec);
        }
        void startFromMsec(const double from_ms) {
            for (auto& t : tasks) t->startFromMsec(from_ms);
        }
        void startFromUsec(const double from_us) {
            for (auto& t : tasks) t->startFromUsec(from_us);
        }

        void startForSec(const double for_sec, const bool loop = false) {
            for (auto& t : tasks) t->startForSec(for_sec, loop);
        }
        void startForMsec(const double for_ms, const bool loop = false) {
            for (auto& t : tasks) t->startForMsec(for_ms, loop);
        }
        void startForUsec(const double for_us, const bool loop = false) {
            for (auto& t : tasks) t->startForUsec(for_us, loop);
        }

        void startFromForSec(const double from_sec, const double for_sec, const bool loop = false) {
            for (auto& t : tasks) t->startFromForSec(from_sec, for_sec, loop);
        }
        void startFromForMsec(const double from_ms, const double for_ms, const bool loop = false) {
            for (auto& t : tasks) t->startFromForMsec(from_ms, for_ms, loop);
        }
        void startFromForUsec(const double from_us, const double for_us, const bool loop = false) {
            for (auto& t : tasks) t->startFromForUsec(from_us, for_us, loop);
        }
        void startFromForUsec64(const int64_t from_us, const int64_t for_us, const bool loop = false) {
            for (auto& t : tasks) t->startFromForUsec64(from_us, for_us, loop);
        }

        void startFromCount(const double from_count) {
            for (auto& t : tasks) t->startFromCount(from_count);
        }
        void startForCount(const double for_count, const bool loop = false) {
            for (auto& t : tasks) t->startForCount(for_count, loop);
        }
        void startFromForCount(const double from_count, const double for_count, const bool loop = false) {
            for (auto& t : tasks) t->startFromForCount(from_count, for_count, loop);
        }

        void startIntervalSec(const double interval_sec) {
            for (auto& t : tasks) t->startIntervalSec(interval_sec);
        }
        void startIntervalMsec(const double interval_ms) {
            for (auto& t : tasks) t->startIntervalMsec(interval_ms);
        }
        void startIntervalUsec(const double interval_us) {
            for (auto& t : tasks) t->startIntervalUsec(interval_us);
        }

        void startIntervalFromSec(const double interval_sec, const double from_sec) {
            for (auto& t : tasks) t->startIntervalFromSec(interval_sec, from_sec);
        }
        void startIntervalFromMsec(const double interval_ms, const double from_ms) {
            for (auto& t : tasks) t->startIntervalFromMsec(interval_ms, from_ms);
        }
        void startIntervalFromUsec(const double interval_us, const double from_us) {
            for (auto& t : tasks) t->startIntervalFromUsec(interval_us, from_us);
        }
        void startIntervalSecFromCount(const double interval_sec, const double from_count) {
            for (auto& t : tasks) t->startIntervalSecFromCount(interval_sec, from_count);
        }
        void startIntervalMsecFromCount(const double interval_ms, const double from_count) {
            for (auto& t : tasks) t->startIntervalMsecFromCount(interval_ms, from_count);
        }
        void startIntervalUsecFromCount(const double interval_us, const double from_count) {
            for (auto& t : tasks) t->startIntervalUsecFromCount(interval_us, from_count);
        }

        void startIntervalForSec(const double interval_sec, const double for_sec, const bool loop = false) {
            for (auto& t : tasks) t->startIntervalForSec(interval_sec, for_sec, loop);
        }
        void startIntervalForMsec(const double interval_ms, const double for_ms, const bool loop = false) {
            for (auto& t : tasks) t->startIntervalForMsec(interval_ms, for_ms, loop);
        }
        void startIntervalForUsec(const double interval_us, const double for_us, const bool loop = false) {
            for (auto& t : tasks) t->startIntervalForUsec(interval_us, for_us, loop);
        }
        void startIntervalSecForCount(const double interval_sec, const double for_count, const bool loop = false) {
            for (auto& t : tasks) t->startIntervalSecForCount(interval_sec, for_count, loop);
        }
        void startIntervalMsecForCount(const double interval_ms, const double for_count, const bool loop = false) {
            for (auto& t : tasks) t->startIntervalMsecForCount(interval_ms, for_count, loop);
        }
        void startIntervalUsecForCount(const double interval_us, const double for_count, const bool loop = false) {
            for (auto& t : tasks) t->startIntervalUsecForCount(interval_us, for_count, loop);
        }

        void startIntervalFromForSec(
            const double interval_sec, const double from_sec, const double for_sec, const bool loop = false) {
            for (auto& t : tasks) t->startIntervalFromForSec(interval_sec, from_sec, for_sec, loop);
        }
        void startIntervalFromForMsec(
            const double interval_ms, const double from_ms, const double for_ms, const bool loop = false) {
            for (auto& t : tasks) t->startIntervalFromForMsec(interval_ms, from_ms, for_ms, loop);
        }
        void startIntervalFromForUsec(
            const double interval_us, const double from_us, const double for_us, const bool loop = false) {
            for (auto& t : tasks) t->startIntervalFromForUsec(interval_us, from_us, for_us, loop);
        }
        void startIntervalSecFromForCount(
            const double interval_sec, const double from_count, const double for_count, const bool loop = false) {
            for (auto& t : tasks) t->startIntervalSecFromForCount(interval_sec, from_count, for_count, loop);
        }
        void startIntervalMsecFromForCount(
            const double interval_ms, const double from_count, const double for_count, const bool loop = false) {
            for (auto& t : tasks) t->startIntervalMsecFromForCount(interval_ms, from_count, for_count, loop);
        }
        void startIntervalUsecFromForCount(
            const double interval_us, const double from_count, const double for_count, const bool loop = false) {
            for (auto& t : tasks) t->startIntervalUsecFromForCount(interval_us, from_count, for_count, loop);
        }

        void startFromFrame(const double from_frame) {
            for (auto& t : tasks) t->startFromFrame(from_frame);
        }

        void startForFrame(const double for_frame, const bool loop = false) {
            for (auto& t : tasks) t->startForFrame(for_frame, loop);
        }

        void startFromForFrame(const double from_frame, const double for_frame, const bool loop = false) {
            for (auto& t : tasks) t->startFromForFrame(from_frame, for_frame, loop);
        }

        void startFps(const double fps) {
            for (auto& t : tasks) t->startFps(fps);
        }

        void startFpsFromSec(const double fps, const double from_sec) {
            for (auto& t : tasks) t->startFpsFromSec(fps, from_sec);
        }
        void startFpsFromMsec(const double fps, const double from_ms) {
            for (auto& t : tasks) t->startFpsFromMsec(fps, from_ms);
        }
        void startFpsFromUsec(const double fps, const double from_us) {
            for (auto& t : tasks) t->startFpsFromUsec(fps, from_us);
        }
        void startFpsFromFrame(const double fps, const double from_frame) {
            for (auto& t : tasks) t->startFpsFromFrame(fps, from_frame);
        }

        void startFpsForSec(const double fps, const double for_sec, const bool loop = false) {
            for (auto& t : tasks) t->startFpsForSec(fps, for_sec, loop);
        }
        void startFpsForMsec(const double fps, const double for_ms, const bool loop = false) {
            for (auto& t : tasks) t->startFpsForMsec(fps, for_ms, loop);
        }
        void startFpsForUsec(const double fps, const double for_us, const bool loop = false) {
            for (auto& t : tasks) t->startFpsForUsec(fps, for_us, loop);
        }
        void startFpsForFrame(const double fps, const double for_frame, const bool loop = false) {
            for (auto& t : tasks) t->startFpsForFrame(fps, for_frame, loop);
        }

        void startFpsFromForSec(
            const double fps, const double from_sec, const double for_sec, const bool loop = false) {
            for (auto& t : tasks) t->startFpsFromForSec(fps, from_sec, for_sec, loop);
        }
        void startFpsFromForMsec(
            const double fps, const double from_ms, const double for_ms, const bool loop = false) {
            for (auto& t : tasks) t->startFpsFromForMsec(fps, from_ms, for_ms, loop);
        }
        void startFpsFromForUsec(
            const double fps, const double from_us, const double for_us, const bool loop = false) {
            for (auto& t : tasks) t->startFpsFromForUsec(fps, from_us, for_us, loop);
        }
        void startFpsFromForFrame(
            const double fps, const double from_frame, const double for_frame, const bool loop = false) {
            for (auto& t : tasks) t->startFpsFromForFrame(fps, from_frame, for_frame, loop);
        }

        void startOnce() {
            for (auto& t : tasks) t->startOnce();
        }
        void startOnceAfterSec(const double after_sec) {
            for (auto& t : tasks) t->startOnceAfterSec(after_sec);
        }
        void startOnceAfterMsec(const double after_ms) {
            for (auto& t : tasks) t->startOnceAfterMsec(after_ms);
        }
        void startOnceAfterUsec(const double after_us) {
            for (auto& t : tasks) t->startOnceAfterUsec(after_us);
        }

        void stop() {
            for (auto& t : tasks) t->stop();
        }

        void play() {
            for (auto& t : tasks) t->play();
        }

        void pause() {
            for (auto& t : tasks) t->pause();
        }

        void restart() {
            for (auto& t : tasks) t->restart();
        }

        void setOffsetSec(const double sec) {
            for (auto& t : tasks) t->setOffsetSec(sec);
        }
        void setOffsetMsec(const double ms) {
            for (auto& t : tasks) t->setOffsetMsec(ms);
        }
        void setOffsetUsec(const double us) {
            for (auto& t : tasks) t->setOffsetUsec(us);
        }
        void setOffsetUsec64(const int64_t us) {
            for (auto& t : tasks) t->setOffsetUsec64(us);
        }

        void addOffsetSec(const double sec) {
            for (auto& t : tasks) t->addOffsetSec(sec);
        }
        void addOffsetMsec(const double ms) {
            for (auto& t : tasks) t->addOffsetMsec(ms);
        }
        void addOffsetUsec(const double us) {
            for (auto& t : tasks) t->addOffsetUsec(us);
        }
        void addOffsetUsec64(const int64_t us) {
            for (auto& t : tasks) t->addOffsetUsec64(us);
        }

        void setDurationSec(const double sec) {
            for (auto& t : tasks) t->setDurationSec(sec);
        }
        void setDurationMsec(const double ms) {
            for (auto& t : tasks) t->setDurationMsec(ms);
        }
        void setDurationUsec(const double us) {
            for (auto& t : tasks) t->setDurationUsec(us);
        }
        void setDurationUsec64(const int64_t us) {
            for (auto& t : tasks) t->setDurationUsec64(us);
        }

        void setTimeSec(const double sec) {
            for (auto& t : tasks) t->setTimeSec(sec);
        }
        void setTimeMsec(const double ms) {
            for (auto& t : tasks) t->setTimeMsec(ms);
        }
        void setTimeUsec(const double us) {
            for (auto& t : tasks) t->setTimeUsec(us);
        }
        void setTimeUsec64(const int64_t us) {
            for (auto& t : tasks) t->setTimeUsec64(us);
        }

        void setLoop(const bool b) {
            for (auto& t : tasks) t->setLoop(b);
        }

        void setIntervalSec(const double sec) {
            for (auto& t : tasks) t->setIntervalSec(sec);
        }
        void setIntervalMsec(const double ms) {
            for (auto& t : tasks) t->setIntervalMsec(ms);
        }
        void setIntervalUsec(const double us) {
            for (auto& t : tasks) t->setIntervalUsec(us);
        }
        void setIntervalUsec64(const int64_t us) {
            for (auto& t : tasks) t->setIntervalUsec64(us);
        }

        void setOffsetCount(const double count) {
            for (auto& t : tasks) t->setOffsetCount(count);
        }

        void setOffsetFrame(const double frame) {
            for (auto& t : tasks) t->setOffsetFrame(frame);
        }

        void setFrameRate(const float fps) {
            for (auto& t : tasks) t->setFrameRate(fps);
        }
    };

}  // namespace task
}  // namespace arduino

#ifndef ARDUINO_TASK_MANAGER_NAMESPACE_BEGIN
#define ARDUINO_TASK_MANAGER_NAMESPACE_BEGIN \
    namespace arduino {                      \
        namespace task {
#endif  // ARDUINO_TASK_MANAGER_NAMESPACE_BEGIN
#ifndef ARDUINO_TASK_MANAGER_NAMESPACE_END
#define ARDUINO_TASK_MANAGER_NAMESPACE_END \
    }                                      \
    }
#endif  // ARDUINO_SCENE_MANAGER_NAMESPACE_END

#define Tasks arduino::task::Manager::get()
namespace Task = arduino::task;

template <typename T>
using TaskRef = Task::Ref<T>;
using TaskBaseRef = TaskRef<Task::Base>;
using SubTaskMode = Task::SubTaskMode;

#include "TaskManager/util/DebugLog/DebugLogRestoreState.h"

#endif  // ARDUINO_TASK_MANAGER_H
