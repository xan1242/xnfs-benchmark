#pragma once
#include "FeDialogScreen_Hook.h"
#define DEFAULT_NUM_AI 7
#define DEFAULT_NUM_LAPS 2
#define DEFAULT_RACE_ID "ct.2.1"
#define DEFAULT_RACE_ID_2 "2.war"
#define DEFAULT_RACE_ID_3 "cs.6.1"
#define DEFAULT_TRAFFIC_DENSITY 100.0
#define DEFAULT_TRAFFIC_ONCOMING 100.0
#define DEFAULT_AI_DIF 2
#define DEFAULT_PLAYER_CAR "darius"
#define DEFAULT_DISABLE_WINGMAN true
#define DEFAULT_DISABLE_COPS true
#define DEFAULT_COP_CHEAT true
#define DEFAULT_BENCH_TIMEOUT 5
#define DEFAULT_REVERSE_TRACK false



#define SKIPFE_ADDRESS 0x00A9E620
#define SKIPFENUMAICARS_ADDRESS 0x00A9E630
#define SKIPFENUMLAPS_ADDRESS 0x00A63154
#define SKIPFERACEID_ADDRESS 0x00A63140
#define SKIPFETRACKDIRECTION_ADDRESS 0x00A9E634

#define SKIPFETRAFFICDENSITY_ADDRESS 0x00A9E640
#define SKIPFETRAFFICONCOMING_ADDRESS 0x00A63160
#define SKIPFEDISABLETRAFFIC_ADDRESS 0x00A9E644
#define SKIPFEDISABLECOPS_ADDRESS 0x00A63158

#define SKIPFEDIFFICULTY_ADDRESS 0x00A63164
#define SKIPFEPLAYERCAR_ADDRESS 0x00A63144
#define SKIPFENOWINGMAN_ADDRESS 0x00A9E655
#define SKIPFEAICONTROLPLAYERCAR_ADDRESS 0x00A9E656
#define SKIPFEAUTORACETEST_ADDRESS 0x00A9E657

#define SKIPMOVIES_ADDRESS 0x00926144
#define SKIPNISS_ADDRESS 0x00A9E64E
#define UNLOCKALLTHINGS_ADDRESS 0x00A9E6C0

#define THEDALMANAGERADDRESS 0x00A8AD30
#define THEGAMEFLOWMANAGER_ADDRESS 0x00A99BBC
#define THEGAMEFLOWMANAGEROBJ_ADDRESS 0x00A99B9C
#define EXITTHEGAMEFLAG_ADDRESS 0x00A99560

#define FEPOSTRACERESULTS_SINSTANCE 0x00A97E00
#define FEPOSTRACE_TIMEOBJHASH 0x678A0000
#define FEPOSTRACE_TITLEOBJHASH 0x4D8173F3
#define FEPOSTRACE_RESULTOBJHASH 0x10AC1AD5
#define FEPOSTRACE_SOMEOBJHASH1 0x0A030CE36
#define FEPOSTRACE_SOMEOBJHASH2 0x7D7FE731
#define FEPOSTRACE_SOMEOBJHASH3 0x0FA9C840C
#define FEPOSTRACE_BAROBJHASH 0x84247121


#define GETTICKERADDRESS 0x0046CEC0
#define INITIALIZERGETTICKER 0x06B786E
#define MAINDISPLAYFRAMEEND 0x0069927F
#define REBUILDLIGHTMATSADDR 0x007473F0
#define DETECTFINISHLOADINGADDR 0x006B6ADF
#define FINISHEDGAMELOADINGADDR 0x006B6A10
#define FINISHEDGAMELOADHOOKADDR 0x6B6B36
#define ENTERPOSTRACEFLOWADDR 0x0065EF00
#define ENTERPOSTRACEFLOWCALL 0x00661255
#define ENTERPOSTRACEFLOWPUSH 0x00669EB6
#define FEPOSTRACESTATEMANAGERSTART 0x005B33D0
#define FEPOSTRACESTATEMANAGERSTARTHOOK 0x009D2CC4

#define FEPOSTRACESTATEMANAGERCONSTRUCTED 0x0057AEB0
#define FEPOSTRACESTATEMANAGERCONSTRUCTEDHOOK 0x009D2DA4

#define DALOPTIONSJUMPCAM_ADDRESS 0x004A4F30

#define AI_ANTIBUST_HOOK 0x0044A3C1
#define AI_ANTISPIKE_HOOK 0x009E5CA4

#define FEPOSTRACESTATEMANAGER_HANDLEPADACCEPT 0x009D2D20
#define EXITPOSTRACEADDRESS 0x005A6370

#define FEMAINSTATEMANAGER_ONSCREENTICK_ADDRESS 0x009F7E78
#define FEPOSTSTATEMANAGER_HANDLESCREENTICK_ADDRESS 0x009D2DAC

#define GRACESTATUSFOBJ 0x00A98284
#define GRACESTATUSSETROAMING 0x00641310

#define GAMEFLOWMANAGER_RELOADTRACK 0x006BC5C0

#define DISABLECOPS_ADDRESS 0x00A83A50

#define GAME_RANDOMSEED 0xA4D3D0
#define SEEDRANDOMNUMBER_CALL 0x006B76EB
#define BRANDOM_INT_ADDRESS 0x0046DB30
#define BRANDOM_FLOAT_ADDRESS 0x0046DB70

#define EADDSMS_SKIP_ADDRESS1 0x0066B771
#define EADDSMS_SKIP_ADDRESS2 0x0066B779

#define FEPOSTRACERESULTS_SETUP_ADDR1 0x005E21AF
#define FEPOSTRACERESULTS_SETUP_ADDR2 0x005E237A

#define DISABLEPAUSE_ADDR1 0x00711F08
#define DISABLEPAUSE_ADDR2 0x711F12
#define DISABLEPAUSEEVENT_ADDR1 0x00670A43
#define DISABLEPAUSEEVENT_ADDR2 0x670A65

#define BSETRANDOMSEED_ADDRESS 0x004736A0
#define FEOBJECT_SETVISIBILITY_ADDRESS 0x00570460
#define GAMEFORCEAI_ADDRESS 0x0065C2C0
#define GAMESTARTRACEFROMINGAME_ADDRESS 0x0064C3B0
#define RACESTARTERSKIPFE_ADDRESS 0x007A9B90
#define RACESTARTER_ADDRESS 0x007CA1F0
#define SPIKESTRIP_SIM_ADDRESS 0x006F4440
#define FESTATEMANAGER_ONPADBUTTON_ADDRESS 0x00579540

// audio stuff
#define ISSOUNDENABLED_ADDRESS 0x00A631B8
#define ISAUDIOSTREAMINGENABLED_ADDRESS 0x00A631BC
#define ISSPEECHENABLED_ADDRESS 0x00A631C0
#define ISNISAUDIOENABLED_ADDRESS 0x00A631C4

#define TIMERDIVIDER_ADDRESS 0x6B4CCE

#define GETBUTTONRETURNEDHASH_ADDRESS 0x005A1BD3
#define FEPOSTRACESTATEMANAGER_SHOWDIALOG_ADDRESS 0x005CF780
#define FEPOSTRACESTATEMANAGER_SHOWDIALOG_VT_ADDRESS 0x009D2DB4
#define FESTATEMANAGER_SHOWDIALOG_ADDRESS 0x00579C10