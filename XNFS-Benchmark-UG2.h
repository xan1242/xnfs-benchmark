#pragma once
#include "UG2\DialogInterfaceHook.h"
#include "NFSU2_Benchmark.h"
#include "XNFS-Benchmark.h"

bool SkipFEAutoRaceTest = false;
bool bWindowedMode = false;
bool bShowedTheDialogBox = false;
bool bInitedBenchTimeout = false;
bool bWentToFE = false;

char BenchMessageBuf[1024];

unsigned int BenchDialogNum;

void* Player_pPlayersByNumber;
void* ThePlayerCareer;

#define SHOWRESULTSHASH 0x53485253
#define GOBACKTOLASTRESHASH 0x4742434B
#define EXITPOSTRACEHASH 0x406415E3

void(__thiscall* TrackLoader_FinishedLoadingPart1)(void* TrackLoader) = (void(__thiscall*)(void*))TRACKLOADER_FINISHEDLOADING_ADDRESS;
void(__thiscall* TrackStreamer_RefreshLoading)(void* TrackStreamer) = (void(__thiscall*)(void*))TRACKSTREAMER_REFRESHLOADING_ADDRESS;
void(__thiscall* Player_AutoPilotOn)(void* Player) = (void(__thiscall*)(void*))PLAYER_AUTOPILOTON_ADDRESS;
void(__thiscall* RaceCoordinator_TheRaceHasFinished)(void* RaceCoordinator) = (void(__thiscall*)(void*))RACECOORDINATOR_THERACEHASFINISHED_ADDRESS;
void(__thiscall* RaceCoordinator_RCPostMessage)(void* RaceCoordinator, int RCMessage, int unk) = (void(__thiscall*)(void*, int, int))0x005F41E0;
void(__thiscall* PostRaceResultsScreen_NotificationMessage)(void* PostRaceResultsScreen, unsigned int unk1, void* FEObject, unsigned int unk2, unsigned int unk3) = (void(__thiscall*)(void*, unsigned int, void*, unsigned int, unsigned int))POSTRACERESULTS_NOTIFICATION_ADDRESS;
void(__thiscall* PlayerCareerState_NotifyExitToMainMenu)(void* PlayerCareerState) = (void(__thiscall*)(void*))0x005139C0;
void(__thiscall* PostRaceMenu_QuitGame)(void* PostRaceMenu) = (void(__thiscall*)(void*))0x004A3E80;
void(__cdecl* EndGameFlowLoadingFrontEnd)() = (void(__cdecl*)())0x00580090;
void(__cdecl* FEngStartInteraction)(const char* pkg_name, int unk) = (void(__cdecl*)(const char*, int))0x555E80;
void(__thiscall* cFrontendDatabase_Default)(void* cFrontendDatabase) = (void(__thiscall*)(void*))0x005346F0;

//feDialogConfig BenchMessageConfig = { "dialog foxy boxy",  OKLANGHASH, EXITPOSTRACEHASH, 0, 0, 0, 0, 0, 0, "", "", 1 };
feDialogConfig BenchMessageConfig = { "dialog foxy boxy",  OKLANGHASH, EXITPOSTRACEHASH, 0, 0, 0, 0, 0, 0, "", "UI_InGameDialog.fng", 1 };
//feDialogConfig FinalBenchMessageConfig = { "dialog foxy boxy", DLGTITLE_INFO, EXITLANGHASH, SHOWRESULTSHASH, 0, 0, 0, 0, 0, 0, NULL, 0, 2 };
feDialogConfig StatsBenchMessageConfig = { "dialog foxy boxy", OKLANGHASH, GOBACKTOLASTRESHASH, 0, 0, 0, 0, 0, 0, "", "", 1 };

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
			//fild dword ptr[esp + 4]
			//push ecx
			//fmul ds : TIMESTEPFLOAT1_ADDRESS
			fmul ds : TIMESTEPFLOAT2_ADDRESS
			add esp, 4
			fstp dword ptr[esp]
			jmp TimeStepCaveExit
		}
	}
}

void DetectFinishLoading()
{
	// Pre-Carbon needs different timestep handling
	SkipFEAutoRaceTest = true;
	bInitedBenchTimeout = false;
	Player_pPlayersByNumber = *(void**)PLAYERSBYNUMBERPOINTER;
	if (Player_pPlayersByNumber)
		Player_AutoPilotOn(Player_pPlayersByNumber);
	else
		printf("BENCHMARK: AutoPilot failed!\n");

	HandleGameLoaded();
}

void EarlyNFSFinishedLoading_Hook()
{
	void* TheThis;
	_asm mov TheThis, ecx
	TrackStreamer_RefreshLoading(TheThis);
	DetectFinishLoading();
}

