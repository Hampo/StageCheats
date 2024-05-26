#include "pch.h"

#include "HackFunctions.h"
#include "MinHook.h"
#include "Shar.h"
#include <stdlib.h>
#include <unordered_map>

extern HMODULE dllHandle;

#ifdef _DEBUG
#define printf(x, ...) Hack_Printf(2, 0, x, __VA_ARGS__)
#define debug_printf(x, ...) Hack_Printf(2, 0, x, __VA_ARGS__)
#else
#define printf(x, ...) Hack_Printf(2, 0, x, __VA_ARGS__)
#define debug_printf(x, ...) 
#endif

class FakeMission
{
public:
	void __thiscall Initialize(unsigned int heap);
	static void* originalInitialize;
};
void* FakeMission::originalInitialize = 0;

class FakeMissionStage
{
public:
	void Start();
	static void* originalStart;
};
void* FakeMissionStage::originalStart = 0;

static int CheatMissionPtr = 0;
static unsigned int EnabledMissionCheats = 0;
static unsigned int DisabledMissionCheats = 0;

static void SetCheatEnabled(unsigned int uiCount, const char** prgpszArguments)
{
	auto mission = GetMissionScriptLoader()->Mission;
	if (mission == NULL)
	{
		// Fuck, error handling or something, they called it at the wrong time
		return;
	}
	int missionPtr = (int)mission;
	int cheat = 1 << atoi(prgpszArguments[1]);

	if (CheatMissionPtr != missionPtr)
	{
		CheatMissionPtr = missionPtr;
		EnabledMissionCheats = 0;
		DisabledMissionCheats = 0;
	}

	EnabledMissionCheats |= cheat;

	debug_printf(L"StageCheats: Set enabled cheats to %d for mission %p\n", EnabledMissionCheats, mission);
}

static void SetCheatDisabled(unsigned int uiCount, const char** prgpszArguments)
{
	auto mission = GetMissionScriptLoader()->Mission;
	if (mission == NULL)
	{
		// Fuck, error handling or something, they called it at the wrong time
		return;
	}
	int missionPtr = (int)mission;
	int cheat = 1 << atoi(prgpszArguments[1]);

	if (CheatMissionPtr != missionPtr)
	{
		CheatMissionPtr = missionPtr;
		EnabledMissionCheats = 0;
		DisabledMissionCheats = 0;
	}

	DisabledMissionCheats |= cheat;

	debug_printf(L"StageCheats: Set disabled cheats to %d for mission %p\n", DisabledMissionCheats, mission);
}

static std::unordered_map<int, int> EnabledStageCheats;
static std::unordered_map<int, int> DisabledStageCheats;

static void SetStageCheatEnabled(unsigned int uiCount, const char** prgpszArguments)
{
	auto missionStage = GetMissionScriptLoader()->Stage;
	if (missionStage == NULL)
	{
		// Fuck, error handling or something, they called it at the wrong time
		return;
	}
	int missionStagePtr = (int)missionStage;
	int cheat = 1 << atoi(prgpszArguments[1]);

	auto it = EnabledStageCheats.find(missionStagePtr);
	if (it != EnabledStageCheats.end())
		EnabledStageCheats[missionStagePtr] = it->second | cheat;
	else
		EnabledStageCheats[missionStagePtr] = cheat;

	debug_printf(L"StageCheats: Set enabled cheat to %d for stage %p\n", EnabledStageCheats[missionStagePtr], missionStage);
}

static void SetStageCheatDisabled(unsigned int uiCount, const char** prgpszArguments)
{
	auto missionStage = GetMissionScriptLoader()->Stage;
	if (missionStage == NULL)
	{
		// Fuck, error handling or something, they called it at the wrong time
		return;
	}
	int missionStagePtr = (int)missionStage;
	int cheat = 1 << atoi(prgpszArguments[1]);

	auto it = DisabledStageCheats.find(missionStagePtr);
	if (it != DisabledStageCheats.end())
		DisabledStageCheats[missionStagePtr] = it->second | cheat;
	else
		DisabledStageCheats[missionStagePtr] = cheat;

	debug_printf(L"StageCheats: Set disabled cheat to %d for stage %p\n", DisabledStageCheats[missionStagePtr], missionStage);
}

extern "C"  __declspec(dllexport)  unsigned int __cdecl HackEntryPoint(HackEvent, void*);

