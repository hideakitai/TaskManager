# TaskManager

cooperative multi-task manager for Arduino


## Usage

`TaskManager` is simple task scheduler which can execute callback functions in several ways.

- execute task once after some secods
- execute repeted task with interval (optionally with N-times limit)
- execute repeted task with framerat (optionally with N-times limit)

If you want to handle more complex tasks, please see [SceneManager](https://github.com/hideakitai/SceneManager).


``` C++
#include <TaskManager.h>

void setup()
{
    // task is executed only once after 5000[ms]
    Tasks.once(5000, []{
        Serial.print("once task: now = ");
        Serial.println(millis());
    });

    // task framerate is 1 and repeat forever
    Tasks.framerate(1, []{
        Serial.print("framerate forever task: now = ");
        Serial.println(millis());
    });

    // task framerate is 2 and 10 times only
    Tasks.framerate(2, 10, []{
        Serial.print("framerate limited task: now = ");
        Serial.println(millis());
    });

    // task interval is 1000[ms] and repeat forever
    Tasks.interval(1000, []{
        Serial.print("interval forever task: now = ");
        Serial.println(millis());
    });

    // task interval is 500[ms] and 10 times only
    Tasks.interval(500, 10, []{
        Serial.print("interval limited task: now = ");
        Serial.println(millis());
    });
}

void loop()
{
    Tasks.update(); // automatically execute tasks
}
```


## APIs

```C++
TaskRef once(const uint32_t after_ms, const Func& task);
TaskRef interval(const uint32_t interval_ms, const uint32_t counts, const Func& task);
TaskRef interval(const uint32_t interval_ms, const Func& task);
TaskRef framerate(const float fps, const uint32_t frame, const Func& task);
TaskRef framerate(const float fps, const Func& task);
void update():
size_t size() const;
```


## Embedded Libraries

- [PollingTimer v0.2.2](https://github.com/hideakitai/PollingTimer)
- [ArxContainer v0.3.7](https://github.com/hideakitai/ArxContainer)
- [ArxSmartPtr v0.1.2](https://github.com/hideakitai/ArxSmartPtr)
- [TeensyDirtySTLErrorSolution v0.1.0](https://github.com/hideakitai/TeensyDirtySTLErrorSolution)


## License

MIT
