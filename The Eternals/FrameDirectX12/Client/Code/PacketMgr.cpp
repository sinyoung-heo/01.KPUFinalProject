#include "stdafx.h"
#include "PacketMgr.h"
#include "ObjectMgr.h"
#include "Management.h"
#include "Renderer.h"
#include "TestPlayer.h"
#include "TestOthers.h"
#include "NPC_Animal.h"
#include "NPC_Boy.h"
#include "NPC_Villagers.h"
#include "NPC_Merchant.h"
#include "Crab.h"
#include "Monkey.h"
#include "CloderA.h"
#include "DrownedSailor.h"
#include "DynamicCamera.h"
#include "TestColPlayer.h"
#include "TestColMonster.h"
#include "PCGladiator.h"
#include "PCOthersGladiator.h"

IMPLEMENT_SINGLETON(CPacketMgr)

CPacketMgr::CPacketMgr()
	: m_pObjectMgr(Engine::CObjectMgr::Get_Instance())
	, m_pManagement(Engine::CManagement::Get_Instance())
	, m_pRenderer(Engine::CRenderer::Get_Instance())
	, m_eCurKey(MVKEY::K_END), m_ePreKey(MVKEY::K_END)
{
	memset(m_recv_buf, 0, sizeof(m_recv_buf));
	m_packet_start = m_recv_buf;
	m_recv_start = m_recv_buf;
	m_next_recv_ptr = m_recv_buf;
}

HRESULT CPacketMgr::Ready_Server(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList)
{
	m_pGraphicDevice = pGraphicDevice;
	m_pCommandList = pCommandList;
	m_eCurKey = MVKEY::K_END;
	m_ePreKey = MVKEY::K_END;

	// Initialize Windows Socket
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);

	// Create Windows Socket
	g_hSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	/* Non-Blocking Socket���� ��ȯ */
	unsigned long ul = 1;
	ioctlsocket(g_hSocket, FIONBIO, (unsigned long*)&ul);

	return S_OK;
}

HRESULT CPacketMgr::Connect_Server()
{
	/* Socket Address Structure */
	SOCKADDR_IN sockAddr;
	memset(&sockAddr, 0, sizeof(sockAddr));

	sockAddr.sin_family = AF_INET;
	sockAddr.sin_port = htons(SERVER_PORT);
	sockAddr.sin_addr.s_addr = inet_addr(SERVER_IP);

	if (connect(g_hSocket, (SOCKADDR*)&sockAddr, sizeof(sockAddr)) == SOCKET_ERROR)
	{
		int err_no = WSAGetLastError();
		if (err_no != WSAEWOULDBLOCK)
		{
			error_display("connect : ", err_no);
			closesocket(g_hSocket);
			return E_FAIL;
		}
	}

#ifdef ERR_CHECK
	cout << "������ ������ ��û�Ͽ����ϴ�. ��ø� ��ٷ��ּ���." << endl;
#endif 

	CPacketMgr::Get_Instance()->send_login();

#ifdef ERR_CHECK
	cout << "Login packet ���ۿϷ�" << endl;
#endif 

	return S_OK;
}

void CPacketMgr::recv_packet()
{
	// Server Data Receive Version - Ring Buffer
	int iLen = MAX_BUF_SIZE - static_cast<int>(m_next_recv_ptr - m_recv_buf);
	int retval = recv(g_hSocket, reinterpret_cast<CHAR*>(m_recv_start), iLen, 0);

#ifdef ERR_CHECK
	if (retval == SOCKET_ERROR)
	{
		int state = WSAGetLastError();
		if (state != WSAEWOULDBLOCK)
		{
			error_display("recv Error : ", state);
			closesocket(g_hSocket);
		}
	}
#endif 

	// ������ ���� ����
	if (retval > 0)
	{
		// ��Ŷ ������(Version - Ring Buffer)
		Process_recv_reassembly(static_cast<size_t>(retval));
	}
}

