#pragma once
#include "Typedef.h"

extern HWND			g_hWnd;
extern HINSTANCE	g_hInst;

#ifdef SERVER
extern SOCKET	g_hSocket;
extern int		g_iSNum;
extern bool		g_bIsActive;
extern char		g_cJob;
#endif // SERVER
extern _tchar g_Text[256];

extern _tchar	g_szPlayerName[32];
extern _tchar	g_szID[32];
extern _tchar	g_szPWD[32];
extern _bool	g_bIsInputID;
extern _bool	g_bIsInputPWD;
extern _bool	g_bIsLoginID;

extern _bool g_bIsOnDebugCaemra;
extern _bool g_bIsStageChange;
extern _bool g_bIsOpenShop;
extern _bool g_bIsChattingInput;
extern _bool g_bIsStartSkillCameraEffect;
extern _bool g_bIsCinemaStart;

// Loading
extern _bool g_bIsLoadingFinish;

// GameStart
extern _bool g_bIsGameStart;

// GameExit
extern _bool g_bIsExitGame;