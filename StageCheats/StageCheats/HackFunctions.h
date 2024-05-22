#pragma once

// Functions from Hacks.dll

enum class HackEvent
{
	Load,
	InstallHacks,
	Unload,
	WindowSet,
	AddScriptFunctions,
	WindowProc,
	SetGameState,
	Process,
	HandleFileNotFound,
	D3DPreResetDevice,
	D3DPostResetDevice,
	D3DLostDevice,
	D3DDeleteDevice,
	D3DSetRenderState,
	D3DSetTexture,
	D3DPredrawPrim,
	D3DPostDrawPrim,
	D3DPreDrawIndexedPrim,
	D3DPostDrawIndexPrim,
	D3DPrePresent,
	D3DPostPresent,
	D3DSetTextureStage,
	D3DSetViewport,
	D3DBeginScene,
	D3DPreCreateDevice,
	D3DPostCreateDevice,
	D3DSetTransform,
	PrePreRenderLayer,
	PreRenderLayer,
	PostRenderLayer,
	PostPostRenderLayer,
	Direct3DCreate8,
	LookupString,
	PostRenderLayers,
	Start,
	IPC,
	ParseAndExecuteMFKOrCon,
	AddScriptCommand,
	Exit,
	RegisterToReceiveEvent,
	GetCarHusk,
	PostRenderCoins,
	PostRenderCharsAndCars,
	RequestAsyncFileLoad,
	GetResolution,
	PreTriggerEvent,
	TriggerEvent,
	LuaScriptSupport1,
	LuaScriptSupport2,
	GameEnd,
	StageStart,
	StageEnd,
	StageChange,
	GameStart,
	SetupPauseSundayScreen,
	Cheat,
	SetupScrapbookContents,
	DebugTextDebugMode,
	DebugTextAddDebugMode,
	ChangeResolution,
	UpdateMovieSite,
	FrameLimiterImpose,
	CustomSaveDataAdded,
	LoadPurchaseMerchant,
	CanCollisionObjectsCollide,
	SetupGlobalPedStuff,
	MissionStageInit,
	MissionStageUninit,
	CarInit,
	CarUninit,
	GetCarIndex,
	AspectRatioSupport1,
	AspectRatioSupport2,
	AspectRatioSupport3,
	PreGuiScreenPhoneBooth,
	PostGuiScreenPhoneBooth,
	Crash,
	AddHighlights,
	CustomFilesCreateFile,
	CustomFilesLoadRSD,
};

// Init all the function pointers
void InitialiseHack();

// Bare function pointer because variable forwarding nonsense
typedef void (*Hack_Printf_t)(int type, int category, const wchar_t* msg, ...);
extern Hack_Printf_t Hack_Printf;

// Get the game release
int Hack_GameRelease();
int Hack_GameLanguage();

// Is a hack loaded
bool Hack_IsHackLoaded(wchar_t const* name);

// TODO: I need to find the user data path somewhere (or I want to)
void* Hack_InjectedData();