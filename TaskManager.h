#pragma once
#ifndef ARDUINO_TASK_MANAGER_H
#define ARDUINO_TASK_MANAGER_H

#include <Arduino.h>

#include "TaskManager/util/ArxContainer/ArxContainer.h"
#include "TaskManager/util/ArxSmartPtr/ArxSmartPtr.h"
#include "TaskManager/util/TeensyDirtySTLErrorSolution/TeensyDirtySTLErrorSolution.h"

#if ARX_HAVE_LIBSTDCPLUSPLUS >= 201103L  // Have libstdc++11
#include <algorithm>
#include <iterator>
#endif

#include "TaskManager/TaskBase.h"
#include "TaskManager/TaskEmpty.h"

namespace arduino {
namespace task {

#if ARX_HAVE_LIBSTDCPLUSPLUS >= 201103L  // Have libstdc++11
    template <typename T>
    using Vec = std::vector<T>;
    template <typename T>
    using Ref = std::shared_ptr<T>;
    using TaskRef = Ref<Base>;
    using TaskList = Vec<TaskRef>;
    using Func = std::function<void(void)>;
    using namespace std;
#else
    template <typename T>
    using Vec = arx::vector<T>;
    template <typename T>
    using Ref = std::shared_ptr<T>;
    using TaskRef = Ref<Base>;
    using TaskList = Vec<TaskRef>;
    using Func = std::function<void(void)>;
    using namespace arx;
#endif

    class Manager {
        Manager() {}
        Manager(const Manager&) = delete;
        Manager& operator=(const Manager&) = delete;

        TaskList tasks;
        bool is_auto_erase {false};

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
            t->addEvent(task);
            tasks.emplace_back(t);
            t->begin();
            return t;
        }

        // TODO: add variable template for flexible constructor arguments?
        template <typename TaskType>
        Ref<TaskType> add() {
            return add<TaskType>("");
        }

        template <typename TaskType>
        Ref<TaskType> add(const String& name) {
            Ref<TaskType> t = std::make_shared<TaskType>(name);
            tasks.emplace_back(t);
            t->begin();
            return t;
        }

        void update() {
            for (auto& t : tasks) {
                if (!t->isRunning()) {
                    continue;
                }
                if (t->FrameRateCounter::update() || t->hasExit()) {
                    t->callUpdate();
                }
            }

            // remove if event has done
            if (is_auto_erase) {
#if ARX_HAVE_LIBSTDCPLUSPLUS >= 201103L  // Have libstdc++11
                auto results = std::remove_if(tasks.begin(), tasks.end(),
                    [&](const Ref<Base>& t) {
                        return t->hasFinished();
                    });
                tasks.erase(results, tasks.end());
#else
                auto it = tasks.begin();
                while (it != tasks.end()) {
                    if ((*it)->hasFinished())
                        it = tasks.erase(it);
                    else
                        it++;
                }
#endif
            }
        }

