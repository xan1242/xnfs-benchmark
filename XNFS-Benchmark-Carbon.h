// Carbon-specific code
#pragma once
#include "NFSC_Benchmark.h"
#include "XNFS-Benchmark.h"
#ifndef GAME_CARBON
#define GAME_CARBON
#endif

void *DALOptionsPointer;
unsigned int *gTheDALManager;

void(__thiscall* GameFlowManager_ReloadTrack)(void* GameFlowManager) = (void(__thiscall*)(void*))GAMEFLOWMANAGER_RELOADTRACK;
void(__cdecl* RaceStarter_StartRace)(const char* eventName, const int entryPoint) = (void(__cdecl*)(const char*, const int))RACESTARTER_ADDRESS;
void(__cdecl* Game_StartRaceFromInGame)(const char* eventID) = (void(__cdecl*)(const char*))GAMESTARTRACEFROMINGAME_ADDRESS;
void(__cdecl* eRebuildLightMaterials)(bool ingame) = (void(__cdecl*)(bool))REBUILDLIGHTMATSADDR;
void(__thiscall* FEPostRaceStateManager_Start)(void* FEPostRaceStateManager) = (void(__thiscall*)(void*))FEPOSTRACESTATEMANAGERSTART;
void(__thiscall* FEPostRaceStateManager_ExitPostRace)(void* FEPostRaceStateManager, int exitReason) = (void(__thiscall*)(void*, int))EXITPOSTRACEADDRESS;
void(__thiscall* FEPostRaceStateManager_HandleScreenConstructed)(void* FEPostRaceStateManager) = (void(__thiscall*)(void*))FEPOSTRACESTATEMANAGERCONSTRUCTED;
void(__thiscall* FEPostRaceStateManager_HandleShowDialog)(void* FEPostRaceStateManager) = (void(__thiscall*)(void*))FEPOSTRACESTATEMANAGER_SHOWDIALOG_ADDRESS;
void(__thiscall* FEStateManager_OnPadButton)(void* FEStateManager) = (void(__thiscall*)(void*))FESTATEMANAGER_ONPADBUTTON_ADDRESS;
void(__thiscall* FEStateManager_ShowDialog)(void* FEStateManager, int nextState) = (void(__thiscall*)(void*, int))FESTATEMANAGER_SHOWDIALOG_ADDRESS;
bool(__thiscall* DALOptions_SetJumpCamOn)(void* DALOptions, int val) = (bool(__thiscall*)(void*, int))DALOPTIONSJUMPCAM_ADDRESS;
void(__cdecl* FE_Object_SetVisibility)(void *FEObject, bool visible) = (void(__cdecl*)(void*, bool))FEOBJECT_SETVISIBILITY_ADDRESS;
void(__thiscall* GRaceStatus_SetRoaming)(void* GRaceStatus) = (void(__thiscall*)(void*))GRACESTATUSSETROAMING;
void(*bSetRandomSeed)(unsigned int value, unsigned int *seed) = (void(*)(unsigned int, unsigned int*))BSETRANDOMSEED_ADDRESS;

int CurrentPostRace_FEPrintf(unsigned int obj_hash, const char* fmt, ...)
{
	va_list ArgList;
	int Result = 0;
	int* FEDialogScreenInstance = *(int**)FEPOSTRACERESULTS_SINSTANCE;

	if (FEDialogScreenInstance)
	{
		__crt_va_start(ArgList, fmt);
		Result = vsprintf(FEPrintf_Buffer, fmt, ArgList);
		__crt_va_end(ArgList);

		mbstowcs(FEPrintf_Wide_Buffer, FEPrintf_Buffer, 1024);

		FE_String_SetString(FEPackage_FindObjectByHash(cFEng_FindPackage((void*)CFENG_MINSTANCE, (char*)(FEDialogScreenInstance[4])), obj_hash), FEPrintf_Wide_Buffer);
	}
	return Result;
}

int CurrentPostRace_SetVisibility(unsigned int obj_hash, bool visible)
{
	int* FEDialogScreenInstance = *(int**)FEPOSTRACERESULTS_SINSTANCE;

	if (FEDialogScreenInstance)
	{
		FE_Object_SetVisibility(FEPackage_FindObjectByHash(cFEng_FindPackage((void*)CFENG_MINSTANCE, (char*)(FEDialogScreenInstance[4])), obj_hash), visible);
	}

	return 0;
}

void FEPostRaceStateManager_HandleShowDialogHook()
{
	void* TheThis;
	_asm mov TheThis, ecx


	switch (((int*)TheThis)[1])
	{
	case 6:
		FEDialogScreen_ShowDialog("Do you want to playtest this part of the benchmark?", "No", "Yes", NULL);
		break;
	default:
		FEDialogScreen_ShowDialog(ResultTextBuffer, "Close", NULL, NULL);
		((int*)TheThis)[1] = 6; // hack to get the close button to work
	}
}

