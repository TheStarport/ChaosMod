#pragma once
#include "Systems/ConfigManager.hpp"

class GlobalTimers : public Singleton<GlobalTimers>
{
    public:
        typedef bool (*GlobalTimerFunc)(float delta);

    private:
        struct GlobalTimer
        {
                GlobalTimerFunc func;
                float timeBetweenExecutions;
                float countdown;
        };

        std::unordered_map<int, GlobalTimer> timers;

    public:
        GlobalTimers() { timers.reserve(32); }

        void Update(const float delta)
        {
            for (auto timer = timers.begin(); timer != timers.end();)
            {
                auto& [func, timeBetweenExecutions, countdown] = timer->second;
                countdown -= delta;
                if (countdown <= 0.0f)
                {
                    if (func(delta))
                    {
                        timer = timers.erase(timer);
                        continue;
                    }

                    countdown = timeBetweenExecutions;
                }

                ++timer;
            }
        }

        int AddTimer(const GlobalTimerFunc func, const float timeBetweenExecutions)
        {
            while (true)
            {
                int id = Random::i()->Uniform(0, INT32_MAX);
                if (timers.contains(id))
                {
                    continue;
                }

                timers[id] = { func, timeBetweenExecutions, 0.0f };
                return id;
            }
        }

        void RemoveTimer(const int id)
        {
            if (const auto timer = timers.find(id); timer != timers.end())
            {
                timers.erase(timer);
            }
        }
};