        void start() {
            startFromForUsec64(0, 0);
        }
        bool start(const String& name) {
            return startFromForUsec64(name, 0, 0);
        }
        bool start(const size_t idx) {
            return startFromForUsec64(idx, 0, 0);
        }
        void startFrom(const double from_sec) {
            startFromForUsec64(from_sec * 1000000., 0);
        }
        bool startFrom(const String& name, const double from_sec) {
            return startFromForUsec64(name, from_sec * 1000000., 0);
        }
        bool startFrom(const size_t idx, const double from_sec) {
            return startFromForUsec64(idx, from_sec * 1000000., 0);
        }
        void startFromMsec(const double from_ms) {
            startFromForUsec64(from_ms * 1000., 0);
        }
        bool startFromMsec(const String& name, const double from_ms) {
            return startFromForUsec64(name, from_ms * 1000., 0);
        }
        bool startFromMsec(const size_t idx, const double from_ms) {
            return startFromForUsec64(idx, from_ms * 1000., 0);
        }
        void startFromUsec(const double from_us) {
            startFromForUsec64(from_us, 0);
        }
        bool startFromUsec(const String& name, const double from_us) {
            return startFromForUsec64(name, from_us, 0);
        }
        bool startFromUsec(const size_t idx, const double from_us) {
            return startFromForUsec64(idx, from_us, 0);
        }
        void startFor(const double for_sec) {
            startFromForUsec64(0, for_sec * 1000000.);
        }
        bool startFor(const String& name, const double for_sec) {
            return startFromForUsec64(name, 0, for_sec * 1000000.);
        }
        bool startFor(const size_t idx, const double for_sec) {
            return startFromForUsec64(idx, 0, for_sec * 1000000.);
        }
        void startForMsec(const double for_ms) {
            startFromForUsec64(0, for_ms * 1000.);
        }
        bool startForMsec(const String& name, const double for_ms) {
            return startFromForUsec64(name, 0, for_ms * 1000.);
        }
        bool startForMsec(const size_t idx, const double for_ms) {
            return startFromForUsec64(idx, 0, for_ms * 1000.);
        }
        void startForUsec(const double for_us) {
            startFromForUsec64(0, for_us);
        }
        bool startForUsec(const String& name, const double for_us) {
            return startFromForUsec64(name, 0, for_us);
        }
        bool startForUsec(const size_t idx, const double for_us) {
            return startFromForUsec64(idx, 0, for_us);
        }
        void startFromFor(const double from_sec, const double for_sec) {
            startFromForUsec64(from_sec * 1000000., for_sec * 1000000.);
        }
        bool startFromFor(const String& name, const double from_sec, const double for_sec) {
            return startFromForUsec64(name, from_sec * 1000000., for_sec * 1000000.);
        }
        bool startFromFor(const size_t idx, const double from_sec, const double for_sec) {
            return startFromForUsec64(idx, from_sec * 1000000., for_sec * 1000000.);
        }
        void startFromForMsec(const double from_ms, const double for_ms) {
            startFromForUsec64(from_ms * 1000., for_ms * 1000.);
        }
        bool startFromForMsec(const String& name, const double from_ms, const double for_ms) {
            return startFromForUsec64(name, from_ms * 1000., for_ms * 1000.);
        }
        bool startFromForMsec(const size_t idx, const double from_ms, const double for_ms) {
            return startFromForUsec64(idx, from_ms * 1000., for_ms * 1000.);
        }
        void startFromForUsec(const double from_us, const double for_us) {
            startFromForUsec64(from_us, for_us);
        }
        bool startFromForUsec(const String& name, const double from_us, const double for_us) {
            return startFromForUsec64(name, from_us, for_us);
        }
        bool startFromForUsec(const size_t idx, const double from_us, const double for_us) {
            return startFromForUsec64(idx, from_us, for_us);
        }
        void startFromForUsec64(const int64_t from_us, const int64_t for_us) {
            for (auto& t : tasks) t->startFromForUsec64(from_us, for_us);
        }
        bool startFromForUsec64(const String& name, const int64_t from_us, const int64_t for_us) {
            auto t = getTaskByName(name);
            if (t) {
                t->startFromForUsec64(from_us, for_us);
                return true;
            }
            return false;
        }
        bool startFromForUsec64(const size_t idx, const int64_t from_us, const int64_t for_us) {
            auto t = getTaskByIndex(idx);
            if (t) {
                t->startFromForUsec64(from_us, for_us);
                return true;
            }
            return false;
        }

        void startFps(const double fps) {
            startFpsFromFor(fps, 0., 0.);
        }
        bool startFps(const String& name, const double fps) {
            return startFpsFromFor(name, fps, 0., 0.);
        }
        void startFpsFrom(const double fps, const double from_frame) {
            startFpsFromFor(fps, from_frame, 0.);
        }
        bool startFpsFrom(const String& name, const double fps, const double from_frame) {
            return startFpsFromFor(name, fps, from_frame, 0.);
        }
        void startFpsFor(const double fps, const double for_frame) {
            startFpsFromFor(fps, 0., for_frame);
        }
        bool startFpsFor(const String& name, const double fps, const double for_frame) {
            return startFpsFromFor(name, fps, 0., for_frame);
        }
        void startFpsFromFor(const double fps, const double from_frame, const double for_frame) {
            for (auto& t : tasks) t->startFpsFromFor(fps, from_frame, for_frame);
        }
        bool startFpsFromFor(const String& name, const double fps, const double from_frame, const double for_frame) {
            auto t = getTaskByName(name);
            if (t) {
                t->startFpsFromFor(fps, from_frame, for_frame);
                return true;
            }
            return false;
        }

