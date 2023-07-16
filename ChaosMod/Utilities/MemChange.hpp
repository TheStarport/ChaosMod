#pragma once

enum class MemEvent
{
    None = 0,
    OnUpdate = 1 << 0,
    OnFixedUpdate = 1 << 2,
};

class MemChange final
{
        bool active;
        DWORD address;
        size_t length;
        std::function<void(const std::vector<byte>& currentData, float frameDelta)> callback;

        std::vector<byte> originalData;

        static std::unordered_map<MemEvent, std::vector<MemChange*>> memoryChanges;

    public:
        explicit MemChange(MemEvent desiredEvents, DWORD address, size_t length,
                           std::function<void(const std::vector<byte>& currentData, float frameDelta)> callback);
        ~MemChange();

        void Reset();

        static void TriggerEvent(MemEvent event, float frameDelta = 0.0f);

        struct ActiveChange
        {
                uint address;
                size_t length;
                bool active;
        };

        static std::vector<ActiveChange> GetMemChanges();
};
