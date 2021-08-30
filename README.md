# TaskManager

polling-based cooperative multi-task manager for Arduino

## Feature

`TaskManager` is polling-based flexible task scheduler which can execute two kinds of tasks in several ways.
Two kinds of tasks are:

- task callbacks
- task classes (the collection of `begin()` `enter()` `update()` `exit()` `idle()` `reset()`)

and they can be handled like:

- execute repeted task with framerate (optionally with N-times limit)
- execute repeted task with interval (optionally with N-times limit)
- execute task once after some seconds
- control timing and behavior of tasks by name and index
- task callbacks and task classes can be handled in the same way
- subtask support with two kinds of mode
  - `SubTaskMode::SYNC` : all subtasks runs at the same time with the parent
  - `SubTaskMode::SEQUENCE` : subtask runs one by one in order if the current subtask stops

## Task Callbacks

```C++
#include <TaskManager.h>

void setup() {
    Tasks.add([] {
        Serial.println("Hello, World");
    })->startFps(1);                // call this function in 1[fps]
    // })->startFpsFor(1, 10);      // call this function in 1[fps] 10 times only
    // })->startInterval(1);        // call this function in 1[sec]
    // })->startIntervalFor(1, 10); // call this function in 1[sec] 10 times only
    // })->startOnceAfter(1);       // call this function once after 1[sec]
}

void loop() {
    Tasks.update(); // automatically execute tasks
}
```

## Timing Control with Task Name or Index

You can control how to execute tasks by using several methods. Please see APIs section for details.

```C++
#include <TaskManager.h>

void setup() {
    // just add the task in setup()
    Tasks.add("MyTask", [] {
        Serial.println("Hello, World");
    });
}

void loop() {
    Tasks.update(); // automatically execute tasks

    // if "MyTask" is not running after 5000[ms],
    if (millis() > 5000 && !Tasks.isRunning("MyTask")) {
        // start task 1[fps]
        Tasks.startFps("MyTask", 1);
        // or you can do it by index without the name
        // Tasks.startFps(0, 1);
    }
}
```

## Task Class

By using task class, you can manage more comlex task in flexible way. To use it, you need to define your own class using `Task::Base` class.

```C++
#include <TaskManager.h>

// Your Blink class
class Blink : public Task::Base {
    bool b;
public:
    Blink(const String& name) : Task::Base(name) , b(false) {
        pinMode(LED_BUILTIN, OUTPUT);
        digitalWrite(LED_BUILTIN, LOW);
    }
    virtual ~Blink() {}

    virtual void update() override {
        digitalWrite(LED_BUILTIN, b);
        b = !b;
    }
};

void setup() {
    Tasks.add<Blink>("Blink")->startFps(1); // constructor and begin() will be called
    Tasks.startFps("Blink", 1);             // of course you can start task by name or index
}

void loop() {
    Tasks.update(); // Blink::enter() will be called if Tasks.udpate() is called for the first time,
                    // and Blink::update() will be automatically called every Tasks.update()
                    // if you erase the task, Blink::exit() will be called after last Blink::update()
}
```

All methods you can define in your own task class is:

```C++
class MyTask : public Task::Base
{
public:
    MyTask(const String& name) : Task::Base(name, fps) {}
    virtual ~MyTask() {}

    virtual void begin() override {}  // optional: called once when task has created
    virtual void enter() override {}  // optional: called once when task has started
    virtual void update() override {} // must be implemented: called every Tasks.update()
    virtual void exit() override {}   // optional: called once after task has stopped
    virtual void idle() override {}   // optional: called when task isn't running (every loop)
    virtual void reset() override {}  // optional
};
```

## Task Class with Parameters

I recommend to use builder-pattern like method to set parameters to your task class.