        void startInterval(const double interval_sec) {
            startIntervalFromFor(interval_sec, 0., 0.);
        }
        bool startInterval(const String& name, const double interval_sec) {
            return startIntervalFromFor(name, interval_sec, 0., 0.);
        }
        bool startInterval(const size_t idx, const double interval_sec) {
            return startIntervalFromFor(idx, interval_sec, 0., 0.);
        }
        void startIntervalFrom(const double interval_sec, const double from_count) {
            startIntervalFromFor(interval_sec, from_count, 0.);
        }
        bool startIntervalFrom(const String& name, const double interval_sec, const double from_count) {
            return startIntervalFromFor(name, interval_sec, from_count, 0.);
        }
        bool startIntervalFrom(const size_t idx, const double interval_sec, const double from_count) {
            return startIntervalFromFor(idx, interval_sec, from_count, 0.);
        }
        void startIntervalFor(const double interval_sec, const double for_count) {
            startIntervalFromFor(interval_sec, 0., for_count);
        }
        bool startIntervalFor(const String& name, const double interval_sec, const double for_count) {
            return startIntervalFromFor(name, interval_sec, 0., for_count);
        }
        bool startIntervalFor(const size_t idx, const double interval_sec, const double for_count) {
            return startIntervalFromFor(idx, interval_sec, 0., for_count);
        }
        void startIntervalFromFor(const double interval_sec, const double from_count, const double for_count) {
            for (auto& t : tasks) t->startIntervalFromFor(interval_sec, from_count, for_count);
        }
        bool startIntervalFromFor(const String& name, const double interval_sec, const double from_count, const double for_count) {
            auto t = getTaskByName(name);
            if (t) {
                t->startIntervalFromFor(interval_sec, from_count, for_count);
                return true;
            }
            return false;
        }
        bool startIntervalFromFor(const size_t idx, const double interval_sec, const double from_count, const double for_count) {
            auto t = getTaskByIndex(idx);
            if (t) {
                t->startIntervalFromFor(interval_sec, from_count, for_count);
                return true;
            }
            return false;
        }

        void startOnceAfter(const double after_sec) {
            startIntervalFor(after_sec, 1);
        }
        bool startOnceAfter(const String& name, const double after_sec) {
            return startIntervalFor(name, after_sec, 1);
        }
        bool startOnceAfter(const size_t idx, const double after_sec) {
            return startIntervalFor(idx, after_sec, 1);
        }

        void stop() {
            for (auto& t : tasks) t->stop();
        }
        bool stop(const String& name) {
            auto t = getTaskByName(name);
            if (t) {
                t->stop();
                t->callUpdate();
                return true;
            }
            return false;
        }
        bool stop(const size_t idx) {
            auto t = getTaskByIndex(idx);
            if (t) {
                t->stop();
                t->callUpdate();
                return true;
            }
            return false;
        }

        void play() {
            for (auto& t : tasks) t->play();
        }
        bool play(const String& name) {
            auto t = getTaskByName(name);
            if (t) {
                t->play();
                return true;
            }
            return false;
        }
        bool play(const size_t idx) {
            auto t = getTaskByIndex(idx);
            if (t) {
                t->play();
                return true;
            }
            return false;
        }

        void pause() {
            for (auto& t : tasks) t->pause();
        }
        bool pause(const String& name) {
            auto t = getTaskByName(name);
            if (t) {
                t->pause();
                return true;
            }
            return false;
        }
        bool pause(const size_t idx) {
            auto t = getTaskByIndex(idx);
            if (t) {
                t->pause();
                return true;
            }
            return false;
        }