void CPacketMgr::Process_recv_reassembly(size_t iosize)
{
	// m_packet_start		: ó���� ������ ���� �� ��ġ (= iocp_buffer)
	// m_packet_start[0]	: ó���� ��Ŷ�� ũ��
	unsigned char p_size = m_packet_start[0];

	/* ���� ��Ŷ�� ���� ������ ��ġ ���� */
	// m_recv_start			: ������ ��Ŷ�� ���� ��ġ
	m_next_recv_ptr = m_recv_start + iosize;

	// ó���� ��Ŷ ũ�⺸�� �����ִ� ������ ������ ��� ��Ŷ ó�� ����
	while (p_size <= m_next_recv_ptr - m_packet_start)
	{
		// ��Ŷ ó��
		Process_packet();

		// ��Ŷ ó�� �� ������ ������ ó�� ���� ���� ��ġ ����
		m_packet_start += p_size;

		// recv�� �����͸� ó���ߴµ� ���� ó������ ���� �����͸� ������ ���� ���� �ִ�.
		// ó���ؾ� �� ��Ŷ ũ�� ����
		if (m_packet_start < m_next_recv_ptr)
			p_size = m_packet_start[0];
		else break;
	}

	// ó������ ����(�����ִ�) �������� ���� ��ġ
	long long left_data = m_next_recv_ptr - m_packet_start;

	// recv ó�� ���� -> �ٽ� recv�� �ؾ� �Ѵ�.
	// �ٽ� recv�� �ϱ� ���� �� ������ ���� ������ Ȯ�� �� �� ������� ��� �ʱ�ȭ�Ѵ�.
	// �����ִ� ������ MIN_BUFFER ���� ������ ���� �����͸� �� ���� �� ������ �Ű��ش�.
	
	if ((MAX_BUF_SIZE - (int)(m_next_recv_ptr - m_recv_buf)) < MIN_BUF_SIZE)
	{
		// ���� ������ ���۸� recv_buf�� �����Ѵ�.
		memcpy(m_recv_buf, m_packet_start, left_data);
		// ó���� ������ ���� �����Ѵ�.
		m_packet_start = m_recv_buf;
		// ���� ��Ŷ�� ���� ������ ���� ��ġ�� �����Ѵ�.
		m_next_recv_ptr = m_packet_start + left_data;
	}

	/* ���� recv �غ� */
	m_recv_start = m_next_recv_ptr;
}

void CPacketMgr::Process_packet()
{
	// ��Ŷ Ÿ��
	char p_type = m_packet_start[1];

	switch (p_type)
	{
	case SC_PACKET_LOGIN_OK:
	{
		sc_packet_login_ok* packet = reinterpret_cast<sc_packet_login_ok*>(m_packet_start);
		Login_Player(packet);
	}
	break;

	case SC_PACKET_LOGIN_FAIL:
		break;

	case SC_PACKET_ENTER:
	{
		sc_packet_enter* packet = reinterpret_cast<sc_packet_enter*>(m_packet_start);

		int retflag;
		Enter_Others(packet, retflag);
		if (retflag == 2) break;
	}
	break;

	case SC_PACKET_MOVE:
	{
		sc_packet_move* packet = reinterpret_cast<sc_packet_move*>(m_packet_start);

		Move_User(packet);
	}
	break;

	case SC_PACKET_MOVE_STOP:
	{
		sc_packet_move* packet = reinterpret_cast<sc_packet_move*>(m_packet_start);

		MoveStop_User(packet);
	}
	break;

	case SC_PACKET_STAT_CHANGE:
	{
		sc_packet_stat_change* packet = reinterpret_cast<sc_packet_stat_change*>(m_packet_start);

		ChangeStat_User(packet);
	}
	break;

	case SC_PACKET_ATTACK:
	{
		sc_packet_attack* packet = reinterpret_cast<sc_packet_attack*>(m_packet_start);

		Attack_User(packet);
	}
	break;

	case SC_PACKET_ATTACK_STOP:
	{
		sc_packet_attack* packet = reinterpret_cast<sc_packet_attack*>(m_packet_start);

		AttackStop_User(packet);
	}
	break;

	case SC_PACKET_LEAVE:
	{
		sc_packet_leave* packet = reinterpret_cast<sc_packet_leave*>(m_packet_start);

		int retflag;
		Leave_Object(packet, retflag);
		if (retflag == 2) break;
	}
	break;

	case SC_PACKET_STANCE_CHANGE:
	{
		sc_packet_stance_change* packet = reinterpret_cast<sc_packet_stance_change*>(m_packet_start);
		int retflag;
		Change_Stance_Others(packet, retflag);
		if (retflag == 2) break;
	}
	break;

	case SC_PACKET_NPC_ENTER:
	{
		sc_packet_npc_enter* packet = reinterpret_cast<sc_packet_npc_enter*>(m_packet_start);
		Enter_NPC(packet);
	}
	break;

	case SC_PACKET_NPC_MOVE:
	{
		sc_packet_move* packet = reinterpret_cast<sc_packet_move*>(m_packet_start);

		Move_NPC(packet);
	}
	break;

	case SC_PACKET_MONSTER_ENTER:
	{
		sc_packet_monster_enter* packet = reinterpret_cast<sc_packet_monster_enter*>(m_packet_start);
		Enter_Monster(packet);
	}
	break;

	case SC_PACKET_MONSTER_MOVE:
	{
		sc_packet_move* packet = reinterpret_cast<sc_packet_move*>(m_packet_start);
		Move_Monster(packet);
	}
	break;

	case SC_PACKET_MONSTER_ATTACK:
	{
		sc_packet_monster_attack* packet = reinterpret_cast<sc_packet_monster_attack*>(m_packet_start);
		Attack_Monster(packet);
	}
	break;

	case SC_PACKET_MONSTER_RUSH:
	{
		sc_packet_monster_rushAttack* packet = reinterpret_cast<sc_packet_monster_rushAttack*>(m_packet_start);

		int s_num = packet->id;

		Engine::CGameObject* pObj = m_pObjectMgr->Get_ServerObject(L"Layer_GameObject", L"MONSTER", s_num);

		pObj->Set_State(packet->animIdx);
		pObj->Set_MoveStop(true);
		pObj->Set_Attack(true);
		pObj->Set_Other_direction(_vec3(packet->dirX, packet->dirY, packet->dirZ));
		//pObj->Get_Transform()->m_vPos = _vec3(packet->posX, packet->posY, packet->posZ);
		pObj->Set_DeadReckoning(_vec3(packet->posX, packet->posY, packet->posZ));
	}
	break;

	case SC_PACKET_MONSTER_STAT:
	{
		sc_packet_stat_change* packet = reinterpret_cast<sc_packet_stat_change*>(m_packet_start);

		Change_Monster_Stat(packet);
	}
	break;

	default:
#ifdef ERR_CHECK
		printf("Unknown PACKET type [%d]\n", m_packet_start[1]);
#endif 
		break;
	}
}

