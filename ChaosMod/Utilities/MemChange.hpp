#pragma once

enum class MemEvent
{
    None = 0,
    OnUpdate = 1 << 0,
    OnFixedUpdate = 1 << 2,
};

class final MemChange
{
        DWORD address;
        size_t length;
        std::function<void(MemEvent triggeringEvent, const std::vector<byte>& currentData, float frameDelta)> callback;

        std::vector<byte> originalData;

        static std::unordered_map<MemEvent, std::vector<MemChange*>> memoryChanges;

    public:
        explicit MemChange(MemEvent desiredEvents, const DWORD address, const size_t length,
                           const std::function<void(MemEvent triggeringEvent, const std::vector<byte>& currentData, float frameDelta)> callback);

        void Reset();
        ~MemChange();

        static void TriggerEvent(MemEvent event, float frameDelta = 0.0f);
};
