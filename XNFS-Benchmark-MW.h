// MW-specific code
#pragma once
#include "NFSMW_Benchmark.h"
#include "XNFS-Benchmark.h"
#include "MW\DialogInterfaceHook.h"
#ifndef GAME_MW
#define GAME_MW
#endif

#define SHOWRESULTSHASH 0x53485253
#define GOBACKTOLASTRESHASH 0x4742434B
#define EXITPOSTRACEHASH 0xE1FDE1D1

void(__cdecl* FE_ShowPostRaceScreen)(bool raceWon) = (void(__cdecl*)(bool))SHOWPOSTRACESCREEN_ADDRESS;
void(__cdecl* EndGameFlowLoadingFrontEnd)() = (void(__cdecl*)())0x0064B6D0;
//void(__cdecl* RaceStarter_StartSkipFERace)() = (void(__cdecl*)())0x0057ECD0;
void(__thiscall* PostRaceResultsScreen_NotificationMessage)(void* PostRaceResultsScreen, unsigned int unk1, void* FEObject, unsigned int unk2, unsigned int unk3) = (void(__thiscall*)(void*, unsigned int, void*, unsigned int, unsigned int))POSTRACERESULTS_NOTIFICATION_ADDRESS;

bool SkipFEAutoRaceTest = false;
bool bWindowedMode = true;
bool bShowedTheDialogBox = false;
bool bInitedBenchTimeout = false;
bool bRaceWonByAI = false;

char BenchMessageBuf[1024];

// an feDialogConfig MUST be used because FE locks up if you generate one on the fly!
feDialogConfig BenchMessageConfig = { "dialog foxy boxy", DLGTITLE_INFO, OKLANGHASH, EXITPOSTRACEHASH, 0, 0, 0, 0, 0, 0, NULL, 0, 1 };
//feDialogConfig FinalBenchMessageConfig = { "dialog foxy boxy", DLGTITLE_INFO, EXITLANGHASH, SHOWRESULTSHASH, 0, 0, 0, 0, 0, 0, NULL, 0, 2 };
feDialogConfig StatsBenchMessageConfig = { "dialog foxy boxy", DLGTITLE_INFO, OKLANGHASH, GOBACKTOLASTRESHASH, 0, 0, 0, 0, 0, 0, NULL, 0, 1 };

void DisplayResultsInDialogHook()
{

	if (BenchmarkCounter >= BenchmarkInfoCount)
	{
		BenchMessageConfig.NumberOfButtons = 2;
		BenchMessageConfig.Button1LangHash = EXITLANGHASH;
		BenchMessageConfig.Button1Hash = EXITPOSTRACEHASH;
		BenchMessageConfig.Button2Hash = SHOWRESULTSHASH;
	}

	sprintf(BenchMessageBuf ,"Benchmark %d / %d complete\n%i frames, %3.1f seconds: %3.1f fps\n", BenchmarkCounter, BenchmarkInfoCount, TimeDemoFrames, TimeDemoEnd / 1000.0, TimeDemoFrames*1000.0 / TimeDemoEnd);
	
	if (bRaceWonByAI)
		strcpy(BenchMessageBuf + strlen(BenchMessageBuf), "& give the AI a high five for winning the race! :)");

	BenchMessageBuf[512] = 0;
	strcpy(BenchMessageConfig.MsgBuffer, BenchMessageBuf);

	DialogInterface_ShowDialog(&BenchMessageConfig);
}

void DisplayAllResultsInDialog()
{
	GenerateResultingTextString(ResultTextBuffer);
	if (strlen(ResultTextBuffer) > 512)
	{
		strcpy(StatsBenchMessageConfig.MsgBuffer, "Too long for this dialog box to show!^Please check the console output.");
		puts(ResultTextBuffer);
	}
	else
	{
		strcpy(StatsBenchMessageConfig.MsgBuffer, ResultTextBuffer);
	}
	DialogInterface_ShowDialog(&StatsBenchMessageConfig);
}

void DoTimeout(unsigned int *Message)
{
	if (!bInitedBenchTimeout)
	{
		NextBenchStart = Sys_Milliseconds();
		NextBenchEnd = NextBenchStart + NextBenchTimeout;
		NextBenchTimer = NextBenchTimeout;
		bInitedBenchTimeout = true;
	}

	if ((BenchmarkCounter < BenchmarkInfoCount) && !bNextBenchTimeoutStop)
	{
		NextBenchTimer = NextBenchEnd - Sys_Milliseconds();
		sprintf(BenchMessageBuf, "Next bench in: %d", (NextBenchTimer / 1000) + 1);
		DialogInterface_SetButtonText(1, BenchMessageBuf, false);
	}

	if (NextBenchTimer <= 0)
		*Message = EXITPOSTRACEHASH;
}

