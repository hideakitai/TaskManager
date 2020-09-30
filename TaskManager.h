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
    template <typename T> using Vec = std::vector<T>;
    template <typename T> using Ref = std::shared_ptr<T>;
    using namespace std;
#else
    template <typename T> using Vec = arx::vector<T>;
    template <typename T> using Ref = std::shared_ptr<T>;
    using namespace arx;
#endif

using TaskRef = Ref<IntervalCounter>;
using Func = std::function<void(void)>;

class Manager
{
    Manager() {}
    Manager(const Manager&) = delete;
    Manager& operator=(const Manager&) = delete;

    Vec<TaskRef> tasks;

public:

    static Manager& get()
    {
        static Manager m;
        return m;
    }

    TaskRef once(const uint32_t after_ms, const Func& task)
    {
        auto e = std::make_shared<OneShotTimer>((double)after_ms * 0.001, task);
        tasks.emplace_back(e);
        e->start();
        return e;
    }

    TaskRef interval(const uint32_t interval_ms, const uint32_t counts, const Func& task)
    {
        auto e = std::make_shared<IntervalCounter>((double)interval_ms * 0.001);
        tasks.emplace_back(e);
        e->addEvent(task);
        e->startForCount(counts);
        return e;
    }

    TaskRef interval(const uint32_t interval_ms, const Func& task)
    {
        return interval(interval_ms, 0, task);
    }

    TaskRef framerate(const float fps, const uint32_t frame, const Func& task)
    {
        auto e = std::make_shared<FrameRateCounter>(fps);
        tasks.emplace_back(e);
        e->addEvent(task);
        e->startForCount(frame);
        return e;
    }

    TaskRef framerate(const float fps, const Func& task)
    {
        return framerate(fps, 0, task);
    }

    void update()
    {
        for (auto& e : tasks) e->update();

        // remove if event has done
#if ARX_HAVE_LIBSTDCPLUSPLUS >= 201103L // Have libstdc++11
        auto results = std::remove_if(tasks.begin(), tasks.end(),
            [&](const TaskRef& e) {
                return e->hasFinished();
            }
        );
        tasks.erase(results, tasks.end());
#else
        auto it = tasks.begin();
        while (it != tasks.end())
        {
            if ((*it)->hasFinished()) it = tasks.erase(it);
            else it++;
        }
#endif
    }

    size_t size() const { return tasks.size(); }
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
