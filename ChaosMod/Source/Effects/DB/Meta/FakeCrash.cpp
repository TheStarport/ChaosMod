#include "PCH.hpp"

#include "Effects/ActiveEffect.hpp"

class FakeCrash final : public ActiveEffect
{
        static void SleepAllThreads(const DWORD ms)
        {
            std::vector<HANDLE> threads;

            HANDLE handle = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);

            THREADENTRY32 threadEntry{};
            threadEntry.dwSize = sizeof(threadEntry);

            Thread32First(handle, &threadEntry);
            do
            {
                if (threadEntry.dwSize >= FIELD_OFFSET(THREADENTRY32, th32OwnerProcessID) + sizeof(threadEntry.th32OwnerProcessID))
                {
                    if (threadEntry.th32ThreadID != GetCurrentThreadId() && threadEntry.th32OwnerProcessID == GetCurrentProcessId())
                    {
                        HANDLE thread = OpenThread(THREAD_ALL_ACCESS, FALSE, threadEntry.th32ThreadID);

                        threads.push_back(thread);
                    }
                }

                threadEntry.dwSize = sizeof(threadEntry);
            }
            while (Thread32Next(handle, &threadEntry));

            for (const HANDLE thread : threads)
            {
                SuspendThread(thread);
            }

            PlaySoundA("SystemExclamation", nullptr, SND_SYNC);
            Sleep(ms);

            for (const HANDLE thread : threads)
            {
                ResumeThread(thread);

                CloseHandle(thread);
            }

            CloseHandle(handle);
        }

        void Begin() override
        {
            if (Get<Random>()->Uniform(0, 1))
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(Get<Random>()->Uniform(0, 2000)));
            }

            if (Get<Random>()->Uniform(0, 1))
            {
                const uint delay = Get<Random>()->Uniform(250u, 2000u);

                SleepAllThreads(delay);
                std::this_thread::sleep_for(std::chrono::milliseconds(Get<Random>()->Uniform(0, 2000)));
            }

            SleepAllThreads(Get<Random>()->Uniform(3000u, 10000u));
        }

    public:
        explicit FakeCrash(const EffectInfo& effectInfo) : ActiveEffect(effectInfo) {}
};

// clang-format off
SetupEffect(FakeCrash, {
    .effectName = "Fake Crash",
    .description = "It looks and feels very real. But unlike a real crash, you get to keep playing after. Magical isn't it?",
    .category = EffectType::Meta,
    .timingModifier = 0.0f,
    .isTimed = false
});