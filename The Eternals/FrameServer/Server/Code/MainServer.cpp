#include "stdafx.h"
#include "Player.h"
#include "Npc.h"

/* IOCP SERVER ���� ����*/
HANDLE g_hIocp;
SOCKET g_hListenSock;
OVER_EX g_accept_over;

/* OBJECT ���� ���� */
mutex g_id_lock;

/* TIMER ���� ���� */
priority_queue<event_type> g_timer_queue;
mutex g_timer_lock;

/*==============================================================�Լ� �����========================================================================*/
void Ready_ServerManager();			// ���� �Ŵ��� �ʱ�ȭ
void Ready_Server();				// ���� ���� ���� �ʱ�ȭ
void Release_Server();				// ���� ����

void Initialize_NPC();
void Delete_NPC();
void add_new_client(SOCKET ns);		// ���ο� ���� ���� �Լ�
void disconnect_client(int id);		// ���� ���� ���� �Լ�

void time_worker();					// Timer Thread Enter Function
void worker_thread();				// Worker Thread Enter Function

/*==============================================================MAIN CODE=========================================================================*/
int main()
{
	srand(unsigned int(time(nullptr)));
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
	CNaviMesh::GetInstance()->Ready_NaviMesh(L"../../../FrameDirectX12/Bin/ToolData/TestNavigationCell.navimeshcellinfo");
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

	/* �������� NPC�� ������ ��ȣ ���� */
	for (int i = NPC_NUM_START; i < NPC_NUM_START + MAX_NPC; ++i)
	{
		pNew = static_cast<CNpc*>(CObjPoolMgr::GetInstance()->use_Object(L"NPC"));

		if (pNew)
		{
			pNew->m_sNum += NPC_NUM_START;
			s_num = pNew->m_sNum;
		}
		else continue;

		/* NPC�� ���� �ʱ�ȭ */	
		pNew->Set_IsConnected(true);
		pNew->Set_IsDead(false);	
		sprintf_s(pNew->m_ID, "NPC%d", s_num);
	
		pNew->m_vPos = _vec3((rand() % 100) * 1.f + 1000.f, (rand() % 100) * 1.f + 500.f, 0.f);
		pNew->m_vDir = _vec3(0.f, 0.f, 1.f);
		pNew->m_vAngle = _vec3(0.f, 0.f, 0.f);
		pNew->m_type = '1';
		pNew->spd = 10.f;
		pNew->m_status = STATUS::ST_NONACTIVE;
		
		CSectorMgr::GetInstance()->Enter_ClientInSector(s_num, (int)(pNew->m_vPos.z / SECTOR_SIZE), (int)(pNew->m_vPos.x / SECTOR_SIZE));
		CObjMgr::GetInstance()->Add_GameObject(L"NPC", pNew, s_num);

#ifdef TEST
		cout << "NPC [" << s_num << "] Initialize Finish.\n";
#endif
	}

#ifdef TEST
	cout << "NPC Initialize Finish.\n";
#endif
}

void Delete_NPC()
{
	CNpc* pNPC = nullptr;

	for (int i = 1; i < MAX_NPC + 1; ++i)
	{
		pNPC = static_cast<CNpc*>(CObjMgr::GetInstance()->Get_GameObject(L"NPC", i));
		CObjPoolMgr::GetInstance()->return_Object(L"NPC", pNPC);
		CObjMgr::GetInstance()->Delete_GameObject(L"NPC", pNPC);
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

		pNew->m_type = '0';
		pNew->level = 1;
		pNew->Hp = 100;
		pNew->maxHp = 100;
		pNew->Exp = 0;
		pNew->maxExp = 100;
		pNew->att = 10;
		pNew->spd = 10.f;
		//pNew->m_vPos = _vec3((rand() % 10) * 1.f + 30.f, 0.f, (rand() % 10) * 1.f + 20.f);
		pNew->m_vPos = _vec3(25.0f, 0.f, 20.0f);
		pNew->m_vDir = _vec3(0.f, 0.f, 1.f);
		pNew->m_vAngle = _vec3(0.f, 0.f, 0.f);

		CSectorMgr::GetInstance()->Enter_ClientInSector((int)s_num, (int)(pNew->m_vPos.z / SECTOR_SIZE), (int)(pNew->m_vPos.x / SECTOR_SIZE));
		CObjMgr::GetInstance()->Add_GameObject(L"PLAYER",pNew, (int)s_num);

		/* �ش� Ŭ���̾�Ʈ ������ IOCP�� ��� */
		CreateIoCompletionPort(reinterpret_cast<HANDLE>(ns), g_hIocp, s_num, 0);

		DWORD flags = 0;
		int ret;

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

void disconnect_client(int id)
{
	CPlayer* pPlayer = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", id));

	if (pPlayer == nullptr) return;

	/* sector���� �ش� �÷��̾� ����� */
	CSectorMgr::GetInstance()->Leave_ClientInSector(id, (int)(pPlayer->m_vPos.z / SECTOR_SIZE), (int)(pPlayer->m_vPos.x / SECTOR_SIZE));

	/* �ش� �÷��̾ ��ϵǾ� �ִ� ���� ���� �����鿡�� ���� ���Ḧ �˸� */
	unordered_set<pair<int, int>> nearSector;
	nearSector.reserve(5);
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
						send_leave_packet(obj_num, id);
					}
					else pOther->v_lock.unlock();					
				}
			}
		}
	}

	pPlayer->Get_ClientLock().lock();

#ifndef DUMMY
	// DB ���� ���� �� ����
	CDBMgr::GetInstance()->Update_stat_DB(id);
#endif // !DUMMY

	CObjPoolMgr::GetInstance()->return_Object(L"PLAYER", pPlayer);
	CObjMgr::GetInstance()->Delete_GameObject(L"PLAYER", pPlayer);
	pPlayer->Set_IsConnected(false);
	closesocket(pPlayer->m_sock);
	pPlayer->m_sock = 0;
	pPlayer->m_vPos = _vec3(0.f, 0.f, 0.f);
	pPlayer->m_vDir = _vec3(0.f, 0.f, 0.f);
	pPlayer->m_ID[0] = 0;

	pPlayer->Get_ClientLock().unlock();

	if (CObjMgr::GetInstance()->Get_OBJLIST(L"PLAYER")->size() <= 0)
	{
		Delete_NPC();
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

		case OPMODE::OP_RANDOM_MOVE_NPC:
		{
			/* �����ؾ� �� ��: �Ʒ� �Լ� bool ���� ���� �� ��ü �÷��̾� �� �þ� ���� ���ٸ� STATUS ���� */
			random_move_npc(key);
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
