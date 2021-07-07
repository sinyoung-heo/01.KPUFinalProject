#include "stdafx.h"
#include "Player.h"
#include "Npc.h"
#include "Monster.h"
#include "Ai.h"

/* IOCP SERVER ���� ����*/
HANDLE g_hIocp;
SOCKET g_hListenSock;
OVER_EX g_accept_over;

/* OBJECT ���� ���� */
mutex g_id_lock;

/* TIMER ���� ���� */
priority_queue<event_type> g_timer_queue;
mutex g_timer_lock;

/* GameLogic ���� ���� */
CTimer* g_pTimerFPS;
CTimer* g_pTimerTimeDelta;

bool g_bIsGameEnd = false;

/*==============================================================�Լ� �����========================================================================*/
void Ready_ServerManager();			// ���� �Ŵ��� �ʱ�ȭ
void Ready_Server();				// ���� ���� ���� �ʱ�ȭ
void Release_Server();				// ���� ����

void Initialize_NPC();				// Create Stage Velika NPC 
void Delete_NPC();					// Delete All NPC
void Delete_AI();

void Initialize_Monster();			// Create Monster(test)
void Delete_Monster();				// Delete All MONSTER

void add_new_client(SOCKET ns);		// ���ο� ���� ���� �Լ�
void disconnect_client(const int& id);		// ���� ���� ���� �Լ�

void time_worker();					// Timer Thread Enter Function
void worker_thread();				// Worker Thread Enter Function
void gameLogic_worker();			// Game Logic Thread Enter Function

/*==============================================================MAIN CODE=========================================================================*/
int main()
{
	srand(unsigned int(time(nullptr)));
	Ready_Server();

	/* Time Thread ���� */
	thread time_thread{ time_worker };

	thread gameLogic_thread{ gameLogic_worker };
	
	/* Worker Threads ���� - 4�� */
	vector<thread> vecWorker;
	for (int i = 0; i < SERVER_CORE; ++i)
		vecWorker.emplace_back(worker_thread);

	/* Worker Threads �Ҹ� */
	for (auto& th : vecWorker)
		th.join();

	/* Time Thread �Ҹ� */
	time_thread.join();

	/* Game Logic Thread �Ҹ� */
	gameLogic_thread.join();

	Release_Server();
	closesocket(g_hListenSock);
	WSACleanup();

	return NO_EVENT;
}
/*==============================================================�Լ� ���Ǻ�========================================================================*/
void Ready_ServerManager()
{
	// INIT OBJECT POOL MANAGER
	CSectorMgr::GetInstance();
	CObjPoolMgr::GetInstance()->Init_ObjPoolMgr();	
	CObjMgr::GetInstance()->Init_ObjMgr();
	CDBMgr::GetInstance()->Ready_DB();
	CItemMgr::GetInstance()->Ready_Item();
	CNaviMesh::GetInstance()->Ready_NaviMesh(L"../../../FrameDirectX12/Bin/ToolData/StageVelika_NaviMesh.navimeshcellinfo");
	CNaviMesh_Beach::GetInstance()->Ready_NaviMesh(L"../../../FrameDirectX12/Bin/ToolData/StageBeach_NaviMesh3.navimeshcellinfo");	
	CNaviMesh_Winter::GetInstance()->Ready_NaviMesh(L"../../../FrameDirectX12/Bin/ToolData/StageWendigo_NaviMesh.navimeshcellinfo");
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

	/* WINSOCK �ʱ�ȭ */
	WSADATA wsaData;
	if (0 != WSAStartup(MAKEWORD(2, 2), &wsaData))
	{
		WSACleanup();
		return;
	}

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
	CItemMgr::GetInstance()->DestroyInstance();
	CNaviMesh::GetInstance()->DestroyInstance();
	CNaviMesh_Beach::GetInstance()->DestroyInstance();
	CNaviMesh_Winter::GetInstance()->DestroyInstance();
	CCollisionMgr::GetInstance()->DestroyInstance();

	closesocket(g_hListenSock);
	WSACleanup();
	exit(1);
}

