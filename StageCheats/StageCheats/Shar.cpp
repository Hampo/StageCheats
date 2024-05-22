#include "pch.h"

#include "HackFunctions.h"
#include "Shar.h"

static int SharVersion = 0;

inline static void* Choose(unsigned int rel0, unsigned int rel1, unsigned int rel2, unsigned int rel3)
{
	void* releases[] = { (void*)rel0, (void*)rel1, (void*)rel2, (void*)rel3 };
	return releases[SharVersion];
}

int IdentifySharVersion()
{
	SharVersion = Hack_GameRelease();
	return SharVersion;
}

void* GetMissionStageStart_Ptr()
{
	return Choose(0x456AE0, 0x456BA0, 0x4567F0, 0x456650);
}

unsigned int GetCheats()
{
	return *(unsigned int*)Choose(0x6C8420, 0x6C83E0, 0x6C83E0, 0x6C8418);
}

void SetCheats(unsigned int enabledCheats)
{
	*(unsigned int*)Choose(0x6C8420, 0x6C83E0, 0x6C83E0, 0x6C8418) = enabledCheats;
}

void* GetScriptManager()
{
	return *(void**)Choose(0x6C9050, 0x6C9010, 0x6C9010, 0x6C9048);
}

bool AddScriptCommand(const char* pszName, const char* pszDescription, unsigned int uiMinimumArgumentCount, void* pScriptManager, SCRIPTCOMMANDPROC pCallback, unsigned int uiMaximumArgumentCount)
{
	void* pFunc = Choose(0x42E2B0, 0x42E570, 0x42E240, 0x42DEA0);
	unsigned int uiResult;
	_asm
	{
		push uiMaximumArgumentCount
		push pCallback
		push pScriptManager
		mov ecx, uiMinimumArgumentCount
		mov edx, pszDescription
		mov edi, pszName
		call pFunc
		mov uiResult, eax
	}
	return uiResult != 0;
}

MissionScriptLoader* GetMissionScriptLoader()
{
	return *(MissionScriptLoader**)Choose(0x6C8990, 0x6C8950, 0x6C8950, 0x6C8988);
}

