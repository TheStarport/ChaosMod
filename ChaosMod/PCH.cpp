#include "PCH.hpp"

#include "Systems/UiManager.hpp"

void Log(const std::string& log) { UiManager::i()->DebugLog(log); }
void Assert(const bool cond, const std::string& text, const std::string& file, const int line)
{
    if (!cond)
    {
        throw std::runtime_error(text + ". In file: " + file + " on line: " + std::to_string(line));
    }
}
