#pragma once

#define _USE_MATH_DEFINES
#include <FLCoreDefs.h>
#include <Windows.h>
#include <random>
#include <cmath>
#include <algorithm> 
#include <cctype>
#include <locale>
#include <sstream>
#include <chrono>
#include <functional>

#include <FLCoreCommon.h>

#define RadToDeg(x, xx) static_cast<float>(((x / xx) * (180 / M_PI)))

#ifndef IMPORT
#define IMPORT _declspec(dllimport)
#define EXPORT _declspec(dllexport)
#endif

#pragma comment(lib, "Shell32.lib")

extern DWORD dummy;
#define ProtectExecuteReadWrite( addr, size ) VirtualProtect( addr, size, PAGE_EXECUTE_READWRITE, &dummy )
#define ProtectReadWrite( addr, size ) VirtualProtect( addr, size, PAGE_READWRITE, &dummy )

template <typename T, size_t N>
constexpr size_t countof(T(&)[N])
{
	return N;
}

namespace Utils
{
	namespace String
	{
		// Utils
		std::string IniGetS(const std::string& scFile, const std::string& scApp, const std::string& scKey, const std::string& scDefault);

		// String
		std::wstring stows(const std::string& scText);
		std::string wstos(const std::wstring& wscText);
		std::string itos(int i);
		std::string ftos(float i);
		std::string itohs(UINT i);
		std::wstring GetParam(const std::wstring& wscLine, wchar_t wcSplitChar, uint iPos);
		std::wstring GetParamToEnd(std::wstring wscLine, wchar_t wcSplitChar, uint iPos);
		std::string GetParam(const std::string& scLine, char cSplitChar, int iPos);
		void LeftTrim(std::string& trim);
		void RightTrim(std::string& trim);
		void Trim(std::string& trim);
		bool StartsWith(std::string const& fullString, std::string const& start);
		bool EndsWith(std::string const& fullString, std::string const& ending);
		bool Replace(std::string& str, const std::string& phrase, const std::string& result);
		void ReplaceAll(std::string& str, const std::string& phrase, const std::string& result);
		bool ReplaceW(std::wstring& str, const std::wstring& phrase, const std::wstring& result);
		void ReplaceAllW(std::wstring& str, const std::wstring& phrase, const std::wstring& result);
		std::vector<std::string> Split(const std::string& s, char delim);
		std::wstring XMLText(const std::wstring& wscText);
		bool HkFMsgEncodeMsg(std::wstring wscMessage, char* szBuf, uint iSize, uint& iRet);
		std::string GenerateTimestamp();
		std::string FmtStr(const char* text, ...);
	}

	namespace Memory
	{
		// Memory
		void SetGuardPages(void* pAddress, int iSize);
		void WriteProcMem(DWORD address, void* pMem, int iSize);
		void WriteProcMem(void* pAddress, void* pMem, int iSize);
		void NopAddress(unsigned int address, unsigned int pSize);
		void ReadProcMem(void* pAddress, void* pMem, int iSize);
		FARPROC PatchCallAddr(char* hMod, DWORD dwInstallAddress, char* dwHookFunction);
		typedef void* (*RTDynamicCast)(void*, long, void*, void*, int);
		inline RTDynamicCast dynCast = RTDynamicCast(GetProcAddress(GetModuleHandle(L"msvcrt.dll"), "__RTDynamicCast"));
		
		// Detour
		void Detour(unsigned char* pOFunc, void* pHkFunc, unsigned char* originalData);
		void* DetourLength(BYTE* src, const BYTE* dst, const int len);
		void* DetourClassLength(BYTE* src, const BYTE* dst, const int len);
		void UnDetour(unsigned char* pOFunc, unsigned char* originalData);
		void Patch(unsigned char* pOFunc, void* pHkFunc);
		bool IsMemReadable(void* ptr, size_t byteCount);

		// Break some data that will cause the game to crash, while not directly pointing to which method caused it to happen.
		void CorruptData();
	}

	namespace Math
	{
		float DegreesToRadians(float deg);
		float RadiansToDegrees(float rad);

		float VectorLengthSquared(Vector v1);
		Vector VectorSubtract(Vector v1, Vector v2);
		Vector VectorAdd(Vector v1, Vector v2);
		Vector VectorMultiply(Vector v1, Vector v2);
		
