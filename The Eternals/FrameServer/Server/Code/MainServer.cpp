#include "stdafx.h"
#define TEST

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

/*==============================================================함수 선언부========================================================================*/
void Ready_Server();				// 서버 메인 루프 초기화

void add_new_client(SOCKET ns);		// 새로운 유저 접속 함수
void disconnect_client(int id);		// 유저 접속 정료 함수

void time_worker();					// Timer Thread Enter Function
void worker_thread();				// Worker Thread Enter Function

/*==============================================================MAIN CODE=========================================================================*/
int main()
{
	Ready_Server();

	/* Time Thread 생성 */
	thread time_thread{ time_worker };
	
	/* Worker Threads 생성 - 5개 */
	vector<thread> vecWorker;
	for (int i = 0; i < SERVER_CORE; ++i)
		vecWorker.emplace_back(worker_thread);

	/* Worker Threads 소멸 */
	for (auto& th : vecWorker)
		th.join();

	/* Time Thread 소멸 */
	time_thread.join();

	closesocket(g_hListenSock);
	WSACleanup();

	return NO_EVENT;
}

/*==============================================================함수 정의부========================================================================*/
void Ready_Server()
{
	std::wcout.imbue(std::locale("korean"));

	/* WINSOCK 초기화 */
	WSADATA wsaData;
	if (0 != WSAStartup(MAKEWORD(2, 2), &wsaData))
	{
		WSACleanup();
		return;
	}

	/* IOCP 생성 */
	g_hIocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, 0);

	/* Listen Socket 생성 */
	g_hListenSock = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);

	/* Listen Socket -> IOCP 등록 */
	CreateIoCompletionPort(reinterpret_cast<HANDLE>(g_hListenSock), g_hIocp, KEY_SERVER, 0);

	/* Server Socket Addr 구조체 생성 */
	SOCKADDR_IN serverAddr;
	memset(&serverAddr, 0, sizeof(SOCKADDR_IN));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(SERVER_PORT);
	serverAddr.sin_addr.s_addr = INADDR_ANY;

	/* bind */
	::bind(g_hListenSock, (sockaddr*)&serverAddr, sizeof(serverAddr));
	/* listen */
	listen(g_hListenSock, SOMAXCONN);

	/* 통신용 Client Socket 생성*/
	SOCKET cSocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	g_accept_over.op_mode = OP_MODE_ACCEPT;
	g_accept_over.wsa_buf.len = static_cast<int>(cSocket);
	ZeroMemory(&g_accept_over.wsa_over, sizeof(&g_accept_over.wsa_over));

	/* 비동기 Accept */
	AcceptEx(g_hListenSock, cSocket, g_accept_over.iocp_buf, 0, 32, 32, NULL, &g_accept_over.wsa_over);
}

void add_new_client(SOCKET ns)
{
}

void disconnect_client(int id)
{
}

void time_worker()
{
}

void worker_thread()
{
	// [worker_thread가 하는 일 - MAIN]
	// 현재 쓰레드를 IOCP thread pool에 등록  => GQCS
	// iocp가 처리를 맡긴 완료된 I/O 데이터를 꺼내기 => GQCS
	// 꺼낸 데이터를 처리

	while (true)
	{
		DWORD io_size;				// 받은 I/O 크기
		int key;					// 보낸이의 KEY
		ULONG_PTR iocp_key;			// 보낸이의 KEY
		WSAOVERLAPPED* lpover;		// IOCP는 OVERLAPPED 기반의 I/O

		/* GQCS => I/O 꺼내기 */
		int ret = GetQueuedCompletionStatus(g_hIocp, &io_size, &iocp_key, &lpover, INFINITE);
		key = static_cast<int>(iocp_key);

#ifdef TEST
		cout << "Completion Detected" << endl;
#endif

		if (ret == FALSE)
			error_display("GQCS Error : ", WSAGetLastError());

		/* <I/O 처리 작업> */
		OVER_EX* over_ex = reinterpret_cast<OVER_EX*>(lpover);

		switch (over_ex->op_mode)
		{
		case OPMODE::OP_MODE_ACCEPT:
			break;

		case OPMODE::OP_MODE_RECV:
			if (0 == io_size)
				disconnect_client(key);
			else
			{
#ifdef TEST
				cout << "Packet from Client [" << key << "]" << endl;
#endif 
				/* Packet 재조립 */
				//process_recv(key, io_size);
			}
			break;

		case OPMODE::OP_MODE_SEND:
			delete over_ex;
			break;

		case OPMODE::OP_MODE_ATTACK:
			break;
		case OPMODE::OP_PLAYER_MOVE_NOTIFY:
			break;
		case OPMODE::OP_RANDOM_MOVE:
			break;

		default:
#ifdef TEST
			cout << "[ERROR] Unknown Type MODE in Worker Thread!" << endl;
#endif 
			break;
		}
	}
}
