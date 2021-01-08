#include "stdafx.h"
#define TEST
#include "dataType.h"

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

/*==============================================================�Լ� �����========================================================================*/
void Ready_Server();				// ���� ���� ���� �ʱ�ȭ

void time_worker();					// Timer Thread Enter Function
void worker_thread();				// Worker Thread Enter Function

/*==============================================================MAIN CODE=========================================================================*/
int main()
{
	Ready_Server();

	/* Time Thread ���� */
	thread time_thread{ time_worker };

	/* Worker Threads ���� - 4�� */
	vector<thread> vecWorker;
	for (int i = 0; i < SERVER_CORE; ++i)
		vecWorker.emplace_back(worker_thread);

	/* Worker Threads �Ҹ� */
	for (auto& th : vecWorker)
		th.join();

	/* Time Thread �Ҹ� */
	time_thread.join();

	closesocket(g_hListenSock);
	WSACleanup();

	return NO_EVENT;
}

/*==============================================================�Լ� ���Ǻ�========================================================================*/
void Ready_Server()
{
	std::wcout.imbue(std::locale("korean"));

	/* WINSOCK �ʱ�ȭ */
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);

	/* IOCP ���� */
	g_hIocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, 0);

	/* Listen Socket ���� */
	g_hListenSock = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);

	/* Listen Socket -> IOCP ��� */
	CreateIoCompletionPort(reinterpret_cast<HANDLE>(g_hListenSock), g_hIocp, KEY_SERVER, 0);

	/* Server Socket Addr ����ü ���� */
	SOCKADDR_IN serverAddr;
	memset(&serverAddr, 0, sizeof(SOCKADDR_IN));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(SERVER_PORT);
	serverAddr.sin_addr.s_addr = INADDR_ANY;

	/* bind */
	::bind(g_hListenSock, (sockaddr*)&serverAddr, sizeof(serverAddr));
	/* listen */
	listen(g_hListenSock, SOMAXCONN);

	/* ��ſ� Client Socket ����*/
	SOCKET cSocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	g_accept_over.op_mode = OP_MODE_ACCEPT;
	g_accept_over.wsa_buf.len = static_cast<int>(cSocket);
	ZeroMemory(&g_accept_over.wsa_over, sizeof(&g_accept_over.wsa_over));

	/* �񵿱� Accept */
	AcceptEx(g_hListenSock, cSocket, g_accept_over.iocp_buf, 0, 32, 32, NULL, &g_accept_over.wsa_over);
}






void time_worker()
{
}

void worker_thread()
{
}