void EarlyNFSRaceFinished_Hook()
{
	void* TheThis;
	_asm mov TheThis, ecx
	
	RaceCoordinator_TheRaceHasFinished(TheThis);

	SkipFEAutoRaceTest = false;
	HandleRaceFinish();
}

void GotoFE()
{

	printf("Going to FE\n");

	struct PostRaceMenuStruct
	{
		unsigned int unk;
		const char* PostRaceMenuFNG;
	}PostRaceMenu = { 0, (const char*)0x0078F158 };

	PostRaceMenu_QuitGame(&PostRaceMenu);
	bWentToFE = true;
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
	*(bool*)SKIPMOVIES_ADDRESS = true;
	*(bool*)UNLOCKALLTHINGS_ADDRESS = true;
	bWentToFE = false;
	bShowedTheDialogBox = false;

	*(unsigned int*)SKIPFENUMAICARS_ADDRESS = BenchInfos[BenchmarkCounter].NumAICars;
	*(unsigned int*)SKIPFENUMLAPS_ADDRESS = BenchInfos[BenchmarkCounter].NumLaps;
	*(unsigned int*)SKIPFEDIFFICULTY_ADDRESS = BenchInfos[BenchmarkCounter].AIDifficulty;
	//
	//		//*(char**)SKIPFERACEID_ADDRESS = BenchInfos[BenchmarkCounter].RaceID;
	//		if (BenchInfos[BenchmarkCounter].RaceID[strlen(BenchInfos[BenchmarkCounter].RaceID) - 1] == '\n')
	//			BenchInfos[BenchmarkCounter].RaceID[strlen(BenchInfos[BenchmarkCounter].RaceID) - 1] = '\0';
	//
	sscanf(BenchInfos[BenchmarkCounter].RaceID, "%d", SKIPFETRACKNUMBER_ADDRESS);
	//		//strncpy((char*)SKIPFERACEID_ADDRESS, BenchInfos[BenchmarkCounter].RaceID, 11);
	//		//*(char**)SKIPFEPLAYERCAR_ADDRESS = BenchInfos[BenchmarkCounter].PlayerCar;
	sscanf(BenchInfos[BenchmarkCounter].PlayerCar, "%d", DEFAULTPLAYER1CARTYPE_ADDRESS);
	//
	*(unsigned int*)SKIPFETRAFFICDENSITY_ADDRESS = (unsigned int)BenchInfos[BenchmarkCounter].TrafficDensity;

	cFrontendDatabase_Default((void*)FEDATABASE_ADDRESS);

	RaceStarter_StartSkipFERace();
}

void DisplayResultsInDialogHook()
{
	//printf("feDialogConfig start: %X\nfeDialogConfig FNG: %X\n", &BenchMessageConfig, &BenchMessageConfig.DialogFNG);

	BenchMessageConfig.Button1LangHash = OKLANGHASH;
	if (BenchmarkCounter >= BenchmarkInfoCount)
	{
		BenchMessageConfig.NumberOfButtons = 2;
		BenchMessageConfig.Button1LangHash = QUITGAMELANGHASH;
		BenchMessageConfig.Button1Hash = EXITPOSTRACEHASH;
		BenchMessageConfig.Button2Hash = SHOWRESULTSHASH;
	}

	sprintf(BenchMessageBuf, "Benchmark %d / %d complete\n%i frames, %3.1f seconds: %3.1f fps\n", BenchmarkCounter, BenchmarkInfoCount, TimeDemoFrames, TimeDemoEnd / 1000.0, TimeDemoFrames*1000.0 / TimeDemoEnd);

	BenchMessageBuf[768] = 0;
	strcpy(BenchMessageConfig.MsgBuffer, BenchMessageBuf);

	//DialogInterface_ShowTwoButtons("", "UI_InGameDialog.fng", OKLANGHASH, 0x541389E5, 0x30ED2368, 0xB4623F67, 0xB4623F67, 1, 0xE15B558A);

	BenchDialogNum = DialogInterface_ShowDialog(&BenchMessageConfig);
}

void DisplayAllResultsInDialog()
{
	GenerateResultingTextString(ResultTextBuffer);
	if (strlen(ResultTextBuffer) > 768)
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
		FEPrintf(*(char**)CURRENTDIALOGPKG_ADDRESS, BUTTON2OBJECTHASH, "Next bench in: %d", (NextBenchTimer / 1000) + 1);
		//sprintf(BenchMessageBuf, "Next bench in: %d", (NextBenchTimer / 1000) + 1);
		//DialogInterface_SetButtonText(1, BenchMessageBuf, false);
	}

	if (NextBenchTimer <= 0)
	{
		//LoadNextRace();
		//PlayerCareerState_NotifyExitToMainMenu((void*)THEPLAYERCAREER_ADDRESS);
		if (!bWentToFE)
		{
			DialogInterface_DismissDialog(BenchDialogNum);
			GotoFE();
			bWentToFE = true;
		}
		//GotoFE2();
		//*Message = 0xB4623F67;
	}
}

