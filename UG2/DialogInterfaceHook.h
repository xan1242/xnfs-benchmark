// The NFS Most Wanted (and Underground 2?) DialogInterface hook
// by Xanvier / xan1242

#include "stdio.h"
#include "DialogInterfaceAddresses.h"

//#define MESSAGEOBJECTHASH 0x1E2640FA
#define BUTTON1OBJECTHASH 0xDA743D2F
#define BUTTON2OBJECTHASH 0xDA743D30
#define BUTTON3OBJECTHASH 0xDA473D31


// Unnecessary but left for reference
/*#define ANIMATING_FNG "OL_Dialog.fng"
#define STACKED_FNG "OL_Dialog_Stacked_Buttons.fng"
#define NORMAL_FNG "Dialog.fng"
#define INGAME_FNG "InGameDialog.fng"*/

#define DLG_ANIMATING "animating"
#define DLG_3BUTTON "3button"
#define DLG_STACKED "3buttons_stacked"

#define DLGTITLE_INFO 0
#define DLGTITLE_ATTN 1
#define DLGTITLE_SEL 3

#define CONTINUELANGHASH 0x8098A54C
#define OKLANGHASH 0x639
#define QUITGAMELANGHASH 0x1C101778
//#define EXITLANGHASH 0xA9950B93
#define YESLANGHASH 0x88579775
#define NOLANGHASH 0xBE503981

#define CANCELACTION 0x42CA53FC


struct feDialogConfig
{
	char MsgBuffer[768]; // The message buffer
	unsigned int Button1LangHash; // Button 1 language hash
	unsigned int Button1Hash; // Button 1 message hash
	unsigned int Button2LangHash; // Button 2 language hash
	unsigned int Button2Hash; // Button 2 message hash
	unsigned int Button3LangHash; // Button 3 language hash
	unsigned int Button3Hash; // Button 3 message hash
	unsigned int DefaultMsgHash; // Undefined action message hash
	int eDialogFirstButtons; // First selected button
	char* MasterFNG; // If defined, dialog box can send messages to this FNG and simulate its actions
	const char* Type; // Type of dialog box
	int NumberOfButtons; // Number of buttons in a dialog box
	int v27; // Unknown
	unsigned int unk1; // Unknown 
	const char* unk2;
	unsigned int unk3; // Unknown
};

struct FEColor
{
	unsigned int Blue;
	unsigned int Green;
	unsigned int Red;
	unsigned int Alpha;
};

#define SECRETDIALOGINFO_ADDRESS 0x00838974
#define FNGTHING1_ADDRESS 0x00838964
unsigned int FEngFindScreen = 0x0051BD10;
unsigned int FEngFindString = 0x00537A10;
unsigned int FEString_SetString = 0x00535FE0;
unsigned int FEWideString__as = 0x00532670;

bool __declspec(naked) DialogInterface_SetButtonText(int ButtonNumber, const char* Text, bool IsWideString)
{
	_asm
	{
	mov     eax, ds:SECRETDIALOGINFO_ADDRESS
	sub     esp, 0xC
	push    ebx
	xor     bl, bl
	test    eax, eax
	jz      loc_5990F3
	mov     eax, ds:FNGTHING1_ADDRESS
	push    eax
	call    FEngFindScreen
	test    eax, eax
	jz      loc_5990F3
	mov     eax, [eax+0x1C]
	test    eax, eax
	jz      loc_5990F3
	mov     ecx, [esp+0x14]
	mov     eax, [eax+0x10]
	push    esi
	mov     dword ptr [esp+8], BUTTON1OBJECTHASH
	mov     dword ptr [esp+0xC], BUTTON2OBJECTHASH
	mov     dword ptr [esp+0x10], BUTTON3OBJECTHASH
	mov     edx, [esp+ecx*4+8]
	push    edx
	push    eax
	call    FEngFindString
	mov     esi, eax
	add     esp, 8
	test    esi, esi
	jz      loc_5990EB
	mov     al, [esp+0x20]
	test    al, al
	jz      loc_5990CF
	mov     eax, [esp+0x1C]
	push    eax
	mov     ecx, esi
	call    FEString_SetString
	pop     esi
	mov     al, 1
	pop     ebx
	add     esp, 0xC
	retn


loc_5990CF:
	mov     ecx, [esp+0x1C]
	push    ecx
	lea     ecx, [esi+0x64]
	call    FEWideString__as
	or      dword ptr [esi+0x1C], 0x400000
	pop     esi
	mov     al, 1
	pop     ebx
	add     esp, 0xC
	retn


loc_5990EB:
	pop     esi
	mov     al, bl
	pop     ebx
	add     esp, 0xC
	retn


loc_5990F3:
	mov     al, bl
	pop     ebx
	add     esp, 0xC
	retn
	}
}

int(__cdecl *DialogInterface_ShowDialog)(feDialogConfig* DialogConfig) = (int(__cdecl*)(feDialogConfig*))SHOWDIALOG_ADDRESS; // return value is its DialogNum (dialog number)

int(__cdecl *DialogInterface_ShowTwoButtons)(const char* ParentPackage, const char* DialogPkg, unsigned int Button1LangHash, unsigned int Button1Hash, unsigned int Button2LangHash, unsigned int Button2Hash, unsigned int DefaultMsgHash, int eDialogFirstButtons, unsigned int MessageLangHash, ...) =
(int(__cdecl*)(const char*, const char*, unsigned int, unsigned int, unsigned int, unsigned int, unsigned int, int, unsigned int, ...))0x00558790;

