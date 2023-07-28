#pragma once

class KeyManager : public Singleton<KeyManager>
{
        std::map<int, int> keyMap = {};
        std::vector<int> possibleKeys;

        bool randomiseKeys = false;
        inline static bool nextKeyRandom = false;
        static bool __stdcall HandleKey(int keyCmd);

    public:
        KeyManager();
        void ToggleRandomisedKeys();
};
