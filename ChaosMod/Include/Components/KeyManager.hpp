#pragma once

class KeyManager final : public Component
{
        std::map<int, int> keyMap = {};
        std::vector<int> possibleKeys;
        std::vector<Utils::Keys> blockedKeys = {};

        inline static bool blockPlayerInput = false;
        bool randomiseKeys = false;
        inline static bool nextKeyRandom = false;

    public:
        static bool __stdcall HandleKey(int keyCmd);

        KeyManager();
        void FireKeyCommand(Utils::Keys key);
        void ToggleAllowedKey(Utils::Keys key, bool allow);
        void ToggleRandomisedKeys();
        static void BlockPlayerInput(bool input);
};