void CPacketMgr::Change_Monster_Stat(sc_packet_stat_change* packet)
{
	int s_num = packet->id;
	Engine::CGameObject* pObj = m_pObjectMgr->Get_ServerObject(L"Layer_GameObject", L"MONSTER", s_num);

	pObj->Get_Info()->m_iHp = packet->hp;
	pObj->Get_Info()->m_iMp = packet->mp;
	pObj->Get_Info()->m_iExp = packet->exp;
}

void CPacketMgr::Move_NPC(sc_packet_move* packet)
{
	int s_num = packet->id;

	Engine::CGameObject* pObj = m_pObjectMgr->Get_ServerObject(L"Layer_GameObject", L"NPC", s_num);
	pObj->Set_DeadReckoning(_vec3(packet->posX, packet->posY, packet->posZ));

	pObj->Set_Other_direction(_vec3(packet->dirX, packet->dirY, packet->dirZ));
	pObj->Set_MoveStop(false);
}

void CPacketMgr::Enter_NPC(sc_packet_npc_enter* packet)
{
	/*__________________________________________________________________________________________________________
	[ GameLogic Object(NPC) ���� ]
	____________________________________________________________________________________________________________*/
	Engine::CGameObject* pGameObj = nullptr;

	if (!strcmp(packet->name, "Chicken") || !strcmp(packet->name, "Cat"))
	{
		pGameObj = CNPC_Animal::Create(m_pGraphicDevice, m_pCommandList,
			wstring(packet->name, &packet->name[MAX_ID_LEN]),				// MeshTag
			wstring(packet->naviType, &packet->naviType[MIDDLE_STR_LEN]),	// NaviMeshTag
			_vec3(0.05f, 0.05f, 0.05f),										// Scale
			_vec3(packet->angleX, packet->angleY, packet->angleZ),			// Angle
			_vec3(packet->posX, packet->posY, packet->posZ));				// Pos
	}
	else if (!strcmp(packet->name, "Aman_boy") || !strcmp(packet->name, "Human_boy")
		|| !strcmp(packet->name, "Popori_boy"))
	{
		pGameObj = CNPC_Boy::Create(m_pGraphicDevice, m_pCommandList,
			wstring(packet->name, &packet->name[MAX_ID_LEN]),				// MeshTag
			wstring(packet->naviType, &packet->naviType[MIDDLE_STR_LEN]),	// NaviMeshTag
			_vec3(0.05f, 0.05f, 0.05f),										// Scale
			_vec3(packet->angleX, packet->angleY, packet->angleZ),			// Angle
			_vec3(packet->posX, packet->posY, packet->posZ));				// Pos
	}
	else if (!strcmp(packet->name, "NPC_Villagers"))
	{
		pGameObj = CNPC_Villagers::Create(m_pGraphicDevice, m_pCommandList,
			wstring(packet->name, &packet->name[MAX_ID_LEN]),				// MeshTag
			wstring(packet->naviType, &packet->naviType[MIDDLE_STR_LEN]),	// NaviMeshTag
			_vec3(0.05f, 0.05f, 0.05f),										// Scale
			_vec3(packet->angleX, packet->angleY, packet->angleZ),			// Angle
			_vec3(packet->posX, packet->posY, packet->posZ));				// Pos
	}

	pGameObj->Set_ServerNumber(packet->id);
	Engine::FAILED_CHECK_RETURN(m_pObjectMgr->Add_GameObject(L"Layer_GameObject", L"NPC", pGameObj), E_FAIL);
}

