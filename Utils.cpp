#include "PCH.hpp"

#include "Utils.hpp"
#include <shellapi.h>

DWORD dummy;

namespace Utils
{
    std::vector<std::wstring> CommandLineParser::tokens;

    bool FindLaunchArg(std::string argToFind)
    {
        std::string cmdLine = GetCommandLineA();
        int argc = 0;
        while (true)
        {
            std::string arg = String::GetParam(cmdLine, ' ', argc++);
            if (arg.length() == 0)
            {
                return false;
            }

            if (arg.find(argToFind) != 0)
            {
                continue;
            }

            return true;
        }
    }

    namespace String
    {
        std::string IniGetS(const std::string& scFile, const std::string& scApp, const std::string& scKey, const std::string& scDefault)
        {
            char szRet[2048 * 2];
            GetPrivateProfileStringA(scApp.c_str(), scKey.c_str(), scDefault.c_str(), szRet, sizeof(szRet), scFile.c_str());
            return szRet;
        }

        /// 8 bit chars to wide characters
        std::wstring stows(const std::string& scText)
        {
            int iSize = MultiByteToWideChar(CP_ACP, 0, scText.c_str(), -1, nullptr, 0);
            wchar_t* wszText = new wchar_t[iSize];
            MultiByteToWideChar(CP_ACP, 0, scText.c_str(), -1, wszText, iSize);
            std::wstring wscRet = wszText;
            delete[] wszText;
            return wscRet;
        }

        /// Wide characters to 8 bit characters
        std::string wstos(const std::wstring& wscText)
        {
            uint iLen = (uint)wscText.length() + 1;
            char* szBuf = new char[iLen];
            WideCharToMultiByte(CP_ACP, 0, wscText.c_str(), -1, szBuf, iLen, nullptr, nullptr);
            std::string scRet = szBuf;
            delete[] szBuf;
            return scRet;
        }

        /// Convert int to string.
        std::string itos(int i)
        {
            char szBuf[100];
            sprintf_s(szBuf, "%u", i);
            return szBuf;
        }

        /// Convert float to string.
        std::string ftos(float i)
        {
            char szBuf[100];
            _snprintf_s(szBuf, sizeof(szBuf), "%0.8f", i);
            return szBuf;
        }

        /// Convert UINT to string.
        std::string itohs(UINT i)
        {
            char szBuf[100];
            sprintf_s(szBuf, "%08X", i);
            return szBuf;
        }

        /// Return the field out of the line seperated by the split char.
        std::wstring GetParam(const std::wstring& wscLine, wchar_t wcSplitChar, uint iPos)
        {
            uint i = 0, j = 0;

            std::wstring wscResult = L"";
            for (i = 0, j = 0; (i <= iPos) && (j < wscLine.length()); j++)
            {
                if (wscLine[j] == wcSplitChar)
                {
                    while (((j + 1) < wscLine.length()) && (wscLine[j + 1] == wcSplitChar))
                    {
                        j++; // skip "whitechar"
                    }

                    i++;
                    continue;
                }

                if (i == iPos)
                {
                    wscResult += wscLine[j];
                }
            }

            return wscResult;
        }

        std::wstring GetParamToEnd(std::wstring wscLine, wchar_t wcSplitChar, uint iPos)
        {
            for (uint i = 0, iCurArg = 0; (i < wscLine.length()); i++)
            {
                if (wscLine[i] == wcSplitChar)
                {
                    iCurArg++;

                    if (iCurArg == iPos)
                    {
                        return wscLine.substr(i + 1);
                    }

                    while (((i + 1) < wscLine.length()) && (wscLine[i + 1] == wcSplitChar))
                    {
                        i++; // skip "whitechar"
                    }
                }
            }

            return L"";
        }

        /// Return the field out of the line seperated by the split char.
        std::string GetParam(const std::string& scLine, char cSplitChar, int iPos)
        {
            std::string scResult = "";
            for (int i = 0, j = 0; (i <= iPos) && (j < (int)scLine.length()); j++)
            {
                if (scLine[j] == cSplitChar)
                {
                    while (((j + 1) < (int)scLine.length()) && (scLine[j + 1] == cSplitChar))
                    {
                        j++; // skip "whitechar"
                    }

                    i++;
                    continue;
                }

                if (i == iPos)
                {
                    scResult += scLine[j];
                }
            }

            return scResult;
        }

