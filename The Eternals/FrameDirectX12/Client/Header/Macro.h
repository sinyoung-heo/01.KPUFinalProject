#pragma once

#define	MSG_BOX(_message)			MessageBoxW(NULL, _message, L"System Message", MB_OK)
#define	TAGMSG_BOX(_tag,_message)	MessageBoxW(NULL, TEXT(_message), _tag, MB_OK)



#ifdef _DEBUG

#define CLIENT_LOG 0

#endif

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

