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

class FakeMissionStage
{
public:
	void Start();
	static void* originalStart;
};

void* FakeMissionStage::originalStart = 0;

static std::unordered_map<int, int> EnabledStageCheats;
static std::unordered_map<int, int> DisabledStageCheats;
static bool RestoreCheats = false;
static int RestoreCheatValue = 0;

static void SetStageCheatEnabled(unsigned int uiCount, const char** prgpszArguments)
{
	auto missionStage = GetMissionScriptLoader()->Stage;
	if (missionStage == NULL)
	{
		// Fuck, error handling or something, they called it at the wrong time
		return;
	}
	int missionStagePtr = (int)missionStage;
	int cheat = atoi(prgpszArguments[1]);

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
	int cheat = atoi(prgpszArguments[1]);

	auto it = DisabledStageCheats.find(missionStagePtr);
	if (it != DisabledStageCheats.end())
		DisabledStageCheats[missionStagePtr] = it->second | cheat;
	else
		DisabledStageCheats[missionStagePtr] = cheat;

	debug_printf(L"StageCheats: Set disabled cheat to %d for stage %p\n", EnabledStageCheats[missionStagePtr], missionStage);
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

			void* missionStageStart = GetMissionStageStart_Ptr();
			FakeMissionStage::originalStart = 0;

			if (MH_Initialize() != MH_OK)
			{
				printf(L"StageCheats: Did not initialise MinHook\n");
				return 1;
			}

			auto newStart = &FakeMissionStage::Start;
			void** newStart_ptr = (void**)&newStart;
			if (MH_CreateHook(missionStageStart, *newStart_ptr, &FakeMissionStage::originalStart) != MH_OK)
			{
				printf(L"StageCheats: Did not create the MissionStage::Start Hook\n");
				return 1;
			}

			if (MH_EnableHook(missionStageStart) != MH_OK)
			{
				printf(L"WAD Support: Did not activate the MissionStage::Start Hook\n");
				return 1;
			}

			debug_printf(L"StageCheats: Hooked %p to %p\n", FakeMissionStage::originalStart, *newStart_ptr);

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
		case HackEvent::StageStart:
		{
			debug_printf(L"StageCheats: Stage started %d %p\n", event, data);

			break;
		}
		case HackEvent::StageEnd:
		{
			debug_printf(L"StageCheats: Stage ended %d %p\n", event, data);

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

void FakeMissionStage::Start()
{
	void* thisptr;
	_asm
	{
		mov thisptr, esi
	}
	debug_printf(L"StageCheats: Mission started: %p\n", thisptr);
	
	if (RestoreCheats)
	{
		SetCheats(RestoreCheatValue);
		printf(L"StageCheats: Restored cheats value to: %d\n", RestoreCheatValue);
		RestoreCheats = false;
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
		RestoreCheats = true;
		RestoreCheatValue = currentCheatValue;
		SetCheats(newCheatValue);
		printf(L"StageCheats: Set cheats value to: %d\n", newCheatValue);
	}

	_asm
	{
		mov esi, thisptr
		call FakeMissionStage::originalStart
	}
}