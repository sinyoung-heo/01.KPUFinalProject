#include "stdafx.h"
#include "Player.h"
#include "Npc.h"
#include "Monster.h"

/* IOCP SERVER 관련 변수*/
HANDLE g_hIocp;
SOCKET g_hListenSock;
OVER_EX g_accept_over;

/* OBJECT 관련 변수 */
mutex g_id_lock;

/* TIMER 관련 변수 */
priority_queue<event_type> g_timer_queue;
mutex g_timer_lock;

/* GameLogic 관련 변수 */
CTimer* g_pTimerFPS;
CTimer* g_pTimerTimeDelta;

bool g_bIsGameEnd = false;

/*==============================================================함수 선언부========================================================================*/
void Ready_ServerManager();			// 서버 매니저 초기화
void Ready_Server();				// 서버 메인 루프 초기화
void Release_Server();				// 서버 종료

void Initialize_NPC();				// Create Stage Velika NPC 
void Delete_NPC();					// Delete All NPC

void Initialize_Monster();			// Create Monster(test)
void Delete_Monster();				// Delete All MONSTER

void add_new_client(SOCKET ns);		// 새로운 유저 접속 함수
void disconnect_client(int id);		// 유저 접속 정료 함수

void time_worker();					// Timer Thread Enter Function
void worker_thread();				// Worker Thread Enter Function
void gameLogic_worker();			// Game Logic Thread Enter Function

/*==============================================================MAIN CODE=========================================================================*/
int main()
{
	srand(unsigned int(time(nullptr)));
	Ready_Server();

	/* Time Thread 생성 */
	thread time_thread{ time_worker };

	thread gameLogic_thread{ gameLogic_worker };
	
	/* Worker Threads 생성 - 4개 */
	vector<thread> vecWorker;
	for (int i = 0; i < SERVER_CORE; ++i)
		vecWorker.emplace_back(worker_thread);

	/* Worker Threads 소멸 */
	for (auto& th : vecWorker)
		th.join();

	/* Time Thread 소멸 */
	time_thread.join();

	/* Game Logic Thread 소멸 */
	gameLogic_thread.join();

	Release_Server();
	closesocket(g_hListenSock);
	WSACleanup();

	return NO_EVENT;
}
/*==============================================================함수 정의부========================================================================*/
void Ready_ServerManager()
{
	// INIT OBJECT POOL MANAGER
	CSectorMgr::GetInstance();
	CObjPoolMgr::GetInstance()->Init_ObjPoolMgr();	
	CObjMgr::GetInstance()->Init_ObjMgr();
	CDBMgr::GetInstance()->Ready_DB();
	CNaviMesh::GetInstance()->Ready_NaviMesh(L"../../../FrameDirectX12/Bin/ToolData/StageVelika_NaviMesh.navimeshcellinfo");
	
	CCollisionMgr::GetInstance();
#ifdef TEST
	cout << "Finish Server Managers" << endl;
#endif // TEST
}

void Ready_Server()
{
	/* Init Server Managers */
	Ready_ServerManager();

	/* Create NPC */
	//Initialize_NPC();

	/* Create Monster */
	Initialize_Monster();

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

#ifdef TEST
	cout << "Finish Server Initialization" << endl;
#endif 
}

void Release_Server()
{
	CSectorMgr::GetInstance()->DestroyInstance();
	CObjMgr::GetInstance()->DestroyInstance();
	CObjPoolMgr::GetInstance()->DestroyInstance();
	CDBMgr::GetInstance()->DestroyInstance();
	CNaviMesh::GetInstance()->DestroyInstance();
	CCollisionMgr::GetInstance()->DestroyInstance();

	closesocket(g_hListenSock);
	WSACleanup();
	exit(1);
}

