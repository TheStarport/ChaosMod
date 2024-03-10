#include "PCH.hpp"

#include "Systems/ImguiComponents/Debug.hpp"

void Log(const std::string& log)
{
    DebugMenu::Log(log);

    static char path[MAX_PATH];
    GetUserDataPath(path);
    static std::ofstream file(std::format("{}/chaos.log", path), std::ios::beg | std::ios::trunc);

    auto now = std::chrono::system_clock::now();
    file << std::format("{0:%F_%T}: ", now) << log << std::endl;
}

void Assert(const bool cond, const std::string& text, const std::string& file, const int line)
{
    if (!cond)
    {
        throw std::runtime_error(text + ". In file: " + file + " on line: " + std::to_string(line));
    }
}
