#include "stdafx.h"
#include "PacketMgr.h"
#include "Player.h"

void process_packet(int id)
{
	/* Server Number 해당 유저 */
	CPlayer* pPlayer = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", id));

	// 패킷 타입
	char p_type = pPlayer->m_packet_start[1];

	switch (p_type)
	{
	case CS_LOGIN:
	{
		cs_packet_login* p = reinterpret_cast<cs_packet_login*>(pPlayer->m_packet_start);

		pPlayer->Get_ClientLock().lock();
		strcpy_s(pPlayer->m_ID, p->name);
		pPlayer->Get_ClientLock().unlock();

		/* CHECK ID in Database Server */
		send_login_ok(id);

		// 접속 중인 유저 탐색
		auto player_list = CObjMgr::GetInstance()->Get_OBJLIST(L"PLAYER");

		for (auto& others : *player_list)
		{		
			if (others.second->Get_IsConnected())
			{
				if (others.second->m_sNum != pPlayer->m_sNum)
				{
					send_enter_packet(others.second->m_sNum, pPlayer->m_sNum);
					send_enter_packet(pPlayer->m_sNum, others.second->m_sNum);
				}			
			}
		}
	}
		break;
	case CS_MOVE:
		break;
	case CS_ATTACK:
		break;
	case CS_CHAT:
		break;
	case CS_LOGOUT:
		break;
	case CS_TELEPORT:
		break;
	}
}

/* ========================패킷 재조립========================*/
void process_recv(int id, DWORD iosize)
{
	/* Server Number 해당 유저 */
	CPlayer* pPlayer = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", id));
	
	// m_packet_start		: 처리할 데이터 버퍼 및 위치 (= iocp_buffer)
	// m_packet_start[0]	: 처리할 패킷의 크기
	unsigned char p_size = pPlayer->m_packet_start[0];
	
	/* 다음 패킷을 받을 링버퍼 위치 설정 */
	// m_recv_start			: 수신한 패킷의 시작 위치
	unsigned char* next_recv_ptr = pPlayer->m_recv_start + iosize;

	// 처리할 패킷 크기보다 남아있는 공간이 적당할 경우 패킷 처리 시작
	while (p_size <= next_recv_ptr - pPlayer->m_packet_start)
	{
		// 패킷 처리 (= 패킷 만들기)
		process_packet(id);
		// 패킷 처리 후 유저의 데이터 처리 버퍼 시작 위치 변경
		pPlayer->m_packet_start += p_size;
		
		// recv한 데이터를 처리를 했는데 아직 처리하지 못한 데이터를 가지고 있을 수도 있다.
		// 처리해야 할 패킷 크기 갱신
		if (pPlayer->m_packet_start < next_recv_ptr)
			p_size = pPlayer->m_packet_start[0];
		else
			break;
	} 

	// 처리하지 못한(남아 있는) 데이터의 시작 위치 
	long long left_data = next_recv_ptr - pPlayer->m_packet_start;

	// Recv 처리 끝 -> 다시 Recv를 해야 한다.
	// 다시 Recv를 하기 전에 링 버퍼의 여유 공간을 확인 후 다 사용했을 경우 초기화
	// 남아있는 공간이 MIN_BUUFER 보다 작으면 남은 데이터를 링버퍼 맨 앞으로 옮겨준다. 
	if ((MAX_BUFFER - (next_recv_ptr - pPlayer->m_recv_over.iocp_buf)) < MIN_BUFFER)
	{
		// 남은 데이터 버퍼를 iocp_buf에 복사
		memcpy(pPlayer->m_recv_over.iocp_buf, pPlayer->m_packet_start, left_data);
		// 처리할 데이터 버퍼 갱신
		pPlayer->m_packet_start = pPlayer->m_recv_over.iocp_buf;
		// 다음 패킷을 받을 버퍼의 시작위치 갱신
		next_recv_ptr = pPlayer->m_packet_start + left_data;
	}

	// 링 버퍼 공간 갱신 여부 확인 후 다음 Recv 준비하기
	DWORD recv_flag = 0;
	pPlayer->m_recv_start = next_recv_ptr;
	pPlayer->m_recv_over.wsa_buf.buf = reinterpret_cast<CHAR*>(next_recv_ptr);
	pPlayer->m_recv_over.wsa_buf.len = MAX_BUFFER - static_cast<int>(next_recv_ptr - pPlayer->m_recv_over.iocp_buf);

	pPlayer->Get_ClientLock().lock();
	if (pPlayer->Get_IsConnected())
		WSARecv(pPlayer->m_sock, &pPlayer->m_recv_over.wsa_buf, 1, NULL, &recv_flag, &pPlayer->m_recv_over.wsa_over, NULL);
	pPlayer->Get_ClientLock().unlock();
}

void send_packet(int id, void* p)
{
	unsigned char* packet = reinterpret_cast<unsigned char*>(p);

	OVER_EX* send_over = new OVER_EX;

	memcpy(send_over->iocp_buf, packet, packet[0]);
	send_over->op_mode = OP_MODE_SEND;
	send_over->wsa_buf.buf = reinterpret_cast<CHAR*>(send_over->iocp_buf);
	send_over->wsa_buf.len = packet[0];
	ZeroMemory(&send_over->wsa_over, sizeof(send_over->wsa_over));

	CPlayer* pPlayer = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", id));

	pPlayer->Get_ClientLock().lock();
	if (pPlayer->Get_IsConnected())
		WSASend(pPlayer->m_sock, &send_over->wsa_buf, 1, NULL, 0, &send_over->wsa_over, NULL);
	pPlayer->Get_ClientLock().unlock();
}

void send_login_ok(int id)
{
	sc_packet_login_ok p;

	CPlayer* pPlayer = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", id));

	p.size = sizeof(p);
	p.type = SC_PACKET_LOGIN_OK;
	p.id = id;

	p.att = pPlayer->att;
	p.exp = pPlayer->Exp;
	p.hp = pPlayer->Hp;
	p.level = pPlayer->level;
	p.maxExp = pPlayer->maxExp;
	p.maxHp = pPlayer->maxHp;
	p.spd = pPlayer->spd;

	p.posX = pPlayer->m_vPos.x;
	p.posY = pPlayer->m_vPos.y;
	p.posZ = pPlayer->m_vPos.z;

	p.dirX = pPlayer->m_vDir.x;
	p.dirY = pPlayer->m_vDir.y;
	p.dirZ = pPlayer->m_vDir.z;

	send_packet(id, &p);
}

void send_enter_packet(int to_client, int new_id)
{
	sc_packet_enter p;

	CPlayer* pNewPlayer = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", new_id));

	p.size = sizeof(p);
	p.type = SC_PACKET_ENTER;
	p.id = new_id;

	pNewPlayer->Get_ClientLock().lock();
	strcpy_s(p.name, pNewPlayer->m_ID);
	pNewPlayer->Get_ClientLock().unlock();
	p.o_type = pNewPlayer->m_type;

	p.posX = pNewPlayer->m_vPos.x;
	p.posY = pNewPlayer->m_vPos.y;
	p.posZ = pNewPlayer->m_vPos.z;

	p.dirX = pNewPlayer->m_vDir.x;
	p.dirY = pNewPlayer->m_vDir.y;
	p.dirZ = pNewPlayer->m_vDir.z;

	send_packet(to_client, &p);
}
