#pragma once
#ifndef ARDUINO_TASK_MANAGER_H
#define ARDUINO_TASK_MANAGER_H

#include <Arduino.h>

#include "TaskManager/util/ArxContainer/ArxContainer.h"
#include "TaskManager/util/ArxSmartPtr/ArxSmartPtr.h"
#include "TaskManager/util/TeensyDirtySTLErrorSolution/TeensyDirtySTLErrorSolution.h"

#if ARX_HAVE_LIBSTDCPLUSPLUS >= 201103L // Have libstdc++11
    #include <algorithm>
    #include <iterator>
#endif

#include "TaskManager/util/PollingTimer/IntervalCounter.h"
#include "TaskManager/util/PollingTimer/FrameRateCounter.h"
#include "TaskManager/util/PollingTimer/OneShotTimer.h"


namespace arduino {
namespace task {

#if ARX_HAVE_LIBSTDCPLUSPLUS >= 201103L // Have libstdc++11
    template <typename T> using Ref = std::shared_ptr<T>;
    using TaskRef = Ref<IntervalCounter>;
    struct Task { String name; TaskRef task_ref; };
    using Tasks = std::vector<Task>;
    using Func = std::function<void(void)>;
    using namespace std;
#else
    template <typename T> using Ref = std::shared_ptr<T>;
    using TaskRef = Ref<IntervalCounter>;
    struct Task { String name; TaskRef task_ref; };
    using Tasks = arx::vector<Task>;
    using Func = std::function<void(void)>;
    using namespace arx;
#endif


class Manager
{
    Manager() {}
    Manager(const Manager&) = delete;
    Manager& operator=(const Manager&) = delete;

    Tasks tasks;

public:

    static Manager& get()
    {
        static Manager m;
        return m;
    }

    TaskRef once(const String& name, const uint32_t after_ms, const Func& task)
    {
        auto ref = std::make_shared<OneShotTimer>((double)after_ms * 0.001, task);
        tasks.emplace_back(Task {name, ref});
        ref->start();
        return ref;
    }

    TaskRef once(const uint32_t after_ms, const Func& task)
    {
        return once("", after_ms, task);
    }

    TaskRef interval(const String& name, const uint32_t interval_ms, const uint32_t counts, const Func& task)
    {
        auto ref = std::make_shared<IntervalCounter>((double)interval_ms * 0.001);
        tasks.emplace_back(Task {name, ref});
        ref->addEvent(task);
        ref->startForCount(counts);
        return ref;
    }

    TaskRef interval(const String& name, const uint32_t interval_ms, const Func& task)
    {
        return interval(name, interval_ms, 0, task);
    }

    TaskRef interval(const uint32_t interval_ms, const uint32_t counts, const Func& task)
    {
        return interval("", interval_ms, counts, task);
    }

    TaskRef interval(const uint32_t interval_ms, const Func& task)
    {
        return interval("", interval_ms, 0, task);
    }

    TaskRef framerate(const String& name, const float fps, const uint32_t frame, const Func& task)
    {
        auto ref = std::make_shared<FrameRateCounter>(fps);
        tasks.emplace_back(Task {name, ref});
        ref->addEvent(task);
        ref->startForCount(frame);
        return ref;
    }

    TaskRef framerate(const String& name, const float fps, const Func& task)
    {
        return framerate(name, fps, 0, task);
    }

    TaskRef framerate(const float fps, const uint32_t frame, const Func& task)
    {
        return framerate("", fps, frame, task);
    }

    TaskRef framerate(const float fps, const Func& task)
    {
        return framerate("", fps, 0, task);
    }

    void update()
    {
        for (auto& t : tasks) t.task_ref->update();

        // remove if event has done
#if ARX_HAVE_LIBSTDCPLUSPLUS >= 201103L // Have libstdc++11
        auto results = std::remove_if(tasks.begin(), tasks.end(),
            [&](const Task& t) {
                return t.task_ref->hasFinished();
            }
        );
        tasks.erase(results, tasks.end());
#else
        auto it = tasks.begin();
        while (it != tasks.end())
        {
            if ((*it).task_ref->hasFinished()) it = tasks.erase(it);
            else it++;
        }
#endif
    }

    size_t size() const { return tasks.size(); }


    TaskRef getTaskByName(const String& name) const
    {
        for (auto& t : tasks)
            if (t.name == name)
                return t.task_ref;

        return nullptr;
    }