        void LeftTrim(std::string& s)
        {
            s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int ch) { return !std::isspace(ch); }));
        }

        void RightTrim(std::string& s)
        {
            s.erase(std::find_if(s.rbegin(), s.rend(), [](int ch) { return !std::isspace(ch); }).base(), s.end());
        }

        void Trim(std::string& s)
        {
            LeftTrim(s);
            RightTrim(s);
        }

        bool StartsWith(const std::string& fullString, const std::string& start) { return fullString.rfind(start, 0) == 0; }

        bool EndsWith(const std::string& fullString, const std::string& ending)
        {
            if (fullString.length() >= ending.length())
            {
                return 0 == fullString.compare(fullString.length() - ending.length(), ending.length(), ending);
            }

            return false;
        }

        bool Replace(std::string& str, const std::string& phrase, const std::string& result)
        {
            size_t start_pos = str.find(phrase);
            if (start_pos == std::string::npos)
            {
                return false;
            }

            str.replace(start_pos, phrase.length(), result);
            return true;
        }

        bool ReplaceW(std::wstring& str, const std::wstring& phrase, const std::wstring& result)
        {
            size_t start_pos = str.find(phrase);
            if (start_pos == std::wstring::npos)
            {
                return false;
            }

            str.replace(start_pos, phrase.length(), result);
            return true;
        }

        void ReplaceAllW(std::wstring& str, const std::wstring& phrase, const std::wstring& result)
        {
            while (ReplaceW(str, phrase, result)) {}
        }

        void ReplaceAll(std::string& str, const std::string& phrase, const std::string& result)
        {
            while (Replace(str, phrase, result)) {}
        }

        /** Encode any XML markup characters */
        std::wstring XMLText(const std::wstring& wscText)
        {
            std::wstring wscRet;
            for (uint i = 0; (i < wscText.length()); i++)
            {
                if (wscText[i] == '<')
                {
                    wscRet.append(L"&#60;");
                }
                else if (wscText[i] == '>')
                {
                    wscRet.append(L"&#62;");
                }
                else if (wscText[i] == '&')
                {
                    wscRet.append(L"&#38;");
                }
                else
                {
                    wscRet.append(1, wscText[i]);
                }
            }

            return wscRet;
        }

        /** This function is not exported by FLHook so we include it here */
        bool HkFMsgEncodeMsg(std::wstring wscMessage, char* szBuf, uint iSize, uint& iRet)
        {
            std::wstring wscMsg = L"<?xml version=\"1.0\" encoding=\"UTF-16\"?><RDL><PUSH/>";
            wscMsg += L"<TRA data=\"0x00000000\" mask=\"-1\"/><TEXT>" + XMLText(wscMessage) + L"</TEXT>";
            wscMsg += L"<PARA/><POP/></RDL>\x000A\x000A";

            XMLReader rdr;
            RenderDisplayList rdl;
            if (!rdr.read_buffer(rdl, (const char*)wscMsg.c_str(), (uint)wscMsg.length() * 2))
            {
                return false;
            }

            BinaryRDLWriter rdlwrite;
            return rdlwrite.write_buffer(rdl, szBuf, iSize, iRet);
        }

        std::string GenerateTimestamp()
        {
            std::time_t rawtime;
            char timestamp[100];
            std::time(&rawtime);
            std::tm timeInfo;
            localtime_s(&timeInfo, &rawtime);
            std::strftime(timestamp, 80, "%Y%m%d-%H:%M:%S", &timeInfo);
            return std::string("[") + timestamp + "]";
        }

        template <typename Out>
        void Split(const std::string& s, char delim, Out result)
        {
            std::istringstream iss(s);
            std::string item;
            while (std::getline(iss, item, delim))
            {
                *result++ = item;
            }
        }

        std::vector<std::string> Split(const std::string& s, char delim)
        {
            std::vector<std::string> elems;
            Split(s, delim, std::back_inserter(elems));
            return elems;
        }
        std::string FmtStr(const char* text, ...)
        {
            char buffer[4096];
            va_list args;
            va_start(args, text);
            vsnprintf(buffer, 4096, text, args);
            va_end(args);
            return buffer;
        }
    } // namespace String

    namespace Memory
    {
        FARPROC PatchCallAddr(char* hMod, DWORD dwInstallAddress, char* dwHookFunction)
        {
            DWORD dwRelAddr;
            ReadProcMem(hMod + dwInstallAddress + 1, &dwRelAddr, 4);

            DWORD dwOffset = (DWORD)dwHookFunction - (DWORD)(hMod + dwInstallAddress + 5);
            WriteProcMem(hMod + dwInstallAddress + 1, &dwOffset, 4);

            return (FARPROC)(hMod + dwRelAddr + dwInstallAddress + 5);
        }

        // Set guard page bits on the specified range of addresses
        void SetGuardPages(void* pAddress, int iSize)
        {
            HANDLE hProc = OpenProcess(PROCESS_VM_OPERATION | PROCESS_VM_WRITE | PROCESS_VM_READ, FALSE, GetCurrentProcessId());
            DWORD dwOld;
            VirtualProtectEx(hProc, pAddress, iSize, PAGE_EXECUTE_READ | PAGE_GUARD, &dwOld);
            CloseHandle(hProc);
        }

        // Write proc mem but cast the address to a pointer for you
        void WriteProcMem(DWORD address, void* pMem, int iSize) { WriteProcMem(PVOID(address), pMem, iSize); }

        void NopAddress(unsigned int address, unsigned int pSize)
        {
            DWORD dwOldProtection = 0;
            VirtualProtect((void*)address, pSize, PAGE_READWRITE, &dwOldProtection);
            memset((void*)address, 0x90, pSize);
            VirtualProtect((void*)address, pSize, dwOldProtection, NULL);
        }

        /// Write iSize bytes from pMem into pAddress
        void WriteProcMem(void* pAddress, void* pMem, int iSize)
        {
            HANDLE hProc = OpenProcess(PROCESS_VM_OPERATION | PROCESS_VM_WRITE | PROCESS_VM_READ, FALSE, GetCurrentProcessId());
            DWORD dwOld;
            VirtualProtectEx(hProc, pAddress, iSize, PAGE_EXECUTE_READWRITE, &dwOld);
            WriteProcessMemory(hProc, pAddress, pMem, iSize, nullptr);
            CloseHandle(hProc);
        }

        /// Read iSize bytes from pAddress into pMem
        void ReadProcMem(void* pAddress, void* pMem, int iSize)
        {
            HANDLE hProc = OpenProcess(PROCESS_VM_OPERATION | PROCESS_VM_WRITE | PROCESS_VM_READ, FALSE, GetCurrentProcessId());
            DWORD dwOld;
            VirtualProtectEx(hProc, pAddress, iSize, PAGE_EXECUTE_READWRITE, &dwOld);
            ReadProcessMemory(hProc, pAddress, pMem, iSize, nullptr);
            CloseHandle(hProc);
        }

        void Detour(unsigned char* pOFunc, void* pHkFunc, unsigned char* originalData)
        {
            DWORD dwOldProtection = 0; // Create a DWORD for VirtualProtect calls to allow us to write.
            BYTE bPatch[5];            // We need to change 5 bytes and I'm going to use memcpy so this is the simplest way.
            bPatch[0] = 0xE9;          // Set the first byte of the byte array to the op code for the JMP instruction.
            VirtualProtect((void*)pOFunc, 5, PAGE_EXECUTE_READWRITE, &dwOldProtection); // Allow us to write to the memory we need to change
            DWORD dwRelativeAddress = (DWORD)pHkFunc - (DWORD)pOFunc - 5;               // Calculate the relative JMP address.
            memcpy(&bPatch[1], &dwRelativeAddress, 4);                                  // Copy the relative address to the byte array.
            memcpy(originalData, pOFunc, 5);
            memcpy(pOFunc, bPatch, 5);                                  // Change the first 5 bytes to the JMP instruction.
            VirtualProtect((void*)pOFunc, 5, dwOldProtection, nullptr); // Set the protection back to what it was.
        }

        void* DetourLength(BYTE* src, const BYTE* dst, const int len)
        {
            BYTE* jmp = (BYTE*)malloc(len + 5);
            DWORD dwback;

            VirtualProtect(src, len, PAGE_READWRITE, &dwback);

            memcpy(jmp, src, len);
            jmp += len;

            jmp[0] = 0xE9;
            *(DWORD*)(jmp + 1) = (DWORD)(src + len - jmp) - 5;

            src[0] = 0xE9;
            *(DWORD*)(src + 1) = (DWORD)(dst - src) - 5;

            VirtualProtect(src, len, dwback, &dwback);

            return (jmp - len);
        }

        void* DetourClassLength(BYTE* src, const BYTE* dst, const int len)
        {
            BYTE* jmp = (BYTE*)malloc(len + 5);
            BYTE* pre = (BYTE*)malloc(len + 8);
            DWORD dwBack;
            VirtualProtect(src, len, PAGE_READWRITE, &dwBack);
            memcpy(jmp + 3, src, len);

            // calculate callback function call
            jmp[0] = 0x58;       // pop eax
            jmp[1] = 0x59;       // pop ecx
            jmp[2] = 0x50;       // push eax
            jmp[len + 3] = 0xE9; // jmp
            *(DWORD*)(jmp + len + 4) = (DWORD)((src + len) - (jmp + len + 3)) - 5;

            // detour source function call
            src[0] = 0xE9;
            *(DWORD*)(src + 1) = (DWORD)(pre - src) - 5;

            for (int i = 5; i < len; i++)
            {
                src[i] = 0x90;
            }

            // detour source function call
            pre[0] = 0x58; // pop eax;
            pre[1] = 0x51; // push ecx
            pre[2] = 0x50; // push eax
            pre[3] = 0xE9; // jmp
            *(DWORD*)(pre + 4) = (DWORD)(dst - (pre + 3)) - 5;

            VirtualProtect(src, len, dwBack, &dwBack);
            return jmp;
        }

        void UnDetour(unsigned char* pOFunc, unsigned char* originalData)
        {
            DWORD dwOldProtection = 0;                                                  // Create a DWORD for VirtualProtect calls to allow us to write.
            VirtualProtect((void*)pOFunc, 5, PAGE_EXECUTE_READWRITE, &dwOldProtection); // Allow us to write to the memory we need to change
            memcpy(pOFunc, originalData, 5);
            VirtualProtect((void*)pOFunc, 5, dwOldProtection, nullptr); // Set the protection back to what it was.
        }

        void Patch(unsigned char* pOFunc, void* pHkFunc)
        {
            DWORD dwOldProtection = 0; // Create a DWORD for VirtualProtect calls to allow us to write.
            BYTE bPatch[5];            // We need to change 5 bytes and I'm going to use memcpy so this is the simplest way.
            bPatch[0] = 0xE9;          // Set the first byte of the byte array to the op code for the JMP instruction.
            VirtualProtect((void*)pOFunc, 5, PAGE_EXECUTE_READWRITE, &dwOldProtection); // Allow us to write to the memory we need to change
            DWORD dwRelativeAddress = (DWORD)pHkFunc - (DWORD)pOFunc - 5;               // Calculate the relative JMP address.
            memcpy(&bPatch[1], &dwRelativeAddress, 4);                                  // Copy the relative address to the byte array.
            memcpy(pOFunc, bPatch, 5);                                                  // Change the first 5 bytes to the JMP instruction.
            VirtualProtect((void*)pOFunc, 5, dwOldProtection, nullptr);                 // Set the protection back to what it was.
        }

        bool IsMemReadable(void* ptr, size_t byteCount)
        {
            MEMORY_BASIC_INFORMATION mbi;
            if (VirtualQuery(ptr, &mbi, sizeof(MEMORY_BASIC_INFORMATION)) == 0)
            {
                return false;
            }

            if (mbi.State != MEM_COMMIT)
            {
                return false;
            }

            if (mbi.Protect == PAGE_NOACCESS || mbi.Protect == PAGE_EXECUTE)
            {
                return false;
            }

            // This checks that the start of memory block is in the same "region" as the
            // end. If it isn't you "simplify" the problem into checking that the rest of
            // the memory is readable.
            size_t blockOffset = (size_t)((char*)ptr - (char*)mbi.AllocationBase);
            size_t blockBytesPostPtr = mbi.RegionSize - blockOffset;

            if (blockBytesPostPtr < byteCount)
            {
                return IsMemReadable((char*)ptr + blockBytesPostPtr, byteCount - blockBytesPostPtr);
            }

            return true;
        }

        void* Bork()
        {
            void* newAddress = new CommandLineParser();
            return newAddress;
        }

        void CorruptData()
        {
            // Dacom Acquire

            // Give some error
            // Hooks::HookUtilities::WriteSpew("Err: Unable to Init. Data bad. Attempting patch.");

            typedef void* (*DacomAcquire)();
            DacomAcquire address = DacomAcquire(0x65B31D0);
            Detour((PBYTE)address, Bork, PBYTE(malloc(5)));
        }
    } // namespace Memory

    double GetCurrentTimestamp()
    {
        unsigned __int64 iCount;
        QueryPerformanceCounter((LARGE_INTEGER*)&iCount);
        unsigned __int64 iFreq;
        QueryPerformanceFrequency((LARGE_INTEGER*)&iFreq);
        return iCount / double(iFreq);
    }

    namespace Math
    {
        constexpr float Pi = 3.14159265358979323846f;
        float DegreesToRadians(float deg) { return (deg * Pi) / 180.0f; }

        float RadiansToDegrees(float rad) { return (rad * 180.0f) / Pi; }

        float VectorLengthSquared(Vector v1) { return 0.0f; }

        float QuaternionDotProduct(Quaternion left, Quaternion right) { return left.x * right.x + left.y * right.y + left.z * right.z + left.w * right.w; }

        float QuaternionAngleDifference(Quaternion left, Quaternion right)
        {
            const float dot = QuaternionDotProduct(left, right);
            return acos(std::min(abs(dot), 0.1f)) * 0.02f * 57.29578f;
        }

        Vector VectorSubtract(Vector v1, Vector v2)
        {
            Vector v3;
            v3.x = v1.x - v2.x;
            v3.y = v1.y - v2.y;
            v3.z = v1.z - v2.z;
            return v3;
        }

        Vector VectorMultiply(Vector v1, Vector v2)
        {
            Vector v3;
            v3.x = v1.x * v2.x;
            v3.y = v1.y * v2.y;
            v3.z = v1.z * v2.z;
            return v3;
        }

        Vector VectorAdd(Vector v1, Vector v2)
        {
            Vector v3;
            v3.x = v1.x + v2.x;
            v3.y = v1.y + v2.y;
            v3.z = v1.z + v2.z;
            return v3;
        }
    } // namespace Math

    uint GetClientId()
    {
        uint* id = (uint*)0x673344;
        return *id;
    }

    Archetype::Ship* GetShipArch()
    {
        uint* shipId = (uint*)0x67337C;
        return Archetype::GetShip(*shipId);
    }

    __declspec(naked) CShip* GetCShip()
    {
        __asm
        {
			mov	eax, 0x54baf0
			call	eax
			test	eax, eax
			jz	noship
			add	eax, 12
			mov	eax, [eax + 4]
			noship:
			ret
        }
    }

    // Original Author: https://stackoverflow.com/a/868894/7910389
    CommandLineParser::CommandLineParser()
    {
        if (Utils::CommandLineParser::tokens.empty())
        {
            int argc = 0;
            LPWSTR* argv = CommandLineToArgvW(GetCommandLineW(), &argc);

            for (int i = 1; i < argc; ++i)
            {
                Utils::CommandLineParser::tokens.emplace_back(argv[i]);
            }
        }
    }

    // Get the value following a command line argument
    // Example, say this was your command line. something.exe -w -de -val=something
    // GetCmdOption on -w or -de would return an empty string, while doing it on -val would return "something".
    std::wstring CommandLineParser::GetCmdOption(const std::wstring& option) const
    {
        // If any of the strings contains the target substring, return the first one.
        const auto itr =
            std::find_if(this->tokens.begin(), this->tokens.end(), [option](const std::wstring& str) { return str.find(option) != std::wstring::npos; });

        if (itr != this->tokens.end() && itr->find('=') != std::wstring::npos)
        {
            return itr->substr(itr->find('=') + 1);
        }
        else
        {
            static const std::wstring empty_string;
            return empty_string;
        }
    }

    bool CommandLineParser::CmdOptionExists(const std::wstring& option) const
    {
        return std::find(this->tokens.begin(), this->tokens.end(), option) != this->tokens.end();
    }

    int CommandLineParser::GetIntCmd(const std::wstring& option) const
    {
        int value;
        const std::wstring wscVal = this->GetCmdOption(option);
        std::istringstream iss(String::wstos(wscVal));
        iss >> value;
        return value;
    }

    DWORD CommandLineParser::GetHexCmd(const std::wstring& option) const
    {
        DWORD value;
        const std::wstring wscVal = this->GetCmdOption(option);
        std::istringstream iss(String::wstos(wscVal));
        iss >> value;
        return value;
    }

    uint64_t CommandLineParser::GetLongInt(const std::wstring& option) const
    {
        uint64_t value;
        const std::wstring wscVal = this->GetCmdOption(option);
        std::istringstream iss(String::wstos(wscVal));
        iss >> value;
        return value;
    }

    float CalculateDistance(Vector v1, Vector v2)
    {
        float sq1 = v1.x - v2.x, sq2 = v1.y - v2.y, sq3 = v1.z - v2.z;
        return sqrt(sq1 * sq1 + sq2 * sq2 + sq3 * sq3);
    }

    EquipDescList* GetEquipDescList() { return (EquipDescList*)(0x672960); }
} // namespace Utils
