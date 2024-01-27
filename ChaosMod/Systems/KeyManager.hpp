#pragma once

class KeyManager : public Singleton<KeyManager>
{
        std::map<int, int> keyMap = {};
        std::vector<int> possibleKeys;
        std::vector<Utils::Keys> blockedKeys = {};

        inline static bool blockPlayerInput = false;
        bool randomiseKeys = false;
        inline static bool nextKeyRandom = false;
        static bool __stdcall HandleKey(int keyCmd);

    public:
        KeyManager();
        void FireKeyCommand(Utils::Keys key);
        void ToggleAllowedKey(Utils::Keys key, bool allow);
        void ToggleRandomisedKeys();
        static void BlockPlayerInput(bool input);
};
