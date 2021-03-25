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
#include "Monster_Normal.h"
#include "DynamicCamera.h"
#include "TestColPlayer.h"
#include "TestColMonster.h"
#include "PCGladiator.h"

IMPLEMENT_SINGLETON(CPacketMgr)

CPacketMgr::CPacketMgr()
	: m_pObjectMgr(Engine::CObjectMgr::Get_Instance())
	, m_pManagement(Engine::CManagement::Get_Instance())
	, m_pRenderer(Engine::CRenderer::Get_Instance())
{

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

	/* Non-Blocking Socket으로 전환 */
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
	cout << "서버에 접속을 요청하였습니다. 잠시만 기다려주세요." << endl;
#endif 

	CPacketMgr::Get_Instance()->send_login();

#ifdef ERR_CHECK
	cout << "Login packet 전송완료" << endl;
#endif 

	return S_OK;
}

void CPacketMgr::recv_packet()
{
	unsigned char net_buf[MAX_BUF_SIZE];

	// Server Data Receive.
	int retval = recv(g_hSocket, reinterpret_cast<char*>(net_buf), MAX_BUF_SIZE, 0);

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

	// 데이터 수신 성공
	if (retval > 0)
	{
		// 패킷 재조립
		ProcessData(net_buf, static_cast<size_t>(retval));
	}
}

void CPacketMgr::ProcessData(unsigned char* net_buf, size_t io_byte)
{
	unsigned char* ptr = net_buf;

	/* 처리해야 할 Original Packet Size */
	static size_t in_packet_size = 0;
	/* 처리하지 못한 Remain Packet Size */
	static size_t saved_packet_size = 0;
	/* recv packet을 저장한 Buffer */
	static char packet_buffer[MAX_BUF_SIZE];

	/* 처리해야 할 패킷이 있을 경우 */
	while (0 != io_byte)
	{
		/* 처리해야 할 Packet Size 갱신 */
		if (0 == in_packet_size)
			in_packet_size = ptr[0];

		/* 패킷을 처리할 수 있을 경우 -> 패킷 처리 */
		if (io_byte + saved_packet_size >= in_packet_size)
		{
			/* 
			이전에 처리하지 못한 패킷(saved_packet_size)이 존재할 수 있으므로,
			Packet_buffer의 다음 메모리 공간(packet_buffer + saved_packet_size)에
			새로 들어온 패킷(ptr) 저장
			*/
			memcpy(packet_buffer + saved_packet_size, ptr, in_packet_size - saved_packet_size);

			/* 패킷 처리 */
			ProcessPacket(packet_buffer);

			/* 다음 recv 받을 메모리 공간 갱신 */
			ptr += in_packet_size - saved_packet_size;
		
			/* 패킷을 처리하고 남은 recv size 갱신 */
			io_byte -= in_packet_size - saved_packet_size;

			/* Original Packet을 처리했으므로 다음 들어올 패킷을 위해 Original Packet Size 초기화 */
			in_packet_size = 0;
			saved_packet_size = 0;
		}
		/* 패킷을 처리할 수 없을 경우 -> 패킷 저장 */
		else
		{
			memcpy(packet_buffer + saved_packet_size, ptr, io_byte);
			saved_packet_size += io_byte;
			io_byte = 0;
		}
	}
}

