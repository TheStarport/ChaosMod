#include "PCH.hpp"

#include "Systems/UiManager.hpp"

void Log(const std::string& log) { UiManager::i()->DebugLog(log); }
