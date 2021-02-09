#include "stdafx.h"
#include "PacketMgr.h"
#include "ObjectMgr.h"

#include "TestPlayer.h"
#include "TestOthers.h"
#include "DynamicCamera.h"

IMPLEMENT_SINGLETON(CPacketMgr)

CPacketMgr::CPacketMgr()
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
	char net_buf[MAX_BUF_SIZE];

	// Server Data Receive.
	int retval = recv(g_hSocket, net_buf, MAX_BUF_SIZE, 0);

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
		// ��Ŷ ������
		ProcessData(net_buf, static_cast<size_t>(retval));
	}
}

void CPacketMgr::ProcessData(char* net_buf, size_t io_byte)
{
	char* ptr = net_buf;

	/* ó���ؾ� �� Original Packet Size */
	static size_t in_packet_size = 0;
	/* ó������ ���� Remain Packet Size */
	static size_t saved_packet_size = 0;
	/* recv packet�� ������ Buffer */
	static char packet_buffer[MAX_BUF_SIZE];

	/* ó���ؾ� �� ��Ŷ�� ���� ��� */
	while (0 != io_byte)
	{
		/* ó���ؾ� �� Packet Size ���� */
		if (0 == in_packet_size)
			in_packet_size = ptr[0];

		/* ��Ŷ�� ó���� �� ���� ��� -> ��Ŷ ó�� */
		if (io_byte + saved_packet_size >= in_packet_size)
		{
			/* 
			������ ó������ ���� ��Ŷ(saved_packet_size)�� ������ �� �����Ƿ�,
			Packet_buffer�� ���� �޸� ����(packet_buffer + saved_packet_size)��
			���� ���� ��Ŷ(ptr) ����
			*/
			memcpy(packet_buffer + saved_packet_size, ptr, in_packet_size - saved_packet_size);

			/* ��Ŷ ó�� */
			ProcessPacket(packet_buffer);

			/* ���� recv ���� �޸� ���� ���� */
			ptr += in_packet_size - saved_packet_size;
		
			/* ��Ŷ�� ó���ϰ� ���� recv size ���� */
			io_byte -= in_packet_size - saved_packet_size;

			/* Original Packet�� ó�������Ƿ� ���� ���� ��Ŷ�� ���� Original Packet Size �ʱ�ȭ */
			in_packet_size = 0;
			saved_packet_size = 0;
		}
		/* ��Ŷ�� ó���� �� ���� ��� -> ��Ŷ ���� */
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
		[ GameLogic Object(player) ���� ]
		____________________________________________________________________________________________________________*/
		
		Engine::CGameObject* pGameObj = nullptr;

		pGameObj = CTestPlayer::Create(m_pGraphicDevice, m_pCommandList,
									   L"PoporiR19",					// MeshTag
									   _vec3(0.05f, 0.05f, 0.05f),		// Scale
									   _vec3(0.0f, 0.0f, 0.0f),			// Angle
									   _vec3(packet->posX, packet->posY, packet->posZ));		// Pos

		pGameObj->Set_ServerNumber(g_iSNum);
		Engine::FAILED_CHECK_RETURN(Engine::CObjectMgr::Get_Instance()->Add_GameObject(L"Layer_GameObject", L"Popori_F", pGameObj), E_FAIL);

		/* Camera Target Setting */
		CDynamicCamera* pCamera = static_cast<CDynamicCamera*>(Engine::CObjectMgr::Get_Instance()->Get_GameObject(L"Layer_Camera", L"DynamicCamera"));
		pCamera->Set_Target(pGameObj);
		
#ifdef ERR_CHECK
		cout << "Login OK! ���ӿϷ�!" << endl;
#endif
	}
	break;

	case SC_PACKET_LOGIN_FAIL:
		break;

	case SC_PACKET_ENTER:
	{
		sc_packet_enter* packet = reinterpret_cast<sc_packet_enter*>(ptr);

		/* ��ü�� Ÿ�� �Ǻ�: Player/Monster/NPC */
		switch (packet->o_type)
		{
		/* other player */
		case 48:
		{
			if (packet->id == g_iSNum) break;

			/*__________________________________________________________________________________________________________
			[ GameLogic Object(player) ���� ]
			____________________________________________________________________________________________________________*/

			Engine::CGameObject* pGameObj = nullptr;

			pGameObj = CTestOthers::Create(m_pGraphicDevice, m_pCommandList,
											L"PoporiH25",											// MeshTag
										   _vec3(0.05f, 0.05f, 0.05f),								// Scale
										   _vec3(0.0f, 0.0f, 0.0f),									// Angle
										   _vec3(packet->posX, packet->posY, packet->posZ));		// Pos

			pGameObj->Set_ServerNumber(packet->id);

			Engine::FAILED_CHECK_RETURN(Engine::CObjectMgr::Get_Instance()->Add_GameObject(L"Layer_GameObject", L"Others", pGameObj), E_FAIL);
		}
		break;
		/* npc */
		case 49:
		{

		}
		break;

		default:
			break;
		}
	}
	break;

	case SC_PACKET_MOVE:
	{
		sc_packet_move* packet = reinterpret_cast<sc_packet_move*>(ptr);

		int s_num = packet->id;

		/* ��ü�� Ÿ�� �Ǻ�: Player/Monster/NPC */
		switch (packet->o_type)
		{
		/* player */
		case 48:
		{
			/* ���� Ŭ���̾�Ʈ�� ������ ��� */
			if (s_num == g_iSNum)
			{
				Engine::CGameObject* pObj = Engine::CObjectMgr::Get_Instance()->Get_GameObject(L"Layer_GameObject", L"Popori_F", 0);

				auto d_ms = duration_cast<milliseconds>(high_resolution_clock::now().time_since_epoch()).count() - packet->move_time;
				
				pObj->Set_DeadReckoning(pObj->Get_Transform()->m_vPos,
										pObj->Get_Transform()->m_vPos,
										pObj->Get_Transform()->m_vPos, 
										_vec3(packet->posX, packet->posY, packet->posZ));
			
				pObj->Get_Transform()->m_vAngle.y = packet->angleY;
			}
			/* �ٸ� Ŭ���̾�Ʈ�� ������ ��� */
			else
			{
				Engine::CGameObject* pObj = Engine::CObjectMgr::Get_Instance()->Get_ServerObject(L"Layer_GameObject", L"Others", s_num);

				auto d_ms = duration_cast<milliseconds>(high_resolution_clock::now().time_since_epoch()).count() - packet->move_time;

				pObj->Set_DeadReckoning(pObj->Get_Transform()->m_vPos,
										pObj->Get_Transform()->m_vPos,
										pObj->Get_Transform()->m_vPos,
										_vec3(packet->posX, packet->posY, packet->posZ));
				//pObj->Get_Transform()->m_vPos = _vec3(packet->posX, packet->posY, packet->posZ);
				pObj->Get_Transform()->m_vAngle.y = packet->angleY;
			}
		}
		break;

		/* npc */
		case 49:
			break;

		default:
			break;
		}	
	}
	break;

	case SC_PACKET_LEAVE:
	{
		sc_packet_move* packet = reinterpret_cast<sc_packet_move*>(ptr);

		if (packet->id == g_iSNum) break;

		Engine::CObjectMgr::Get_Instance()->Delete_ServerObject(L"Layer_GameObject", L"Others", packet->id);
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

void CPacketMgr::send_move(char dir, const _vec3& vDir, const _vec3& vAngle)
{
	cs_packet_move p;

	p.size = sizeof(p);
	p.type = CS_MOVE;

	p.dir = dir;
	p.dirX = vDir.x;
	p.dirY = vDir.y;
	p.dirZ = vDir.z;

	p.angleX = vAngle.x;
	p.angleY = vAngle.y;
	p.angleZ = vAngle.z;

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

void CPacketMgr::send_move_stop(const _vec3& vPos, const _vec3& vAngle)
{
	cs_packet_move_stop p;

	p.size = sizeof(p);
	p.type = CS_MOVE_STOP;

	p.posX = vPos.x;
	p.posY = vPos.y;
	p.posZ = vPos.z;

	p.angleX = vAngle.x;
	p.angleY = vAngle.y;
	p.angleZ = vAngle.z;

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