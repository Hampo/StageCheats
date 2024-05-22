#include "pch.h"
#include "HackFunctions.h"

static int* Hack_GameReleasePtr = 0;
static int* Hack_GameLanguagePtr = 0;
static void* Hack_InjectedDataPtr = 0;

typedef int (*Hack_IsHackLoaded_t)(wchar_t const* name, int unk);
static Hack_IsHackLoaded_t Hack_IsLoadedPtr = 0;

void InitialiseHack()
{
	HMODULE hacks = GetModuleHandle(L"Hacks.dll");
	Hack_Printf = (Hack_Printf_t)GetProcAddress(hacks, MAKEINTRESOURCEA(3631));
	Hack_GameReleasePtr = (int*)GetProcAddress(hacks, MAKEINTRESOURCEA(3153));
	Hack_GameLanguagePtr = (int*)GetProcAddress(hacks, MAKEINTRESOURCEA(3152));
	Hack_IsLoadedPtr = (Hack_IsHackLoaded_t)GetProcAddress(hacks, MAKEINTRESOURCEA(2508));
	Hack_InjectedDataPtr = (void*)GetProcAddress(hacks, MAKEINTRESOURCEA(3154));
}

// Bare function pointer because variable forwarding nonsense
Hack_Printf_t Hack_Printf = nullptr;

// Get the game release
int Hack_GameRelease()
{
	return *Hack_GameReleasePtr;
}

int Hack_GameLanguage()
{
	return *Hack_GameLanguagePtr;
}

// Is a hack loaded
bool Hack_IsHackLoaded(wchar_t const* name)
{
	return Hack_IsLoadedPtr(name, 1) != 0;
}

void* Hack_InjectedData()
{
	return Hack_InjectedDataPtr;
}