void Initialize_NPC()
{
	CNpc* pNew = nullptr;
	int s_num = -1;

	/* ______________________________________________________________________________________________________*/
	/*											SCENE - VELIKA TOWN											 */
	/* ______________________________________________________________________________________________________*/

	// NORMAL NPC 1
	pNew = static_cast<CNpc*>(CObjPoolMgr::GetInstance()->use_Object(L"NPC"));

	if (pNew)
	{
		/* NPC의 정보 초기화 */
		pNew->m_sNum += NPC_NUM_START;
		s_num = pNew->m_sNum;

		pNew->Set_IsConnected(true);
		pNew->Set_IsDead(false);
		strncpy_s(pNew->m_ID, "Chicken", strlen("Chicken"));
		strncpy_s(pNew->m_naviType, "StageVelika_NaviMesh", strlen("StageVelika_NaviMesh"));

		pNew->m_vPos = _vec3(110.0f, 0.f, 70.0f);
		pNew->m_vTempPos = pNew->m_vPos;
		pNew->m_vDir = _vec3(0.f, 0.f, 1.f);
		pNew->m_vAngle = _vec3(0.f, 0.f, 0.f);
		pNew->m_type = TYPE_NPC;
		pNew->m_npcNum = NPC_NORMAL;
		pNew->m_status = STATUS::ST_NONACTIVE;

		CSectorMgr::GetInstance()->Enter_ClientInSector(s_num, (int)(pNew->m_vPos.z / SECTOR_SIZE), (int)(pNew->m_vPos.x / SECTOR_SIZE));
		CObjMgr::GetInstance()->Add_GameObject(L"NPC", pNew, s_num);
	}
	else return;

	// NORMAL NPC 2
	pNew = static_cast<CNpc*>(CObjPoolMgr::GetInstance()->use_Object(L"NPC"));

	if (pNew)
	{
		/* NPC의 정보 초기화 */
		pNew->m_sNum += NPC_NUM_START;
		s_num = pNew->m_sNum;

		pNew->Set_IsConnected(true);
		pNew->Set_IsDead(false);
		strncpy_s(pNew->m_ID, "Cat", strlen("Cat"));
		strncpy_s(pNew->m_naviType, "StageVelika_NaviMesh", strlen("StageVelika_NaviMesh"));

		pNew->m_vPos = _vec3(98.0f, 0.f, 80.0f);
		pNew->m_vTempPos = pNew->m_vPos;
		pNew->m_vDir = _vec3(0.f, 0.f, 1.f);
		pNew->m_vAngle = _vec3(0.f, 0.f, 0.f);
		pNew->m_type = TYPE_NPC;
		pNew->m_npcNum = NPC_NORMAL;
		pNew->m_status = STATUS::ST_NONACTIVE;

		CSectorMgr::GetInstance()->Enter_ClientInSector(s_num, (int)(pNew->m_vPos.z / SECTOR_SIZE), (int)(pNew->m_vPos.x / SECTOR_SIZE));
		CObjMgr::GetInstance()->Add_GameObject(L"NPC", pNew, s_num);
	}
	else return;

	// NORMAL NPC 3
	pNew = static_cast<CNpc*>(CObjPoolMgr::GetInstance()->use_Object(L"NPC"));

	if (pNew)
	{
		/* NPC의 정보 초기화 */
		pNew->m_sNum += NPC_NUM_START;
		s_num = pNew->m_sNum;

		pNew->Set_IsConnected(true);
		pNew->Set_IsDead(false);
		strncpy_s(pNew->m_ID, "Cat", strlen("Cat"));
		strncpy_s(pNew->m_naviType, "StageVelika_NaviMesh", strlen("StageVelika_NaviMesh"));


		pNew->m_vPos = _vec3(140.0f, 0.f, 125.0f);
		pNew->m_vTempPos = pNew->m_vPos;
		pNew->m_vDir = _vec3(0.f, 0.f, 1.f);
		pNew->m_vAngle = _vec3(0.f, 0.f, 0.f);
		pNew->m_type = TYPE_NPC;
		pNew->m_npcNum = NPC_NORMAL;
		pNew->m_status = STATUS::ST_NONACTIVE;

		CSectorMgr::GetInstance()->Enter_ClientInSector(s_num, (int)(pNew->m_vPos.z / SECTOR_SIZE), (int)(pNew->m_vPos.x / SECTOR_SIZE));
		CObjMgr::GetInstance()->Add_GameObject(L"NPC", pNew, s_num);
	}
	else return;

	// NORMAL NPC 4
	pNew = static_cast<CNpc*>(CObjPoolMgr::GetInstance()->use_Object(L"NPC"));

	if (pNew)
	{
		/* NPC의 정보 초기화 */
		pNew->m_sNum += NPC_NUM_START;
		s_num = pNew->m_sNum;

		pNew->Set_IsConnected(true);
		pNew->Set_IsDead(false);
		strncpy_s(pNew->m_ID, "Aman_boy", strlen("Aman_boy"));
		strncpy_s(pNew->m_naviType, "StageVelika_NaviMesh", strlen("StageVelika_NaviMesh"));

		pNew->m_vPos = _vec3(91.2f, 0.f, 60.0f);
		pNew->m_vTempPos = pNew->m_vPos;
		pNew->m_vDir = _vec3(0.f, 0.f, 1.f);
		pNew->m_vAngle = _vec3(0.f, -90.f, 0.f);
		pNew->m_type = TYPE_NPC;
		pNew->m_npcNum = NPC_BG;
		pNew->m_status = STATUS::ST_NONACTIVE;

		CSectorMgr::GetInstance()->Enter_ClientInSector(s_num, (int)(pNew->m_vPos.z / SECTOR_SIZE), (int)(pNew->m_vPos.x / SECTOR_SIZE));
		CObjMgr::GetInstance()->Add_GameObject(L"NPC", pNew, s_num);
	}
	else return;

	// NORMAL NPC 5
	pNew = static_cast<CNpc*>(CObjPoolMgr::GetInstance()->use_Object(L"NPC"));

	if (pNew)
	{
		/* NPC의 정보 초기화 */
		pNew->m_sNum += NPC_NUM_START;
		s_num = pNew->m_sNum;

		pNew->Set_IsConnected(true);
		pNew->Set_IsDead(false);
		strncpy_s(pNew->m_ID, "Human_boy", strlen("Human_boy"));
		strncpy_s(pNew->m_naviType, "StageVelika_NaviMesh", strlen("StageVelika_NaviMesh"));

		pNew->m_vPos = _vec3(90.0f, 0.f, 60.0f);
		pNew->m_vTempPos = pNew->m_vPos;
		pNew->m_vDir = _vec3(0.f, 0.f, 1.f);
		pNew->m_vAngle = _vec3(0.f, 90.f, 0.f);
		pNew->m_type = TYPE_NPC;
		pNew->m_npcNum = NPC_BG;
		pNew->m_status = STATUS::ST_NONACTIVE;

		CSectorMgr::GetInstance()->Enter_ClientInSector(s_num, (int)(pNew->m_vPos.z / SECTOR_SIZE), (int)(pNew->m_vPos.x / SECTOR_SIZE));
		CObjMgr::GetInstance()->Add_GameObject(L"NPC", pNew, s_num);
	}
	else return;

	// NORMAL NPC 6
	pNew = static_cast<CNpc*>(CObjPoolMgr::GetInstance()->use_Object(L"NPC"));

	if (pNew)
	{
		/* NPC의 정보 초기화 */
		pNew->m_sNum += NPC_NUM_START;
		s_num = pNew->m_sNum;

		pNew->Set_IsConnected(true);
		pNew->Set_IsDead(false);
		strncpy_s(pNew->m_ID, "Human_boy", strlen("Human_boy"));
		strncpy_s(pNew->m_naviType, "StageVelika_NaviMesh", strlen("StageVelika_NaviMesh"));

		pNew->m_vPos = _vec3(90.0f, 0.f, 110.0f);
		pNew->m_vTempPos = pNew->m_vPos;
		pNew->m_vDir = _vec3(0.f, 0.f, 1.f);
		pNew->m_vAngle = _vec3(0.f, 0.f, 0.f);
		pNew->m_type = TYPE_NPC;
		pNew->m_npcNum = NPC_NORMAL;
		pNew->m_status = STATUS::ST_NONACTIVE;

		CSectorMgr::GetInstance()->Enter_ClientInSector(s_num, (int)(pNew->m_vPos.z / SECTOR_SIZE), (int)(pNew->m_vPos.x / SECTOR_SIZE));
		CObjMgr::GetInstance()->Add_GameObject(L"NPC", pNew, s_num);
	}
	else return;

	// NORMAL NPC 7
	pNew = static_cast<CNpc*>(CObjPoolMgr::GetInstance()->use_Object(L"NPC"));

	if (pNew)
	{
		/* NPC의 정보 초기화 */
		pNew->m_sNum += NPC_NUM_START;
		s_num = pNew->m_sNum;

		pNew->Set_IsConnected(true);
		pNew->Set_IsDead(false);
		strncpy_s(pNew->m_ID, "Human_boy", strlen("Human_boy"));
		strncpy_s(pNew->m_naviType, "StageVelika_NaviMesh", strlen("StageVelika_NaviMesh"));

		pNew->m_vPos = _vec3(140.0f, 0.f, 110.0f);
		pNew->m_vTempPos = pNew->m_vPos;
		pNew->m_vDir = _vec3(0.f, 0.f, 1.f);
		pNew->m_vAngle = _vec3(0.f, 0.f, 0.f);
		pNew->m_type = TYPE_NPC;
		pNew->m_npcNum = NPC_NORMAL;
		pNew->m_status = STATUS::ST_NONACTIVE;

		CSectorMgr::GetInstance()->Enter_ClientInSector(s_num, (int)(pNew->m_vPos.z / SECTOR_SIZE), (int)(pNew->m_vPos.x / SECTOR_SIZE));
		CObjMgr::GetInstance()->Add_GameObject(L"NPC", pNew, s_num);
	}
	else return;

	// NORMAL NPC 8
	pNew = static_cast<CNpc*>(CObjPoolMgr::GetInstance()->use_Object(L"NPC"));

	if (pNew)
	{
		/* NPC의 정보 초기화 */
		pNew->m_sNum += NPC_NUM_START;
		s_num = pNew->m_sNum;

		pNew->Set_IsConnected(true);
		pNew->Set_IsDead(false);
		strncpy_s(pNew->m_ID, "Cat", strlen("Cat"));
		strncpy_s(pNew->m_naviType, "StageVelika_NaviMesh", strlen("StageVelika_NaviMesh"));

		pNew->m_vPos = _vec3(125.0f, 0.f, 80.0f);
		pNew->m_vTempPos = pNew->m_vPos;
		pNew->m_vDir = _vec3(0.f, 0.f, 1.f);
		pNew->m_vAngle = _vec3(0.f, 0.f, 0.f);
		pNew->m_type = TYPE_NPC;
		pNew->m_npcNum = NPC_NORMAL;
		pNew->m_status = STATUS::ST_NONACTIVE;

		CSectorMgr::GetInstance()->Enter_ClientInSector(s_num, (int)(pNew->m_vPos.z / SECTOR_SIZE), (int)(pNew->m_vPos.x / SECTOR_SIZE));
		CObjMgr::GetInstance()->Add_GameObject(L"NPC", pNew, s_num);
	}
	else return;

	// NORMAL NPC 9
	pNew = static_cast<CNpc*>(CObjPoolMgr::GetInstance()->use_Object(L"NPC"));

	if (pNew)
	{
		/* NPC의 정보 초기화 */
		pNew->m_sNum += NPC_NUM_START;
		s_num = pNew->m_sNum;

		pNew->Set_IsConnected(true);
		pNew->Set_IsDead(false);
		strncpy_s(pNew->m_ID, "Aman_boy", strlen("Aman_boy"));
		strncpy_s(pNew->m_naviType, "StageVelika_NaviMesh", strlen("StageVelika_NaviMesh"));

		pNew->m_vPos = _vec3(138.0f, 0.f, 85.0f);
		pNew->m_vTempPos = pNew->m_vPos;
		pNew->m_vDir = _vec3(0.f, 0.f, 1.f);
		pNew->m_vAngle = _vec3(0.f, -90.f, 0.f);
		pNew->m_type = TYPE_NPC;
		pNew->m_npcNum = NPC_BG;
		pNew->m_status = STATUS::ST_NONACTIVE;

		CSectorMgr::GetInstance()->Enter_ClientInSector(s_num, (int)(pNew->m_vPos.z / SECTOR_SIZE), (int)(pNew->m_vPos.x / SECTOR_SIZE));
		CObjMgr::GetInstance()->Add_GameObject(L"NPC", pNew, s_num);
	}
	else return;

	// NORMAL NPC 10
	pNew = static_cast<CNpc*>(CObjPoolMgr::GetInstance()->use_Object(L"NPC"));

	if (pNew)
	{
		/* NPC의 정보 초기화 */
		pNew->m_sNum += NPC_NUM_START;
		s_num = pNew->m_sNum;

		pNew->Set_IsConnected(true);
		pNew->Set_IsDead(false);
		strncpy_s(pNew->m_ID, "Human_boy", strlen("Human_boy"));
		strncpy_s(pNew->m_naviType, "StageVelika_NaviMesh", strlen("StageVelika_NaviMesh"));

		pNew->m_vPos = _vec3(137.0f, 0.f, 85.f);
		pNew->m_vTempPos = pNew->m_vPos;
		pNew->m_vDir = _vec3(0.f, 0.f, 1.f);
		pNew->m_vAngle = _vec3(0.f, 90.f, 0.f);
		pNew->m_type = TYPE_NPC;
		pNew->m_npcNum = NPC_BG;
		pNew->m_status = STATUS::ST_NONACTIVE;

		CSectorMgr::GetInstance()->Enter_ClientInSector(s_num, (int)(pNew->m_vPos.z / SECTOR_SIZE), (int)(pNew->m_vPos.x / SECTOR_SIZE));
		CObjMgr::GetInstance()->Add_GameObject(L"NPC", pNew, s_num);
	}
	else return;

	// NORMAL NPC 11
	pNew = static_cast<CNpc*>(CObjPoolMgr::GetInstance()->use_Object(L"NPC"));

	if (pNew)
	{
		/* NPC의 정보 초기화 */
		pNew->m_sNum += NPC_NUM_START;
		s_num = pNew->m_sNum;

		pNew->Set_IsConnected(true);
		pNew->Set_IsDead(false);
		strncpy_s(pNew->m_ID, "NPC_Villagers", strlen("NPC_Villagers"));
		strncpy_s(pNew->m_naviType, "StageVelika_NaviMesh", strlen("StageVelika_NaviMesh"));

		pNew->m_vPos = _vec3(175.0f, 0.f, 119.f);
		pNew->m_vTempPos = pNew->m_vPos;
		pNew->m_vDir = _vec3(0.f, 0.f, 1.f);
		pNew->m_vAngle = _vec3(0.f, 0.f, 0.f);
		pNew->m_type = TYPE_NPC;
		pNew->m_npcNum = NPC_BG;
		pNew->m_status = STATUS::ST_NONACTIVE;

		CSectorMgr::GetInstance()->Enter_ClientInSector(s_num, (int)(pNew->m_vPos.z / SECTOR_SIZE), (int)(pNew->m_vPos.x / SECTOR_SIZE));
		CObjMgr::GetInstance()->Add_GameObject(L"NPC", pNew, s_num);
	}
	else return;

	// NORMAL NPC 12
	pNew = static_cast<CNpc*>(CObjPoolMgr::GetInstance()->use_Object(L"NPC"));

	if (pNew)
	{
		/* NPC의 정보 초기화 */
		pNew->m_sNum += NPC_NUM_START;
		s_num = pNew->m_sNum;

		pNew->Set_IsConnected(true);
		pNew->Set_IsDead(false);
		strncpy_s(pNew->m_ID, "NPC_Villagers", strlen("NPC_Villagers"));
		strncpy_s(pNew->m_naviType, "StageVelika_NaviMesh", strlen("StageVelika_NaviMesh"));

		pNew->m_vPos = _vec3(82.0f, 0.f, 63.f);
		pNew->m_vTempPos = pNew->m_vPos;
		pNew->m_vDir = _vec3(0.f, 0.f, 1.f);
		pNew->m_vAngle = _vec3(0.f, 0.f, 0.f);
		pNew->m_type = TYPE_NPC;
		pNew->m_npcNum = NPC_BG;
		pNew->m_status = STATUS::ST_NONACTIVE;

		CSectorMgr::GetInstance()->Enter_ClientInSector(s_num, (int)(pNew->m_vPos.z / SECTOR_SIZE), (int)(pNew->m_vPos.x / SECTOR_SIZE));
		CObjMgr::GetInstance()->Add_GameObject(L"NPC", pNew, s_num);
	}
	else return;

	// NORMAL NPC 13
	pNew = static_cast<CNpc*>(CObjPoolMgr::GetInstance()->use_Object(L"NPC"));

	if (pNew)
	{
		/* NPC의 정보 초기화 */
		pNew->m_sNum += NPC_NUM_START;
		s_num = pNew->m_sNum;

		pNew->Set_IsConnected(true);
		pNew->Set_IsDead(false);
		strncpy_s(pNew->m_ID, "Cat", strlen("Cat"));
		strncpy_s(pNew->m_naviType, "StageVelika_NaviMesh", strlen("StageVelika_NaviMesh"));

		pNew->m_vPos = _vec3(168.0f, 0.f, 46.f);
		pNew->m_vTempPos = pNew->m_vPos;
		pNew->m_vDir = _vec3(0.f, 0.f, 1.f);
		pNew->m_vAngle = _vec3(0.f, 0.f, 0.f);
		pNew->m_type = TYPE_NPC;
		pNew->m_npcNum = NPC_NORMAL;
		pNew->m_status = STATUS::ST_NONACTIVE;

		CSectorMgr::GetInstance()->Enter_ClientInSector(s_num, (int)(pNew->m_vPos.z / SECTOR_SIZE), (int)(pNew->m_vPos.x / SECTOR_SIZE));
		CObjMgr::GetInstance()->Add_GameObject(L"NPC", pNew, s_num);
	}
	else return;

	// NORMAL NPC 14
	pNew = static_cast<CNpc*>(CObjPoolMgr::GetInstance()->use_Object(L"NPC"));

	if (pNew)
	{
		/* NPC의 정보 초기화 */
		pNew->m_sNum += NPC_NUM_START;
		s_num = pNew->m_sNum;

		pNew->Set_IsConnected(true);
		pNew->Set_IsDead(false);
		strncpy_s(pNew->m_ID, "Cat", strlen("Cat"));
		strncpy_s(pNew->m_naviType, "StageVelika_NaviMesh", strlen("StageVelika_NaviMesh"));

		pNew->m_vPos = _vec3(177.0f, 0.f, 55.f);
		pNew->m_vTempPos = pNew->m_vPos;
		pNew->m_vDir = _vec3(0.f, 0.f, 1.f);
		pNew->m_vAngle = _vec3(0.f, 0.f, 0.f);
		pNew->m_type = TYPE_NPC;
		pNew->m_npcNum = NPC_NORMAL;
		pNew->m_status = STATUS::ST_NONACTIVE;

		CSectorMgr::GetInstance()->Enter_ClientInSector(s_num, (int)(pNew->m_vPos.z / SECTOR_SIZE), (int)(pNew->m_vPos.x / SECTOR_SIZE));
		CObjMgr::GetInstance()->Add_GameObject(L"NPC", pNew, s_num);
	}
	else return;

	// NORMAL NPC 15
	pNew = static_cast<CNpc*>(CObjPoolMgr::GetInstance()->use_Object(L"NPC"));

	if (pNew)
	{
		/* NPC의 정보 초기화 */
		pNew->m_sNum += NPC_NUM_START;
		s_num = pNew->m_sNum;

		pNew->Set_IsConnected(true);
		pNew->Set_IsDead(false);
		strncpy_s(pNew->m_ID, "Cat", strlen("Cat"));
		strncpy_s(pNew->m_naviType, "StageVelika_NaviMesh", strlen("StageVelika_NaviMesh"));

		pNew->m_vPos = _vec3(170.0f, 0.f, 120.f);
		pNew->m_vTempPos = pNew->m_vPos;
		pNew->m_vDir = _vec3(0.f, 0.f, 1.f);
		pNew->m_vAngle = _vec3(0.f, 0.f, 0.f);
		pNew->m_type = TYPE_NPC;
		pNew->m_npcNum = NPC_NORMAL;
		pNew->m_status = STATUS::ST_NONACTIVE;

		CSectorMgr::GetInstance()->Enter_ClientInSector(s_num, (int)(pNew->m_vPos.z / SECTOR_SIZE), (int)(pNew->m_vPos.x / SECTOR_SIZE));
		CObjMgr::GetInstance()->Add_GameObject(L"NPC", pNew, s_num);
	}
	else return;

	// NORMAL NPC 16
	pNew = static_cast<CNpc*>(CObjPoolMgr::GetInstance()->use_Object(L"NPC"));

	if (pNew)
	{
		/* NPC의 정보 초기화 */
		pNew->m_sNum += NPC_NUM_START;
		s_num = pNew->m_sNum;

		pNew->Set_IsConnected(true);
		pNew->Set_IsDead(false);
		strncpy_s(pNew->m_ID, "Chicken", strlen("Chicken"));
		strncpy_s(pNew->m_naviType, "StageVelika_NaviMesh", strlen("StageVelika_NaviMesh"));

		pNew->m_vPos = _vec3(168.0f, 0.f, 78.f);
		pNew->m_vTempPos = pNew->m_vPos;
		pNew->m_vDir = _vec3(0.f, 0.f, 1.f);
		pNew->m_vAngle = _vec3(0.f, 0.f, 0.f);
		pNew->m_type = TYPE_NPC;
		pNew->m_npcNum = NPC_NORMAL;
		pNew->m_status = STATUS::ST_NONACTIVE;

		CSectorMgr::GetInstance()->Enter_ClientInSector(s_num, (int)(pNew->m_vPos.z / SECTOR_SIZE), (int)(pNew->m_vPos.x / SECTOR_SIZE));
		CObjMgr::GetInstance()->Add_GameObject(L"NPC", pNew, s_num);
	}
	else return;

	// NORMAL NPC 18
	pNew = static_cast<CNpc*>(CObjPoolMgr::GetInstance()->use_Object(L"NPC"));

	if (pNew)
	{
		/* NPC의 정보 초기화 */
		pNew->m_sNum += NPC_NUM_START;
		s_num = pNew->m_sNum;

		pNew->Set_IsConnected(true);
		pNew->Set_IsDead(false);
		strncpy_s(pNew->m_ID, "Chicken", strlen("Chicken"));
		strncpy_s(pNew->m_naviType, "StageVelika_NaviMesh", strlen("StageVelika_NaviMesh"));

		pNew->m_vPos = _vec3(147.0f, 0.f, 125.f);
		pNew->m_vTempPos = pNew->m_vPos;
		pNew->m_vDir = _vec3(0.f, 0.f, 1.f);
		pNew->m_vAngle = _vec3(0.f, 0.f, 0.f);
		pNew->m_type = TYPE_NPC;
		pNew->m_npcNum = NPC_NORMAL;
		pNew->m_status = STATUS::ST_NONACTIVE;

		CSectorMgr::GetInstance()->Enter_ClientInSector(s_num, (int)(pNew->m_vPos.z / SECTOR_SIZE), (int)(pNew->m_vPos.x / SECTOR_SIZE));
		CObjMgr::GetInstance()->Add_GameObject(L"NPC", pNew, s_num);
	}
	else return;

	// NORMAL NPC 19
	pNew = static_cast<CNpc*>(CObjPoolMgr::GetInstance()->use_Object(L"NPC"));

	if (pNew)
	{
		/* NPC의 정보 초기화 */
		pNew->m_sNum += NPC_NUM_START;
		s_num = pNew->m_sNum;

		pNew->Set_IsConnected(true);
		pNew->Set_IsDead(false);
		strncpy_s(pNew->m_ID, "Popori_boy", strlen("Popori_boy"));
		strncpy_s(pNew->m_naviType, "StageVelika_NaviMesh", strlen("StageVelika_NaviMesh"));

		pNew->m_vPos = _vec3(90.0f, 0.f, 155.f);
		pNew->m_vTempPos = pNew->m_vPos;
		pNew->m_vDir = _vec3(0.f, 0.f, 1.f);
		pNew->m_vAngle = _vec3(0.f, 0.f, 0.f);
		pNew->m_type = TYPE_NPC;
		pNew->m_npcNum = NPC_NORMAL;
		pNew->m_status = STATUS::ST_NONACTIVE;

		CSectorMgr::GetInstance()->Enter_ClientInSector(s_num, (int)(pNew->m_vPos.z / SECTOR_SIZE), (int)(pNew->m_vPos.x / SECTOR_SIZE));
		CObjMgr::GetInstance()->Add_GameObject(L"NPC", pNew, s_num);
	}
	else return;

	// NORMAL NPC 20
	pNew = static_cast<CNpc*>(CObjPoolMgr::GetInstance()->use_Object(L"NPC"));

	if (pNew)
	{
		/* NPC의 정보 초기화 */
		pNew->m_sNum += NPC_NUM_START;
		s_num = pNew->m_sNum;

		pNew->Set_IsConnected(true);
		pNew->Set_IsDead(false);
		strncpy_s(pNew->m_ID, "Human_boy", strlen("Human_boy"));
		strncpy_s(pNew->m_naviType, "StageVelika_NaviMesh", strlen("StageVelika_NaviMesh"));

		pNew->m_vPos = _vec3(84.0f, 0.f, 146.f);
		pNew->m_vTempPos = pNew->m_vPos;
		pNew->m_vDir = _vec3(0.f, 0.f, 1.f);
		pNew->m_vAngle = _vec3(0.f, 0.f, 0.f);
		pNew->m_type = TYPE_NPC;
		pNew->m_npcNum = NPC_NORMAL;
		pNew->m_status = STATUS::ST_NONACTIVE;

		CSectorMgr::GetInstance()->Enter_ClientInSector(s_num, (int)(pNew->m_vPos.z / SECTOR_SIZE), (int)(pNew->m_vPos.x / SECTOR_SIZE));
		CObjMgr::GetInstance()->Add_GameObject(L"NPC", pNew, s_num);
	}
	else return;

	// NORMAL NPC 21
	pNew = static_cast<CNpc*>(CObjPoolMgr::GetInstance()->use_Object(L"NPC"));

	if (pNew)
	{
		/* NPC의 정보 초기화 */
		pNew->m_sNum += NPC_NUM_START;
		s_num = pNew->m_sNum;

		pNew->Set_IsConnected(true);
		pNew->Set_IsDead(false);
		strncpy_s(pNew->m_ID, "Popori_boy", strlen("Popori_boy"));
		strncpy_s(pNew->m_naviType, "StageVelika_NaviMesh", strlen("StageVelika_NaviMesh"));

		pNew->m_vPos = _vec3(94.0f, 0.f, 78.f);
		pNew->m_vTempPos = pNew->m_vPos;
		pNew->m_vDir = _vec3(0.f, 0.f, 1.f);
		pNew->m_vAngle = _vec3(0.f, 0.f, 0.f);
		pNew->m_type = TYPE_NPC;
		pNew->m_npcNum = NPC_NORMAL;
		pNew->m_status = STATUS::ST_NONACTIVE;

		CSectorMgr::GetInstance()->Enter_ClientInSector(s_num, (int)(pNew->m_vPos.z / SECTOR_SIZE), (int)(pNew->m_vPos.x / SECTOR_SIZE));
		CObjMgr::GetInstance()->Add_GameObject(L"NPC", pNew, s_num);
	}
	else return;

	// NORMAL NPC 22
	pNew = static_cast<CNpc*>(CObjPoolMgr::GetInstance()->use_Object(L"NPC"));

	if (pNew)
	{
		/* NPC의 정보 초기화 */
		pNew->m_sNum += NPC_NUM_START;
		s_num = pNew->m_sNum;

		pNew->Set_IsConnected(true);
		pNew->Set_IsDead(false);
		strncpy_s(pNew->m_ID, "Popori_boy", strlen("Popori_boy"));
		strncpy_s(pNew->m_naviType, "StageVelika_NaviMesh", strlen("StageVelika_NaviMesh"));

		pNew->m_vPos = _vec3(139.0f, 0.f, 138.f);
		pNew->m_vTempPos = pNew->m_vPos;
		pNew->m_vDir = _vec3(0.f, 0.f, 1.f);
		pNew->m_vAngle = _vec3(0.f, 0.f, 0.f);
		pNew->m_type = TYPE_NPC;
		pNew->m_npcNum = NPC_NORMAL;
		pNew->m_status = STATUS::ST_NONACTIVE;

		CSectorMgr::GetInstance()->Enter_ClientInSector(s_num, (int)(pNew->m_vPos.z / SECTOR_SIZE), (int)(pNew->m_vPos.x / SECTOR_SIZE));
		CObjMgr::GetInstance()->Add_GameObject(L"NPC", pNew, s_num);
	}
	else return;

	// MERCHANT NPC 1
	pNew = static_cast<CNpc*>(CObjPoolMgr::GetInstance()->use_Object(L"NPC"));

	if (pNew)
	{
		/* NPC의 정보 초기화 */
		pNew->m_sNum += NPC_NUM_START;
		s_num = pNew->m_sNum;

		pNew->Set_IsConnected(true);
		pNew->Set_IsDead(false);
		strncpy_s(pNew->m_ID, "NPC_Villagers", strlen("NPC_Villagers"));
		strncpy_s(pNew->m_naviType, "StageVelika_NaviMesh", strlen("StageVelika_NaviMesh"));

		pNew->m_vPos = _vec3(143.0f, 0.f, 79.0f);
		pNew->m_vTempPos = pNew->m_vPos;
		pNew->m_vDir = _vec3(0.f, 0.f, 1.f);
		pNew->m_vAngle = _vec3(0.f, 0.f, 0.f);
		pNew->m_type = TYPE_NPC;
		pNew->m_npcNum = NPC_MERCHANT;
		pNew->m_status = STATUS::ST_NONACTIVE;

		CSectorMgr::GetInstance()->Enter_ClientInSector(s_num, (int)(pNew->m_vPos.z / SECTOR_SIZE), (int)(pNew->m_vPos.x / SECTOR_SIZE));
		CObjMgr::GetInstance()->Add_GameObject(L"NPC", pNew, s_num);
	}
	else return;

	// MERCHANT NPC 2
	pNew = static_cast<CNpc*>(CObjPoolMgr::GetInstance()->use_Object(L"NPC"));

	if (pNew)
	{
		/* NPC의 정보 초기화 */
		pNew->m_sNum += NPC_NUM_START;
		s_num = pNew->m_sNum;

		pNew->Set_IsConnected(true);
		pNew->Set_IsDead(false);
		strncpy_s(pNew->m_ID, "NPC_Villagers", strlen("NPC_Villagers"));
		strncpy_s(pNew->m_naviType, "StageVelika_NaviMesh", strlen("StageVelika_NaviMesh"));

		pNew->m_vPos = _vec3(150.0f, 0.f, 90.0f);
		pNew->m_vTempPos = pNew->m_vPos;
		pNew->m_vDir = _vec3(0.f, 0.f, 1.f);
		pNew->m_vAngle = _vec3(0.f, 0.f, 0.f);
		pNew->m_type = TYPE_NPC;
		pNew->m_npcNum = NPC_MERCHANT;
		pNew->m_status = STATUS::ST_NONACTIVE;

		CSectorMgr::GetInstance()->Enter_ClientInSector(s_num, (int)(pNew->m_vPos.z / SECTOR_SIZE), (int)(pNew->m_vPos.x / SECTOR_SIZE));
		CObjMgr::GetInstance()->Add_GameObject(L"NPC", pNew, s_num);
	}
	else return;

	// MERCHANT NPC 3
	pNew = static_cast<CNpc*>(CObjPoolMgr::GetInstance()->use_Object(L"NPC"));

	if (pNew)
	{
		/* NPC의 정보 초기화 */
		pNew->m_sNum += NPC_NUM_START;
		s_num = pNew->m_sNum;

		pNew->Set_IsConnected(true);
		pNew->Set_IsDead(false);
		strncpy_s(pNew->m_ID, "NPC_Villagers", strlen("NPC_Villagers"));
		strncpy_s(pNew->m_naviType, "StageVelika_NaviMesh", strlen("StageVelika_NaviMesh"));

		pNew->m_vPos = _vec3(153.0f, 0.f, 103.0f);
		pNew->m_vTempPos = pNew->m_vPos;
		pNew->m_vDir = _vec3(0.f, 0.f, 1.f);
		pNew->m_vAngle = _vec3(0.f, 0.f, 0.f);
		pNew->m_type = TYPE_NPC;
		pNew->m_npcNum = NPC_MERCHANT;
		pNew->m_status = STATUS::ST_NONACTIVE;

		CSectorMgr::GetInstance()->Enter_ClientInSector(s_num, (int)(pNew->m_vPos.z / SECTOR_SIZE), (int)(pNew->m_vPos.x / SECTOR_SIZE));
		CObjMgr::GetInstance()->Add_GameObject(L"NPC", pNew, s_num);
	}
	else return;

	// QUEST NPC
	pNew = static_cast<CNpc*>(CObjPoolMgr::GetInstance()->use_Object(L"NPC"));

	if (pNew)
	{
		/* NPC의 정보 초기화 */
		pNew->m_sNum += NPC_NUM_START;
		s_num = pNew->m_sNum;

		pNew->Set_IsConnected(true);
		pNew->Set_IsDead(false);
		strncpy_s(pNew->m_ID, "NPC_Villagers", strlen("NPC_Villagers"));
		strncpy_s(pNew->m_naviType, "StageVelika_NaviMesh", strlen("StageVelika_NaviMesh"));

		pNew->m_vPos = _vec3(120.0f, 0.f, 25.0f);
		pNew->m_vTempPos = pNew->m_vPos;
		pNew->m_vDir = _vec3(0.f, 0.f, 1.f);
		pNew->m_vAngle = _vec3(0.f, 0.f, 0.f);
		pNew->m_type = TYPE_NPC;
		pNew->m_npcNum = NPC_QUEST;
		pNew->m_status = STATUS::ST_NONACTIVE;

		CSectorMgr::GetInstance()->Enter_ClientInSector(s_num, (int)(pNew->m_vPos.z / SECTOR_SIZE), (int)(pNew->m_vPos.x / SECTOR_SIZE));
		CObjMgr::GetInstance()->Add_GameObject(L"NPC", pNew, s_num);
	}
	else return;

#ifdef TEST
	cout << "NPC Initialize Finish.\n";
#endif
}