void __stdcall PostRaceResultsScreen_NotificationMessage_Hook(unsigned int Message, void* FEObject, unsigned int unk2, unsigned int unk3)
{
	void* TheThis;
	_asm mov TheThis, ecx

	//printf("%X, %X, %X, %X\n", Message, FEObject, unk2, unk3);

	if ((BenchmarkCounter >= BenchmarkInfoCount))
	{
		/*if (bShowedTheDialogBox)
		{
			DialogInterface_SetButtonText(1, "Results", false);
		}*/
		if (Message == EXITPOSTRACEHASH)
			*(int*)EXITTHEGAMEFLAG_ADDRESS = 1;
	}
	else
	{
		if (Message == EXITPOSTRACEHASH)
		{
			DialogInterface_DismissDialog(BenchDialogNum);
			GotoFE();
		}
		DoTimeout(&Message);
	}
	
	if (Message == SHOWRESULTSHASH)
	{
		DisplayAllResultsInDialog();
	}

	PostRaceResultsScreen_NotificationMessage(TheThis, Message, FEObject, unk2, unk3);


	if (!bShowedTheDialogBox /*|| (Message == GOBACKTOLASTRESHASH)*/)
	{
		DisplayResultsInDialogHook();
		bShowedTheDialogBox = true;
	}
}

void EndGameFlowLoadingFrontEnd_Hook()
{
	EndGameFlowLoadingFrontEnd();
//	FEngStartInteraction((const char*)0x00799A08, 0);
	LoadNextRace();
}

unsigned int __stdcall CalculateFirstFrontendScreenToSeeAfterQuitRace(int eQuitRaceTypes)
{
	return 0x0078FCD8;
}

int EnforceValues(BenchmarkInfo FirstBenchInfo, BenchmarkInfo TheDefault)
{
	// constant settings for benching
	*(bool*)SKIPFE_ADDRESS = true;
	*(bool*)SKIPMOVIES_ADDRESS = true;
//	*(bool*)SKIPNISS_ADDRESS = true;
	*(bool*)UNLOCKALLTHINGS_ADDRESS = true;
	*(int*)WINDOWEDMODE_ADDRESS = bWindowedMode;
//	*(int*)ISMEMCARDENABLED_ADDRESS = 0;
//	*(int*)ISAUTOSAVEENABLED_ADDRESS = 0;
	SkipFEAutoRaceTest = true;

	*(char**)0x007F4570 = "UI_PostRaceResults.fng";

	*(int*)VSYNC_ADDRESS = 0;

	if (!bSoundEnabled)
	{
		*(bool*)ISSOUNDENABLED_ADDRESS = false;
	}

	// variable settings
	if (BenchmarkInfoCount)
	{
		*(unsigned int*)SKIPFENUMAICARS_ADDRESS = FirstBenchInfo.NumAICars;
		*(unsigned int*)SKIPFENUMLAPS_ADDRESS = FirstBenchInfo.NumLaps;
		*(unsigned int*)SKIPFEDIFFICULTY_ADDRESS = FirstBenchInfo.AIDifficulty;
//
//		//*(char**)SKIPFERACEID_ADDRESS = FirstBenchInfo.RaceID;
//		if (FirstBenchInfo.RaceID[strlen(FirstBenchInfo.RaceID) - 1] == '\n')
//			FirstBenchInfo.RaceID[strlen(FirstBenchInfo.RaceID) - 1] = '\0';
//
		sscanf(FirstBenchInfo.RaceID, "%d", SKIPFETRACKNUMBER_ADDRESS);
//		//strncpy((char*)SKIPFERACEID_ADDRESS, FirstBenchInfo.RaceID, 11);
//		//*(char**)SKIPFEPLAYERCAR_ADDRESS = FirstBenchInfo.PlayerCar;
		sscanf(FirstBenchInfo.PlayerCar, "%d", DEFAULTPLAYER1CARTYPE_ADDRESS);
//
		*(unsigned int*)SKIPFETRAFFICDENSITY_ADDRESS = (unsigned int)FirstBenchInfo.TrafficDensity;
	}
	else
	{
		*(unsigned int*)SKIPFENUMAICARS_ADDRESS = TheDefault.NumAICars;
		*(unsigned int*)SKIPFENUMLAPS_ADDRESS = TheDefault.NumLaps;
		*(unsigned int*)SKIPFEDIFFICULTY_ADDRESS = TheDefault.AIDifficulty;

		if (TheDefault.RaceID[strlen(TheDefault.RaceID) - 1] == '\n')
			TheDefault.RaceID[strlen(TheDefault.RaceID) - 1] = '\0';

		sscanf(TheDefault.RaceID, "%d", SKIPFETRACKNUMBER_ADDRESS);
		//strncpy((char*)SKIPFERACEID_ADDRESS, TheDefault.RaceID, 11);
		//*(char**)SKIPFEPLAYERCAR_ADDRESS = TheDefault.PlayerCar;

		*(float*)SKIPFETRAFFICDENSITY_ADDRESS = (unsigned int)TheDefault.TrafficDensity;
	}
	return 0;
}