		float QuaternionDotProduct(Quaternion left, Quaternion right);
		float QuaternionAngleDifference(Quaternion left, Quaternion right);
	}

	template<class T> inline T operator~ (T a) { return (T)~(int)a; }
	template<class T> inline T operator| (T a, T b) { return (T)((int)a | (int)b); }
	template<class T> inline T operator& (T a, T b) { return (T)((int)a & (int)b); }
	template<class T> inline T operator^ (T a, T b) { return (T)((int)a ^ (int)b); }
	template<class T> inline T& operator|= (T& a, T b) { return (T&)((int&)a |= (int)b); }
	template<class T> inline T& operator&= (T& a, T b) { return (T&)((int&)a &= (int)b); }
	template<class T> inline T& operator^= (T& a, T b) { return (T&)((int&)a ^= (int)b); }

	double GetCurrentTimestamp();
	uint GetClientId();
	
	// Get data values from game
	Archetype::Ship* GetShipArch();
	CShip* GetCShip();
	EquipDescList* GetEquipDescList();

	// Calculate the distance between the two vectors
	float CalculateDistance(Vector v1, Vector v2);

	// Get Launch Args
	bool FindLaunchArg(std::string argToFind);
	class CommandLineParser
	{
	public:
		CommandLineParser();
		[[nodiscard]] std::wstring GetCmdOption(const std::wstring& option) const;
		[[nodiscard]] bool CmdOptionExists(const std::wstring& option) const;
		int GetIntCmd(const std::wstring& option) const;
		DWORD GetHexCmd(const std::wstring& option) const;
		uint64_t GetLongInt(const std::wstring& option) const;
	private:
		static std::vector <std::wstring> tokens;
	};

