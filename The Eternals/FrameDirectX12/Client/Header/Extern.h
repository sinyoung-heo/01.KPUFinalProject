#pragma once
#include "Typedef.h"

extern HWND			g_hWnd;
extern HINSTANCE	g_hInst;

#ifdef SERVER
extern SOCKET	g_hSocket;
extern int		g_iSNum;
extern bool		g_bIsActive;
#endif // SERVER


/*__________________________________________________________________________________________________________
[ Camera ]
____________________________________________________________________________________________________________*/
extern _bool g_bIsOnDebugCaemra;