        void restart() {
            for (auto& t : tasks) {
                t->stop();
                t->callUpdate();
                t->start();
            }
        }
        bool restart(const String& name) {
            auto t = getTaskByName(name);
            if (t) {
                t->stop();
                t->callUpdate();
                t->start();
                return true;
            }
            return false;
        }
        bool restart(const size_t idx) {
            auto t = getTaskByIndex(idx);
            if (t) {
                t->stop();
                t->callUpdate();
                t->start();
                return true;
            }
            return false;
        }

        void reset() {
            for (auto& t : tasks) t->reset();
        }
        bool reset(const String& name) {
            auto t = getTaskByName(name);
            if (t) {
                t->reset();
                return true;
            }
            return false;
        }
        bool reset(const size_t idx) {
            auto t = getTaskByIndex(idx);
            if (t) {
                t->reset();
                return true;
            }
            return false;
        }

        void erase(const String& name) {
#if ARX_HAVE_LIBSTDCPLUSPLUS >= 201103L  // Have libstdc++11
            auto results = std::remove_if(tasks.begin(), tasks.end(),
                [&](const TaskRef& t) {
                    return (t->getName() == name);
                });
            tasks.erase(results, tasks.end());
#else
            auto it = tasks.begin();
            while (it != tasks.end()) {
                if ((*it)->getName() == name)
                    it = tasks.erase(it);
                else
                    ++it;
            }
#endif
        }
        void erase(const size_t idx) {
            auto it = tasks.begin() + idx;
            tasks.erase(it);
        }

        void setAutoErase(const bool b) {
            is_auto_erase = b;
        }

        bool isAutoErase() const {
            return is_auto_erase;
        }

        bool empty() const {
            return tasks.size() == 0;
        }

        size_t size() const {
            return tasks.size();
        }

        size_t getActiveTaskSize() const {
            size_t i = 0;
            for (const auto& t : tasks) {
                if (t->isRunning()) ++i;
            }
            return i;
        }

        bool isRunning(const String& name) const {
            auto t = getTaskByName(name);
            if (t) return t->isRunning();
            return false;
        }
        bool isRunning(const size_t idx) const {
            auto t = getTaskByIndex(idx);
            if (t) return t->isRunning();
            return false;
        }

        bool isPausing(const String& name) const {
            auto t = getTaskByName(name);
            if (t) return t->isPausing();
            return false;
        }
        bool isPausing(const size_t idx) const {
            auto t = getTaskByIndex(idx);
            if (t) return t->isPausing();
            return false;
        }

        bool isStopping(const String& name) const {
            auto t = getTaskByName(name);
            if (t) return t->isStopping();
            return false;
        }
        bool isStopping(const size_t idx) const {
            auto t = getTaskByIndex(idx);
            if (t) return t->isStopping();
            return false;
        }

        bool isNext(const String& name) const {
            auto t = getTaskByName(name);
            if (t) return t->FrameRateCounter::update();
            return false;
        }
        bool isNext(const size_t idx) const {
            auto t = getTaskByIndex(idx);
            if (t) return t->FrameRateCounter::update();
            return false;
        }

        bool hasStarted(const String& name) const {
            auto t = getTaskByName(name);
            if (t) return t->hasStarted();
            return false;
        }
        bool hasStarted(const size_t idx) const {
            auto t = getTaskByIndex(idx);
            if (t) return t->hasStarted();
            return false;
        }

        bool hasFinished(const String& name) const {
            auto t = getTaskByName(name);
            if (t) return t->hasFinished();
            return false;
        }
        bool hasFinished(const size_t idx) const {
            auto t = getTaskByIndex(idx);
            if (t) return t->hasFinished();
            return false;
        }

        double frame(const String& name) {
            auto t = getTaskByName(name);
            if (t)
                return t->frame();
            else
                return 0.;
        }
        double frame(const size_t idx) {
            auto t = getTaskByIndex(idx);
            if (t)
                return t->frame();
            else
                return 0.;
        }