	enum Keys
	{	// Comment denotes unused - Hex Address will be listed for overwriting
		USER_NONE,
		USER_KEY,
		USER_CHAR,
		USER_BUTTON, // 00614DE4
		USER_WHEEL,
		USER_MOVE,
		USER_BUTTON0,
		USER_BUTTON1,
		USER_BUTTON2,
		USER_BUTTON3,
		USER_BUTTON4,
		USER_BUTTON5,
		USER_BUTTON6,
		USER_BUTTON7,
		USER_EVENTS, // 00614E10
		USER_CANCEL,
		USER_STACKABLE_CANCEL, // 00614E18
		USER_ENTER,
		USER_NO, // 00614E20
		USER_YES, // 00614E24
		USER_PAUSE,
		USER_RESET,
		USER_SAVE_GAME, // 00614E30
		USER_FULLSCREEN,
		USER_USE_ITEM, // 00614E38
		USER_ACTIVATE_ITEM, // 00614E3C
		USER_NEXT_ITEM, // 00614E40
		USER_PREV_ITEM, // 00614E44
		USER_SET_TARGET, // 00614E48
		USER_NEXT_TARGET,
		USER_PREV_TARGET,
		USER_NEXT_WEAPON_SUBTARGET, // 00614E54
		USER_PREV_WEAPON_SUBTARGET, //00614E58
		USER_NEXT_SUBTARGET,
		USER_PREV_SUBTARGET,
		USER_CLEAR_TARGET,
		USER_NEXT_ENEMY,
		USER_PREV_ENEMY,
		USER_CLOSEST_ENEMY,
		USER_ZOOM_IN, // 00614E70
		USER_ZOOM_OUT, // 00614E74
		USER_ZOOM_TOGGLE, // 00614E78
		USER_X_ROTATE,
		USER_X_UNROTATE,
		USER_Y_ROTATE,
		USER_Y_UNROTATE,
		USER_Z_ROTATE,
		USER_Z_UNROTATE,
		USER_VIEW_RESET, // 00614E98
		USER_COCKPIT_CAMERA_MODE,
		USER_LOOK_ROTATE_CAMERA_LEFT,
		USER_LOOK_ROTATE_CAMERA_RIGHT,
		USER_LOOK_ROTATE_CAMERA_UP,
		USER_LOOK_ROTATE_CAMERA_DOWN,
		USER_LOOK_ROTATE_CAMERA_RESET,
		USER_CHASE_CAMERA_MODE, // 00614EB4
		USER_REARVIEW_CAMERA_MODE,
		USER_REARVIEW_CAMERA_MODE_OFF,
		USER_AFTERBURN,
		USER_AFTERBURN_OFF,
		USER_AFTERBURN_ON,
		USER_TOGGLE_AUTO_AVOIDANCE,
		USER_TOGGLE_AUTO_LEVEL,
		USER_TOGGLE_LEVEL_CAMERA,
		USER_WARP, // 00614ED8
		USER_ENGINE_TOGGLE, // 00614EDC
		USER_SET_THROTTLE, // 00614EE0
		USER_INC_THROTTLE,
		USER_DEC_THROTTLE,
		USER_CRUISE,
		USER_THROTTLE_0, //00614EF0
		USER_THROTTLE_10, // 00614EF4
		USER_THROTTLE_20, // 00614EF8
		USER_THROTTLE_25, // 00614EFC
		USER_THROTTLE_30, // 00614F00
		USER_THROTTLE_40, // 00614F04
		USER_THROTTLE_50, // 00614F08
		USER_THROTTLE_60, // 00614F0C
		USER_THROTTLE_70, // 00614F10 
		USER_THROTTLE_75, // 00614F14
		USER_THROTTLE_80, // 00614F18
		USER_THROTTLE_90, // 00614F1C
		USER_THROTTLE_100, // 00614F20
		USER_RADAR, // 00614F24
		USER_CONTACT_LIST,
		USER_RADAR_ZOOM,
		USER_SWITCH_TO_PLAYER_SHIP,
		USER_SWITCH_TO_WEAPON_LIST,
		USER_SWITCH_TO_TARGET,
		USER_FORMATION_LIST,
		USER_RADAR_ZOOM_IN, // 00614F40
		USER_RADAR_ZOOM_OUT, // 00614F44
		USER_RADIO,
		USER_CHAT,
		USER_STATUS,
		USER_TARGET,
		USER_STATUS_MODE, // 00614F58
		USER_TOGGLE_MAINFRAME, // 00614F5C
		USER_NN, // 00614F60
		USER_NAV_MAP,
		USER_NAVMAP, // 00614F68
		USER_PLAYER_STATS,
		USER_INVENTORY,
		USER_STORY_STAR,
		USER_CHAT_WINDOW,
		USER_GROUP_WINDOW,
		USER_HELP,
		USER_INVENTORY_CLOSE,
		NN_TOGGLE_OPEN, // 00614F88
		USER_MINIMIZE_HUD,
		USER_DISPLAY_LAST_OBJECTIVE,
		USER_COLLECT_LOOT,
		USER_ACTIONS, // 00614F98
		USER_MANEUVER, // 00614F9C
		USER_SET_MANEUVER_DIRECTION, // 00614FA0
		USER_MANEUVER_WINDOW, // 00614FA4
		USER_MANEUVER_TRAIL,
		USER_MANEUVER_AFTERBURNER, // 00614FAC
		USER_MANEUVER_EVADE,
		USER_MANEUVER_ENGINEKILL,
		USER_MANEUVER_BRAKE_REVERSE,
		USER_MANEUVER_DOCK,
		USER_MANEUVER_GOTO,
		USER_MANEUVER_FACE,
		USER_MANEUVER_CRUISE, // 00614FC8
		USER_MANEUVER_TRADE_LANE, // 00614FC
		USER_MANEUVER_DRASTIC_EVADE,
		USER_MANEUVER_FORMATION,
		USER_MANEUVER_STRAFE,
		USER_MANEUVER_TRAIL_CLOSER,
		USER_MANEUVER_TRAIL_FARTHER,
		USER_MANEUVER_CORKSCREW_EVADE,
		USER_MANEUVER_SLIDE_EVADE,
		USER_MANEUVER_SLIDE_EVADE_LEFT,
		USER_MANEUVER_SLIDE_EVADE_RIGHT,
		USER_MANEUVER_SLIDE_EVADE_UP,
		USER_MANEUVER_SLIDE_EVADE_DOWN,
		USER_ACTIVATE_MANEUVER, // 00614FFC
		USER_SCAN_CARGO,
		USER_TRACTOR_BEAM,
		USER_JAMMER, // 00615008
		USER_STEALTH, // 0061500C
		USER_CLOAK, // 00615010
		USER_REPAIR_HEALTH,
		USER_REPAIR_SHIELD,
		USER_WEAPON_GROUP1,
		USER_WEAPON_GROUP2,
		USER_WEAPON_GROUP3,
		USER_WEAPON_GROUP4,
		USER_WEAPON_GROUP5,
		USER_WEAPON_GROUP6,
		USER_TOGGLE_WEAPON1,
		USER_TOGGLE_WEAPON2,
		USER_TOGGLE_WEAPON3,
		USER_TOGGLE_WEAPON4,
		USER_TOGGLE_WEAPON5,
		USER_TOGGLE_WEAPON6,
		USER_TOGGLE_WEAPON7,
		USER_TOGGLE_WEAPON8,
		USER_TOGGLE_WEAPON9,
		USER_TOGGLE_WEAPON10,
		USER_FIRE_WEAPON1,
		USER_FIRE_WEAPON2,
		USER_FIRE_WEAPON3,
		USER_FIRE_WEAPON4,
		USER_FIRE_WEAPON5,
		USER_FIRE_WEAPON6,
		USER_FIRE_WEAPON7,
		USER_FIRE_WEAPON8,
		USER_FIRE_WEAPON9,
		USER_FIRE_WEAPON10,
		USER_FIRE_WEAPON_GROUP1,
		USER_FIRE_WEAPON_GROUP2,
		USER_FIRE_WEAPON_GROUP3,
		USER_FIRE_WEAPON_GROUP4,
		USER_FIRE_WEAPON_GROUP5,
		USER_FIRE_WEAPON_GROUP6,
		USER_ASSIGN_WEAPON_GROUP1,
		USER_ASSIGN_WEAPON_GROUP2,
		USER_ASSIGN_WEAPON_GROUP3,
		USER_ASSIGN_WEAPON_GROUP4,
		USER_ASSIGN_WEAPON_GROUP5,
		USER_ASSIGN_WEAPON_GROUP6,
		USER_REMAPPABLE_LEFT,
		USER_REMAPPABLE_RIGHT,
		USER_REMAPPABLE_UP,
		USER_REMAPPABLE_DOWN,
		USER_FIRE_FORWARD,
		USER_LAUNCH_MISSILES,
		USER_LAUNCH_MINES,
		USER_LAUNCH_COUNTERMEASURES,
		USER_AUTO_TURRET,
		USER_LAUNCH_TORPEDOS,
		USER_LAUNCH_CRUISE_DISRUPTORS,
		USER_TURN, // 006150E0
		USER_NEXT_OBJECT,
		USER_PREV_OBJECT,
		USER_EXIT_GAME,
		USER_MANEUVER_FREE_FLIGHT,
		USER_FIRE_WEAPONS,
		USER_TURN_SHIP,
		USER_GROUP_INVITE,
		USER_TRADE_REQUEST,
		USER_SCREEN_SHOT
	};