void FEPostRaceStateManager_HandleButtonX()
{
	void* TheThis;
	_asm mov TheThis, ecx
	//FEDialogScreen_ShowDialog("Benchmark info goes here...", "Close", NULL, NULL);
	FEStateManager_ShowDialog(TheThis, 0);
}


void DisplayResultsInDialogHook()
{
	void* TheThis;
	_asm mov TheThis, ecx
	FEPostRaceStateManager_HandleScreenConstructed(TheThis);

	NextBenchStart = Sys_Milliseconds();
	NextBenchEnd = NextBenchStart + NextBenchTimeout;
	NextBenchTimer = NextBenchTimeout;

	CurrentPostRace_SetVisibility(FEPOSTRACE_SOMEOBJHASH1, false);
	CurrentPostRace_SetVisibility(FEPOSTRACE_SOMEOBJHASH2, false);
	CurrentPostRace_SetVisibility(FEPOSTRACE_SOMEOBJHASH3, false);

	if (BenchmarkCounter >= BenchmarkInfoCount)
	{
		CurrentPostRace_SetVisibility(FEPOSTRACE_BAROBJHASH, false);
		CurrentPostRace_FEPrintf(FEPOSTRACE_TITLEOBJHASH, "ALL BENCHMARKS COMPLETE");
		CurrentPostRace_FEPrintf(FEPOSTRACE_RESULTOBJHASH, "%i frames, %3.1f seconds: %3.1f fps\n\nPlease check the results by pressing 2", TimeDemoFrames, TimeDemoEnd / 1000.0, TimeDemoFrames*1000.0 / TimeDemoEnd);		
		//FEStateManager_ShowDialog(TheThis, 2);
		GenerateResultingTextString(ResultTextBuffer);
	}
	else
	{
		CurrentPostRace_FEPrintf(FEPOSTRACE_TITLEOBJHASH, "BENCHMARK COMPLETE");
		CurrentPostRace_FEPrintf(FEPOSTRACE_RESULTOBJHASH, "%i frames, %3.1f seconds: %3.1f fps", TimeDemoFrames, TimeDemoEnd / 1000.0, TimeDemoFrames*1000.0 / TimeDemoEnd);
	}
	CurrentPostRace_FEPrintf(FEPOSTRACE_TIMEOBJHASH, "%d / %d complete", BenchmarkCounter, BenchmarkInfoCount);
	CurrentPostRace_FEPrintf(0x6611F08B, "Bench Results");

	*(bool*)SKIPFEAUTORACETEST_ADDRESS = false;
}
void DetectFinishLoading()
{
	// Pre-Carbon needs different timestep handling
	*(bool*)SKIPFEAUTORACETEST_ADDRESS = true;
	//Game_ForceAIControl(0);

	// disable jump cam
	DALOptions_SetJumpCamOn(DALOptionsPointer, 0);
	HandleGameLoaded();

	//	return eRebuildLightMaterials(ingame);
}

void DetectRaceFinish()
{
	HandleRaceFinish();
	Game_EnterPostRaceFlow();
}


void FEPostRaceStateManager_HandlePadAccept()
{
	void* TheThis;
	_asm mov TheThis, ecx

	bNextBenchTimeoutStop = false;

	if (!BenchmarkInfoCount || BenchmarkCounter >= BenchmarkInfoCount)
	{
		*(int*)EXITTHEGAMEFLAG_ADDRESS = 1;
		return;
	}

	FEPostRaceStateManager_ExitPostRace(TheThis, 5);
}

void FEPostRaceStateManager_OnPadButton()
{
	void* TheThis;
	_asm mov TheThis, ecx

	bNextBenchTimeoutStop = true; // doesn't work :/

	return FEStateManager_OnPadButton(TheThis);
}

void FEPostRaceStateManager_HandleScreenTick()
{
	void* TheThis;
	_asm mov TheThis, ecx

	if ((BenchmarkCounter < BenchmarkInfoCount) && !bNextBenchTimeoutStop)
	{
		NextBenchTimer = NextBenchEnd - Sys_Milliseconds();
		CurrentPostRace_FEPrintf(FEPOSTRACE_BAROBJHASH, "Next bench in: %d\n", (NextBenchTimer / 1000)+1);
		//CurrentPostRace_FEPrintf(0x918C3DDE, "Next bench in: 20\n");
		//CurrentPostRace_FEPrintf(0x62E6B564, "Playtest");
	//	CurrentPostRace_FEPrintf(0x3E831733, "Next bench in: %d\n", NextBenchTimer / 1000);

	}

	if (NextBenchTimer <= 0)
	{
		FEPostRaceStateManager_ExitPostRace(TheThis, 5);
	}
}