void CPacketMgr::ProcessPacket(char* ptr)
{
	switch (ptr[1])
	{
	case SC_PACKET_LOGIN_OK:
	{
		sc_packet_login_ok* packet = reinterpret_cast<sc_packet_login_ok*>(ptr);

		g_iSNum = packet->id;

		/*__________________________________________________________________________________________________________
		[ GameLogic Object(player) 생성 ]
		____________________________________________________________________________________________________________*/
		
		Engine::CGameObject* pGameObj = nullptr;

#ifdef STAGE_LDH
		//pGameObj = CTestPlayer::Create(m_pGraphicDevice, m_pCommandList,
		//							   L"PoporiR19",					// MeshTag
		//							   _vec3(0.05f, 0.05f, 0.05f),		// Scale
		//							   _vec3(0.0f, 0.0f, 0.0f),			// Angle
		//							   _vec3(packet->posX, packet->posY, packet->posZ));		// Pos

		pGameObj = CTestColPlayer::Create(m_pGraphicDevice, m_pCommandList,
									   _vec3(1.f, 1.f, 1.f),			// Scale
									   _vec3(0.0f, 0.0f, 0.0f),			// Angle
									   _vec3(packet->posX, packet->posY, packet->posZ));		// Pos

#else
		pGameObj =	CPCGladiator::Create(m_pGraphicDevice, m_pCommandList,
										 L"PoporiR27Gladiator",								// MeshTag
										 L"StageVelika_NaviMesh",							// NaviMeshTag
										 _vec3(0.05f, 0.05f, 0.05f),						// Scale
										 _vec3(0.0f, 0.0f, 0.0f),							// Angle
										 _vec3(packet->posX, packet->posY, packet->posZ));	// Pos

#endif
		pGameObj->Set_ServerNumber(g_iSNum);
		pGameObj->Set_Info(packet->level, packet->hp, packet->maxHp, packet->mp, packet->maxMp, packet->exp, packet->maxExp, packet->att, packet->spd);
		Engine::FAILED_CHECK_RETURN(m_pObjectMgr->Add_GameObject(L"Layer_GameObject", L"ThisPlayer", pGameObj), E_FAIL);
		
#ifdef ERR_CHECK
		cout << "Login OK! 접속완료!" << endl;
#endif
	}
	break;

	case SC_PACKET_LOGIN_FAIL:
		break;

	case SC_PACKET_ENTER:
	{
		sc_packet_enter* packet = reinterpret_cast<sc_packet_enter*>(ptr);

		if (packet->id == g_iSNum) break;

		/*__________________________________________________________________________________________________________
		[ GameLogic Object(player) 생성 ]
		____________________________________________________________________________________________________________*/

		Engine::CGameObject* pGameObj = nullptr;

		pGameObj = CTestOthers::Create(m_pGraphicDevice, m_pCommandList,
									   L"PoporiH25",											// MeshTag
									   _vec3(0.05f, 0.05f, 0.05f),								// Scale
									   _vec3(0.0f, 0.0f, 0.0f),									// Angle
									   _vec3(packet->posX, packet->posY, packet->posZ));		// Pos

		pGameObj->Set_ServerNumber(packet->id);

		Engine::FAILED_CHECK_RETURN(m_pObjectMgr->Add_GameObject(L"Layer_GameObject", L"Others", pGameObj), E_FAIL);
	}
	break;

	case SC_PACKET_MOVE:
	{
		sc_packet_move* packet = reinterpret_cast<sc_packet_move*>(ptr);

		int s_num = packet->id;

		/* 현재 클라이언트가 움직인 경우 */
		if (s_num == g_iSNum)
		{
			Engine::CGameObject* pObj = m_pObjectMgr->Get_GameObject(L"Layer_GameObject", L"ThisPlayer", 0);

			auto d_ms = duration_cast<milliseconds>(high_resolution_clock::now().time_since_epoch()).count() - packet->move_time;

			pObj->Set_DeadReckoning(_vec3(packet->posX, packet->posY, packet->posZ));

		}
		/* 다른 클라이언트가 움직인 경우 */
		else
		{
			Engine::CGameObject* pObj = m_pObjectMgr->Get_ServerObject(L"Layer_GameObject", L"Others", s_num);

			auto d_ms = duration_cast<milliseconds>(high_resolution_clock::now().time_since_epoch()).count() - packet->move_time;

			pObj->Set_DeadReckoning(_vec3(packet->posX, packet->posY, packet->posZ));
			pObj->Set_Other_direction(_vec3(packet->dirX, packet->dirY, packet->dirZ));
			pObj->Set_MoveStop(false);
		}
	}
	break;

	case SC_PACKET_MOVE_STOP:
	{
		sc_packet_move* packet = reinterpret_cast<sc_packet_move*>(ptr);

		int s_num = packet->id;

		/* 현재 클라이언트가 움직인 경우 */
		if (s_num == g_iSNum)
		{
			Engine::CGameObject* pObj = m_pObjectMgr->Get_GameObject(L"Layer_GameObject", L"ThisPlayer", 0);

			auto d_ms = duration_cast<milliseconds>(high_resolution_clock::now().time_since_epoch()).count() - packet->move_time;

			pObj->Set_DeadReckoning(_vec3(packet->posX, packet->posY, packet->posZ));
		}

		/* 다른 클라이언트가 움직인 경우 */
		else	
		{
			Engine::CGameObject* pObj = m_pObjectMgr->Get_ServerObject(L"Layer_GameObject", L"Others", s_num);

			auto d_ms = duration_cast<milliseconds>(high_resolution_clock::now().time_since_epoch()).count() - packet->move_time;

			pObj->Get_Transform()->m_vPos = _vec3(packet->posX, packet->posY, packet->posZ);

			pObj->Set_Other_direction(_vec3(packet->dirX, packet->dirY, packet->dirZ));
			pObj->Set_MoveStop(true);
		}
	}
	break;

	case SC_PACKET_STAT_CHANGE:
	{
		sc_packet_stat_change* packet = reinterpret_cast<sc_packet_stat_change*>(ptr);

		int s_num = packet->id;

		Engine::CGameObject* pObj = nullptr;
		/* 현재 클라이언트의 스탯이 갱신된 경우 */
		if (s_num == g_iSNum)
			pObj = m_pObjectMgr->Get_GameObject(L"Layer_GameObject", L"ThisPlayer", 0);	
		/* 다른 클라이언트의 스탯이 갱신된 경우 */
		else
			pObj = m_pObjectMgr->Get_ServerObject(L"Layer_GameObject", L"Others", s_num);
					
		pObj->Get_Info()->m_iHp = packet->hp;
		pObj->Get_Info()->m_iMp = packet->mp;
		pObj->Get_Info()->m_iExp = packet->exp;
	}
	break;

	case SC_PACKET_ATTACK:
	{
		sc_packet_attack* packet= reinterpret_cast<sc_packet_attack*>(ptr);

		int s_num = packet->id;

		/* 현재 클라이언트가 공격한 경우 */
		if (s_num == g_iSNum)
		{
			Engine::CGameObject* pObj = m_pObjectMgr->Get_GameObject(L"Layer_GameObject", L"ThisPlayer", 0);
			pObj->Set_DeadReckoning(_vec3(packet->posX, packet->posY, packet->posZ));
		}
		/* 다른 클라이언트가 공격한 경우 */
		else
		{
			Engine::CGameObject* pObj = m_pObjectMgr->Get_ServerObject(L"Layer_GameObject", L"Others", s_num);

			pObj->Set_DeadReckoning(_vec3(packet->posX, packet->posY, packet->posZ));
			pObj->Set_Other_direction(_vec3(packet->dirX, packet->dirY, packet->dirZ));

			pObj->Set_Attack(true);
		}
	}
	break;

	case SC_PACKET_LEAVE:
	{
		sc_packet_leave* packet = reinterpret_cast<sc_packet_leave*>(ptr);

		if (packet->id == g_iSNum) break;

		if (packet->id >= NPC_NUM_START && packet->id < MON_NUM_START)
			m_pObjectMgr->Delete_ServerObject(L"Layer_GameObject", L"NPC", packet->id);
		else if (packet->id >= MON_NUM_START)
			m_pObjectMgr->Delete_ServerObject(L"Layer_GameObject", L"MONSTER", packet->id);
		else
			m_pObjectMgr->Delete_ServerObject(L"Layer_GameObject", L"Others", packet->id);
	}
	break;

	case SC_PACKET_NPC_ENTER:
	{
		sc_packet_npc_enter* packet = reinterpret_cast<sc_packet_npc_enter*>(ptr);

		/*__________________________________________________________________________________________________________
		[ GameLogic Object(NPC) 생성 ]
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
	break;

	case SC_PACKET_NPC_MOVE:
	{
		sc_packet_move* packet = reinterpret_cast<sc_packet_move*>(ptr);

		int s_num = packet->id;

		Engine::CGameObject* pObj = m_pObjectMgr->Get_ServerObject(L"Layer_GameObject", L"NPC", s_num);
		pObj->Set_DeadReckoning(_vec3(packet->posX, packet->posY, packet->posZ));

		pObj->Set_Other_direction(_vec3(packet->dirX, packet->dirY, packet->dirZ));
		pObj->Set_MoveStop(false);
	}
	break;

	case SC_PACKET_MONSTER_ENTER:
	{
		sc_packet_monster_enter* packet = reinterpret_cast<sc_packet_monster_enter*>(ptr);

		/*__________________________________________________________________________________________________________
		[ GameLogic Object(MONSTER) 생성 ]
		____________________________________________________________________________________________________________*/
		Engine::CGameObject* pGameObj = nullptr;

		pGameObj = CMonster_Normal::Create(m_pGraphicDevice, m_pCommandList,
										   wstring(packet->name, &packet->name[MAX_ID_LEN]),				// MeshTag
										   wstring(packet->naviType, &packet->naviType[MIDDLE_STR_LEN]),	// NaviMeshTag
										   _vec3(0.05f, 0.05f, 0.05f),										// Scale
										   _vec3(packet->angleX, packet->angleY, packet->angleZ),			// Angle
										   _vec3(packet->posX, packet->posY, packet->posZ));

		
		//pGameObj = CTestColMonster::Create(m_pGraphicDevice, m_pCommandList,
		//	_vec3(1.f, 1.f, 1.f),									// Scale
		//	_vec3(0.0f, 0.0f, 0.0f),								// Angle
		//	_vec3(packet->posX, packet->posY, packet->posZ));		// Pos

		pGameObj->Set_ServerNumber(packet->id);
		pGameObj->Set_Info(1, packet->Hp, packet->maxHp, 0, 0, 0, 0, 0, 5.f);

		Engine::FAILED_CHECK_RETURN(m_pObjectMgr->Add_GameObject(L"Layer_GameObject", L"MONSTER", pGameObj), E_FAIL);

	}
	break;

	case SC_PACKET_MONSTER_MOVE:
	{
		sc_packet_move* packet = reinterpret_cast<sc_packet_move*>(ptr);

		int s_num = packet->id;

		Engine::CGameObject* pObj = m_pObjectMgr->Get_ServerObject(L"Layer_GameObject", L"MONSTER", s_num);
		pObj->Set_DeadReckoning(_vec3(packet->posX, packet->posY, packet->posZ));

		pObj->Set_Other_direction(_vec3(packet->dirX, packet->dirY, packet->dirZ));
		pObj->Set_MoveStop(false);
	}
	break;

	case SC_PACKET_MONSTER_ATTACK:
	{
		sc_packet_monster_attack* packet = reinterpret_cast<sc_packet_monster_attack*>(ptr);

		int s_num = packet->id;
		Engine::CGameObject* pObj = m_pObjectMgr->Get_ServerObject(L"Layer_GameObject", L"MONSTER", s_num);
		cout << "몬스터 공격 패킷 받음" << endl;
	}
	break;

	case SC_PACKET_MONSTER_STAT:
	{
		sc_packet_stat_change* packet = reinterpret_cast<sc_packet_stat_change*>(ptr);

		int s_num = packet->id;
		Engine::CGameObject* pObj = m_pObjectMgr->Get_ServerObject(L"Layer_GameObject", L"MONSTER", s_num);

		pObj->Get_Info()->m_iHp = packet->hp;
		pObj->Get_Info()->m_iMp = packet->mp;
		pObj->Get_Info()->m_iExp = packet->exp;
	}
	break;

	default:
#ifdef ERR_CHECK
		printf("Unknown PACKET type [%d]\n", ptr[1]);
#endif 
		break;
	}
}