void CPacketMgr::Change_Stance_Others(sc_packet_stance_change* packet, int& retflag)
{
	retflag = 1;
	int s_num = packet->id;

	if (s_num == g_iSNum) { retflag = 2; return; };

	Engine::CGameObject* pObj = m_pObjectMgr->Get_ServerObject(L"Layer_GameObject", L"Others", s_num);

	if (PC_GLADIATOR == packet->o_type)
	{
		static_cast<CPCOthersGladiator*>(pObj)->Set_StanceChange(packet->animIdx, packet->is_stance_attack);
	}
	else if (PC_ARCHER == packet->o_type)
	{

	}
	else if (PC_PRIEST == packet->o_type)
	{

	}
	else
	{
		static_cast<CPCOthersGladiator*>(pObj)->Set_StanceChange(packet->animIdx, packet->is_stance_attack);
	}
}

void CPacketMgr::Leave_Object(sc_packet_leave* packet, int& retflag)
{
	retflag = 1;
	if (packet->id == g_iSNum) { retflag = 2; return; };

	if (packet->id >= NPC_NUM_START && packet->id < MON_NUM_START)
		m_pObjectMgr->Delete_ServerObject(L"Layer_GameObject", L"NPC", packet->id);
	else if (packet->id >= MON_NUM_START)
		m_pObjectMgr->Delete_ServerObject(L"Layer_GameObject", L"MONSTER", packet->id);
	else
		m_pObjectMgr->Delete_ServerObject(L"Layer_GameObject", L"Others", packet->id);
}

void CPacketMgr::AttackStop_User(sc_packet_attack* packet)
{
	int s_num = packet->id;

	/* ���� Ŭ���̾�Ʈ�� ������ ���� ��� */
	if (s_num == g_iSNum)
	{
		Engine::CGameObject* pObj = m_pObjectMgr->Get_GameObject(L"Layer_GameObject", L"ThisPlayer", 0);
		pObj->Get_Transform()->m_vPos = _vec3(packet->posX, packet->posY, packet->posZ);
	}
	/* �ٸ� Ŭ���̾�Ʈ�� ������ ���� ��� */
	else
	{
		Engine::CGameObject* pObj = m_pObjectMgr->Get_ServerObject(L"Layer_GameObject", L"Others", s_num);

		if (PC_GLADIATOR == packet->o_type)
		{
			static_cast<CPCOthersGladiator*>(pObj)->Set_AnimationIdx(packet->animIdx);
		}
		else if (PC_ARCHER == packet->o_type)
		{
		}
		else if (PC_PRIEST == packet->o_type)
		{
		}
		else
			static_cast<CPCOthersGladiator*>(pObj)->Set_AnimationIdx(packet->animIdx);

		pObj->Set_IsStartPosInterpolation(true);
		pObj->Set_LinearPos(pObj->Get_Transform()->m_vPos, _vec3(packet->posX, packet->posY, packet->posZ));
		//pObj->Get_Transform()->m_vPos = _vec3(packet->posX, packet->posY, packet->posZ);
		pObj->Set_Other_direction(_vec3(packet->dirX, packet->dirY, packet->dirZ));
		pObj->Set_Attack(false);
		pObj->Set_MoveStop(true);
	}
}

