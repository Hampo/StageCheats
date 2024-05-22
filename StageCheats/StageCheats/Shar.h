#pragma once
int IdentifySharVersion();

void* GetMissionStageStart_Ptr();

unsigned int GetCheats();
void SetCheats(unsigned int enabledCheats);

typedef void(_cdecl* SCRIPTCOMMANDPROC)(unsigned int uiCount, const char** prgpszArguments);
void* GetScriptManager();
bool AddScriptCommand(const char* pszName, const char* pszDescription, unsigned int uiMinimumArgumentCount, void* pScriptManager, SCRIPTCOMMANDPROC pCallback, unsigned int uiMaximumArgumentCount);

struct MissionScriptLoader
{
	unsigned char m_Offset[0x18];
	void* Stage;
};
MissionScriptLoader* GetMissionScriptLoader();