void CPacketMgr::send_login()
{
	cs_packet_login p;

	p.size = sizeof(p);
	p.type = CS_LOGIN;

	int t_id = GetCurrentProcessId();
	sprintf_s(p.name, "P%03d", t_id % 1000);
	sprintf_s(p.password, "%03d", t_id % 1000);

	send_packet(&p);
}

void CPacketMgr::send_move(const _vec3& vDir, const _vec3& vPos)
{
	cs_packet_move p;

	p.size = sizeof(p);
	p.type = CS_MOVE;

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

	/* 키 입력이 변경되었을 경우 */
	if (m_eCurKey != m_ePreKey)
	{
		m_ePreKey = m_eCurKey;
		return true;
	}

	return false;
}

void CPacketMgr::send_move_stop(const _vec3& vPos, const _vec3& vDir)
{
	cs_packet_move_stop p;

	p.size = sizeof(p);
	p.type = CS_MOVE_STOP;

	p.posX = vPos.x;
	p.posY = vPos.y;
	p.posZ = vPos.z;

	p.dirX = vDir.x;
	p.dirY = vDir.y;
	p.dirZ = vDir.z;

	send_packet(&p);
}

void CPacketMgr::send_attack(const _vec3& vDir, const _vec3& vPos)
{
	cs_packet_attack p;

	p.size = sizeof(p);
	p.type = CS_ATTACK;

	p.posX = vPos.x;
	p.posY = vPos.y;
	p.posZ = vPos.z;

	p.dirX = vDir.x;
	p.dirY = vDir.y;
	p.dirZ = vDir.z;

	send_packet(&p);
}

void CPacketMgr::send_attack_stop(const _vec3& vDir, const _vec3& vPos)
{
	cs_packet_attack p;

	p.size = sizeof(p);
	p.type = CS_ATTACK_STOP;

	p.posX = vPos.x;
	p.posY = vPos.y;
	p.posZ = vPos.z;

	p.dirX = vDir.x;
	p.dirY = vDir.y;
	p.dirZ = vDir.z;

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