```C++
#include <TaskManager.h>

class Speak : public Task::Base {
    int num {0};

public:
    Speak(const String& name)
    : Base(name) {
        Serial.begin(115200);
    }

    virtual ~Speak() {}

    // You can set paramters like builder pattern
    Speak* number(const int n) {
        num = n;
        return this;
    }

    virtual void update() override {
        Serial.print("Task ");
        Serial.println(num);
    }
};

void setup() {
    Tasks.add<Speak>("speak")
        ->number(123)  // you can set required parameter like this
        ->startFps(1);
}

void loop() {
    Tasks.update();
}
```

## SubTasks (`SubTaskMode::SYNC`)

The default behavior of subtasks: `SubTaskMode::SYNC` runs subtasks synchronously with parent task. If parent task starts, subtasks also start. If parent task stops, subtasks also stop.

```C++
#include <TaskManager.h>

// ...
// Use Speak class defined above
// ...

void setup() {
    Tasks.add<Speak>("Main")
        ->subtask<Speak>("Sub1", [&](TaskRef<Speak> task) {
            task->number(1);  // configure subtasks by lambda
        })
        ->subtask<Speak>("Sub2", [&](TaskRef<Speak> task) {
            task->number(2);
        })
        ->subtask<Speak>("Sub3", [&](TaskRef<Speak> task) {
            task->number(3);
        });

    // Running task Main also runs all subtasks at the same time
    Tasks["Main"]->startFps(1.);
}

void loop() {
    Tasks.update();  // Runs Main, Sub1, Sub2, and Sub3 at the same time
}
```

## SubTasks (`SubTaskMode::SEQUENCE`)

On the other hand, `SubTaskMode::SEQUENCE` runs subtasks one by one if the current subtask stops. There are two way to control it. One is "Auto Run" and the other is "Manual Run".

### Automatically run subtasks one by one

- `SubTaskMode` should be `SubTaskMode::SEQUENCE`
- All subtask should have duration
- Parent task should have longer duration than the sum of subtasks' duration
- If the current task has finished the duration, next subtask starts running automatically

```C++
#include <TaskManager.h>

// ...
// Use Speak class defined above
// ...

void setup() {
    Tasks.add<Speak>("Main")
        // You can set SubTaskMode (default: SubTaskMode::SYNC)
        ->setSubTaskMode(SubTaskMode::SEQUENCE)

        ->subtask<Speak>("Sub1", [&](TaskRef<Speak> task) {
            task->number(1)->setDurationSec(3);
        })
        ->subtask<Speak>("Sub2", [&](TaskRef<Speak> task) {
            task->number(2)->setDurationSec(3);
        })
        ->subtask<Speak>("Sub3", [&](TaskRef<Speak> task) {
            task->number(3)->setDurationSec(3);
        });

    // You can also choose whether to loop
    bool b_loop = true;
    // Running task Main also runs first subtask
    Tasks["Main"]->startFpsForSec(1., 12, b_loop);
}

void loop() {
    Tasks.update();  // Runs Sub1 -> Sub2 -> Sub3 in order if each subtask stops
}

```

### Manually run next subtasks

- `SubTaskMode` should be `SubTaskMode::SEQUENCE`
- At least one subtask should NOT have duration (the timings of subtasks should not be fixed)
- `nextSubTask()` method will stop current subtask and run next subtask

```C++
#include <TaskManager.h>

// ...
// Use Speak class defined above
// ...

void setup() {
    Tasks.add<Speak>("Main")
        // You can set SubTaskMode (default: SubTaskMode::SYNC)
        ->setSubTaskMode(SubTaskMode::SEQUENCE)

        // Add subtasks
        ->subtask<Speak>("Sub1", [&](TaskRef<Speak> task) {
            task->number(1);
        })
        ->subtask<Speak>("Sub2", [&](TaskRef<Speak> task) {
            task->number(2);
        })
        ->subtask<Speak>("Sub3", [&](TaskRef<Speak> task) {
            task->number(3);
        });

    // Running task Main also runs first subtask
    Tasks["Main"]->startFps(1.);
}

void loop() {
    Tasks.update();

    while (Serial.available()) {
        char c = Serial.read();
        if (c == 'n') {
            // nextSubTask can run next subtask anytime if the conditions above are satisfied
            bool success = Tasks["Main"]->nextSubTask();

            if (success) {
                Serial.println("Let's run next subtask");
            } else {
                Serial.println("No next task -> restart Main task");
                Tasks["Main"]->restart();
            }
        }
    }
}

```

