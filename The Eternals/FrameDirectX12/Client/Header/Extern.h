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

extern _bool g_bIsOnDebugCaemra;
extern _bool g_bIsStageChange;
extern _bool g_bIsOpenShop;
extern _bool g_bIsChattingInput;

// Loading
extern _bool g_bIsLoadingFinish;

// GameExit
extern _bool g_bIsExitGame;