#include "pch.h"

#include "HackFunctions.h"
#include "Shar.h"

static int SharVersion = 0;

inline static void* Choose(unsigned int originalEnglish, unsigned int international, unsigned int bestSellerSeries, unsigned int demo)
{
	void* releases[] = { (void*)originalEnglish, (void*)international, (void*)bestSellerSeries, (void*)demo };
	return releases[SharVersion];
}

int IdentifySharVersion()
{
	SharVersion = Hack_GameRelease();
	return SharVersion;
}

void* GetMissionStageStart_Ptr()
{
	return Choose(0x456AE0, 0x4567F0, 0x456650, 0x456BA0);
}

void* GetMissionInitialize_Ptr()
{
	return Choose(0x44BE20, 0x44BB00, 0x44B900, 0x44BE80);
}

unsigned int GetCheats()
{
	return *(unsigned int*)Choose(0x6C8420, 0x6C83E0, 0x6C8418, 0x6C83E0);
}

void SetCheats(unsigned int enabledCheats)
{
	*(unsigned int*)Choose(0x6C8420, 0x6C83E0, 0x6C8418, 0x6C83E0) = enabledCheats;
}

void* GetScriptManager()
{
	return *(void**)Choose(0x6C9050, 0x6C9010, 0x6C9048, 0x6C9010);
}

bool AddScriptCommand(const char* pszName, const char* pszDescription, unsigned int uiMinimumArgumentCount, void* pScriptManager, SCRIPTCOMMANDPROC pCallback, unsigned int uiMaximumArgumentCount)
{
	void* pFunc = Choose(0x42E2B0, 0x42E240, 0x42DEA0, 0x42E570);
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
	return *(MissionScriptLoader**)Choose(0x6C8990, 0x6C8950, 0x6C8988, 0x6C8950);
}