void FEMainStateManager_OnScreenTick()
{
	if (!BenchmarkInfoCount)
	{
		*(int*)EXITTHEGAMEFLAG_ADDRESS = 1;
		return;
	}

	*(unsigned int*)SKIPFE_ADDRESS = 1;
	*(bool*)SKIPFEAICONTROLPLAYERCAR_ADDRESS = true;
	*(bool*)SKIPFEAUTORACETEST_ADDRESS = true;
	*(bool*)SKIPFEDISABLETRAFFIC_ADDRESS = false;
	*(bool*)SKIPMOVIES_ADDRESS = true;
	*(bool*)SKIPNISS_ADDRESS = true;
	*(bool*)UNLOCKALLTHINGS_ADDRESS = true;

	*(unsigned int*)SKIPFENUMAICARS_ADDRESS = BenchInfos[BenchmarkCounter].NumAICars;
	*(unsigned int*)SKIPFENUMLAPS_ADDRESS = BenchInfos[BenchmarkCounter].NumLaps;
	*(unsigned int*)SKIPFEDIFFICULTY_ADDRESS = BenchInfos[BenchmarkCounter].AIDifficulty;

	*(char**)SKIPFERACEID_ADDRESS = BenchInfos[BenchmarkCounter].RaceID;
	*(char**)SKIPFEPLAYERCAR_ADDRESS = BenchInfos[BenchmarkCounter].PlayerCar;

	*(float*)SKIPFETRAFFICDENSITY_ADDRESS = BenchInfos[BenchmarkCounter].TrafficDensity;
	*(float*)SKIPFETRAFFICONCOMING_ADDRESS = BenchInfos[BenchmarkCounter].TrafficOncoming;

	*(bool*)SKIPFENOWINGMAN_ADDRESS = BenchInfos[BenchmarkCounter].bDisableWingman;
	*(bool*)SKIPFEDISABLECOPS_ADDRESS = BenchInfos[BenchmarkCounter].bDisableCops;
	*(int*)DISABLECOPS_ADDRESS = BenchInfos[BenchmarkCounter].bDisableCops;

	//RaceStarter_StartRace(BenchInfos[BenchmarkCounter].RaceID, 7);
	RaceStarter_StartSkipFERace();
}

int InitInjector()
{
	injector::MakeCALL(INITIALIZERGETTICKER, InitTimeBase, true); // initing right at the end of InitializeEverything
	injector::MakeJMP(MAINDISPLAYFRAMEEND, CountTimeDemoFrames, true);
	//injector::MakeCALL(DETECTFINISHLOADINGADDR, DetectFinishLoading, true);
	injector::WriteMemory<unsigned int>(FINISHEDGAMELOADHOOKADDR, (unsigned int)&FinishedGameLoading_Hook, true);
	injector::MakeCALL(ENTERPOSTRACEFLOWCALL, DetectRaceFinish, true);

	// derandomization for benching
	injector::MakeJMP(BRANDOM_INT_ADDRESS, bRandom_Int, true);
	injector::MakeJMP(BRANDOM_FLOAT_ADDRESS, bRandom_Float, true);

	// AI is kinda bad at driving
	injector::MakeCALL(AI_ANTIBUST_HOOK, Game_IsOnlineGame, true);
	// disable spike strips
	injector::WriteMemory<void(*)(float)>(AI_ANTISPIKE_HOOK, &SpikeStrip_OnTaskSimulateHook, true);

	injector::WriteMemory<unsigned int>(ENTERPOSTRACEFLOWPUSH, (unsigned int)&DetectRaceFinish, true);
	injector::WriteMemory<unsigned int>(FEPOSTRACESTATEMANAGERCONSTRUCTEDHOOK, (unsigned int)&DisplayResultsInDialogHook, true);
	injector::WriteMemory<unsigned int>(FEPOSTRACESTATEMANAGER_HANDLEPADACCEPT, (unsigned int)&FEPostRaceStateManager_HandlePadAccept, true);


	injector::WriteMemory<unsigned int>(FEMAINSTATEMANAGER_ONSCREENTICK_ADDRESS, (unsigned int)&FEMainStateManager_OnScreenTick, true);
	injector::WriteMemory<unsigned int>(FEPOSTSTATEMANAGER_HANDLESCREENTICK_ADDRESS, (unsigned int)&FEPostRaceStateManager_HandleScreenTick, true);

	injector::WriteMemory<unsigned int>(FEPOSTRACESTATEMANAGER_SHOWDIALOG_VT_ADDRESS, (unsigned int)&FEPostRaceStateManager_HandleShowDialogHook, true);
	injector::WriteMemory<unsigned int>(0x009D2D3C, (unsigned int)&FEPostRaceStateManager_HandleButtonX, true);

	// game code optimization
	injector::MakeJMP(FEPOSTRACERESULTS_SETUP_ADDR1, FEPOSTRACERESULTS_SETUP_ADDR2, true);
	injector::MakeJMP(DISABLEPAUSEEVENT_ADDR1, DISABLEPAUSEEVENT_ADDR2, true);

	// disable SMS
	injector::MakeJMP(EADDSMS_SKIP_ADDRESS1, EADDSMS_SKIP_ADDRESS2, true);

	// FULLY unlock the timer
	injector::WriteMemory<float>(TIMERDIVIDER_ADDRESS, 1.0, true);

	// force control of cops disabler
	//injector::MakeNOP(0x00611BCC, 10, true);
	//injector::MakeNOP(0x00406E8E, 5, true);
	//injector::MakeNOP(0x00647A17, 5, true);
	//injector::MakeNOP(0x0044B12B, 6, true);
	//injector::MakeNOP(0x0064171B, 6, true);

	return 0;
}

