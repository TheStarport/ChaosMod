#include <PCH.hpp>

#include <Components/TTS.hpp>
#include <shellapi.h>

void TTS::GenerateAudio(std::string message)
{
    generatedFile = std::format("{}/{}.wav", directory, Get<Random>()->UniformString<std::string>(12));
    auto voice = voices[Get<Random>()->Uniform(0u, voices.size() - 1)];

    HANDLE writeIn, readIn;
    SECURITY_ATTRIBUTES saPipe = { 0 };
    saPipe.nLength = sizeof(SECURITY_ATTRIBUTES);
    saPipe.bInheritHandle = TRUE;
    saPipe.lpSecurityDescriptor = nullptr;
    CreatePipe(&readIn, &writeIn, &saPipe, 0);
    SetHandleInformation(writeIn, HANDLE_FLAG_INHERIT, 0);

    PROCESS_INFORMATION procInfo = { nullptr };
    STARTUPINFOA procSi;

    ZeroMemory(&procSi, sizeof(STARTUPINFOA));
    procSi.cb = sizeof(STARTUPINFOA);
    procSi.hStdError = GetStdHandle(STD_ERROR_HANDLE);
    procSi.hStdOutput = GetStdHandle(STD_OUTPUT_HANDLE);
    procSi.hStdInput = readIn;
    procSi.dwFlags |= STARTF_USESTDHANDLES;

    auto command = std::format("./utilities/piper.exe --model \"{}\" --output_file \"{}\"", voice, generatedFile);
    CreateProcessA(nullptr, command.data(), nullptr, nullptr, TRUE, CREATE_NO_WINDOW, nullptr, nullptr, &procSi, &procInfo);
    const std::string msg = message;
    WriteFile(writeIn, msg.c_str(), msg.size(), nullptr, nullptr);
    CloseHandle(writeIn);
    CloseHandle(readIn);

    WaitForSingleObject(procInfo.hProcess, INFINITE);

    generatingComplete = true;
}

void TTS::PurgeAudioFiles() const
{
    for (auto file : std::filesystem::directory_iterator(directory))
    {
        if (file.path().string().ends_with(".wav"))
        {
            std::filesystem::remove(file.path());
        }
    }
}

TTS::TTS()
{
    std::string path;
    path.resize(MAX_PATH, '\0');
    GetUserDataPath(path.data());
    path.erase(std::ranges::find(path, '\0'), path.end());

    directory = std::format("{}/temp_sounds", path);
    CreateDirectoryA(directory.c_str(), nullptr);

    for (auto file : std::filesystem::directory_iterator("./utilities/voices"))
    {
        if (file.path().string().ends_with(".onnx"))
        {
            voices.emplace_back(file.path().string());
        }
    }
    ma_engine_init(nullptr, &engine);
};

TTS::~TTS()
{
    if (thread.joinable())
    {
        thread.join();
    }

    if (alreadyPlaying)
    {
        ma_sound_stop(&sound);
        ma_sound_uninit(&sound);
    }

    // ma_engine_uninit(&engine);
    PurgeAudioFiles();
}

void TTS::Update(const float delta)
{
    timeUntilAudioPurge -= delta;

    if (timeUntilAudioPurge < 0)
    {
        timeUntilAudioPurge = 120;
        PurgeAudioFiles();
    }

    if (!generatingComplete)
    {
        return;
    }

    generatingComplete = false;
    if (alreadyPlaying)
    {
        ma_sound_stop(&sound);
        ma_sound_uninit(&sound);
        alreadyPlaying = false;
    }

    if (ma_sound_init_from_file(&engine, generatedFile.c_str(), 0, nullptr, nullptr, &sound) != MA_SUCCESS)
    {
        Log(std::format("Failed to load {} with ma_sound", generatedFile));
        return;
    }

    ma_sound_set_volume(&sound, 0.4f);
    ma_sound_start(&sound);
    alreadyPlaying = true;
}

void TTS::Speak(const std::string_view msg)
{
    if (msg.empty() || msg.find("|") != std::string::npos || msg.front() == '"' || msg.front() == '\'' || msg.front() == '`')
    {
        MessageBoxA(nullptr, "Cannot TTS a message that contains a pipe symbol (|), or start with quotes.", "Bad TTS", MB_OK);
        return;
    }

    // Wait for thread termination, pretty much only going to happen if double time triggers two TTS commands in a row
    if (thread.joinable())
    {
        thread.join();
    }

    thread = std::jthread{ std::bind_front(&TTS::GenerateAudio, this, std::string(msg)) };
}