int(__cdecl *DialogInterface_DismissDialog)(int DialogNum) = (int(__cdecl*)(int))DISMISSDIALOG_ADDRESS;
//bool(__cdecl *DialogInterface_SetButtonText)(int ButtonNumber, const char* Text, bool IsWideString) = (bool(__cdecl*)(int, const char*, bool))SETBUTTONTEXT_ADDRESS;
int(__cdecl *FEPrintf)(const char* pkg_name, unsigned int obj_hash, const char* format, ...) = (int(__cdecl*)(const char*, unsigned int, const char*, ...))FEPRINTF_ADDRESS;
/*int(__cdecl *FEPrintf_Obj)(const char* pkg_name, void *FEObject, const char* format, ...) = (int(__cdecl*)(const char*, void*, const char*, ...))FEPRINTF2_ADDRESS;

void*(__cdecl *FEObject_FindObject)(const char *pkg_name, unsigned int obj_hash) = (void*(__cdecl*)(const char*, unsigned int))FENGFINDOBJECT_ADDRESS;
void(__thiscall *FEObject_SetColor)(void* FEObject, FEColor &color, bool bRelative) = (void(__thiscall*)(void*, FEColor&, bool))FEOBJECT_SETCOLOR_ADDRESS;
*/

int __cdecl CurrentDialog_Dismiss()
{
	return DialogInterface_DismissDialog(*(int*)CURRENTDIALOGNUM_ADDRESS);
}

/*int DialogInterface_ShowDialog_Custom(feDialogConfig* DialogConfig, const char* Button1Text, const char* Button2Text, const char* Button3Text)
{
	int Result;
	feDialogConfig ButtonChecker = *DialogConfig;

	Result = DialogInterface_ShowDialog(DialogConfig);

	Sleep(20); // hack to satisfy race conditions

	switch (ButtonChecker.NumberOfButtons)
	{
	case 3:
		if (Button3Text)
			DialogInterface_SetButtonText(2, Button3Text, 0);
	case 2:
		if (Button2Text)
			DialogInterface_SetButtonText(1, Button2Text, 0);
	case 1:
		if (Button1Text)
			DialogInterface_SetButtonText(0, Button1Text, 0);
	default:
		break;
	}

	return Result;
}*/

int DialogInterface_ShowMessage(const char* Type, const char* format, ...)
{
	va_list Args;
	feDialogConfig TheDialogConfig = { 0 };

	TheDialogConfig.Type = Type;

	va_start(Args, format);
	vsprintf(TheDialogConfig.MsgBuffer, format, Args);
	va_end(Args);

	return DialogInterface_ShowDialog(&TheDialogConfig);
}

int DialogInterface_ShowNButtons(unsigned int NumberOfButtons, const char* Type, int eDialogFirstButtons, unsigned int Button1Hash, unsigned int Button2Hash, unsigned int Button3Hash, unsigned int Button1LangHash, unsigned int Button2LangHash, unsigned int Button3LangHash, const char* format, ...)
{
	va_list Args;
	feDialogConfig TheDialogConfig = { 0 };

	TheDialogConfig.Type = Type;
	TheDialogConfig.eDialogFirstButtons = eDialogFirstButtons;

	if (NumberOfButtons > 3)
		NumberOfButtons = 3;

	TheDialogConfig.NumberOfButtons = NumberOfButtons;

	switch (NumberOfButtons)
	{
	case 3:
		TheDialogConfig.Button3Hash = Button3Hash;
		TheDialogConfig.Button3LangHash = Button3LangHash;
	case 2:
		TheDialogConfig.Button2Hash = Button2Hash;
		TheDialogConfig.Button2LangHash = Button2LangHash;
	case 1:
		TheDialogConfig.Button1Hash = Button1Hash;
		TheDialogConfig.Button1LangHash = Button1LangHash;
	default:
		break;
	}

	va_start(Args, format);
	vsprintf(TheDialogConfig.MsgBuffer, format, Args);
	va_end(Args);

	return DialogInterface_ShowDialog(&TheDialogConfig);
}

/*int DialogInterface_ShowNButtons_Custom(unsigned int NumberOfButtons, const char* Type, int eDialogFirstButtons, unsigned int Button1Hash, unsigned int Button2Hash, unsigned int Button3Hash, const char* Button1Text, const char* Button2Text, const char* Button3Text, const char* format, ...)
{
	va_list Args;
	feDialogConfig TheDialogConfig = { 0 };

	TheDialogConfig.Type = Type;
	TheDialogConfig.eDialogFirstButtons = eDialogFirstButtons;

	if (NumberOfButtons > 3)
		NumberOfButtons = 3;

	TheDialogConfig.NumberOfButtons = NumberOfButtons;

	switch (NumberOfButtons)
	{
	case 3:
		TheDialogConfig.Button3Hash = Button3Hash;
	case 2:
		TheDialogConfig.Button2Hash = Button2Hash;
	case 1:
		TheDialogConfig.Button1Hash = Button1Hash;
	default:
		break;
	}

	va_start(Args, format);
	vsprintf(TheDialogConfig.MsgBuffer, format, Args);
	va_end(Args);

	return DialogInterface_ShowDialog_Custom(&TheDialogConfig, Button1Text, Button2Text, Button3Text);
}*/

#pragma warning(disable:4100)
int __cdecl CurrentDialog_FEPrintf(const char* Reserved, unsigned int obj_hash, const char* format, ...)
{
	_asm
	{
		pop ebp
		add esp, 8
		mov eax, ds:CURRENTDIALOGPKG_ADDRESS
		push eax
		sub esp, 4
		jmp FEPrintf
	}
}
#pragma warning(default:4100)