#include "stdafx.h"

/* IOCP SERVER ���� ����*/
HANDLE g_hIocp;
SOCKET g_hListenSock;
OVER_EX g_accept_over;

/* OBJECT ���� ���� */
mutex g_id_lock;


/* TIMER ���� ���� */
priority_queue<event_type> g_timer_queue;
mutex g_timer_lock;

/* DATABASE ���� ���� */
SQLHDBC g_hdbc;
SQLHSTMT g_hstmt;
SQLRETURN g_retcode;
SQLHENV g_henv;

int main()
{
	return NO_EVENT;
}