void CPacketMgr::Attack_User(sc_packet_attack* packet)
{
	int s_num = packet->id;

	/* ���� Ŭ���̾�Ʈ�� ������ ��� */
	if (s_num == g_iSNum)
	{
		Engine::CGameObject* pObj = m_pObjectMgr->Get_GameObject(L"Layer_GameObject", L"ThisPlayer", 0);
		pObj->Set_DeadReckoning(_vec3(packet->posX, packet->posY, packet->posZ));
	}
	/* �ٸ� Ŭ���̾�Ʈ�� ������ ��� */
	else
	{
		Engine::CGameObject* pObj = m_pObjectMgr->Get_ServerObject(L"Layer_GameObject", L"Others", s_num);

		if (PC_GLADIATOR == packet->o_type)
		{
			static_cast<CPCOthersGladiator*>(pObj)->Set_AnimationIdx(packet->animIdx);
			static_cast<CPCOthersGladiator*>(pObj)->Reset_AttackMoveInterpolationRatio();
		}
		else if (PC_ARCHER == packet->o_type)
		{
		}
		else if (PC_PRIEST == packet->o_type)
		{
		}
		else
			static_cast<CPCOthersGladiator*>(pObj)->Set_AnimationIdx(packet->animIdx);

		pObj->Set_DeadReckoning(_vec3(packet->posX, packet->posY, packet->posZ));
		pObj->Set_Other_direction(_vec3(packet->dirX, packet->dirY, packet->dirZ));
		if (-1 != packet->end_angleY)
		{
			pObj->Set_IsStartAngleInterpolation(true);
			pObj->Set_LinearAngle(pObj->Get_Transform()->m_vAngle.y, packet->end_angleY);
		}
		pObj->Set_Attack(true);
	}
}

void CPacketMgr::ChangeStat_User(sc_packet_stat_change* packet)
{
	int s_num = packet->id;

	Engine::CGameObject* pObj = nullptr;
	/* ���� Ŭ���̾�Ʈ�� ������ ���ŵ� ��� */
	if (s_num == g_iSNum)
		pObj = m_pObjectMgr->Get_GameObject(L"Layer_GameObject", L"ThisPlayer", 0);
	/* �ٸ� Ŭ���̾�Ʈ�� ������ ���ŵ� ��� */
	else
		pObj = m_pObjectMgr->Get_ServerObject(L"Layer_GameObject", L"Others", s_num);

	pObj->Get_Info()->m_iHp = packet->hp;
	pObj->Get_Info()->m_iMp = packet->mp;
	pObj->Get_Info()->m_iExp = packet->exp;
}

void CPacketMgr::MoveStop_User(sc_packet_move* packet)
{
	int s_num = packet->id;

	/* ���� Ŭ���̾�Ʈ�� ������ ��� */
	if (s_num == g_iSNum)
	{
		Engine::CGameObject* pObj = m_pObjectMgr->Get_GameObject(L"Layer_GameObject", L"ThisPlayer", 0);

		auto d_ms = duration_cast<milliseconds>(high_resolution_clock::now().time_since_epoch()).count() - packet->move_time;

		pObj->Set_DeadReckoning(_vec3(packet->posX, packet->posY, packet->posZ));
	}

	/* �ٸ� Ŭ���̾�Ʈ�� ������ ��� */
	else
	{
		Engine::CGameObject* pObj = m_pObjectMgr->Get_ServerObject(L"Layer_GameObject", L"Others", s_num);
		auto d_ms = duration_cast<milliseconds>(high_resolution_clock::now().time_since_epoch()).count() - packet->move_time;

		static_cast<CPCOthersGladiator*>(pObj)->Set_AnimationIdx(packet->animIdx);
		pObj->Set_IsStartPosInterpolation(true);
		pObj->Set_LinearPos(pObj->Get_Transform()->m_vPos, _vec3(packet->posX, packet->posY, packet->posZ));
		//pObj->Get_Transform()->m_vPos = _vec3(packet->posX, packet->posY, packet->posZ);
		pObj->Set_Other_direction(_vec3(packet->dirX, packet->dirY, packet->dirZ));
		pObj->Set_MoveStop(true);
	}
}

