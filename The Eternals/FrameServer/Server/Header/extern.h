#pragma once
/*=================================�ý��� ������=================================*/
extern HANDLE g_hIocp;
extern SOCKET g_hListenSock;
extern OVER_EX g_accept_over;

/*==================================���� ������=================================*/
/* ID LOCK */
extern mutex g_id_lock;

/* Timer Queue */
extern priority_queue<event_type> g_timer_queue;
extern mutex g_timer_lock;

/* Game Logic */
class CTimer;
extern CTimer* g_pTimerFPS;
extern CTimer* g_pTimerTimeDelta;

/* Boss Monster Vergos Server Number */
extern int g_iVergosServerNum;