void DetectFinishLoading()
{
	// Pre-Carbon needs different timestep handling
	SkipFEAutoRaceTest = true;
	bShowedTheDialogBox = false;
	bInitedBenchTimeout = false;
	bRaceWonByAI = false;
	Game_ForceAIControl(0);
	HandleGameLoaded();
}

void DetectRaceFinish()
{
	HandleRaceFinish();
	Game_EnterPostRaceFlow();
}

unsigned int TimeStepCaveExit = TIMESTEPCAVEEXIT_ADDRESS;
void __declspec(naked) InjectTimestepUnlock()
{
	if (SkipFEAutoRaceTest)
	{
		_asm
		{
			//push 0x3D888889
			push 0x3f800000
			jmp TimeStepCaveExit
		}
	}
	else
	{
		_asm
		{
			fild dword ptr [esp+4]
			push ecx
			fmul ds:TIMESTEPFLOAT1_ADDRESS
			fmul ds:TIMESTEPFLOAT2_ADDRESS
			fstp dword ptr [esp]
			jmp TimeStepCaveExit
		}
	}
}

void FE_ShowPostRaceScreen_Hook(bool raceWon)
{
	bRaceWonByAI = raceWon;
	SkipFEAutoRaceTest = false;
	HandleRaceFinish();
	FE_ShowPostRaceScreen(false);
}

void __stdcall PostRaceResultsScreen_NotificationMessage_Hook(unsigned int Message, void* FEObject, unsigned int unk2, unsigned int unk3)
{
	void* TheThis;
	_asm mov TheThis, ecx

	if ((BenchmarkCounter >= BenchmarkInfoCount))
	{
		if (bShowedTheDialogBox)
		{
			DialogInterface_SetButtonText(1, "Results", false);
		}

		if (Message == EXITPOSTRACEHASH)
			*(int*)EXITTHEGAMEFLAG_ADDRESS = 1;
	}
	else
		DoTimeout(&Message);

	if (Message == SHOWRESULTSHASH)
	{
		DisplayAllResultsInDialog();
	}

	PostRaceResultsScreen_NotificationMessage(TheThis, Message, FEObject, unk2, unk3);
	//printf("%X, %X, %X, %X\n", Message, FEObject, unk2, unk3);


	if (!bShowedTheDialogBox || (Message == GOBACKTOLASTRESHASH))
	{
		DisplayResultsInDialogHook();
		bShowedTheDialogBox = true;
	}
}

void LoadNextRace()
{
	if (!BenchmarkInfoCount)
	{
		*(int*)EXITTHEGAMEFLAG_ADDRESS = 1;
		return;
	}

	*(unsigned int*)SKIPFE_ADDRESS = 1;
	SkipFEAutoRaceTest = true;
	*(bool*)SKIPFEDISABLETRAFFIC_ADDRESS = false;
	*(bool*)SKIPMOVIES_ADDRESS = true;
	*(bool*)SKIPNISS_ADDRESS = true;
	*(bool*)UNLOCKALLTHINGS_ADDRESS = true;

	*(unsigned int*)SKIPFENUMAICARS_ADDRESS = BenchInfos[BenchmarkCounter].NumAICars;
	*(unsigned int*)SKIPFENUMLAPS_ADDRESS = BenchInfos[BenchmarkCounter].NumLaps;
	*(unsigned int*)SKIPFEDIFFICULTY_ADDRESS = BenchInfos[BenchmarkCounter].AIDifficulty;

	//*(char**)SKIPFERACEID_ADDRESS = BenchInfos[BenchmarkCounter].RaceID;
	if (BenchInfos[BenchmarkCounter].RaceID[strlen(BenchInfos[BenchmarkCounter].RaceID) - 1] == '\n')
		BenchInfos[BenchmarkCounter].RaceID[strlen(BenchInfos[BenchmarkCounter].RaceID) - 1] = '\0';
	strncpy((char*)SKIPFERACEID_ADDRESS, BenchInfos[BenchmarkCounter].RaceID, 11);
	*(char**)SKIPFEPLAYERCAR_ADDRESS = BenchInfos[BenchmarkCounter].PlayerCar;

	*(float*)SKIPFETRAFFICDENSITY_ADDRESS = BenchInfos[BenchmarkCounter].TrafficDensity;
	*(float*)SKIPFETRAFFICONCOMING_ADDRESS = BenchInfos[BenchmarkCounter].TrafficOncoming;

//	*(bool*)SKIPFENOWINGMAN_ADDRESS = BenchInfos[BenchmarkCounter].bDisableWingman;
	*(bool*)SKIPFEDISABLECOPS_ADDRESS = BenchInfos[BenchmarkCounter].bDisableCops;
	*(int*)DISABLECOPS_ADDRESS = BenchInfos[BenchmarkCounter].bDisableCops;

	RaceStarter_StartSkipFERace();
}

void EndGameFlowLoadingFrontEnd_Hook()
{
	EndGameFlowLoadingFrontEnd();
	LoadNextRace();
}