void CPacketMgr::Move_User(sc_packet_move* packet)
{
	int s_num = packet->id;

	/* ���� Ŭ���̾�Ʈ�� ������ ��� */
	if (s_num == g_iSNum)
	{
		Engine::CGameObject* pObj = m_pObjectMgr->Get_GameObject(L"Layer_GameObject", L"ThisPlayer", 0);

		auto d_ms = duration_cast<milliseconds>(high_resolution_clock::now().time_since_epoch()).count() - packet->move_time;

		pObj->Set_DeadReckoning(_vec3(packet->posX, packet->posY, packet->posZ));

	}
	/* �ٸ� Ŭ���̾�Ʈ�� ������ ��� */
	else
	{
		Engine::CGameObject* pObj = m_pObjectMgr->Get_ServerObject(L"Layer_GameObject", L"Others", s_num);

		auto d_ms = duration_cast<milliseconds>(high_resolution_clock::now().time_since_epoch()).count() - packet->move_time;

		static_cast<CPCOthersGladiator*>(pObj)->Set_AnimationIdx(packet->animIdx);
		pObj->Set_DeadReckoning(_vec3(packet->posX, packet->posY, packet->posZ));
		pObj->Set_Other_direction(_vec3(packet->dirX, packet->dirY, packet->dirZ));
		pObj->Set_MoveStop(false);
	}
}

void CPacketMgr::Enter_Others(sc_packet_enter* packet, int& retflag)
{
	retflag = 1;
	if (packet->id == g_iSNum) { retflag = 2; return; };

	/*__________________________________________________________________________________________________________
	[ GameLogic Object(Ohters) ���� ]
	____________________________________________________________________________________________________________*/
	Engine::CGameObject* pGameObj = nullptr;
	wstring wstrMeshTag = L"";

	if (PC_GLADIATOR == packet->o_type)
	{
		wstrMeshTag = L"PoporiR27Gladiator";
		pGameObj = CPCOthersGladiator::Create(m_pGraphicDevice, m_pCommandList,
			L"PoporiR27Gladiator",							// MeshTag
			L"StageVelika_NaviMesh",							// NaviMeshTag
			_vec3(0.05f, 0.05f, 0.05f),						// Scale
			_vec3(0.0f, 0.0f, 0.0f),							// Angle
			_vec3(packet->posX, packet->posY, packet->posZ),	// Pos
			Twohand19_A_SM);									// WeaponType

	}
	else if (PC_ARCHER == packet->o_type)
	{

	}
	else if (PC_PRIEST == packet->o_type)
	{

	}
	else
	{
		wstrMeshTag = L"PoporiR27Gladiator";

		pGameObj = CPCOthersGladiator::Create(m_pGraphicDevice, m_pCommandList,
			wstrMeshTag,							// MeshTag
			L"StageVelika_NaviMesh",							// NaviMeshTag
			_vec3(0.05f, 0.05f, 0.05f),						// Scale
			_vec3(0.0f, 0.0f, 0.0f),							// Angle
			_vec3(packet->posX, packet->posY, packet->posZ),	// Pos
			Twohand19_A_SM);									// WeaponType
	}

	pGameObj->Set_OType(packet->o_type);
	pGameObj->Set_ServerNumber(packet->id);

	Engine::FAILED_CHECK_RETURN(m_pObjectMgr->Add_GameObject(L"Layer_GameObject", L"Others", pGameObj), E_FAIL);

#ifdef ERR_CHECK
	cout << "Others ����!" << endl;
#endif
}

void CPacketMgr::Login_Player(sc_packet_login_ok* packet)
{
	g_iSNum = packet->id;

	/*__________________________________________________________________________________________________________
	[ GameLogic Object(ThisPlayer) ���� ]
	____________________________________________________________________________________________________________*/
	Engine::CGameObject* pGameObj = nullptr;
	wstring wstrMeshTag = L"";

	if (PC_GLADIATOR == packet->o_type)
	{
		wstrMeshTag = L"PoporiR27Gladiator";
	}
	else if (PC_ARCHER == packet->o_type)
	{

	}
	else if (PC_PRIEST == packet->o_type)
	{

	}
	else
		wstrMeshTag = L"PoporiR27Gladiator";

	pGameObj = CPCGladiator::Create(m_pGraphicDevice, m_pCommandList,
									wstrMeshTag,										// MeshTag
									L"StageVelika_NaviMesh",							// NaviMeshTag
									_vec3(0.05f, 0.05f, 0.05f),							// Scale
									_vec3(0.0f, 0.0f, 0.0f),							// Angle
									_vec3(packet->posX, packet->posY, packet->posZ),	// Pos
									TwoHand33_B_SM);									// WeaponType

	pGameObj->Set_OType(packet->o_type);
	pGameObj->Set_ServerNumber(g_iSNum);
	pGameObj->Set_Info(packet->level, packet->hp, packet->maxHp, packet->mp, packet->maxMp, packet->exp, packet->maxExp, packet->att, packet->spd);
	Engine::FAILED_CHECK_RETURN(m_pObjectMgr->Add_GameObject(L"Layer_GameObject", L"ThisPlayer", pGameObj), E_FAIL);

#ifdef ERR_CHECK
	cout << "Login OK! ���ӿϷ�!" << endl;
#endif
}

