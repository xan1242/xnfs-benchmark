#pragma once
#include <Windows.h>
#include <timeapi.h>
#include "includes\injector\injector.hpp"
#include "includes\IniReader.h"
#pragma comment(lib, "Winmm.lib")

struct BenchmarkInfo
{
	char* RaceID;
	char* PlayerCar;
	unsigned int NumAICars;
	unsigned int AIDifficulty;
	unsigned int NumLaps;
	float TrafficDensity;
	float TrafficOncoming;
	bool bDisableWingman;
	bool bDisableCops;
	bool bCopCheat;
	unsigned int TimeoutMS;
	unsigned int TimeDemoFrames; // record data per bench
	unsigned int TimeDemoEnd;
};

unsigned int BenchmarkInfoCount = 0;

char* ResultTextBuffer;

bool bTimeBaseInited = false;
bool bTimeDemoStarted = false;
bool bSoundEnabled = true;

unsigned int TimeBase;

unsigned int TimeDemoFrames;
unsigned int TimeDemoStart;
unsigned int TimeDemoEnd;

unsigned int BenchmarkCounter;
unsigned int FakeRandomCounter = 0;

int NextBenchTimer = 0;
unsigned int NextBenchStart = 0;
unsigned int NextBenchEnd = 0;
unsigned int NextBenchTimeout = (DEFAULT_BENCH_TIMEOUT) * 1000;
bool bNextBenchTimeoutStop = false;

//char ResultBuf[1024];

unsigned int RandomCounterInt = 0;
unsigned int RandomCounterFloat = 0;

BenchmarkInfo DefaultBenchInfo = { DEFAULT_RACE_ID, DEFAULT_PLAYER_CAR, DEFAULT_NUM_AI, DEFAULT_AI_DIF, DEFAULT_NUM_LAPS, DEFAULT_TRAFFIC_DENSITY, DEFAULT_TRAFFIC_ONCOMING, DEFAULT_DISABLE_WINGMAN, DEFAULT_DISABLE_COPS, DEFAULT_COP_CHEAT, 0 };
BenchmarkInfo* BenchInfos;

unsigned int(__cdecl* bGetTicker)() = (unsigned int(__cdecl*)())GETTICKERADDRESS;
void(__cdecl* RaceStarter_StartSkipFERace)() = (void(__cdecl*)())RACESTARTERSKIPFE_ADDRESS;

#ifndef OLD_NFS
void(__thiscall* SpikeStrip_OnTaskSimulate)(void* SpikeStrip, float something) = (void(__thiscall*)(void*, float))SPIKESTRIP_SIM_ADDRESS;
void(__cdecl* Game_ForceAIControl)(int playernum) = (void(__cdecl*)(int))GAMEFORCEAI_ADDRESS;
void(__cdecl* Game_EnterPostRaceFlow)() = (void(__cdecl*)())ENTERPOSTRACEFLOWADDR;
void(*FinishedGameLoading)() = (void(*)())FINISHEDGAMELOADINGADDR;
#endif

unsigned int InitTimeBase();
int Sys_Milliseconds(void); // stealing code from Quake3 HA HA!
void CountTimeDemoFrames();
void DisplayResultsInDialogHook();
void DetectFinishLoading();
void FinishedGameLoading_Hook();
void HandleGameLoaded();
void HandleRaceFinish();
void DetectRaceFinish();
void SpikeStrip_OnTaskSimulateHook(float something);
unsigned int bRandom_Int(int range);
float bRandom_Float(float range);
unsigned int Game_IsOnlineGame();
int InitInjector();
int InitGameSpecificConfig(CIniReader inireader);
int EnforceValues();
int GenerateResultingTextString(char* buffer);