void Initialize_NPC()
{
	CObjMgr::GetInstance()->Create_StageVelikaNPC();
	CObjMgr::GetInstance()->Create_AiPlayer();

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

void Delete_AI()
{
	auto iter_begin = CObjMgr::GetInstance()->Get_OBJLIST(L"AI")->begin();
	auto iter_end = CObjMgr::GetInstance()->Get_OBJLIST(L"AI")->end();

	for (iter_begin; iter_begin != iter_end;)
	{
		static_cast<CAi*>(iter_begin->second)->Release_AI();

		iter_begin = CObjMgr::GetInstance()->Get_OBJLIST(L"AI")->begin();
		iter_end = CObjMgr::GetInstance()->Get_OBJLIST(L"AI")->end();
	}
}

void Initialize_Monster()
{
	CObjMgr::GetInstance()->Create_StageBeachMonster();

	CMonster* pNew = nullptr;
	int s_num = -1;

	// TEST MONSTER - Giant Monkey
	//pNew = static_cast<CMonster*>(CObjPoolMgr::GetInstance()->use_Object(L"MONSTER"));

	//if (pNew)
	//{
	//	/* NPC�� ���� �ʱ�ȭ */
	//	pNew->Ready_Monster(_vec3(125.0f, 0.f, 55.0f), _vec3(0.f, 0.0f, 0.f), MON_NORMAL, MON_GMONKEY, STAGE_VELIKA, 100000, 5, 10, 0.5f);
	//	pNew->Set_NumAnimation(GiantMonkey::NUM_ANIMATION);
	//	pNew->Set_AnimDuration(GiantMonkey::duration);
	//}
	//else return;

	//// TEST MONSTER - drowned sailor
	//pNew = static_cast<CMonster*>(CObjPoolMgr::GetInstance()->use_Object(L"MONSTER"));

	//if (pNew)
	//{
	//	/* NPC�� ���� �ʱ�ȭ */
	//	pNew->Ready_Monster(_vec3(130.0f, 0.f, 55.0f), _vec3(0.f, 0.0f, 0.f), MON_NORMAL, MON_SAILOR, STAGE_VELIKA, 10000, 5, 10, 0.5f);
	//	pNew->Set_NumAnimation(DrownedSailor::NUM_ANIMATION);
	//	pNew->Set_AnimDuration(DrownedSailor::duration);
	//}
	//else return;

	//// TEST MONSTER - monkey
	//pNew = static_cast<CMonster*>(CObjPoolMgr::GetInstance()->use_Object(L"MONSTER"));
	//
	//if (pNew)
	//{
	//	/* NPC�� ���� �ʱ�ȭ */
	//	pNew->Ready_Monster(_vec3(135.0f, 0.f, 55.0f), _vec3(0.f, 0.0f, 0.f), MON_NORMAL, MON_MONKEY, STAGE_VELIKA, 10000, 5, 10, 0.5f);
	//	pNew->Set_NumAnimation(Monkey::NUM_ANIMATION);
	//	pNew->Set_AnimDuration(Monkey::duration);
	//}
	//else return;

	/* ______________________________________________________________________________________________________*/
	/*											SCENE - TEST												 */
	/* ______________________________________________________________________________________________________*/

	// TEST MONSTER - crab
	//pNew = static_cast<CMonster*>(CObjPoolMgr::GetInstance()->use_Object(L"MONSTER"));

	//if (pNew)
	//{
	//	/* NPC�� ���� �ʱ�ȭ */
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
	//	/* NPC�� ���� �ʱ�ȭ */
	//	pNew->Ready_Monster(_vec3(155.0f, 0.f, 55.0f), _vec3(0.f, 0.0f, 0.f), MON_NORMAL, MON_MONKEY, STAGE_VELIKA, 100, 1, 0, 0.5f);
	//	pNew->Set_NumAnimation(Monkey::NUM_ANIMATION);
	//	pNew->Set_AnimDuration(Monkey::duration);
	//}
	//else return;

	// TEST MONSTER - cloder
	//pNew = static_cast<CMonster*>(CObjPoolMgr::GetInstance()->use_Object(L"MONSTER"));

	//if (pNew)
	//{
	//	/* NPC�� ���� �ʱ�ȭ */
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
	//	/* NPC�� ���� �ʱ�ȭ */
	//	pNew->Ready_Monster(_vec3(145.0f, 0.f, 55.0f), _vec3(0.f, 0.0f, 0.f), MON_NORMAL, MON_SAILOR, STAGE_VELIKA, 100, 1, 0, 0.5f);
	//	pNew->Set_NumAnimation(DrownedSailor::NUM_ANIMATION);
	//	pNew->Set_AnimDuration(DrownedSailor::duration);
	//}
	//else return;

	//// TEST MONSTER - Giant Beetle
	//pNew = static_cast<CMonster*>(CObjPoolMgr::GetInstance()->use_Object(L"MONSTER"));

	//if (pNew)
	//{
	//	/* NPC�� ���� �ʱ�ȭ */
	//	pNew->Ready_Monster(_vec3(135.0f, 0.f, 55.0f), _vec3(0.f, 0.0f, 0.f), MON_NORMAL, MON_GBEETLE, STAGE_VELIKA, 100, 1, 0, 0.5f);
	//	pNew->Set_NumAnimation(GiantBeetle::NUM_ANIMATION);
	//	pNew->Set_AnimDuration(GiantBeetle::duration);
	//}
	//else return;

	//// TEST MONSTER - Giant Monkey
	//pNew = static_cast<CMonster*>(CObjPoolMgr::GetInstance()->use_Object(L"MONSTER"));

	//if (pNew)
	//{
	//	/* NPC�� ���� �ʱ�ȭ */
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
	//	/* NPC�� ���� �ʱ�ȭ */
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

	/* �������� ������ ������ ��ȣ ���� */
	CPlayer* pNew = static_cast<CPlayer*>(CObjPoolMgr::GetInstance()->use_Object(L"PLAYER"));
	if (!pNew) return;
	
	g_id_lock.lock();
	if (CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", pNew->m_sNum) == nullptr)
		s_num = pNew->m_sNum;
	else
		CObjPoolMgr::GetInstance()->return_Object(L"PLAYER", pNew);
	g_id_lock.unlock();

	// �ִ� ���� �ο� �ʰ� ����
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
	
		/* ���� ������ ������ ���� �ʱ�ȭ */
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

		pNew->m_type			= PC_ARCHER;
		pNew->m_chWeaponType	= Bow27_SM;
		pNew->m_chStageId		= STAGE_VELIKA;
		pNew->m_bIsAttackStance	= false;
		pNew->m_bIsPartyState	= false;
		pNew->m_iPartyNumber	= INIT_PARTY_NUMBER;

		pNew->m_iLevel	    = INIT_LEV;
		pNew->m_iHp		    = INIT_HP;
		pNew->m_iMaxHp	    = INIT_HP;
		pNew->m_iMp		    = INIT_MP;
		pNew->m_iMaxMp	    = INIT_MP;
		pNew->m_iExp	    = INIT_EXP;
		pNew->m_iMaxExp     = INIT_MAXEXP;
		pNew->m_iMinAtt     = INIT_MINATT;
		pNew->m_iMaxAtt	    = INIT_MAXATT;
		pNew->m_iMoney		= INIT_MONEY;
		pNew->m_fSpd	    = INIT_SPEED;

		pNew->m_vPos	= _vec3(STAGE_VELIKA_X, 0.f, STAGE_VELIKA_Z);
		pNew->m_vDir	= _vec3(0.f, 0.f, 1.f);
		pNew->m_vAngle	= _vec3(0.f, 0.f, 0.f);

		CSectorMgr::GetInstance()->Enter_ClientInSector((int)s_num, (int)(pNew->m_vPos.z / SECTOR_SIZE), (int)(pNew->m_vPos.x / SECTOR_SIZE));
		CObjMgr::GetInstance()->Add_GameObject(L"PLAYER", pNew, (int)s_num);

		/* �ش� Ŭ���̾�Ʈ ������ IOCP�� ��� */
		CreateIoCompletionPort(reinterpret_cast<HANDLE>(ns), g_hIocp, s_num, 0);

		DWORD flags = 0;
		int ret = -1;

		/* �ش� Ŭ���̾�Ʈ�κ��� ������ RECV */
		pNew->Get_ClientLock().lock();
		// ���� �ش� Ŭ���̾�Ʈ�� ���� ���� �˻�
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

	/* ���� ���� ������ ���� ó�� �Ϸ� -> �ٽ� �񵿱� ACCEPT */
	SOCKET cSocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	g_accept_over.op_mode = OP_MODE_ACCEPT;
	g_accept_over.wsa_buf.len = static_cast<ULONG> (cSocket);
	ZeroMemory(&g_accept_over.wsa_over, sizeof(&g_accept_over.wsa_over));
	AcceptEx(g_hListenSock, cSocket, g_accept_over.iocp_buf, 0, 32, 32, NULL, &g_accept_over.wsa_over);
}

void disconnect_client(const int& id)
{
	CPlayer* pPlayer = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", id));

	if (pPlayer == nullptr) return;

	/* sector���� �ش� �÷��̾� ����� */
	CSectorMgr::GetInstance()->Leave_ClientInSector(id, (int)(pPlayer->m_vPos.z / SECTOR_SIZE), (int)(pPlayer->m_vPos.x / SECTOR_SIZE));

	/* ��Ƽ�� ���ԵǾ� �ִٸ� ��Ƽ Ż�� */
	if (pPlayer->m_bIsPartyState == true)
		process_leave_party(id);

	/* �ش� �÷��̾ ��ϵǾ� �ִ� ���� ���� �����鿡�� ���� ���Ḧ �˸� */
	unordered_set<pair<int, int>> nearSector;
	nearSector.reserve(NEAR_SECTOR);
	CSectorMgr::GetInstance()->Get_NearSectorIndex(&nearSector, (int)pPlayer->m_vPos.x, (int)pPlayer->m_vPos.z);

	// ���� ���� ��ȸ
	for (auto& s : nearSector)
	{
		// ���� ���� ���� Ÿ �������� �ִ��� �˻�
		if (!(CSectorMgr::GetInstance()->Get_SectorList()[s.first][s.second].Get_ObjList().empty()))
		{
			// Ÿ ������ ���� ��ȣ ����
			for (auto obj_num : CSectorMgr::GetInstance()->Get_SectorList()[s.first][s.second].Get_ObjList())
			{
				/* ���� �����鿡�Ը� ��Ŷ�� ���� (NPC ����) */
				if (false == CObjMgr::GetInstance()->Is_Player(obj_num)) continue;		
				// '��'���� ��Ŷ ���� X
				if (obj_num == id) continue;

				CPlayer* pOther = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", obj_num));
				
				// ������ �����鿡�Ը� ���� ���Ḧ �˸�
				if (pOther->Get_IsConnected())
				{
					/* Ÿ ������ �þ� ��� ���� '��'�� �ִٸ� ����� */
					pOther->v_lock.lock();
					if (0 != pOther->view_list.count(id))
					{
						pOther->view_list.erase(id);
						pOther->v_lock.unlock();

						/* Ÿ �������� ���� ���� ��Ŷ ���� */
#ifdef TEST
						cout << obj_num << "�Կ���" << id << "�� ������ ����" << endl;
#endif					
						send_leave_packet(obj_num, id);
					}
					else pOther->v_lock.unlock();					
				}
			}
		}
	}

	pPlayer->Get_ClientLock().lock();
	pPlayer->logout_equipment();
	CDBMgr::GetInstance()->Update_stat_DB(id);
	
	CObjPoolMgr::GetInstance()->return_Object(L"PLAYER", pPlayer);
	CObjMgr::GetInstance()->Delete_GameObject(L"PLAYER", pPlayer);
	pPlayer->Set_IsConnected(false);
	closesocket(pPlayer->m_sock);
	pPlayer->m_sock				= 0;
	pPlayer->m_vPos				= _vec3(0.f, 0.f, 0.f);
	pPlayer->m_vDir				= _vec3(0.f, 0.f, 0.f);
	pPlayer->m_ID[0]			= 0;
	pPlayer->m_type				= 0;
	pPlayer->m_chStageId		= STAGE_VELIKA;
	pPlayer->m_bIsPartyState	= false;
	pPlayer->m_iPartyNumber		= INIT_PARTY_NUMBER;
	pPlayer->m_iMoney			= INIT_MONEY;
	pPlayer->view_list.clear();
	pPlayer->Release_Inventory();
	pPlayer->Release_Equipment();
	pPlayer->Get_ClientLock().unlock();

	if (CObjMgr::GetInstance()->Get_OBJLIST(L"PLAYER")->size() <= 0)
	{
		g_bIsGameEnd = true;
		Delete_NPC();
		Delete_Monster();
		Delete_AI();
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
	// [worker_thread�� �ϴ� �� - MAIN]
	// ���� �����带 IOCP thread pool�� ���  => GQCS
	// iocp�� ó���� �ñ� �Ϸ�� I/O �����͸� ������ => GQCS
	// ���� �����͸� ó��

	while (true)
	{
		DWORD io_size;				// ���� I/O ũ��
		int key;					// �������� KEY
		ULONG_PTR iocp_key;			// �������� KEY
		WSAOVERLAPPED* lpover;		// IOCP�� OVERLAPPED ����� I/O

		/* GQCS => I/O ������ */
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

		/* <I/O ó�� �۾�> */
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
				/* Packet ������ */
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

			/* AI PLAYER UPDATE */
			auto iter_begin_ai = CObjMgr::GetInstance()->Get_OBJLIST(L"AI")->begin();
			auto iter_end_ai = CObjMgr::GetInstance()->Get_OBJLIST(L"AI")->end();

			for (; iter_begin_ai != iter_end_ai;)
			{
				int iEvent = static_cast<CAi*>(iter_begin_ai->second)->Update_AI(g_pTimerTimeDelta->Get_TimeDelta());

				if (DEAD_OBJ == iEvent)
				{
					CObjPoolMgr::GetInstance()->return_Object(L"AI", iter_begin_ai->second);
					CObjMgr::GetInstance()->Delete_GameObject(L"AI", iter_begin_ai->second);

					iter_begin_ai = CObjMgr::GetInstance()->Get_OBJLIST(L"AI")->begin();
					iter_end_ai = CObjMgr::GetInstance()->Get_OBJLIST(L"AI")->end();
				}
				else
					++iter_begin_ai;
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