void Delete_NPC()
{
	auto iter_begin = CObjMgr::GetInstance()->Get_OBJLIST(L"NPC")->begin();
	auto iter_end = CObjMgr::GetInstance()->Get_OBJLIST(L"NPC")->end();
	
	for (iter_begin; iter_begin != iter_end;)
	{
		CObjPoolMgr::GetInstance()->return_Object(L"NPC", iter_begin->second);
		CObjMgr::GetInstance()->Delete_GameObject(L"NPC", iter_begin->second);

		iter_begin = CObjMgr::GetInstance()->Get_OBJLIST(L"NPC")->begin();
		iter_end = CObjMgr::GetInstance()->Get_OBJLIST(L"NPC")->end();
	}
}

void Initialize_Monster()
{
	CMonster* pNew = nullptr;
	int s_num = -1;

	/* ______________________________________________________________________________________________________*/
	/*											SCENE - TEST												 */
	/* ______________________________________________________________________________________________________*/

	// TEST MONSTER - crab
	//pNew = static_cast<CMonster*>(CObjPoolMgr::GetInstance()->use_Object(L"MONSTER"));

	//if (pNew)
	//{
	//	/* NPC의 정보 초기화 */
	//	pNew->m_sNum += MON_NUM_START;
	//	s_num = pNew->m_sNum;

	//	pNew->Set_IsConnected(true);
	//	pNew->Set_IsDead(false);
	//	strncpy_s(pNew->m_ID, "Crab", strlen("Crab"));
	//	strncpy_s(pNew->m_naviType, "StageVelika_NaviMesh", strlen("StageVelika_NaviMesh"));

	//	pNew->m_vPos		= _vec3(133.0f, 0.f, 75.0f);
	//	pNew->m_vTempPos	= pNew->m_vPos;
	//	pNew->m_vOriPos		= pNew->m_vPos;
	//	pNew->m_vDir		= _vec3(0.f, 0.f, 1.f);
	//	pNew->m_vAngle		= _vec3(0.f, 90.f, 0.f);
	//	pNew->m_iHp			= 1000;
	//	pNew->m_iMaxHp		= 1000;
	//	pNew->m_iAtt		= 1;
	//	pNew->m_iExp		= 0;
	//	pNew->m_fSpd		= 0.5f;
	//	pNew->m_type		= MON_NORMAL;
	//	pNew->m_monNum		= MON_CRAB;
	//	pNew->m_status		= STATUS::ST_NONACTIVE;

	//	pNew->Set_NumAnimation(Crab::NUM_ANIMATION);
	//	pNew->Set_AnimDuration(Crab::duration);

	//	CSectorMgr::GetInstance()->Enter_ClientInSector(s_num, (int)(pNew->m_vPos.z / SECTOR_SIZE), (int)(pNew->m_vPos.x / SECTOR_SIZE));
	//	CObjMgr::GetInstance()->Add_GameObject(L"MONSTER", pNew, s_num);
	//}
	//else return;

	// TEST MONSTER - monkey
	//pNew = static_cast<CMonster*>(CObjPoolMgr::GetInstance()->use_Object(L"MONSTER"));

	//if (pNew)
	//{
	//	/* NPC의 정보 초기화 */
	//	pNew->m_sNum += MON_NUM_START;
	//	s_num = pNew->m_sNum;

	//	pNew->Set_IsConnected(true);
	//	pNew->Set_IsDead(false);
	//	strncpy_s(pNew->m_ID, "Monkey", strlen("Monkey"));
	//	strncpy_s(pNew->m_naviType, "StageVelika_NaviMesh", strlen("StageVelika_NaviMesh"));

	//	pNew->m_vPos = _vec3(143.0f, 0.f, 60.0f);
	//	pNew->m_vTempPos = pNew->m_vPos;
	//	pNew->m_vOriPos = pNew->m_vPos;
	//	pNew->m_vDir = _vec3(0.f, 0.f, 1.f);
	//	pNew->m_vAngle = _vec3(0.f, 90.f, 0.f);
	//	pNew->m_iHp = 1000;
	//	pNew->m_iMaxHp = 1000;
	//	pNew->m_iAtt = 1;
	//	pNew->m_iExp = 0;
	//	pNew->m_fSpd = 0.5f;
	//	pNew->m_type = MON_NORMAL;
	//	pNew->m_monNum = MON_MONKEY;
	//	pNew->m_status = STATUS::ST_NONACTIVE;

	//	pNew->Set_NumAnimation(Monkey::NUM_ANIMATION);
	//	pNew->Set_AnimDuration(Monkey::duration);

	//	CSectorMgr::GetInstance()->Enter_ClientInSector(s_num, (int)(pNew->m_vPos.z / SECTOR_SIZE), (int)(pNew->m_vPos.x / SECTOR_SIZE));
	//	CObjMgr::GetInstance()->Add_GameObject(L"MONSTER", pNew, s_num);
	//}
	//else return;

	// TEST MONSTER - cloder
	//pNew = static_cast<CMonster*>(CObjPoolMgr::GetInstance()->use_Object(L"MONSTER"));

	//if (pNew)
	//{
	//	/* NPC의 정보 초기화 */
	//	pNew->m_sNum += MON_NUM_START;
	//	s_num = pNew->m_sNum;

	//	pNew->Set_IsConnected(true);
	//	pNew->Set_IsDead(false);
	//	strncpy_s(pNew->m_ID, "CloderA", strlen("CloderA"));
	//	strncpy_s(pNew->m_naviType, "StageVelika_NaviMesh", strlen("StageVelika_NaviMesh"));

	//	pNew->m_vPos = _vec3(143.0f, 0.f, 50.0f);
	//	pNew->m_vTempPos = pNew->m_vPos;
	//	pNew->m_vOriPos = pNew->m_vPos;
	//	pNew->m_vDir = _vec3(0.f, 0.f, 1.f);
	//	pNew->m_vAngle = _vec3(0.f, 90.f, 0.f);
	//	pNew->m_iHp = 1000;
	//	pNew->m_iMaxHp = 1000;
	//	pNew->m_iAtt = 1;
	//	pNew->m_iExp = 0;
	//	pNew->m_fSpd = 0.5f;
	//	pNew->m_type = MON_NORMAL;
	//	pNew->m_monNum = MON_CLODER;
	//	pNew->m_status = STATUS::ST_NONACTIVE;

	//	pNew->Set_NumAnimation(Cloder::NUM_ANIMATION);
	//	pNew->Set_AnimDuration(Cloder::duration);

	//	CSectorMgr::GetInstance()->Enter_ClientInSector(s_num, (int)(pNew->m_vPos.z / SECTOR_SIZE), (int)(pNew->m_vPos.x / SECTOR_SIZE));
	//	CObjMgr::GetInstance()->Add_GameObject(L"MONSTER", pNew, s_num);
	//}
	//else return;

	// TEST MONSTER - drowned sailor
	//pNew = static_cast<CMonster*>(CObjPoolMgr::GetInstance()->use_Object(L"MONSTER"));
	//
	//if (pNew)
	//{
	//	/* NPC의 정보 초기화 */
	//	pNew->m_sNum += MON_NUM_START;
	//	s_num = pNew->m_sNum;
	//
	//	pNew->Set_IsConnected(true);
	//	pNew->Set_IsDead(false);
	//	strncpy_s(pNew->m_ID, "DrownedSailor", strlen("DrownedSailor"));
	//	strncpy_s(pNew->m_naviType, "StageVelika_NaviMesh", strlen("StageVelika_NaviMesh"));
	//
	//	pNew->m_vPos = _vec3(133.0f, 0.f, 50.0f);
	//	pNew->m_vTempPos = pNew->m_vPos;
	//	pNew->m_vOriPos = pNew->m_vPos;
	//	pNew->m_vDir = _vec3(0.f, 0.f, 1.f);
	//	pNew->m_vAngle = _vec3(0.f, 90.f, 0.f);
	//	pNew->m_iHp = 1000;
	//	pNew->m_iMaxHp = 1000;
	//	pNew->m_iAtt = 1;
	//	pNew->m_iExp = 0;
	//	pNew->m_fSpd = 0.5f;
	//	pNew->m_type = MON_NORMAL;
	//	pNew->m_monNum = MON_SAILOR;
	//	pNew->m_status = STATUS::ST_NONACTIVE;

	//	pNew->Set_NumAnimation(DrownedSailor::NUM_ANIMATION);
	//	pNew->Set_AnimDuration(DrownedSailor::duration);
	//
	//	CSectorMgr::GetInstance()->Enter_ClientInSector(s_num, (int)(pNew->m_vPos.z / SECTOR_SIZE), (int)(pNew->m_vPos.x / SECTOR_SIZE));
	//	CObjMgr::GetInstance()->Add_GameObject(L"MONSTER", pNew, s_num);
	//}
	//else return;

	// TEST MONSTER - Giant Beetle
	//pNew = static_cast<CMonster*>(CObjPoolMgr::GetInstance()->use_Object(L"MONSTER"));

	//if (pNew)
	//{
	//	/* NPC의 정보 초기화 */
	//	pNew->m_sNum += MON_NUM_START;
	//	s_num = pNew->m_sNum;

	//	pNew->Set_IsConnected(true);
	//	pNew->Set_IsDead(false);
	//	strncpy_s(pNew->m_ID, "GiantBeetle", strlen("GiantBeetle"));
	//	strncpy_s(pNew->m_naviType, "StageVelika_NaviMesh", strlen("StageVelika_NaviMesh"));

	//	pNew->m_vPos = _vec3(143.0f, 0.f, 45.0f);
	//	pNew->m_vTempPos = pNew->m_vPos;
	//	pNew->m_vOriPos = pNew->m_vPos;
	//	pNew->m_vDir = _vec3(0.f, 0.f, 1.f);
	//	pNew->m_vAngle = _vec3(0.f, 90.f, 0.f);
	//	pNew->m_iHp = 1000;
	//	pNew->m_iMaxHp = 1000;
	//	pNew->m_iAtt = 1;
	//	pNew->m_iExp = 0;
	//	pNew->m_fSpd = 0.5f;
	//	pNew->m_type = MON_NORMAL;
	//	pNew->m_monNum = MON_GBEETLE;
	//	pNew->m_status = STATUS::ST_NONACTIVE;

	//	pNew->Set_NumAnimation(GiantBeetle::NUM_ANIMATION);
	//	pNew->Set_AnimDuration(GiantBeetle::duration);

	//	CSectorMgr::GetInstance()->Enter_ClientInSector(s_num, (int)(pNew->m_vPos.z / SECTOR_SIZE), (int)(pNew->m_vPos.x / SECTOR_SIZE));
	//	CObjMgr::GetInstance()->Add_GameObject(L"MONSTER", pNew, s_num);
	//}
	//else return;

	// TEST MONSTER - Giant Monkey
	//pNew = static_cast<CMonster*>(CObjPoolMgr::GetInstance()->use_Object(L"MONSTER"));
	//
	//if (pNew)
	//{
	//	/* NPC의 정보 초기화 */
	//	pNew->m_sNum += MON_NUM_START;
	//	s_num = pNew->m_sNum;

	//	pNew->Set_IsConnected(true);
	//	pNew->Set_IsDead(false);
	//	strncpy_s(pNew->m_ID, "GiantMonkey", strlen("GiantMonkey"));
	//	strncpy_s(pNew->m_naviType, "StageVelika_NaviMesh", strlen("StageVelika_NaviMesh"));

	//	pNew->m_vPos = _vec3(143.0f, 0.f, 50.0f);
	//	pNew->m_vTempPos = pNew->m_vPos;
	//	pNew->m_vOriPos = pNew->m_vPos;
	//	pNew->m_vDir = _vec3(0.f, 0.f, 1.f);
	//	pNew->m_vAngle = _vec3(0.f, 90.f, 0.f);
	//	pNew->m_iHp = 1000;
	//	pNew->m_iMaxHp = 1000;
	//	pNew->m_iAtt = 1;
	//	pNew->m_iExp = 0;
	//	pNew->m_fSpd = 0.5f;
	//	pNew->m_type = MON_NORMAL;
	//	pNew->m_monNum = MON_GMONKEY;
	//	pNew->m_status = STATUS::ST_NONACTIVE;

	//	pNew->Set_NumAnimation(GiantMonkey::NUM_ANIMATION);
	//	pNew->Set_AnimDuration(GiantMonkey::duration);

	//	CSectorMgr::GetInstance()->Enter_ClientInSector(s_num, (int)(pNew->m_vPos.z / SECTOR_SIZE), (int)(pNew->m_vPos.x / SECTOR_SIZE));
	//	CObjMgr::GetInstance()->Add_GameObject(L"MONSTER", pNew, s_num);
	//}
	//else return;

	// TEST MONSTER - Crafty Arachne
	pNew = static_cast<CMonster*>(CObjPoolMgr::GetInstance()->use_Object(L"MONSTER"));
	
	if (pNew)
	{
		/* NPC의 정보 초기화 */
		pNew->m_sNum += MON_NUM_START;
		s_num = pNew->m_sNum;
	
		pNew->Set_IsConnected(true);
		pNew->Set_IsDead(false);
		strncpy_s(pNew->m_ID, "CraftyArachne", strlen("CraftyArachne"));
		strncpy_s(pNew->m_naviType, "StageVelika_NaviMesh", strlen("StageVelika_NaviMesh"));
	
		pNew->m_vPos = _vec3(143.0f, 0.f, 60.0f);
		pNew->m_vTempPos = pNew->m_vPos;
		pNew->m_vOriPos = pNew->m_vPos;
		pNew->m_vDir = _vec3(0.f, 0.f, 1.f);
		pNew->m_vAngle = _vec3(0.f, 90.f, 0.f);
		pNew->m_iHp = 1000;
		pNew->m_iMaxHp = 1000;
		pNew->m_iAtt = 1;
		pNew->m_iExp = 0;
		pNew->m_fSpd = 0.5f;
		pNew->m_type = MON_NORMAL;
		pNew->m_monNum = MON_ARACHNE;
		pNew->m_status = STATUS::ST_NONACTIVE;
	
		pNew->Set_NumAnimation(CraftyArachne::NUM_ANIMATION);
		pNew->Set_AnimDuration(CraftyArachne::duration);
	
		CSectorMgr::GetInstance()->Enter_ClientInSector(s_num, (int)(pNew->m_vPos.z / SECTOR_SIZE), (int)(pNew->m_vPos.x / SECTOR_SIZE));
		CObjMgr::GetInstance()->Add_GameObject(L"MONSTER", pNew, s_num);
	}
	else return;
}

