#pragma once
/*=================================�ý��� ������=================================*/
extern HANDLE g_hIocp;
extern SOCKET g_hListenSock;
extern OVER_EX g_accept_over;

extern SQLHDBC g_hdbc;
extern SQLHSTMT g_hstmt;
extern SQLRETURN g_retcode;
extern SQLHENV g_henv;

/*==================================���� ������=================================*/
/* ID LOCK */
extern mutex g_id_lock;

/* CLIENT(USER) ��ü �ڷᱸ�� */


/* Timer Queue */
extern priority_queue<event_type> g_timer_queue;
extern mutex g_timer_lock;