## Other Options

### Enable Error Info

Error information report is disabled by default. You can enable it by defining this macro.

```C++
#define TASKMANAGER_DEBUGLOG_ENABLE
```

Also you can change debug info stream by calling this macro (default: `Serial`).

```C++
DEBUG_LOG_ATTACH_STREAM(Serial1);
```

See [DebugLog](https://github.com/hideakitai/DebugLog) for details.

## APIs

### TaskManager

```C++
Ref<TaskEmpty> add(const Func& task);
Ref<TaskEmpty> add(const String& name, const Func& task);
template <typename TaskType> Ref<TaskType> add();
template <typename TaskType> Ref<TaskType> add(const String& name);

void update();
void reset();
bool reset(const String& name);
bool reset(const size_t idx);
bool erase(const String& name);
bool erase(const size_t idx);
void clear();
bool empty() const;
size_t size() const;
bool exists(const String& name) const;

size_t getActiveTaskSize() const;
void setAutoErase(const bool b);
void setSubTaskMode(const SubTaskMode m);
template <typename TaskType = Base> Ref<TaskType> getTaskByName(const String& name) const;
template <typename TaskType = Base> Ref<TaskType> getTaskByIndex(const size_t i) const;
template <typename TaskType = Base> Ref<TaskType> operator[](const String& name) const;
template <typename TaskType = Base> Ref<TaskType> operator[](const size_t i) const;

// ========== Task method wrappers ==========

void start();
void stop();
void play();
void pause();
void restart();

void startFromSec(const double from_sec);
void startFromMsec(const double from_ms);
void startFromUsec(const double from_us);

void startForSec(const double for_sec, const bool loop = false);
void startForMsec(const double for_ms, const bool loop = false);
void startForUsec(const double for_us, const bool loop = false);

void startFromForSec(const double from_sec, const double for_sec, const bool loop = false);
void startFromForMsec(const double from_ms, const double for_ms, const bool loop = false);
void startFromForUsec(const double from_us, const double for_us, const bool loop = false);
void startFromForUsec64(const int64_t from_us, const int64_t for_us, const bool loop = false);

void startFromCount(const double from_count);
void startForCount(const double for_count, const bool loop = false);
void startFromForCount(const double from_count, const double for_count, const bool loop = false);

void startIntervalSec(const double interval_sec);
void startIntervalMsec(const double interval_ms);
void startIntervalUsec(const double interval_us);

void startIntervalFromSec(const double interval_sec, const double from_sec);
void startIntervalFromMsec(const double interval_ms, const double from_ms);
void startIntervalFromUsec(const double interval_us, const double from_us);
void startIntervalSecFromCount(const double interval_sec, const double from_count);
void startIntervalMsecFromCount(const double interval_ms, const double from_count);
void startIntervalUsecFromCount(const double interval_us, const double from_count);

void startIntervalForSec(const double interval_sec, const double for_sec, const bool loop = false);
void startIntervalForMsec(const double interval_ms, const double for_ms, const bool loop = false);
void startIntervalForUsec(const double interval_us, const double for_us, const bool loop = false);
void startIntervalSecForCount(const double interval_sec, const double for_count, const bool loop = false);
void startIntervalMsecForCount(const double interval_ms, const double for_count, const bool loop = false);
void startIntervalUsecForCount(const double interval_us, const double for_count, const bool loop = false);

void startIntervalFromForSec(const double interval_sec, const double from_sec, const double for_sec, const bool loop = false);
void startIntervalFromForMsec(const double interval_ms, const double from_ms, const double for_ms, const bool loop = false);
void startIntervalFromForUsec(const double interval_us, const double from_us, const double for_us, const bool loop = false);
void startIntervalSecFromForCount(const double interval_sec, const double from_count, const double for_count, const bool loop = false);
void startIntervalMsecFromForCount(const double interval_ms, const double from_count, const double for_count, const bool loop = false);
void startIntervalUsecFromForCount(const double interval_us, const double from_count, const double for_count, const bool loop = false);

void startFromFrame(const double from_frame);
void startForFrame(const double for_frame, const bool loop = false);
void startFromForFrame(const double from_frame, const double for_frame, const bool loop = false);

void startFps(const double fps);

void startFpsFromSec(const double fps, const double from_sec);
void startFpsFromMsec(const double fps, const double from_ms);
void startFpsFromUsec(const double fps, const double from_us);
void startFpsFromFrame(const double fps, const double from_frame);

void startFpsForSec(const double fps, const double for_sec, const bool loop = false);
void startFpsForMsec(const double fps, const double for_ms, const bool loop = false);
void startFpsForUsec(const double fps, const double for_us, const bool loop = false);
void startFpsForFrame(const double fps, const double for_frame, const bool loop = false);

void startFpsFromForSec(const double fps, const double from_sec, const double for_sec, const bool loop = false);
void startFpsFromForMsec(const double fps, const double from_ms, const double for_ms, const bool loop = false);
void startFpsFromForUsec(const double fps, const double from_us, const double for_us, const bool loop = false);
void startFpsFromForFrame(const double fps, const double from_frame, const double for_frame, const bool loop = false);

void startOnce();
void startOnceAfterSec(const double after_sec);
void startOnceAfterMsec(const double after_ms);
void startOnceAfterUsec(const double after_us);

void setOffsetSec(const double sec);
void setOffsetMsec(const double ms);
void setOffsetUsec(const double us);
void setOffsetUsec64(const int64_t us);

void addOffsetSec(const double sec);
void addOffsetMsec(const double ms);
void addOffsetUsec(const double us);
void addOffsetUsec64(const int64_t us);

void setDurationSec(const double sec);
void setDurationMsec(const double ms);
void setDurationUsec(const double us);
void setDurationUsec64(const int64_t us);

void setTimeSec(const double sec);
void setTimeMsec(const double ms);
void setTimeUsec(const double us);
void setTimeUsec64(const int64_t us);

void setIntervalSec(const double sec);
void setIntervalMsec(const double ms);
void setIntervalUsec(const double us);
void setIntervalUsec64(const int64_t us);

void setLoop(const bool b);
void setOffsetCount(const double count);
void setOffsetFrame(const double frame);
void setFrameRate(const float fps);
```

### Task::Base

```C++
bool hasEnter() const {
bool hasExit() const {
Base* setAutoErase(const bool b) {
bool isAutoErase() const {
const String& getName() const {

// =========== for SubTask ==========
template <typename TaskType> Base* subtask(const String& name, std::function<void(Ref<TaskType>)> setup);
Vec<Ref<Base>>& getSubTasks();
const Vec<Ref<Base>>& getSubTasks() const;
Base* setSubTaskIndex(const size_t i);
size_t getSubTaskIndex() const;
Base* setSubTaskMode(const SubTaskMode m);
SubTaskMode getSubTaskMode() const;
bool hasSubTasks() const;
size_t numSubTasks() const;

// ========== only for SubTaskMode::SEQUENCE ==========
bool nextSubTask()
```

### Types

```C++


enum class SubTaskMode : uint8_t {
    SYNC,
    SEQUENCE
};
```

## Embedded Libraries

- [PollingTimer v0.4.0](https://github.com/hideakitai/PollingTimer)
- [ArxContainer v0.3.14](https://github.com/hideakitai/ArxContainer)
- [ArxSmartPtr v0.2.1](https://github.com/hideakitai/ArxSmartPtr)
- [DebugLog v0.6.0](https://github.com/hideakitai/DebugLog)
- [TeensyDirtySTLErrorSolution v0.1.0](https://github.com/hideakitai/TeensyDirtySTLErrorSolution)

## License

MIT