void CPacketMgr::Attack_Monster(sc_packet_monster_attack* packet)
{
	int s_num = packet->id;

	Engine::CGameObject* pObj = m_pObjectMgr->Get_ServerObject(L"Layer_GameObject", L"MONSTER", s_num);
	
	pObj->Set_State(packet->animIdx);
	pObj->Set_MoveStop(true);
	pObj->Set_Other_direction(_vec3(packet->dirX, packet->dirY, packet->dirZ));
	pObj->Set_DeadReckoning(pObj->Get_Transform()->m_vPos);
}

void CPacketMgr::Move_Monster(sc_packet_move* packet)
{
	int s_num = packet->id;

	Engine::CGameObject* pObj = m_pObjectMgr->Get_ServerObject(L"Layer_GameObject", L"MONSTER", s_num);

	//pObj->Get_Info()->m_fSpeed = packet->spd;
	//pObj->Set_MoveStop(false);
	pObj->Get_Transform()->m_vPos = _vec3(packet->posX, packet->posY, packet->posZ);
	pObj->Set_Other_direction(_vec3(packet->dirX, packet->dirY, packet->dirZ));
	pObj->Set_State(packet->animIdx);
}

void CPacketMgr::Enter_Monster(sc_packet_monster_enter* packet)
{
	Engine::CGameObject* pGameObj = nullptr;

	if (packet->mon_num == MON_CRAB)
	{
		pGameObj = CCrab::Create(m_pGraphicDevice, m_pCommandList,
								 wstring(packet->name, &packet->name[MAX_ID_LEN]),				// MeshTag
								 wstring(packet->naviType, &packet->naviType[MIDDLE_STR_LEN]),	// NaviMeshTag
								 _vec3(0.05f, 0.05f, 0.05f),									// Scale
								 _vec3(packet->angleX, packet->angleY, packet->angleZ),			// Angle
								 _vec3(packet->posX, packet->posY, packet->posZ));
	}
	else if (packet->mon_num == MON_MONKEY)
	{
		pGameObj = CMonkey::Create(m_pGraphicDevice, m_pCommandList,
								   wstring(packet->name, &packet->name[MAX_ID_LEN]),				// MeshTag
								   wstring(packet->naviType, &packet->naviType[MIDDLE_STR_LEN]),	// NaviMeshTag
								   _vec3(0.05f, 0.05f, 0.05f),										// Scale
								   _vec3(packet->angleX, packet->angleY, packet->angleZ),			// Angle
								   _vec3(packet->posX, packet->posY, packet->posZ));
	}
	else if (packet->mon_num == MON_CLODER)
	{
		pGameObj = CCloderA::Create(m_pGraphicDevice, m_pCommandList,
								   wstring(packet->name, &packet->name[MAX_ID_LEN]),				// MeshTag
								   wstring(packet->naviType, &packet->naviType[MIDDLE_STR_LEN]),	// NaviMeshTag
								   _vec3(0.05f, 0.05f, 0.05f),										// Scale
								   _vec3(packet->angleX, packet->angleY, packet->angleZ),			// Angle
								   _vec3(packet->posX, packet->posY, packet->posZ));
	}
	else if (packet->mon_num == MON_SAILOR)
	{
		pGameObj = CDrownedSailor::Create(m_pGraphicDevice, m_pCommandList,
										  wstring(packet->name, &packet->name[MAX_ID_LEN]),					// MeshTag
										  wstring(packet->naviType, &packet->naviType[MIDDLE_STR_LEN]),		// NaviMeshTag
										  _vec3(0.05f, 0.05f, 0.05f),										// Scale
										  _vec3(packet->angleX, packet->angleY, packet->angleZ),			// Angle
										  _vec3(packet->posX, packet->posY, packet->posZ));
	}


	pGameObj->Set_ServerNumber(packet->id);
	pGameObj->Set_Info(1, packet->Hp, packet->maxHp, 0, 0, 0, 0, 0, packet->spd);

	Engine::FAILED_CHECK_RETURN(m_pObjectMgr->Add_GameObject(L"Layer_GameObject", L"MONSTER", pGameObj), E_FAIL);
}