void Delete_Monster()
{
	auto iter_begin = CObjMgr::GetInstance()->Get_OBJLIST(L"MONSTER")->begin();
	auto iter_end = CObjMgr::GetInstance()->Get_OBJLIST(L"MONSTER")->end();

	for (iter_begin; iter_begin != iter_end;)
	{
		CObjPoolMgr::GetInstance()->return_Object(L"MONSTER", iter_begin->second);
		CObjMgr::GetInstance()->Delete_GameObject(L"MONSTER", iter_begin->second);

		iter_begin = CObjMgr::GetInstance()->Get_OBJLIST(L"MONSTER")->begin();
		iter_end = CObjMgr::GetInstance()->Get_OBJLIST(L"MONSTER")->end();
	}
}

void add_new_client(SOCKET ns)
{
	size_t s_num = 0;

	/* 서버에서 유저를 관리할 번호 설정 */
	CPlayer* pNew = static_cast<CPlayer*>(CObjPoolMgr::GetInstance()->use_Object(L"PLAYER"));
	if (!pNew) return;
	
	g_id_lock.lock();
	if (CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", pNew->m_sNum) == nullptr)
		s_num = pNew->m_sNum;
	else
		CObjPoolMgr::GetInstance()->return_Object(L"PLAYER", pNew);
	g_id_lock.unlock();

	// 최대 서버 인원 초과 여부
	if (MAX_USER == CObjMgr::GetInstance()->Get_OBJLIST(L"PLAYER")->size())
	{
#ifdef TEST
		cout << "Max user limit exceeded.\n";
#endif
		closesocket(ns);
	}
	else
	{
#ifdef TEST
		cout << "New Client [" << s_num << "] Accepted" << endl;
#endif
		/* 새로 접속한 유저의 정보 초기화 */
		pNew->Get_ClientLock().lock();
		pNew->Set_IsConnected(true);
		pNew->Set_IsDead(false);
		pNew->m_sock = ns;
		pNew->m_ID[0] = 0;
		pNew->Get_ClientLock().unlock();

		pNew->m_packet_start = pNew->m_recv_over.iocp_buf;
		pNew->m_recv_over.op_mode = OPMODE::OP_MODE_RECV;
		pNew->m_recv_over.wsa_buf.buf = reinterpret_cast<CHAR*>(pNew->m_recv_over.iocp_buf);
		pNew->m_recv_over.wsa_buf.len = sizeof(pNew->m_recv_over.iocp_buf);
		ZeroMemory(&pNew->m_recv_over.wsa_over, sizeof(pNew->m_recv_over.wsa_over));
		pNew->m_recv_start = pNew->m_recv_over.iocp_buf;

		pNew->m_type	= PC_GLADIATOR;
		pNew->m_iLevel	= 1;
		pNew->m_iHp		= 100;
		pNew->m_iMaxHp	= 100;
		pNew->m_iMp		= 100;
		pNew->m_iMaxMp	= 100;
		pNew->m_iExp	= 0;
		pNew->m_iMaxExp = 100;
		pNew->m_iAtt	= 10;
		pNew->m_fSpd	= 5.f;

		pNew->m_vPos	= _vec3(143.0f, 0.f, 73.0f);
		pNew->m_vDir	= _vec3(0.f, 0.f, 1.f);
		pNew->m_vAngle	= _vec3(0.f, 0.f, 0.f);

		CSectorMgr::GetInstance()->Enter_ClientInSector((int)s_num, (int)(pNew->m_vPos.z / SECTOR_SIZE), (int)(pNew->m_vPos.x / SECTOR_SIZE));
		CObjMgr::GetInstance()->Add_GameObject(L"PLAYER", pNew, (int)s_num);

		/* 해당 클라이언트 소켓을 IOCP에 등록 */
		CreateIoCompletionPort(reinterpret_cast<HANDLE>(ns), g_hIocp, s_num, 0);

		DWORD flags = 0;
		int ret = -1;

		/* 해당 클라이언트로부터 정보를 RECV */
		pNew->Get_ClientLock().lock();
		// 현재 해당 클라이언트의 접속 여부 검사
		if (pNew->Get_IsConnected())
			ret = WSARecv(pNew->m_sock, &pNew->m_recv_over.wsa_buf, 1, NULL, &flags, &pNew->m_recv_over.wsa_over, NULL);
		pNew->Get_ClientLock().unlock();

		if (SOCKET_ERROR == ret)
		{
			int err_no = WSAGetLastError();
			if (ERROR_IO_PENDING != err_no)
				error_display("WSARecv : ", err_no);
		}
	}

	/* 새로 들어온 유저의 접속 처리 완료 -> 다시 비동기 ACCEPT */
	SOCKET cSocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	g_accept_over.op_mode = OP_MODE_ACCEPT;
	g_accept_over.wsa_buf.len = static_cast<ULONG> (cSocket);
	ZeroMemory(&g_accept_over.wsa_over, sizeof(&g_accept_over.wsa_over));
	AcceptEx(g_hListenSock, cSocket, g_accept_over.iocp_buf, 0, 32, 32, NULL, &g_accept_over.wsa_over);
}

void disconnect_client(int id)
{
	CPlayer* pPlayer = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", id));

	if (pPlayer == nullptr) return;

	/* sector에서 해당 플레이어 지우기 */
	CSectorMgr::GetInstance()->Leave_ClientInSector(id, (int)(pPlayer->m_vPos.z / SECTOR_SIZE), (int)(pPlayer->m_vPos.x / SECTOR_SIZE));

	/* 해당 플레이어가 등록되어 있는 섹터 내의 유저들에게 접속 종료를 알림 */
	unordered_set<pair<int, int>> nearSector;
	nearSector.reserve(5);
	CSectorMgr::GetInstance()->Get_NearSectorIndex(&nearSector, (int)pPlayer->m_vPos.x, (int)pPlayer->m_vPos.z);

	// 인접 섹터 순회
	for (auto& s : nearSector)
	{
		// 인접 섹터 내의 타 유저들이 있는지 검사
		if (!(CSectorMgr::GetInstance()->Get_SectorList()[s.first][s.second].Get_ObjList().empty()))
		{
			// 타 유저의 서버 번호 추출
			for (auto obj_num : CSectorMgr::GetInstance()->Get_SectorList()[s.first][s.second].Get_ObjList())
			{
				/* 오직 유저들에게만 패킷을 전송 (NPC 제외) */
				if (false == CObjMgr::GetInstance()->Is_Player(obj_num)) continue;		
				// '나'에게 패킷 전송 X
				if (obj_num == id) continue;

				CPlayer* pOther = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", obj_num));
				
				// 접속한 유저들에게만 접속 종료를 알림
				if (pOther->Get_IsConnected())
				{
					/* 타 유저의 시야 목록 내에 '나'가 있다면 지운다 */
					pOther->v_lock.lock();
					if (0 != pOther->view_list.count(id))
					{
						pOther->view_list.erase(id);
						pOther->v_lock.unlock();

						/* 타 유저에게 접속 종료 패킷 전송 */
						send_leave_packet(obj_num, id);
					}
					else pOther->v_lock.unlock();					
				}
			}
		}
	}

	pPlayer->Get_ClientLock().lock();

#ifndef DUMMY
	// DB 정보 저장 후 종료
	CDBMgr::GetInstance()->Update_stat_DB(id);
#endif // !DUMMY

	CObjPoolMgr::GetInstance()->return_Object(L"PLAYER", pPlayer);
	CObjMgr::GetInstance()->Delete_GameObject(L"PLAYER", pPlayer);
	pPlayer->Set_IsConnected(false);
	closesocket(pPlayer->m_sock);
	pPlayer->m_sock		= 0;
	pPlayer->m_vPos		= _vec3(0.f, 0.f, 0.f);
	pPlayer->m_vDir		= _vec3(0.f, 0.f, 0.f);
	pPlayer->m_ID[0]	= 0;
	pPlayer->m_type		= 0;
	pPlayer->Get_ClientLock().unlock();

	if (CObjMgr::GetInstance()->Get_OBJLIST(L"PLAYER")->size() <= 0)
	{
		g_bIsGameEnd = true;
		Delete_NPC();
		Delete_Monster();
		Release_Server();
	}
}

void time_worker()
{
	while (true)
	{
		while (true)
		{
			g_timer_lock.lock();
			if (!g_timer_queue.empty())
			{
				event_type ev = g_timer_queue.top();

				if (ev.wakeup_time > system_clock::now())
				{
					g_timer_lock.unlock();
					break;
				}
				g_timer_queue.pop();
				g_timer_lock.unlock();

				OVER_EX* ex_over = new OVER_EX;
				ex_over->op_mode = ev.event_id;

				PostQueuedCompletionStatus(g_hIocp, 1, ev.obj_id, &ex_over->wsa_over);
			}
			else
			{
				g_timer_lock.unlock();
				break;
			}
		}
		this_thread::sleep_for(1ms);
	}
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
		{
			int err_no = WSAGetLastError();
			if (64 == err_no)
				disconnect_client(key);
			else
			{
				disconnect_client(key);
#ifdef TEST
				error_display("GQCS Error : ", WSAGetLastError());
#endif
			}
		}

		/* <I/O 처리 작업> */
		OVER_EX* over_ex = reinterpret_cast<OVER_EX*>(lpover);

		switch (over_ex->op_mode)
		{
		case OPMODE::OP_MODE_ACCEPT:
			add_new_client(static_cast<SOCKET>(over_ex->wsa_buf.len));
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
				process_recv(key, io_size);
			}
			break;

		case OPMODE::OP_MODE_SEND:
			delete over_ex;
			break;

		case OPMODE::OP_RANDOM_MOVE_NPC:
		{
			random_move_npc(key);
			delete over_ex;
		}
		break;

		case OPMODE::OP_MODE_CHASE_MONSTER:
		{
			CMonster* pMonster = static_cast<CMonster*>(CObjMgr::GetInstance()->Get_GameObject(L"MONSTER", key));
			if (nullptr == pMonster) return;

			pMonster->Change_ChaseMode();
			delete over_ex;
		}
		break;

		default:
#ifdef TEST
			cout << "[ERROR] Unknown Type MODE in Worker Thread!" << endl;
#endif 
			break;
		}
	}
}