extern "C" unsigned int __cdecl HackEntryPoint(HackEvent event, void* data)
{
	switch (event)
	{
		case HackEvent::InstallHacks:
		{
			InitialiseHack();
			int sharVersion = IdentifySharVersion();
			debug_printf(L"StageCheats: Entry point reached %d %p\n", event, data);
			debug_printf(L"StageCheats: Shar Version %d\n", sharVersion);

			if (MH_Initialize() != MH_OK)
			{
				printf(L"StageCheats: Did not initialise MinHook\n");
				return 1;
			}

			void* missionInitialize = GetMissionInitialize_Ptr();
			FakeMission::originalInitialize = 0;

			auto newMissionInitialize = &FakeMission::Initialize;
			void** newMissionInitialize_ptr = (void**)&newMissionInitialize;
			if (MH_CreateHook(missionInitialize, *newMissionInitialize_ptr, &FakeMission::originalInitialize) != MH_OK)
			{
				printf(L"StageCheats: Did not create the Mission::Initialize Hook\n");
				return 1;
			}

			void* missionStageStart = GetMissionStageStart_Ptr();
			FakeMissionStage::originalStart = 0;

			auto newMissionStageStart = &FakeMissionStage::Start;
			void** newMissionStageStart_ptr = (void**)&newMissionStageStart;
			if (MH_CreateHook(missionStageStart, *newMissionStageStart_ptr, &FakeMissionStage::originalStart) != MH_OK)
			{
				printf(L"StageCheats: Did not create the MissionStage::Start Hook\n");
				return 1;
			}

			if (MH_EnableHook(missionInitialize) != MH_OK)
			{
				printf(L"WAD Support: Did not activate the Mission::Initialize Hook\n");
				return 1;
			}

			if (MH_EnableHook(missionStageStart) != MH_OK)
			{
				printf(L"WAD Support: Did not activate the MissionStage::Start Hook\n");
				return 1;
			}

			debug_printf(L"StageCheats: Hooked Mission::Initialize %p to %p\n", FakeMission::originalInitialize, *newMissionInitialize_ptr);
			debug_printf(L"StageCheats: Hooked MissionStage::Start %p to %p\n", FakeMissionStage::originalStart, *newMissionStageStart_ptr);

			break;
		}
		case HackEvent::AddScriptFunctions:
		{
			debug_printf(L"StageCheats: Adding Script Functions reached %d %p\n", event, data);

			void* scriptManager = GetScriptManager();
			if (scriptManager == NULL)
			{
				debug_printf(L"StageCheats: Could not find script manager\n");
				return 1;
			}

			if (!AddScriptCommand("SetCheatEnabled", "Enables a cheat for the current mission", 1, scriptManager, SetCheatEnabled, 1))
			{
				debug_printf(L"StageCheats: Failed to add SetCheatEnabled\n");
				return 1;
			}

			if (!AddScriptCommand("SetCheatDisabled", "Disables a cheat for the current mission", 1, scriptManager, SetCheatDisabled, 1))
			{
				debug_printf(L"StageCheats: Failed to add SetCheatDisabled\n");
				return 1;
			}

			if (!AddScriptCommand("SetStageCheatEnabled", "Enables a cheat for the current stage", 1, scriptManager, SetStageCheatEnabled, 1))
			{
				debug_printf(L"StageCheats: Failed to add SetStageCheatEnabled\n");
				return 1;
			}

			if (!AddScriptCommand("SetStageCheatDisabled", "Disables a cheat for the current stage", 1, scriptManager, SetStageCheatDisabled, 1))
			{
				debug_printf(L"StageCheats: Failed to add SetStageCheatDisabled\n");
				return 1;
			}

			printf(L"StageCheats: Commands added\n");

			break;
		}
		default:
		{
			/*if (Hack_Printf != 0)
				debug_printf(L"StageCheats: Unknown event: %d %p\n", event, data);*/

			break;
		}
	}
	return 0;
}

static bool RestoreMissionCheats = false;
static int RestoreMissionCheatsValue = 0;
void __thiscall FakeMission::Initialize(unsigned int heap)
{
	debug_printf(L"StageCheats: Mission started: %p %d\n", this, heap);

	if (RestoreMissionCheats)
	{
		SetCheats(RestoreMissionCheatsValue);
		printf(L"StageCheats: Restored cheats value to: %d\n", RestoreMissionCheatsValue);
		RestoreMissionCheats = false;
	}

	if ((int)this == CheatMissionPtr)
	{
		auto currentCheatValue = GetCheats();
		auto newCheatValue = currentCheatValue;

		newCheatValue |= EnabledMissionCheats;
		newCheatValue &= ~DisabledMissionCheats;

		if (currentCheatValue != newCheatValue)
		{
			RestoreMissionCheats = true;
			RestoreMissionCheatsValue = currentCheatValue;
			SetCheats(newCheatValue);
			printf(L"StageCheats: Set cheats value to: %d\n", newCheatValue);
		}
	}

	((void(__thiscall*)(void*, unsigned int))FakeMission::originalInitialize)(this, heap);
}

static bool RestoreStageCheats = false;
static int RestoreStageCheatsValue = 0;
void FakeMissionStage::Start()
{
	void* thisptr;
	_asm
	{
		mov thisptr, esi
	}
	debug_printf(L"StageCheats: Mission stage started: %p\n", thisptr);
	
	if (RestoreStageCheats)
	{
		SetCheats(RestoreStageCheatsValue);
		printf(L"StageCheats: Restored cheats value to: %d\n", RestoreStageCheatsValue);
		RestoreStageCheats = false;
	}

	auto currentCheatValue = GetCheats();
	auto newCheatValue = currentCheatValue;

	int thisptr_int = (int)thisptr;

	auto enabled_it = EnabledStageCheats.find(thisptr_int);
	if (enabled_it != EnabledStageCheats.end())
		newCheatValue |= enabled_it->second;

	auto disabled_it = DisabledStageCheats.find(thisptr_int);
	if (disabled_it != DisabledStageCheats.end())
		newCheatValue &= ~disabled_it->second;

	if (currentCheatValue != newCheatValue)
	{
		RestoreStageCheats = true;
		RestoreStageCheatsValue = currentCheatValue;
		SetCheats(newCheatValue);
		printf(L"StageCheats: Set cheats value to: %d\n", newCheatValue);
	}

	_asm
	{
		mov esi, thisptr
		call FakeMissionStage::originalStart
	}
}