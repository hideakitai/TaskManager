# TaskManager

polling-based cooperative multi-task manager for Arduino

## Feature

`TaskManager` is polling-based flexible task scheduler which can execute two kinds of tasks in several ways.
Two kinds of tasks are:

- task callbacks
- task classes (the collection of `begin()` `enter()` `update()` `exit()` `reset()`)

and they can be handled like:

- execute repeted task with framerate (optionally with N-times limit)
- execute repeted task with interval (optionally with N-times limit)
- execute task once after some seconds
- control timing and behavior of tasks by name and index
- task callbacks and task classes can be handled in the same way


## Usage

### Task Callbacks

``` C++
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

### Timing Control with Task Name or Index

You can control how to execute tasks by using several methods. Please see APIs section for details.

``` C++
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

### Task Class

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
    virtual void reset() override {}  // optional
};
```


## APIs

```C++
Ref<TaskEmpty> add(const Func& task);
Ref<TaskEmpty> add(const String& name, const Func& task);
template <typename TaskType> Ref<TaskType> add();
template <typename TaskType> Ref<TaskType> add(const String& name);
void update();

void start();
bool start(const String& name);
bool start(const size_t idx);
void startFrom(const double from_sec);
bool startFrom(const String& name, const double from_sec);
bool startFrom(const size_t idx, const double from_sec);
void startFromMsec(const double from_ms);
bool startFromMsec(const String& name, const double from_ms);
bool startFromMsec(const size_t idx, const double from_ms);
void startFromUsec(const double from_us);
bool startFromUsec(const String& name, const double from_us);
bool startFromUsec(const size_t idx, const double from_us);
void startFor(const double for_sec);
bool startFor(const String& name, const double for_sec);
bool startFor(const size_t idx, const double for_sec);
void startForMsec(const double for_ms);
bool startForMsec(const String& name, const double for_ms);
bool startForMsec(const size_t idx, const double for_ms);
void startForUsec(const double for_us);
bool startForUsec(const String& name, const double for_us);
bool startForUsec(const size_t idx, const double for_us);
void startFromFor(const double from_sec, const double for_sec);
bool startFromFor(const String& name, const double from_sec, const double for_sec);
bool startFromFor(const size_t idx, const double from_sec, const double for_sec);
void startFromForMsec(const double from_ms, const double for_ms);
bool startFromForMsec(const String& name, const double from_ms, const double for_ms);
bool startFromForMsec(const size_t idx, const double from_ms, const double for_ms);
void startFromForUsec(const double from_us, const double for_us);
bool startFromForUsec(const String& name, const double from_us, const double for_us);
bool startFromForUsec(const size_t idx, const double from_us, const double for_us);
void startFromForUsec64(const int64_t from_us, const int64_t for_us);
bool startFromForUsec64(const String& name, const int64_t from_us, const int64_t for_us);
bool startFromForUsec64(const size_t idx, const int64_t from_us, const int64_t for_us);
void startFps(const double fps);
bool startFps(const String& name, const double fps);
void startFpsFrom(const double fps, const double from_frame);
bool startFpsFrom(const String& name, const double fps, const double from_frame);
void startFpsFor(const double fps, const double for_frame);
bool startFpsFor(const String& name, const double fps, const double for_frame);
void startFpsFromFor(const double fps, const double from_frame, const double for_frame);
bool startFpsFromFor(const String& name, const double fps, const double from_frame, const double for_frame);
void startInterval(const double interval_sec);
bool startInterval(const String& name, const double interval_sec);
bool startInterval(const size_t idx, const double interval_sec);
void startIntervalFrom(const double interval_sec, const double from_count);
bool startIntervalFrom(const String& name, const double interval_sec, const double from_count);
bool startIntervalFrom(const size_t idx, const double interval_sec, const double from_count);
void startIntervalFor(const double interval_sec, const double for_count);
bool startIntervalFor(const String& name, const double interval_sec, const double for_count);
bool startIntervalFor(const size_t idx, const double interval_sec, const double for_count);
void startIntervalFromFor(const double interval_sec, const double from_count, const double for_count);
bool startIntervalFromFor(const size_t idx, const double interval_sec, const double from_count, const double for_count);
void startOnceAfter(const double after_sec);
bool startOnceAfter(const String& name, const double after_sec);
bool startOnceAfter(const size_t idx, const double after_sec);