void gameLogic_worker()
{
	// Create Timer
	g_pTimerFPS			= CTimer::Create();
	g_pTimerTimeDelta	= CTimer::Create();

	float fFrame	= 1.f / 60.f;
	float fTime		= 0.f;

	while (true)
	{
		/* Timer Update */
		if (nullptr != g_pTimerFPS)
			g_pTimerFPS->Update_Timer();

		/* Limit 60 FPS */
		fTime += g_pTimerFPS->Get_TimeDelta();

		if (fTime >= fFrame)
		{
			fTime = 0.f;

			if (g_pTimerTimeDelta != nullptr)
				g_pTimerTimeDelta->Update_Timer();

			/* MONSTER */
			if (g_bIsGameEnd) return;

			auto iter_begin = CObjMgr::GetInstance()->Get_OBJLIST(L"MONSTER")->begin();
			auto iter_end = CObjMgr::GetInstance()->Get_OBJLIST(L"MONSTER")->end();

			for (; iter_begin != iter_end;)
			{
				int iEvent = static_cast<CMonster*>(iter_begin->second)->Update_Monster(g_pTimerTimeDelta->Get_TimeDelta());

				if (DEAD_OBJ == iEvent)
				{
					CObjPoolMgr::GetInstance()->return_Object(L"MONSTER", iter_begin->second);
					CObjMgr::GetInstance()->Delete_GameObject(L"MONSTER", iter_begin->second);

					iter_begin = CObjMgr::GetInstance()->Get_OBJLIST(L"MONSTER")->begin();
					iter_end = CObjMgr::GetInstance()->Get_OBJLIST(L"MONSTER")->end();
				}
				else 
					++iter_begin;
			}

			/* COLLISION */
		}
	}

	// Free Timer.
	if (g_pTimerFPS)
	{
		delete g_pTimerFPS;
		g_pTimerFPS = nullptr;
	}

	if (g_pTimerTimeDelta)
	{
		delete g_pTimerTimeDelta;
		g_pTimerTimeDelta = nullptr;
	}
}