        double count(const String& name) {
            auto t = getTaskByName(name);
            if (t)
                return t->count();
            else
                return 0.;
        }
        double count(const size_t idx) {
            auto t = getTaskByIndex(idx);
            if (t)
                return t->count();
            else
                return 0.;
        }

        int64_t usec64(const String& name) {
            auto t = getTaskByName(name);
            if (t) return t->usec64();
            return 0;
        }
        int64_t usec64(const size_t idx) {
            auto t = getTaskByIndex(idx);
            if (t) return t->usec64();
            return 0;
        }

        double usec(const String& name) {
            auto t = getTaskByName(name);
            if (t) return t->usec();
            return 0.;
        }
        double usec(const size_t idx) {
            auto t = getTaskByIndex(idx);
            if (t) return t->usec();
            return 0.;
        }

        double msec(const String& name) {
            auto t = getTaskByName(name);
            if (t) return t->msec();
            return 0.;
        }
        double msec(const size_t idx) {
            auto t = getTaskByIndex(idx);
            if (t) return t->msec();
            return 0.;
        }

        double sec(const String& name) {
            auto t = getTaskByName(name);
            if (t) return t->sec();
            return 0.;
        }
        double sec(const size_t idx) {
            auto t = getTaskByIndex(idx);
            if (t) return t->sec();
            return 0.;
        }

        double getDuration(const String& name) const {
            auto t = getTaskByName(name);
            if (t) return t->getDuration();
            return 0.;
        }
        double getDuration(const size_t idx) const {
            auto t = getTaskByIndex(idx);
            if (t) return t->getDuration();
            return 0.;
        }

        double getRemainingTime(const String& name) {
            auto t = getTaskByName(name);
            if (t) return t->getRemainingTime();
            return 0.;
        }
        double getRemainingTime(const size_t idx) {
            auto t = getTaskByIndex(idx);
            if (t) return t->getRemainingTime();
            return 0.;
        }

        double getRemainingLife(const String& name) {
            auto t = getTaskByName(name);
            if (t) return t->getRemainingLife();
            return 0.;
        }
        double getRemainingLife(const size_t idx) {
            auto t = getTaskByIndex(idx);
            if (t) return t->getRemainingLife();
            return 0.;
        }

        void setOffset(const double sec) {
            for (auto& t : tasks)
                t->setOffsetSec(sec);
        }
        void setOffset(const String& name, const double sec) {
            auto t = getTaskByName(name);
            if (t) t->setOffsetSec(sec);
        }
        void setOffset(const size_t idx, const double sec) {
            auto t = getTaskByIndex(idx);
            if (t) t->setOffsetSec(sec);
        }
        void setOffsetMsec(const double ms) {
            for (auto& t : tasks)
                t->setOffsetMsec(ms);
        }
        void setOffsetMsec(const String& name, const double ms) {
            auto t = getTaskByName(name);
            if (t) t->setOffsetMsec(ms);
        }
        void setOffsetMsec(const size_t idx, const double ms) {
            auto t = getTaskByIndex(idx);
            if (t) t->setOffsetMsec(ms);
        }
        void setOffsetUsec(const double us) {
            for (auto& t : tasks)
                t->setOffsetUsec(us);
        }
        void setOffsetUsec(const String& name, const double us) {
            auto t = getTaskByName(name);
            if (t) t->setOffsetUsec(us);
        }
        void setOffsetUsec(const size_t idx, const double us) {
            auto t = getTaskByIndex(idx);
            if (t) t->setOffsetUsec(us);
        }
        void setOffsetUsec64(const int64_t us) {
            for (auto& t : tasks)
                t->setOffsetUsec64(us);
        }
        void setOffsetUsec64(const String& name, const int64_t us) {
            auto t = getTaskByName(name);
            if (t) t->setOffsetUsec64(us);
        }
        void setOffsetUsec64(const size_t idx, const int64_t us) {
            auto t = getTaskByIndex(idx);
            if (t) t->setOffsetUsec64(us);
        }

