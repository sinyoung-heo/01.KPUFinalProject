#pragma once
#define	MSG_BOX(_message)			MessageBoxW(NULL, _message, L"System Message", MB_OK)
#define	TAGMSG_BOX(_tag,_message)	MessageBoxW(NULL, TEXT(_message), _tag, MB_OK)


// #define STAGE_LDH
#define STAGE_HSY

#ifdef _DEBUG

#define CLIENT_LOG 0

#endif

/*____________________________________________________________________
Player Direction
______________________________________________________________________*/
#define	FRONT		0.f
#define	RIGHT		90.f
#define	RIGHT_UP	45.f
#define	RIGHT_DOWN	135.f
#define	LEFT		270.f
#define	LEFT_UP		315.f
#define	LEFT_DOWN	225.f
#define	BACK		180.f

#define NO_EVENT_STATE (!g_bIsOnDebugCaemra && !g_bIsStageChange && !g_bIsOpenShop && !g_bIsChattingInput && !g_bIsCinemaStart && !g_bIsCinemaVergosDeath && !CMouseCursorMgr::Get_Instance()->Get_IsActiveMouse())
#define INPUT_CHATTING (!g_bIsOnDebugCaemra && !g_bIsStageChange && !g_bIsOpenShop && !g_bIsCinemaStart && !g_bIsCinemaVergosDeath)

/* 서버 오류 체크 함수 */
inline void error_display(const char* msg, int err_no)
{
	WCHAR* lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, err_no,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	std::cout << msg;
	std::wcout << L"에러 " << lpMsgBuf << std::endl;
	while (true);
	LocalFree(lpMsgBuf);
}

