// XNFS-Benchmark, main file
// contains main benchmarking logic

#include "stdafx.h"
#include "stdio.h"
#ifdef GAME_CARBON
#include "XNFS-Benchmark-Carbon.h"
#elif GAME_MW
#include "XNFS-Benchmark-MW.h"
#elif GAME_UG2
#include "XNFS-Benchmark-UG2.h"
#endif

int GenerateResultingTextString(char* buffer)
{
	char tempgen[67];
	unsigned int currentpos = 0;

	for (unsigned int i = 0; i < BenchmarkCounter; i++)
	{
		sprintf(tempgen, "Benchmark %d: %i frames, %3.1f seconds: %3.1f fps\n", i + 1, BenchInfos[i].TimeDemoFrames, BenchInfos[i].TimeDemoEnd / 1000.0, BenchInfos[i].TimeDemoFrames*1000.0 / BenchInfos[i].TimeDemoEnd);
		strcpy(buffer + currentpos, tempgen);
		currentpos += strlen(tempgen);
	}
	return 0;
}

unsigned int InitTimeBase()
{
	unsigned int ReturnTicks = bGetTicker();

	TimeBase = timeGetTime();
	bTimeBaseInited = true;

	return ReturnTicks;
}

int Sys_Milliseconds(void) // stealing code from Quake3 HA HA!
{
	int			sys_curtime;

	if (!bTimeBaseInited) {
		TimeBase = timeGetTime();
		bTimeBaseInited = true;
	}
	sys_curtime = timeGetTime() - TimeBase;

	return sys_curtime;
}

void CountTimeDemoFrames()
{
	if (bTimeDemoStarted)
	{
		TimeDemoFrames++;
	}
}

void HandleGameLoaded()
{
	bTimeDemoStarted = true;
	TimeDemoFrames = 0;
	TimeDemoEnd = 0;
	printf("BENCHMARK: Benchmark start!\n");
	TimeDemoStart = Sys_Milliseconds();
}

void HandleRaceFinish()
{

	if (bTimeDemoStarted)
	{

		TimeDemoEnd = Sys_Milliseconds() - TimeDemoStart;
		bTimeDemoStarted = false;
		BenchInfos[BenchmarkCounter].TimeDemoFrames = TimeDemoFrames; // record data per bench
		BenchInfos[BenchmarkCounter].TimeDemoEnd = TimeDemoEnd;
		BenchmarkCounter++;

		printf("BENCHMARK: Benchmark end!\n");
		printf("BENCHMARK: %i frames, %3.1f seconds: %3.1f fps\n", TimeDemoFrames, TimeDemoEnd / 1000.0, TimeDemoFrames*1000.0 / TimeDemoEnd);
	}
}

#ifndef OLD_NFS
void FinishedGameLoading_Hook()
{
	FinishedGameLoading();
	return DetectFinishLoading();
}


void SpikeStrip_OnTaskSimulateHook(float something)
{
	void* TheThis;
	_asm mov TheThis, ecx

	if (BenchInfos[BenchmarkCounter].bCopCheat)
		return;

	return SpikeStrip_OnTaskSimulate(TheThis, something);
}
#endif

unsigned int Game_IsOnlineGame()
{
	if (BenchInfos[BenchmarkCounter].bCopCheat)
		return 1;
	return 0;
}

unsigned int bRandom_Int(int range) // needs more control for derandomization of the game and less performance drop
{
//	return (rand() ^ 0x1D872B41) % range;
	return (RandomCounterInt++ ^ 0x1D872B41) % range;
}

float bRandom_Float(float range)
{
	return (((float)rand()) / (float)(RAND_MAX)) * range;
//	return (((float)(RandomCounterFloat++)) / (float)(RAND_MAX)) * range;
}

DWORD WINAPI StartupValueEnforcer(LPVOID)
{
	printf("BENCHMARK: Game setup...\n");

	while (!*(int*)THEGAMEFLOWMANAGER_ADDRESS) // monitor when the game starts
	{
		EnforceValues(BenchInfos[0], DefaultBenchInfo);
	}

	// disable game pausing in background // CAUSES A WINDOWS BSOD IF IN A WHILE LOOP!!!!
	injector::MakeJMP(DISABLEPAUSE_ADDR1, DISABLEPAUSE_ADDR2, true);
#ifdef GAME_CARBON
	gTheDALManager = *(unsigned int**)THEDALMANAGERADDRESS;
	DALOptionsPointer = (void*)gTheDALManager[20];
#endif


	printf("BENCHMARK: Game setup complete.\n");
	printf("BENCHMARK: Please note that the results are NOT 100%% accurate and may differ between runs\n");
	return 0;
}