void CPacketMgr::send_login()
{
	cs_packet_login p;

	p.size   = sizeof(p);
	p.type   = CS_LOGIN;
	// p.o_type = PC_GLADIATOR;

	int t_id = GetCurrentProcessId();
	sprintf_s(p.name, "P%03d", t_id % 1000);
	sprintf_s(p.password, "%03d", t_id % 1000);

	send_packet(&p);
}

void CPacketMgr::send_move(const _vec3& vDir, const _vec3& vPos, const _int& iAniIdx)
{
	cs_packet_move p;

	p.size = sizeof(p);
	p.type = CS_MOVE;
	p.animIdx = iAniIdx;
	p.posX = vPos.x;
	p.posY = vPos.y;
	p.posZ = vPos.z;

	p.dirX = vDir.x;
	p.dirY = vDir.y;
	p.dirZ = vDir.z;

	p.move_time = static_cast<unsigned>(duration_cast<milliseconds>(high_resolution_clock::now().time_since_epoch()).count());

	send_packet(&p);
}

bool CPacketMgr::change_MoveKey(MVKEY eKey)
{
	m_eCurKey = eKey;

	/* Ű �Է��� ����Ǿ��� ��� */
	if (m_eCurKey != m_ePreKey)
	{
		m_ePreKey = m_eCurKey;
		return true;
	}

	return false;
}

void CPacketMgr::send_move_stop(const _vec3& vPos, const _vec3& vDir, const _int& iAniIdx)
{
	cs_packet_move_stop p;

	p.size = sizeof(p);
	p.type = CS_MOVE_STOP;
	p.animIdx = iAniIdx;
	p.posX = vPos.x;
	p.posY = vPos.y;
	p.posZ = vPos.z;

	p.dirX = vDir.x;
	p.dirY = vDir.y;
	p.dirZ = vDir.z;

	send_packet(&p);
}

void CPacketMgr::send_stance_change(const _int& iAniIdx, const _bool& bIsStanceAttack)
{
	cs_packet_stance_change p;

	p.size             = sizeof(p);
	p.type             = CS_STANCE_CHANGE;
	
	p.animIdx          = iAniIdx;
	p.is_stance_attack = bIsStanceAttack;

	send_packet(&p);
}

void CPacketMgr::send_attack(const _int& iAniIdx, const _vec3& vDir, const _vec3& vPos, const _float& fEndAngleY)
{
	cs_packet_attack p;

	p.size = sizeof(p);
	p.type = CS_ATTACK;

	p.animIdx = iAniIdx;
	p.posX    = vPos.x;
	p.posY    = vPos.y;
	p.posZ    = vPos.z;
	p.dirX    = vDir.x;
	p.dirY    = vDir.y;
	p.dirZ    = vDir.z;
	p.end_angleY = fEndAngleY;

	send_packet(&p);
}

void CPacketMgr::send_attack_stop(const _int& iAniIdx, const _vec3& vDir, const _vec3& vPos)
{
	cs_packet_attack p;

	p.size = sizeof(p);
	p.type = CS_ATTACK_STOP;

	p.animIdx = iAniIdx;

	p.posX    = vPos.x;
	p.posY    = vPos.y;
	p.posZ    = vPos.z;

	p.dirX    = vDir.x;
	p.dirY    = vDir.y;
	p.dirZ    = vDir.z;

	send_packet(&p);
}

void CPacketMgr::send_attackByMonster(int objID)
{
	cs_packet_player_collision p;

	p.size = sizeof(p);
	p.type = CS_COLLIDE;
	p.col_id = objID;

	send_packet(&p);
}

void CPacketMgr::send_attackToMonster(int objID)
{
	cs_packet_player_collision p;

	p.size = sizeof(p);
	p.type = CS_COLLIDE_MONSTER;
	p.col_id = objID;

	send_packet(&p);
}

void CPacketMgr::send_packet(void* packet)
{
	char* p = reinterpret_cast<char*>(packet);

	int iSendBytes = send(g_hSocket, p, p[0], 0);

#ifdef ERR_CHECK
	if (iSendBytes == SOCKET_ERROR)
	{
		error_display("send Error : ", WSAGetLastError());
		closesocket(g_hSocket);
	}
#endif 
	
}

void CPacketMgr::Free()
{
}
