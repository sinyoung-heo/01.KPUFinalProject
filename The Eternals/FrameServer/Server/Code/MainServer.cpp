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
	CNaviMesh_Beach::GetInstance()->Ready_NaviMesh(L"../../../FrameDirectX12/Bin/ToolData/StageBeach_NaviMesh3.navimeshcellinfo");	
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
	Initialize_NPC();

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
	CNaviMesh_Beach::GetInstance()->DestroyInstance();
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

	// NPC - Merchant 1
	pNew = static_cast<CNpc*>(CObjPoolMgr::GetInstance()->use_Object(L"NPC"));

	if (pNew)
	{
		/* NPC의 정보 초기화 */
		pNew->Ready_NPC(_vec3(143.0f, 0.f, 79.0f), _vec3(0.f, -45.f, 0.f), NPC_MERCHANT, NPC_BARAKA_MERCHANT, STAGE_VELIKA);
		pNew->Set_NumAnimation(Baraka_M_Merchant::NUM_ANIMATION);
		pNew->Set_AnimDuration(Baraka_M_Merchant::duration);
	}
	else return;

	// NPC - Merchant 2
	pNew = static_cast<CNpc*>(CObjPoolMgr::GetInstance()->use_Object(L"NPC"));

	if (pNew)
	{
		/* NPC의 정보 초기화 */
		pNew->Ready_NPC(_vec3(151.0f, 0.f, 90.0f), _vec3(0.f, -45.f, 0.f), NPC_MERCHANT, NPC_POPORI_MERCHANT, STAGE_VELIKA);
		pNew->Set_NumAnimation(Popori_M_Merchant::NUM_ANIMATION);
		pNew->Set_AnimDuration(Popori_M_Merchant::duration);
	}
	else return;

	// NPC - Merchant 3
	pNew = static_cast<CNpc*>(CObjPoolMgr::GetInstance()->use_Object(L"NPC"));

	if (pNew)
	{
		/* NPC의 정보 초기화 */
		pNew->Ready_NPC(_vec3(154.0f, 0.f, 103.0f), _vec3(0.f, -90.f, 0.f), NPC_MERCHANT, NPC_BARAKA_MYSTELLIUM, STAGE_VELIKA);
		pNew->Set_NumAnimation(Baraka_M_Mystellium::NUM_ANIMATION);
		pNew->Set_AnimDuration(Baraka_M_Mystellium::duration);
	}
	else return;

	// NPC Assistant - Popori Boy
	pNew = static_cast<CNpc*>(CObjPoolMgr::GetInstance()->use_Object(L"NPC"));

	if (pNew)
	{
		/* NPC의 정보 초기화 */
		pNew->Ready_NPC(_vec3(148.5f, 1.f, 88.5f), _vec3(0.f, -90.f, 0.f), NPC_NONMOVE, NPC_POPORI_BOY, STAGE_VELIKA);
		pNew->Set_NumAnimation(Popori_boy::NUM_ANIMATION);
		pNew->Set_AnimDuration(Popori_boy::duration);
	}
	else return;

	// NPC Assistant - Popori Boy
	pNew = static_cast<CNpc*>(CObjPoolMgr::GetInstance()->use_Object(L"NPC"));

	if (pNew)
	{
		/* NPC의 정보 초기화 */
		pNew->Ready_NPC(_vec3(148.8f, 1.f, 89.0f), _vec3(0.f, -90.f, 0.f), NPC_NONMOVE, NPC_POPORI_BOY, STAGE_VELIKA);
		pNew->Set_NumAnimation(Popori_boy::NUM_ANIMATION);
		pNew->Set_AnimDuration(Popori_boy::duration);
	}
	else return;

	// NPC - Quest 1
	pNew = static_cast<CNpc*>(CObjPoolMgr::GetInstance()->use_Object(L"NPC"));

	if (pNew)
	{
		/* NPC의 정보 초기화 */
		pNew->Ready_NPC(_vec3(104.1f, 0.f, 95.0f), _vec3(0.f, -180.f, 0.f), NPC_QUEST, NPC_CASTANIC_LSMITH, STAGE_VELIKA);
		pNew->Set_NumAnimation(Castanic_M_Lsmith::NUM_ANIMATION);
		pNew->Set_AnimDuration(Castanic_M_Lsmith::duration);
	}
	else return;

	// NPC - Baraka_M_Extractor (문지기)
	pNew = static_cast<CNpc*>(CObjPoolMgr::GetInstance()->use_Object(L"NPC"));

	if (pNew)
	{
		/* NPC의 정보 초기화 */
		pNew->Ready_NPC(_vec3(122.6f, 0.f, 25.0f), _vec3(0.f, 0.f, 0.f), NPC_NONMOVE, NPC_BARAKA_EXTRACTOR, STAGE_VELIKA);
		pNew->Set_NumAnimation(Baraka_M_Extractor::NUM_ANIMATION);
		pNew->Set_AnimDuration(Baraka_M_Extractor::duration);
	}
	else return;

	// NPC - Baraka_M_Extractor (문지기)
	pNew = static_cast<CNpc*>(CObjPoolMgr::GetInstance()->use_Object(L"NPC"));

	if (pNew)
	{
		/* NPC의 정보 초기화 */
		pNew->Ready_NPC(_vec3(138.2f, 0.f, 25.0f), _vec3(0.f, 0.f, 0.f), NPC_NONMOVE, NPC_BARAKA_EXTRACTOR, STAGE_VELIKA);
		pNew->Set_NumAnimation(Baraka_M_Extractor::NUM_ANIMATION);
		pNew->Set_AnimDuration(Baraka_M_Extractor::duration);
	}
	else return;

	// NPC - Baraka_M_Extractor (문지기)
	pNew = static_cast<CNpc*>(CObjPoolMgr::GetInstance()->use_Object(L"NPC"));

	if (pNew)
	{
		/* NPC의 정보 초기화 */
		pNew->Ready_NPC(_vec3(110.158f, 0.f, 206.25f), _vec3(0.f, -180.f, 0.f), NPC_NONMOVE, NPC_BARAKA_EXTRACTOR, STAGE_VELIKA);
		pNew->Set_NumAnimation(Baraka_M_Extractor::NUM_ANIMATION);
		pNew->Set_AnimDuration(Baraka_M_Extractor::duration);
	}
	else return;

	// NPC - Baraka_M_Extractor (문지기)
	pNew = static_cast<CNpc*>(CObjPoolMgr::GetInstance()->use_Object(L"NPC"));

	if (pNew)
	{
		/* NPC의 정보 초기화 */
		pNew->Ready_NPC(_vec3(120.58f, 0.f, 206.25f), _vec3(0.f, 180.f, 0.f), NPC_NONMOVE, NPC_BARAKA_EXTRACTOR, STAGE_VELIKA);
		pNew->Set_NumAnimation(Baraka_M_Extractor::NUM_ANIMATION);
		pNew->Set_AnimDuration(Baraka_M_Extractor::duration);
	}
	else return;

	// NPC - Walker (Chicken)
	pNew = static_cast<CNpc*>(CObjPoolMgr::GetInstance()->use_Object(L"NPC"));

	if (pNew)
	{
		/* NPC의 정보 초기화 */
		pNew->Ready_NPC(_vec3(114.773f, 0.f, 67.6f), _vec3(0.f, 0.f, 0.f), NPC_MOVE, NPC_CHICKEN, STAGE_VELIKA);
		pNew->Set_NumAnimation(Chicken::NUM_ANIMATION);
		pNew->Set_AnimDuration(Chicken::duration);
	}
	else return;

	// NPC - Walker (Cat)
	pNew = static_cast<CNpc*>(CObjPoolMgr::GetInstance()->use_Object(L"NPC"));

	if (pNew)
	{
		/* NPC의 정보 초기화 */
		pNew->Ready_NPC(_vec3(87.91f, 0.f, 84.52f), _vec3(0.f, 0.f, 0.f), NPC_MOVE, NPC_CAT, STAGE_VELIKA);
		pNew->Set_NumAnimation(Cat::NUM_ANIMATION);
		pNew->Set_AnimDuration(Cat::duration);
	}
	else return;

	// NPC - Walker (Aman_boy)
	pNew = static_cast<CNpc*>(CObjPoolMgr::GetInstance()->use_Object(L"NPC"));

	if (pNew)
	{
		/* NPC의 정보 초기화 */
		pNew->Ready_NPC(_vec3(103.84f, 0.f, 125.85f), _vec3(0.f, 0.f, 0.f), NPC_MOVE, NPC_AMAN_BOY, STAGE_VELIKA);
		pNew->Set_NumAnimation(Aman_boy::NUM_ANIMATION);
		pNew->Set_AnimDuration(Aman_boy::duration);
	}
	else return;

	// NPC - Walker (Chicken)
	pNew = static_cast<CNpc*>(CObjPoolMgr::GetInstance()->use_Object(L"NPC"));

	if (pNew)
	{
		/* NPC의 정보 초기화 */
		pNew->Ready_NPC(_vec3(118.9f, 0.f, 159.6f), _vec3(0.f, 0.f, 0.f), NPC_MOVE, NPC_CHICKEN, STAGE_VELIKA);
		pNew->Set_NumAnimation(Chicken::NUM_ANIMATION);
		pNew->Set_AnimDuration(Chicken::duration);
	}
	else return;

	// NPC - Walker (Cat)
	pNew = static_cast<CNpc*>(CObjPoolMgr::GetInstance()->use_Object(L"NPC"));

	if (pNew)
	{
		/* NPC의 정보 초기화 */
		pNew->Ready_NPC(_vec3(89.02f, 0.f, 165.f), _vec3(0.f, 0.f, 0.f), NPC_MOVE, NPC_CAT, STAGE_VELIKA);
		pNew->Set_NumAnimation(Cat::NUM_ANIMATION);
		pNew->Set_AnimDuration(Cat::duration);
	}
	else return;

	// NPC - Walker (Chicken)
	pNew = static_cast<CNpc*>(CObjPoolMgr::GetInstance()->use_Object(L"NPC"));

	if (pNew)
	{
		/* NPC의 정보 초기화 */
		pNew->Ready_NPC(_vec3(108.9f, 0.f, 155.6f), _vec3(0.f, 0.f, 0.f), NPC_MOVE, NPC_CHICKEN, STAGE_VELIKA);
		pNew->Set_NumAnimation(Chicken::NUM_ANIMATION);
		pNew->Set_AnimDuration(Chicken::duration);
	}
	else return;

	// NPC - Walker (Cat)
	pNew = static_cast<CNpc*>(CObjPoolMgr::GetInstance()->use_Object(L"NPC"));

	if (pNew)
	{
		/* NPC의 정보 초기화 */
		pNew->Ready_NPC(_vec3(129.02f, 0.f, 143.f), _vec3(0.f, 0.f, 0.f), NPC_MOVE, NPC_CAT, STAGE_VELIKA);
		pNew->Set_NumAnimation(Cat::NUM_ANIMATION);
		pNew->Set_AnimDuration(Cat::duration);
	}
	else return;

	// NPC - Walker (Aman_boy)
	pNew = static_cast<CNpc*>(CObjPoolMgr::GetInstance()->use_Object(L"NPC"));

	if (pNew)
	{
		/* NPC의 정보 초기화 */
		pNew->Ready_NPC(_vec3(63.5f, 0.f, 146.3f), _vec3(0.f, 0.f, 0.f), NPC_MOVE, NPC_AMAN_BOY, STAGE_VELIKA);
		pNew->Set_NumAnimation(Aman_boy::NUM_ANIMATION);
		pNew->Set_AnimDuration(Aman_boy::duration);
	}
	else return;

	// NPC - Walker (Aman_boy)
	pNew = static_cast<CNpc*>(CObjPoolMgr::GetInstance()->use_Object(L"NPC"));

	if (pNew)
	{
		/* NPC의 정보 초기화 */
		pNew->Ready_NPC(_vec3(175.4f, 0.f, 53.3f), _vec3(0.f, 0.f, 0.f), NPC_MOVE, NPC_AMAN_BOY, STAGE_VELIKA);
		pNew->Set_NumAnimation(Aman_boy::NUM_ANIMATION);
		pNew->Set_AnimDuration(Aman_boy::duration);
	}
	else return;

	// NPC - Walker (Aman_boy)
	pNew = static_cast<CNpc*>(CObjPoolMgr::GetInstance()->use_Object(L"NPC"));

	if (pNew)
	{
		/* NPC의 정보 초기화 */
		pNew->Ready_NPC(_vec3(147.02f, 0.f, 59.3f), _vec3(0.f, 0.f, 0.f), NPC_MOVE, NPC_AMAN_BOY, STAGE_VELIKA);
		pNew->Set_NumAnimation(Aman_boy::NUM_ANIMATION);
		pNew->Set_AnimDuration(Aman_boy::duration);
	}
	else return;

	// NPC - Walker (Aman_boy)
	pNew = static_cast<CNpc*>(CObjPoolMgr::GetInstance()->use_Object(L"NPC"));

	if (pNew)
	{
		/* NPC의 정보 초기화 */
		pNew->Ready_NPC(_vec3(131.25f, 0.f, 57.98f), _vec3(0.f, 0.f, 0.f), NPC_MOVE, NPC_AMAN_BOY, STAGE_VELIKA);
		pNew->Set_NumAnimation(Aman_boy::NUM_ANIMATION);
		pNew->Set_AnimDuration(Aman_boy::duration);
	}
	else return;

	// NPC - Walker (Human Boy)
	pNew = static_cast<CNpc*>(CObjPoolMgr::GetInstance()->use_Object(L"NPC"));

	if (pNew)
	{
		/* NPC의 정보 초기화 */
		pNew->Ready_NPC(_vec3(88.16f, 0.f, 143.19f), _vec3(0.f, -180.f, 0.f), NPC_MOVE, NPC_HUMAN_BOY, STAGE_VELIKA);
		pNew->Set_NumAnimation(Human_boy::NUM_ANIMATION);
		pNew->Set_AnimDuration(Human_boy::duration);
	}
	else return;

	// NPC - Walker (Human girl)
	pNew = static_cast<CNpc*>(CObjPoolMgr::GetInstance()->use_Object(L"NPC"));

	if (pNew)
	{
		/* NPC의 정보 초기화 */
		pNew->Ready_NPC(_vec3(100.13f, 0.f, 138.62f), _vec3(0.f, -180.f, 0.f), NPC_MOVE, NPC_HUMAN_GIRL, STAGE_VELIKA);
		pNew->Set_NumAnimation(Human_girl::NUM_ANIMATION);
		pNew->Set_AnimDuration(Human_girl::duration);
	}
	else return;

	// NPC - Walker (Highelf girl)
	pNew = static_cast<CNpc*>(CObjPoolMgr::GetInstance()->use_Object(L"NPC"));

	if (pNew)
	{
		/* NPC의 정보 초기화 */
		pNew->Ready_NPC(_vec3(142.43f, 0.f, 125.3f), _vec3(0.f, 0.f, 0.f), NPC_MOVE, NPC_HIGHELF_GIRL, STAGE_VELIKA);
		pNew->Set_NumAnimation(Highelf_girl::NUM_ANIMATION);
		pNew->Set_AnimDuration(Highelf_girl::duration);
	}
	else return;

	// NPC - Villigers
	pNew = static_cast<CNpc*>(CObjPoolMgr::GetInstance()->use_Object(L"NPC"));

	if (pNew)
	{
		/* NPC의 정보 초기화 */
		pNew->Ready_NPC(_vec3(56.30f, 0.f, 83.18f), _vec3(0.f, 15.f, 0.f), NPC_NONMOVE, NPC_VILLAGERS, STAGE_VELIKA);
		pNew->Set_NumAnimation(NPC_Villagers::NUM_ANIMATION);
		pNew->Set_AnimDuration(NPC_Villagers::duration);
	}
	else return;
	
	// NPC - Villigers
	pNew = static_cast<CNpc*>(CObjPoolMgr::GetInstance()->use_Object(L"NPC"));

	if (pNew)
	{
		/* NPC의 정보 초기화 */
		pNew->Ready_NPC(_vec3(79.52f, 0.f, 57.38f), _vec3(0.f, 0.f, 0.f), NPC_NONMOVE, NPC_VILLAGERS, STAGE_VELIKA);
		pNew->Set_NumAnimation(NPC_Villagers::NUM_ANIMATION);
		pNew->Set_AnimDuration(NPC_Villagers::duration);
	}
	else return;

	// NPC - Villigers
	pNew = static_cast<CNpc*>(CObjPoolMgr::GetInstance()->use_Object(L"NPC"));

	if (pNew)
	{
		/* NPC의 정보 초기화 */
		pNew->Ready_NPC(_vec3(189.77f, 0.f, 94.47f), _vec3(0.f, -75.f, 0.f), NPC_NONMOVE, NPC_VILLAGERS, STAGE_VELIKA);
		pNew->Set_NumAnimation(NPC_Villagers::NUM_ANIMATION);
		pNew->Set_AnimDuration(NPC_Villagers::duration);
	}
	else return;

	// NPC - Children (Human Boy)
	pNew = static_cast<CNpc*>(CObjPoolMgr::GetInstance()->use_Object(L"NPC"));
	
	if (pNew)
	{
		/* NPC의 정보 초기화 */
		pNew->Ready_NPC(_vec3(109.4f, 0.f, 83.9f), _vec3(0.f, -180.f, 0.f), NPC_NONMOVE, NPC_HUMAN_BOY, STAGE_VELIKA);
		pNew->Set_NumAnimation(Human_boy::NUM_ANIMATION);
		pNew->Set_AnimDuration(Human_boy::duration);
	}
	else return;

	// NPC - Children (Human girl)
	pNew = static_cast<CNpc*>(CObjPoolMgr::GetInstance()->use_Object(L"NPC"));

	if (pNew)
	{
		/* NPC의 정보 초기화 */
		pNew->Ready_NPC(_vec3(109.6f, 0.f, 81.87f), _vec3(0.f, 0.f, 0.f), NPC_NONMOVE, NPC_HUMAN_GIRL, STAGE_VELIKA);
		pNew->Set_NumAnimation(Human_girl::NUM_ANIMATION);
		pNew->Set_AnimDuration(Human_girl::duration);
	}
	else return;

	// NPC - Children (Human girl)
	pNew = static_cast<CNpc*>(CObjPoolMgr::GetInstance()->use_Object(L"NPC"));

	if (pNew)
	{
		/* NPC의 정보 초기화 */
		pNew->Ready_NPC(_vec3(91.13f, 0.f, 104.78f), _vec3(0.f, -180.f, 0.f), NPC_NONMOVE, NPC_HUMAN_GIRL, STAGE_VELIKA);
		pNew->Set_NumAnimation(Human_girl::NUM_ANIMATION);
		pNew->Set_AnimDuration(Human_girl::duration);
	}
	else return;

	// NPC - Children (Highelf girl)
	pNew = static_cast<CNpc*>(CObjPoolMgr::GetInstance()->use_Object(L"NPC"));

	if (pNew)
	{
		/* NPC의 정보 초기화 */
		pNew->Ready_NPC(_vec3(91.33f, 0.f, 103.3f), _vec3(0.f, 0.f, 0.f), NPC_NONMOVE, NPC_HIGHELF_GIRL, STAGE_VELIKA);
		pNew->Set_NumAnimation(Highelf_girl::NUM_ANIMATION);
		pNew->Set_AnimDuration(Highelf_girl::duration);
	}
	else return;

	// NPC - Children (Highelf girl)
	pNew = static_cast<CNpc*>(CObjPoolMgr::GetInstance()->use_Object(L"NPC"));

	if (pNew)
	{
		/* NPC의 정보 초기화 */
		pNew->Ready_NPC(_vec3(176.3f, 0.f, 118.5f), _vec3(0.f, -180.f, 0.f), NPC_NONMOVE, NPC_HIGHELF_GIRL, STAGE_VELIKA);
		pNew->Set_NumAnimation(Highelf_girl::NUM_ANIMATION);
		pNew->Set_AnimDuration(Highelf_girl::duration);
	}
	else return;

	// NPC - Children (Human Boy)
	pNew = static_cast<CNpc*>(CObjPoolMgr::GetInstance()->use_Object(L"NPC"));

	if (pNew)
	{
		/* NPC의 정보 초기화 */
		pNew->Ready_NPC(_vec3(177.821f, 0.f, 118.03f), _vec3(0.f, -90.f, 0.f), NPC_NONMOVE, NPC_HUMAN_BOY, STAGE_VELIKA);
		pNew->Set_NumAnimation(Human_boy::NUM_ANIMATION);
		pNew->Set_AnimDuration(Human_boy::duration);
	}
	else return;

	// NPC - Children (Human Boy)
	pNew = static_cast<CNpc*>(CObjPoolMgr::GetInstance()->use_Object(L"NPC"));

	if (pNew)
	{
		/* NPC의 정보 초기화 */
		pNew->Ready_NPC(_vec3(177.3f, 0.f, 116.5f), _vec3(0.f, 0.f, 0.f), NPC_NONMOVE, NPC_HUMAN_BOY, STAGE_VELIKA);
		pNew->Set_NumAnimation(Human_boy::NUM_ANIMATION);
		pNew->Set_AnimDuration(Human_boy::duration);
	}
	else return;

	// NPC - Children (Human Boy)
	pNew = static_cast<CNpc*>(CObjPoolMgr::GetInstance()->use_Object(L"NPC"));

	if (pNew)
	{
		/* NPC의 정보 초기화 */
		pNew->Ready_NPC(_vec3(193.17f, 0.f, 68.65f), _vec3(0.f, -180.f, 0.f), NPC_NONMOVE, NPC_HUMAN_BOY, STAGE_VELIKA);
		pNew->Set_NumAnimation(Human_boy::NUM_ANIMATION);
		pNew->Set_AnimDuration(Human_boy::duration);
	}
	else return;

	// NPC - Children (Highelf girl)
	pNew = static_cast<CNpc*>(CObjPoolMgr::GetInstance()->use_Object(L"NPC"));

	if (pNew)
	{
		/* NPC의 정보 초기화 */
		pNew->Ready_NPC(_vec3(192.98f, 0.f, 66.44f), _vec3(0.f, 0.f, 0.f), NPC_NONMOVE, NPC_HIGHELF_GIRL, STAGE_VELIKA);
		pNew->Set_NumAnimation(Highelf_girl::NUM_ANIMATION);
		pNew->Set_AnimDuration(Highelf_girl::duration);
	}
	else return;

	// NPC - Children (Human girl)
	pNew = static_cast<CNpc*>(CObjPoolMgr::GetInstance()->use_Object(L"NPC"));

	if (pNew)
	{
		/* NPC의 정보 초기화 */
		pNew->Ready_NPC(_vec3(192.08f, 0.f, 67.51f), _vec3(0.f, 90.f, 0.f), NPC_NONMOVE, NPC_HUMAN_GIRL, STAGE_VELIKA);
		pNew->Set_NumAnimation(Human_girl::NUM_ANIMATION);
		pNew->Set_AnimDuration(Human_girl::duration);
	}
	else return;

	// NPC - Children (Human Boy)
	pNew = static_cast<CNpc*>(CObjPoolMgr::GetInstance()->use_Object(L"NPC"));

	if (pNew)
	{
		/* NPC의 정보 초기화 */
		pNew->Ready_NPC(_vec3(123.95f, 0.f, 81.344f), _vec3(0.f, -180.f, 0.f), NPC_NONMOVE, NPC_HUMAN_BOY, STAGE_VELIKA);
		pNew->Set_NumAnimation(Human_boy::NUM_ANIMATION);
		pNew->Set_AnimDuration(Human_boy::duration);
	}
	else return;

	// NPC - Children (Highelf girl)
	pNew = static_cast<CNpc*>(CObjPoolMgr::GetInstance()->use_Object(L"NPC"));

	if (pNew)
	{
		/* NPC의 정보 초기화 */
		pNew->Ready_NPC(_vec3(123.73f, 0.f, 79.61f), _vec3(0.f, 0.f, 0.f), NPC_NONMOVE, NPC_HIGHELF_GIRL, STAGE_VELIKA);
		pNew->Set_NumAnimation(Highelf_girl::NUM_ANIMATION);
		pNew->Set_AnimDuration(Highelf_girl::duration);
	}
	else return;

	// NPC - Children (Human girl)
	pNew = static_cast<CNpc*>(CObjPoolMgr::GetInstance()->use_Object(L"NPC"));

	if (pNew)
	{
		/* NPC의 정보 초기화 */
		pNew->Ready_NPC(_vec3(122.52f, 0.f, 80.5f), _vec3(0.f, 90.f, 0.f), NPC_NONMOVE, NPC_HUMAN_GIRL, STAGE_VELIKA);
		pNew->Set_NumAnimation(Human_girl::NUM_ANIMATION);
		pNew->Set_AnimDuration(Human_girl::duration);
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
	CObjMgr::GetInstance()->Create_StageBeachMonster();

	//CMonster* pNew = nullptr;
	//int s_num = -1;

	/* ______________________________________________________________________________________________________*/
	/*											SCENE - TEST												 */
	/* ______________________________________________________________________________________________________*/

	// TEST MONSTER - crab
	//pNew = static_cast<CMonster*>(CObjPoolMgr::GetInstance()->use_Object(L"MONSTER"));

	//if (pNew)
	//{
	//	/* NPC의 정보 초기화 */
	//	pNew->Ready_Monster(_vec3(158.0f, 0.f, 55.0f), _vec3(0.f, 0.0f, 0.f), MON_NORMAL, MON_CRAB, STAGE_VELIKA, 100, 1, 0, 0.5f);
	//	pNew->Set_NumAnimation(Crab::NUM_ANIMATION);
	//	pNew->Set_AnimDuration(Crab::duration);
	//}
	//else return;

	// TEST MONSTER - monkey
	//pNew = static_cast<CMonster*>(CObjPoolMgr::GetInstance()->use_Object(L"MONSTER"));
	//
	//if (pNew)
	//{
	//	/* NPC의 정보 초기화 */
	//	pNew->Ready_Monster(_vec3(155.0f, 0.f, 55.0f), _vec3(0.f, 0.0f, 0.f), MON_NORMAL, MON_MONKEY, STAGE_VELIKA, 100, 1, 0, 0.5f);
	//	pNew->Set_NumAnimation(Monkey::NUM_ANIMATION);
	//	pNew->Set_AnimDuration(Monkey::duration);
	//}
	//else return;

	// TEST MONSTER - cloder
	//pNew = static_cast<CMonster*>(CObjPoolMgr::GetInstance()->use_Object(L"MONSTER"));

	//if (pNew)
	//{
	//	/* NPC의 정보 초기화 */
	//	pNew->Ready_Monster(_vec3(152.0f, 0.f, 55.0f), _vec3(0.f, 0.0f, 0.f), MON_NORMAL, MON_CLODER, STAGE_VELIKA, 100, 1, 0, 0.5f);
	//	pNew->Set_NumAnimation(Cloder::NUM_ANIMATION);
	//	pNew->Set_AnimDuration(Cloder::duration);
	//}
	//else return;

	//// TEST MONSTER - drowned sailor
	//pNew = static_cast<CMonster*>(CObjPoolMgr::GetInstance()->use_Object(L"MONSTER"));
	//
	//if (pNew)
	//{
	//	/* NPC의 정보 초기화 */
	//	pNew->Ready_Monster(_vec3(145.0f, 0.f, 55.0f), _vec3(0.f, 0.0f, 0.f), MON_NORMAL, MON_SAILOR, STAGE_VELIKA, 100, 1, 0, 0.5f);
	//	pNew->Set_NumAnimation(DrownedSailor::NUM_ANIMATION);
	//	pNew->Set_AnimDuration(DrownedSailor::duration);
	//}
	//else return;

	//// TEST MONSTER - Giant Beetle
	//pNew = static_cast<CMonster*>(CObjPoolMgr::GetInstance()->use_Object(L"MONSTER"));

	//if (pNew)
	//{
	//	/* NPC의 정보 초기화 */
	//	pNew->Ready_Monster(_vec3(135.0f, 0.f, 55.0f), _vec3(0.f, 0.0f, 0.f), MON_NORMAL, MON_GBEETLE, STAGE_VELIKA, 100, 1, 0, 0.5f);
	//	pNew->Set_NumAnimation(GiantBeetle::NUM_ANIMATION);
	//	pNew->Set_AnimDuration(GiantBeetle::duration);
	//}
	//else return;

	//// TEST MONSTER - Giant Monkey
	//pNew = static_cast<CMonster*>(CObjPoolMgr::GetInstance()->use_Object(L"MONSTER"));

	//if (pNew)
	//{
	//	/* NPC의 정보 초기화 */
	//	pNew->Ready_Monster(_vec3(127.0f, 0.f, 55.0f), _vec3(0.f, 0.0f, 0.f), MON_NORMAL, MON_GMONKEY, STAGE_VELIKA, 100, 1, 0, 0.5f);
	//	pNew->Set_NumAnimation(GiantMonkey::NUM_ANIMATION);
	//	pNew->Set_AnimDuration(GiantMonkey::duration);
	//}
	//else return;

	// TEST MONSTER - Crafty Arachne
	//pNew = static_cast<CMonster*>(CObjPoolMgr::GetInstance()->use_Object(L"MONSTER"));
	//
	//if (pNew)
	//{
	//	/* NPC의 정보 초기화 */
	//	pNew->Ready_Monster(_vec3(122.0f, 0.f, 55.0f), _vec3(0.f, 0.0f, 0.f), MON_NORMAL, MON_ARACHNE, STAGE_VELIKA, 100, 1, 0, 0.5f);
	//	pNew->Set_NumAnimation(CraftyArachne::NUM_ANIMATION);
	//	pNew->Set_AnimDuration(CraftyArachne::duration);
	//}
	//else return;
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
		pNew->m_iLevel	= INIT_LEV;
		pNew->m_iHp		= INIT_HP;
		pNew->m_iMaxHp	= INIT_HP;
		pNew->m_iMp		= INIT_MP;
		pNew->m_iMaxMp	= INIT_MP;
		pNew->m_iExp	= INIT_EXP;
		pNew->m_iMaxExp = INIT_MAXEXP;
		pNew->m_iMinAtt = INIT_MINATT;
		pNew->m_iMaxAtt	= INIT_MAXATT;
		pNew->m_fSpd	= INIT_SPEED;

		pNew->m_vPos	= _vec3(STAGE_VELIKA_X, 0.f, STAGE_VELIKA_Z);
		pNew->m_vDir	= _vec3(0.f, 0.f, 1.f);
		pNew->m_vAngle	= _vec3(0.f, 0.f, 0.f);

		pNew->m_chStageId = STAGE_VELIKA;

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
#ifdef TEST
						cout << obj_num << "님에게" << id << "님 퇴장을 전송" << endl;
#endif					
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
	pPlayer->m_chStageId = STAGE_VELIKA;
	pPlayer->view_list.clear();
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

		case OPMODE::OP_ACTIVE_NPC:
		{
			CNpc* pNpc = static_cast<CNpc*>(CObjMgr::GetInstance()->Get_GameObject(L"NPC", key));
			if (nullptr == pNpc) return;

			pNpc->active_npc();
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

		case OPMODE::OP_MODE_REGEN_MONSTER:
		{
			CMonster* pMonster = static_cast<CMonster*>(CObjMgr::GetInstance()->Get_GameObject(L"MONSTER", key));
			if (nullptr == pMonster) return;

			pMonster->Set_Finish_Regen();
			pMonster->nonActive_monster();
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

			/* MONSTER UPDATE */
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

			/* NPC UPDATE */
			auto iter_begin_npc = CObjMgr::GetInstance()->Get_OBJLIST(L"NPC")->begin();
			auto iter_end_npc = CObjMgr::GetInstance()->Get_OBJLIST(L"NPC")->end();

			for (; iter_begin_npc != iter_end_npc;)
			{
				int iEvent = static_cast<CNpc*>(iter_begin_npc->second)->Update_NPC(g_pTimerTimeDelta->Get_TimeDelta());

				if (DEAD_OBJ == iEvent)
				{
					CObjPoolMgr::GetInstance()->return_Object(L"NPC", iter_begin_npc->second);
					CObjMgr::GetInstance()->Delete_GameObject(L"NPC", iter_begin_npc->second);

					iter_begin_npc = CObjMgr::GetInstance()->Get_OBJLIST(L"NPC")->begin();
					iter_end_npc = CObjMgr::GetInstance()->Get_OBJLIST(L"NPC")->end();
				}
				else
					++iter_begin_npc;
			}

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
