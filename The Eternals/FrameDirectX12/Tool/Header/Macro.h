#pragma once

#define	MSG_BOX(_message)			MessageBoxW(NULL, _message, L"System Message", MB_OK)
#define	TAGMSG_BOX(_tag,_message)	MessageBoxW(NULL, TEXT(_message), _tag, MB_OK)



#ifdef _DEBUG

#define CLIENT_LOG 0

#endif