        void addOffset(const double sec) {
            for (auto& t : tasks)
                t->addOffsetSec(sec);
        }
        void addOffset(const String& name, const double sec) {
            auto t = getTaskByName(name);
            if (t) t->addOffsetSec(sec);
        }
        void addOffset(const size_t idx, const double sec) {
            auto t = getTaskByIndex(idx);
            if (t) t->addOffsetSec(sec);
        }
        void addOffsetMsec(const double ms) {
            for (auto& t : tasks)
                t->addOffsetMsec(ms);
        }
        void addOffsetMsec(const String& name, const double ms) {
            auto t = getTaskByName(name);
            if (t) t->addOffsetMsec(ms);
        }
        void addOffsetMsec(const size_t idx, const double ms) {
            auto t = getTaskByIndex(idx);
            if (t) t->addOffsetMsec(ms);
        }
        void addOffsetUsec(const double us) {
            for (auto& t : tasks)
                t->addOffsetUsec(us);
        }
        void addOffsetUsec(const String& name, const double us) {
            auto t = getTaskByName(name);
            if (t) t->addOffsetUsec(us);
        }
        void addOffsetUsec(const size_t idx, const double us) {
            auto t = getTaskByIndex(idx);
            if (t) t->addOffsetUsec(us);
        }
        void addOffsetUsec64(const int64_t us) {
            for (auto& t : tasks)
                t->addOffsetUsec64(us);
        }
        void addOffsetUsec64(const String& name, const int64_t us) {
            auto t = getTaskByName(name);
            if (t) t->addOffsetUsec64(us);
        }
        void addOffsetUsec64(const size_t idx, const int64_t us) {
            auto t = getTaskByIndex(idx);
            if (t) t->addOffsetUsec64(us);
        }

        void setTime(const double sec) {
            for (auto& t : tasks)
                t->setTimeSec(sec);
        }
        void setTime(const String& name, const double sec) {
            auto t = getTaskByName(name);
            if (t) t->setTimeSec(sec);
        }
        void setTime(const size_t idx, const double sec) {
            auto t = getTaskByIndex(idx);
            if (t) t->setTimeSec(sec);
        }
        void setTimeMsec(const double ms) {
            for (auto& t : tasks)
                t->setTimeMsec(ms);
        }
        void setTimeMsec(const String& name, const double ms) {
            auto t = getTaskByName(name);
            if (t) t->setTimeMsec(ms);
        }
        void setTimeMsec(const size_t idx, const double ms) {
            auto t = getTaskByIndex(idx);
            if (t) t->setTimeMsec(ms);
        }
        void setTimeUsec(const double us) {
            for (auto& t : tasks)
                t->setTimeUsec(us);
        }
        void setTimeUsec(const String& name, const double us) {
            auto t = getTaskByName(name);
            if (t) t->setTimeUsec(us);
        }
        void setTimeUsec(const size_t idx, const double us) {
            auto t = getTaskByIndex(idx);
            if (t) t->setTimeUsec(us);
        }
        void setTimeUsec64(const int64_t us) {
            for (auto& t : tasks)
                t->setTimeUsec64(us);
        }
        void setTimeUsec64(const String& name, const int64_t us) {
            auto t = getTaskByName(name);
            if (t) t->setTimeUsec64(us);
        }
        void setTimeUsec64(const size_t idx, const int64_t us) {
            auto t = getTaskByIndex(idx);
            if (t) t->setTimeUsec64(us);
        }

        void setFrameRate(float fps) {
            for (auto& t : tasks)
                t->setFrameRate(fps);
        }
        void setFrameRate(const String& name, float fps) {
            auto t = getTaskByName(name);
            if (t) t->setFrameRate(fps);
        }
        void setFrameRate(const size_t idx, float fps) {
            auto t = getTaskByIndex(idx);
            if (t) t->setFrameRate(fps);
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
            return nullptr;
        }

        template <typename TaskType = Base>
        Ref<TaskType> getTaskByIndex(const size_t i) const {
            if (i >= tasks.size()) return nullptr;

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

#endif  // ARDUINO_TASK_MANAGER_H
