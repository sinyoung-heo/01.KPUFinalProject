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


// Camera
extern _bool g_bIsOnDebugCaemra;

// Scene Change
extern _bool g_bIsStageChange;
extern _bool g_bIsOpenShop;

// Loading
extern _bool g_bIsLoadingFinish;