int checkIfEnabled(CIniReader inireader)
{
	return inireader.ReadInteger("Bench", "bEnabled", true);
}

int InitConfig(CIniReader inireader)
{
	char inistrbuf1[1024];
	char inistrbuf2[32];
	//CIniReader inireader("");
	bSoundEnabled = inireader.ReadInteger("Bench", "bSoundEnabled", 1);
	BenchmarkInfoCount = inireader.ReadInteger("Bench", "BenchmarkInfoCount", 0);
	NextBenchTimeout = (inireader.ReadInteger("Bench", "NextBenchTimeout", DEFAULT_BENCH_TIMEOUT)) * 1000;
	BenchInfos = (BenchmarkInfo*)calloc(BenchmarkInfoCount, sizeof(BenchmarkInfo));
	ResultTextBuffer = (char*)calloc(BenchmarkInfoCount, sizeof(char)*67);
	strcpy(ResultTextBuffer, "Upon finishing all benchmarks results will go here");

	for (unsigned int i = 0; i < BenchmarkInfoCount; i++)
	{
		sprintf(inistrbuf2, "%d", i);

		strcpy(inistrbuf1, inireader.ReadString(inistrbuf2, "RaceID", DEFAULT_RACE_ID));
		BenchInfos[i].RaceID = (char*)malloc(strlen(inistrbuf1)+1);
		strcpy(BenchInfos[i].RaceID, inistrbuf1);

		strcpy(inistrbuf1, inireader.ReadString(inistrbuf2, "PlayerCar", DEFAULT_PLAYER_CAR));
		BenchInfos[i].PlayerCar = (char*)malloc(strlen(inistrbuf1) + 1);
		strcpy(BenchInfos[i].PlayerCar, inistrbuf1);

		BenchInfos[i].NumAICars = inireader.ReadInteger(inistrbuf2, "NumAICars", DEFAULT_NUM_AI);
		BenchInfos[i].AIDifficulty = inireader.ReadInteger(inistrbuf2, "AIDifficulty", DEFAULT_AI_DIF);
		BenchInfos[i].NumLaps = inireader.ReadInteger(inistrbuf2, "NumLaps", DEFAULT_NUM_LAPS);
		BenchInfos[i].TrafficDensity = inireader.ReadFloat(inistrbuf2, "TrafficDensity", DEFAULT_TRAFFIC_DENSITY);
		BenchInfos[i].TrafficOncoming = inireader.ReadFloat(inistrbuf2, "TrafficOncoming", DEFAULT_TRAFFIC_ONCOMING);
		BenchInfos[i].bDisableWingman = inireader.ReadBoolean(inistrbuf2, "bDisableWingman", DEFAULT_DISABLE_WINGMAN);
		BenchInfos[i].bDisableCops = inireader.ReadBoolean(inistrbuf2, "bDisableCops", DEFAULT_DISABLE_COPS);
		BenchInfos[i].bCopCheat = inireader.ReadBoolean(inistrbuf2, "bCopCheat", DEFAULT_COP_CHEAT);
		BenchInfos[i].TimeoutMS = inireader.ReadInteger(inistrbuf2, "TimeoutMS", 0);
	}

	InitGameSpecificConfig(inireader);

	return 0;
}

int Init()
{
	CIniReader inireader("");
	if (checkIfEnabled(inireader))
	{
		InitInjector();
		srand(1);
		InitConfig(inireader);
#ifndef OLD_NFS
		CreateThread(0, 0, (LPTHREAD_START_ROUTINE)&StartupValueEnforcer, NULL, 0, NULL);
#endif
	}
	return 0;
}

BOOL APIENTRY DllMain(HMODULE /*hModule*/, DWORD reason, LPVOID /*lpReserved*/)
{
	if (reason == DLL_PROCESS_ATTACH)
	{
		freopen("CON", "w", stdout);
		freopen("CON", "w", stderr);
		Init();
	}
	return TRUE;
}