int EnforceValues(BenchmarkInfo FirstBenchInfo, BenchmarkInfo TheDefault)
{
	// constant settings for benching
	*(unsigned int*)SKIPFE_ADDRESS = 1;
	*(bool*)SKIPFEDISABLETRAFFIC_ADDRESS = false;
	*(bool*)SKIPMOVIES_ADDRESS = true;
	*(bool*)SKIPNISS_ADDRESS = true;
	*(bool*)UNLOCKALLTHINGS_ADDRESS = true;

#ifdef GAME_CARBON
	*(bool*)SKIPFEAICONTROLPLAYERCAR_ADDRESS = true;
	*(bool*)SKIPFEAUTORACETEST_ADDRESS = true;
#endif

	if (!bSoundEnabled)
	{
		*(bool*)ISSOUNDENABLED_ADDRESS = false;
		*(bool*)ISAUDIOSTREAMINGENABLED_ADDRESS = false;
		*(bool*)ISSPEECHENABLED_ADDRESS = false;
		*(bool*)ISNISAUDIOENABLED_ADDRESS = false;
	}

	// variable settings
	if (BenchmarkInfoCount)
	{
		*(unsigned int*)SKIPFENUMAICARS_ADDRESS = FirstBenchInfo.NumAICars;
		*(unsigned int*)SKIPFENUMLAPS_ADDRESS = FirstBenchInfo.NumLaps;
		*(unsigned int*)SKIPFEDIFFICULTY_ADDRESS = FirstBenchInfo.AIDifficulty;

		*(char**)SKIPFERACEID_ADDRESS = FirstBenchInfo.RaceID;
		*(char**)SKIPFEPLAYERCAR_ADDRESS = FirstBenchInfo.PlayerCar;

		*(float*)SKIPFETRAFFICDENSITY_ADDRESS = FirstBenchInfo.TrafficDensity;
		*(float*)SKIPFETRAFFICONCOMING_ADDRESS = FirstBenchInfo.TrafficOncoming;

		*(bool*)SKIPFEDISABLECOPS_ADDRESS = FirstBenchInfo.bDisableCops;
		*(int*)DISABLECOPS_ADDRESS = FirstBenchInfo.bDisableCops;
#ifdef GAME_CARBON
		*(bool*)SKIPFENOWINGMAN_ADDRESS = FirstBenchInfo.bDisableWingman;
#endif
	}
	else
	{
		*(unsigned int*)SKIPFENUMAICARS_ADDRESS = TheDefault.NumAICars;
		*(unsigned int*)SKIPFENUMLAPS_ADDRESS = TheDefault.NumLaps;
		*(unsigned int*)SKIPFEDIFFICULTY_ADDRESS = TheDefault.AIDifficulty;

		*(char**)SKIPFERACEID_ADDRESS = TheDefault.RaceID;
		*(char**)SKIPFEPLAYERCAR_ADDRESS = TheDefault.PlayerCar;

		*(float*)SKIPFETRAFFICDENSITY_ADDRESS = TheDefault.TrafficDensity;
		*(float*)SKIPFETRAFFICONCOMING_ADDRESS = TheDefault.TrafficOncoming;

		*(bool*)SKIPFEDISABLECOPS_ADDRESS = TheDefault.bDisableCops;
		*(int*)DISABLECOPS_ADDRESS = TheDefault.bDisableCops;
#ifdef GAME_CARBON
		*(bool*)SKIPFENOWINGMAN_ADDRESS = TheDefault.bDisableWingman;
#endif
	}
	return 0;
}

int InitGameSpecificConfig(CIniReader inireader)
{
	return 0;
}