	// Random
	template <typename RandomGenerator = std::default_random_engine>
	struct random_selector
	{
		//On most platforms, you probably want to use std::random_device("/dev/urandom")()
		random_selector(RandomGenerator g = RandomGenerator(std::random_device()()))
			: gen(g) {}

		template <typename Iter>
		Iter select(Iter start, Iter end) {
			std::uniform_int_distribution<> dis(0, std::distance(start, end) - 1);
			std::advance(start, dis(gen));
			return start;
		}

		//convenience function
		template <typename Iter>
		Iter operator()(Iter start, Iter end) {
			return select(start, end);
		}

		//convenience function that works on anything with a sensible begin() and end(), and returns with a ref to the value type
		template <typename Container>
		auto operator()(const Container& c) -> decltype(*begin(c))& {
			return *select(begin(c), end(c));
		}

	private:
		RandomGenerator gen;
	};

	template <class Key, class Value, int N>
	class CompileTimeMap {
		public:
		struct KV {
			Key   key;
			Value value;
		};

		constexpr Value  operator[] (Key key) const
		{
			return Get (key);
		}

		private:
		constexpr Value  Get (Key key, int i = 0) const
		{
			return i == N ?
			       KeyNotFound () :
			       pairs[i].key == key ? pairs[i].value : Get (key, i + 1);
		}

		static Value KeyNotFound ()     // not constexpr
		{
			return {};
		}

		public:
		KV  pairs[N];
	};
}