bool HookValueEnforcer(char* procname, int somethingidk)
{
	printf("BENCHMARK: Game setup...\n");
	//*(bool*)SKIPFE_ADDRESS = true;
	EnforceValues(BenchInfos[0], DefaultBenchInfo);
	printf("BENCHMARK: Game setup complete.\n");
	printf("BENCHMARK: Please note that the results are NOT 100%% accurate and may differ between runs\n");

	return false;
}

bool Player_IsJumpCamAvailable_Hook()
{
	return false;
}

int InitInjector()
{
	injector::MakeCALL(MAINLOOPSTARTTICK_ADDRESS, InitTimeBase, true); // initing right before MainLoop
	injector::MakeCALL(MAINDISPLAYFRAMEEND, CountTimeDemoFrames, true);
	injector::MakeCALL(TRACKSTREAMER_REFRESHLOADING_HOOK_ADDRESS, EarlyNFSFinishedLoading_Hook, true);

	injector::MakeCALL(THERACEHASFINISHED_CALL1, EarlyNFSRaceFinished_Hook, true);
	injector::MakeCALL(THERACEHASFINISHED_CALL2, EarlyNFSRaceFinished_Hook, true);

//	injector::MakeCALL(0x004A3EB5, CalculateFirstFrontendScreenToSeeAfterQuitRace, true);

	injector::WriteMemory<unsigned int>(0x00797A94, (unsigned int)&PostRaceResultsScreen_NotificationMessage_Hook, true);

	injector::WriteMemory<unsigned int>(0x578371, (unsigned int)&EndGameFlowLoadingFrontEnd_Hook, true);
	//injector::MakeNOP(0x005663C4, 5, true);
	injector::MakeJMP(0x004D9E94, 0x4DA031, true);
	injector::MakeJMP(0x00566352, 0x56636D, true);

	// derandomization for benching
	injector::MakeJMP(BRANDOM_INT_ADDRESS, bRandom_Int, true);
	injector::MakeJMP(BRANDOM_FLOAT_ADDRESS, bRandom_Float, true);

	injector::MakeCALL(0x00558029, printf, true);
	injector::MakeCALL(0x00558038, printf, true);
	injector::MakeCALL(0x0055804F, printf, true);
	injector::MakeCALL(0x00558099, printf, true);
	injector::MakeCALL(0x005580A8, printf, true);
	injector::MakeCALL(0x005580C6, printf, true);
	injector::MakeCALL(0x00558133, printf, true);
	injector::MakeCALL(0x00558194, printf, true);
	injector::MakeCALL(0x005581F6, printf, true);
	injector::MakeCALL(0x0055820E, printf, true);
	injector::MakeCALL(0x0055827D, printf, true);
	injector::MakeCALL(0x00558287, printf, true);

//
//	// AI is kinda bad at driving
//	injector::MakeCALL(AI_ANTIBUST_HOOK, Game_IsOnlineGame, true);
//	// disable spike strips
//	injector::WriteMemory<void(*)(float)>(AI_ANTISPIKE_HOOK, &SpikeStrip_OnTaskSimulateHook, true);
//
//
//	// game code optimization
//	injector::MakeJMP(DISABLEPAUSEEVENT_ADDR1, DISABLEPAUSEEVENT_ADDR2, true);
	injector::MakeJMP(TIMESTEPCAVEENTRY_ADDRESS, InjectTimestepUnlock, true);

	injector::MakeJMP(DISABLEPAUSE_ADDR1, DISABLEPAUSE_ADDR2, true);

	injector::MakeCALL(0x00580E07, HookValueEnforcer, true);

	// disable jump event activity that breaks the game completely
	//injector::MakeJMP(0x00604B9E, 0x604D13, true);
	//injector::MakeJMP(0x00604AC2, 0x604D13, true);
	//injector::MakeJMP(0x604A46, 0x6051D5, true);
	injector::MakeCALL(0x00604BA6, Player_IsJumpCamAvailable_Hook, true);
	injector::MakeCALL(0x005A00E2, Player_IsJumpCamAvailable_Hook, true);

	//*(bool*)0x089CF69 = false;

	//*(unsigned int*)0x0081CB88 = 1;

	injector::MakeJMP(0x004FBE58, 0x4FBEEE, true);


	return 0;
}

int InitGameSpecificConfig(CIniReader inireader)
{
	bWindowedMode = inireader.ReadInteger("Bench", "bWindowedMode", 1);
	return 0;
}