int InitInjector()
{
	injector::MakeCALL(INITIALIZERGETTICKER, InitTimeBase, true); // initing right at the end of InitializeEverything
	injector::MakeJMP(MAINDISPLAYFRAMEEND, CountTimeDemoFrames, true);
	injector::WriteMemory<unsigned int>(FINISHEDGAMELOADHOOKADDR, (unsigned int)&FinishedGameLoading_Hook, true);
	injector::WriteMemory<unsigned int>(ENTERPOSTRACEFLOWPUSH, (unsigned int)&DetectRaceFinish, true);
	injector::MakeCALL(POSTRACEWINCALL_ADDRESS, FE_ShowPostRaceScreen_Hook, true);
	injector::MakeCALL(POSTRACEWINCALL2_ADDRESS, FE_ShowPostRaceScreen_Hook, true);
	injector::MakeCALL(POSTRACELOSECALL_ADDRESS, FE_ShowPostRaceScreen_Hook, true);


	injector::WriteMemory<unsigned int>(0x008A170C, (unsigned int)&PostRaceResultsScreen_NotificationMessage_Hook, true);
	injector::WriteMemory<unsigned int>(0x659731, (unsigned int)&EndGameFlowLoadingFrontEnd_Hook, true);

	// derandomization for benching
	injector::MakeJMP(BRANDOM_INT_ADDRESS, bRandom_Int, true);
	injector::MakeJMP(BRANDOM_FLOAT_ADDRESS, bRandom_Float, true);

	// AI is kinda bad at driving
	injector::MakeCALL(AI_ANTIBUST_HOOK, Game_IsOnlineGame, true);
	// disable spike strips
	injector::WriteMemory<void(*)(float)>(AI_ANTISPIKE_HOOK, &SpikeStrip_OnTaskSimulateHook, true);


	// game code optimization
	injector::MakeJMP(DISABLEPAUSEEVENT_ADDR1, DISABLEPAUSEEVENT_ADDR2, true);
	injector::MakeJMP(TIMESTEPCAVEENTRY_ADDRESS, InjectTimestepUnlock, true);

	//injector::MakeJMP(0x57DC03, 0x0057DC0D, true);

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
	*(int*)WINDOWEDMODE_ADDRESS = bWindowedMode;
	*(int*)ISMEMCARDENABLED_ADDRESS = 0;
	*(int*)ISAUTOSAVEENABLED_ADDRESS = 0;
	SkipFEAutoRaceTest = true;

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

		//*(char**)SKIPFERACEID_ADDRESS = FirstBenchInfo.RaceID;
		if (FirstBenchInfo.RaceID[strlen(FirstBenchInfo.RaceID) - 1] == '\n')
			FirstBenchInfo.RaceID[strlen(FirstBenchInfo.RaceID) - 1] = '\0';
		strncpy((char*)SKIPFERACEID_ADDRESS, FirstBenchInfo.RaceID, 11);
		*(char**)SKIPFEPLAYERCAR_ADDRESS = FirstBenchInfo.PlayerCar;

		*(float*)SKIPFETRAFFICDENSITY_ADDRESS = FirstBenchInfo.TrafficDensity;
		*(float*)SKIPFETRAFFICONCOMING_ADDRESS = FirstBenchInfo.TrafficOncoming;

		*(bool*)SKIPFEDISABLECOPS_ADDRESS = FirstBenchInfo.bDisableCops;
		*(int*)DISABLECOPS_ADDRESS = FirstBenchInfo.bDisableCops;
	}
	else
	{
		*(unsigned int*)SKIPFENUMAICARS_ADDRESS = TheDefault.NumAICars;
		*(unsigned int*)SKIPFENUMLAPS_ADDRESS = TheDefault.NumLaps;
		*(unsigned int*)SKIPFEDIFFICULTY_ADDRESS = TheDefault.AIDifficulty;

		if (TheDefault.RaceID[strlen(TheDefault.RaceID) - 1] == '\n')
			TheDefault.RaceID[strlen(TheDefault.RaceID) - 1] = '\0';
		strncpy((char*)SKIPFERACEID_ADDRESS, TheDefault.RaceID, 11);
		*(char**)SKIPFEPLAYERCAR_ADDRESS = TheDefault.PlayerCar;

		*(float*)SKIPFETRAFFICDENSITY_ADDRESS = TheDefault.TrafficDensity;
		*(float*)SKIPFETRAFFICONCOMING_ADDRESS = TheDefault.TrafficOncoming;

		*(bool*)SKIPFEDISABLECOPS_ADDRESS = TheDefault.bDisableCops;
		*(int*)DISABLECOPS_ADDRESS = TheDefault.bDisableCops;
	}
	return 0;
}

int InitGameSpecificConfig(CIniReader inireader)
{
	bWindowedMode = inireader.ReadInteger("Bench", "bWindowedMode", 1);
	return 0;
}