    void erase(const String& name)
    {
#if ARX_HAVE_LIBSTDCPLUSPLUS >= 201103L // Have libstdc++11
        auto results = std::remove_if(tasks.begin(), tasks.end(),
            [&](const Task& t) {
                return t.name == name;
            }
        );
        tasks.erase(results, tasks.end());
#else
        auto it = tasks.begin();
        while (it != tasks.end())
        {
            if ((*it).name == name) it = tasks.erase(it);
            else it++;
        }
#endif
    }

    void start(const String& name)
    {
        auto task_ref = getTaskByName(name);
        if (task_ref) task_ref->start();
    }

    void startFrom(const String& name, const double from_sec)
    {
        auto task_ref = getTaskByName(name);
        if (task_ref) task_ref->startFrom(from_sec);
    }

    void stop(const String& name)
    {
        auto task_ref = getTaskByName(name);
        if (task_ref) task_ref->stop();
    }

    void play(const String& name)
    {
        auto task_ref = getTaskByName(name);
        if (task_ref) task_ref->play();
    }

    void pause(const String& name)
    {
        auto task_ref = getTaskByName(name);
        if (task_ref) task_ref->pause();
    }

    void restart(const String& name)
    {
        auto task_ref = getTaskByName(name);
        if (task_ref) task_ref->restart();
    }


    bool isRunning(const String& name) const
    {
        auto task_ref = getTaskByName(name);
        if (task_ref) return task_ref->isRunning();
        return false;
    }

    bool isPausing(const String& name) const
    {
        auto task_ref = getTaskByName(name);
        if (task_ref) return task_ref->isPausing();
        return false;
    }

    bool isStopping(const String& name) const
    {
        auto task_ref = getTaskByName(name);
        if (task_ref) return task_ref->isStopping();
        return false;
    }


    int64_t usec64(const String& name)
    {
        auto task_ref = getTaskByName(name);
        if (task_ref) return task_ref->usec64();
        return 0;
    }

    double usec(const String& name)
    {
        auto task_ref = getTaskByName(name);
        if (task_ref) return task_ref->usec();
        return 0.;
    }

    double msec(const String& name)
    {
        auto task_ref = getTaskByName(name);
        if (task_ref) return task_ref->msec();
        return 0.;
    }

    double sec(const String& name)
    {
        auto task_ref = getTaskByName(name);
        if (task_ref) return task_ref->sec();
        return 0.;
    }


    double getDuration(const String& name) const
    {
        auto task_ref = getTaskByName(name);
        if (task_ref) return task_ref->getDuration();
        return 0.;
    }

    double getRemainingTime(const String& name)
    {
        auto task_ref = getTaskByName(name);
        if (task_ref) return task_ref->getRemainingTime();
        return 0.;
    }

    double getRemainingLife(const String& name)
    {
        auto task_ref = getTaskByName(name);
        if (task_ref) return task_ref->getRemainingLife();
        return 0.;
    }


    void setOffset(const String& name, const double sec)
    {
        auto task_ref = getTaskByName(name);
        if (task_ref) task_ref->setOffsetSec(sec);
    }

    void addOffset(const String& name, const double sec)
    {
        auto task_ref = getTaskByName(name);
        if (task_ref) task_ref->addOffsetSec(sec);
    }

    void setTime(const String& name, const double sec)
    {
        auto task_ref = getTaskByName(name);
        if (task_ref) task_ref->setTimeSec(sec);
    }

    void setFrameRate(const String& name, const float fps)
    {
        auto task_ref = getTaskByName(name);
        if (task_ref) task_ref->setInterval(1.f / fps);
    }

    double count(const String& name)
    {
        auto task_ref = getTaskByName(name);
        if (task_ref) return task_ref->count();
        return 0.;
    }

    double frame(const String& name)
    {
        return count(name);
    }

    TaskRef operator[] (const String& name) const
    {
        return getTaskByName(name);
    }
};

} // task
} // arduino


#ifndef ARDUINO_TASK_MANAGER_NAMESPACE_BEGIN
#define ARDUINO_TASK_MANAGER_NAMESPACE_BEGIN namespace arduino { namespace task {
#endif // ARDUINO_TASK_MANAGER_NAMESPACE_BEGIN
#ifndef ARDUINO_TASK_MANAGER_NAMESPACE_END
#define ARDUINO_TASK_MANAGER_NAMESPACE_END   }}
#endif // ARDUINO_SCENE_MANAGER_NAMESPACE_END

#define Tasks arduino::task::Manager::get()

#endif // ARDUINO_TASK_MANAGER_H