void stop();
bool stop(const String& name);
bool stop(const size_t idx);
void play();
bool play(const String& name);
bool play(const size_t idx);
void pause();
bool pause(const String& name);
bool pause(const size_t idx);
void restart();
bool restart(const String& name);
bool restart(const size_t idx);
void reset();
bool reset(const String& name);
bool reset(const size_t idx);

void erase(const String& name);
void erase(const size_t idx);
void setAutoErase(const bool b);
bool isAutoErase() const;
bool empty() const;
size_t size() const;
size_t getActiveTaskSize() const;

bool isRunning(const String& name) const;
bool isRunning(const size_t idx) const;
bool isPausing(const String& name) const;
bool isPausing(const size_t idx) const;
bool isStopping(const String& name) const;
bool isStopping(const size_t idx) const;
bool isNext(const String& name) const;
bool isNext(const size_t idx) const;
bool hasStarted(const String& name) const;
bool hasStarted(const size_t idx) const;
bool hasFinished(const String& name) const;
bool hasFinished(const size_t idx) const;

double frame(const String& name);
double frame(const size_t idx);
double count(const String& name);
double count(const size_t idx);

int64_t usec64(const String& name);
int64_t usec64(const size_t idx);
double usec(const String& name);
double usec(const size_t idx);
double msec(const String& name);
double msec(const size_t idx);
double sec(const String& name);
double sec(const size_t idx);

double getDuration(const String& name) const;
double getDuration(const size_t idx) const;
double getRemainingTime(const String& name);
double getRemainingTime(const size_t idx);
double getRemainingLife(const String& name);
double getRemainingLife(const size_t idx);

void setOffset(const double sec);
void setOffset(const String& name, const double sec);
void setOffset(const size_t idx, const double sec);
void setOffsetMsec(const double ms);
void setOffsetMsec(const String& name, const double ms);
void setOffsetMsec(const size_t idx, const double ms);
void setOffsetUsec(const double us);
void setOffsetUsec(const String& name, const double us);
void setOffsetUsec(const size_t idx, const double us);
void setOffsetUsec64(const int64_t us);
void setOffsetUsec64(const String& name, const int64_t us);
void setOffsetUsec64(const size_t idx, const int64_t us);

void addOffset(const double sec);
void addOffset(const String& name, const double sec);
void addOffset(const size_t idx, const double sec);
void addOffsetMsec(const double ms);
void addOffsetMsec(const String& name, const double ms);
void addOffsetMsec(const size_t idx, const double ms);
void addOffsetUsec(const double us);
void addOffsetUsec(const String& name, const double us);
void addOffsetUsec(const size_t idx, const double us);
void addOffsetUsec64(const int64_t us);
void addOffsetUsec64(const String& name, const int64_t us);
void addOffsetUsec64(const size_t idx, const int64_t us);

void setTime(const double sec);
void setTime(const String& name, const double sec);
void setTime(const size_t idx, const double sec);
void setTimeMsec(const double ms);
void setTimeMsec(const String& name, const double ms);
void setTimeMsec(const size_t idx, const double ms);
void setTimeUsec(const double us);
void setTimeUsec(const String& name, const double us);
void setTimeUsec(const size_t idx, const double us);
void setTimeUsec64(const int64_t us);
void setTimeUsec64(const String& name, const int64_t us);
void setTimeUsec64(const size_t idx, const int64_t us);

void setFrameRate(float fps);
void setFrameRate(const String& name, float fps);
void setFrameRate(const size_t idx, float fps);

template <typename TaskType = Base> Ref<TaskType> getTaskByName(const String& name) const;
template <typename TaskType = Base> Ref<TaskType> getTaskByIndex(const size_t i) const;
template <typename TaskType = Base> Ref<TaskType> operator[] (const String& name) const;
template <typename TaskType = Base> Ref<TaskType> operator[] (const size_t i) const;
```


## Embedded Libraries

- [PollingTimer v0.2.4](https://github.com/hideakitai/PollingTimer)
- [ArxContainer v0.3.13](https://github.com/hideakitai/ArxContainer)
- [ArxSmartPtr v0.2.1](https://github.com/hideakitai/ArxSmartPtr)
- [TeensyDirtySTLErrorSolution v0.1.0](https://github.com/hideakitai/TeensyDirtySTLErrorSolution)


## License

MIT
