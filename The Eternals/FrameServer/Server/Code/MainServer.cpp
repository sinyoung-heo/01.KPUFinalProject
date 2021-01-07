#include "stdafx.h"

/* IOCP SERVER 관련 변수*/
HANDLE g_hIocp;
SOCKET g_hListenSock;
OVER_EX g_accept_over;

/* OBJECT 관련 변수 */
mutex g_id_lock;


/* TIMER 관련 변수 */
priority_queue<event_type> g_timer_queue;
mutex g_timer_lock;

/* DATABASE 관련 변수 */
SQLHDBC g_hdbc;
SQLHSTMT g_hstmt;
SQLRETURN g_retcode;
SQLHENV g_henv;

int main()
{
	return NO_EVENT;
}