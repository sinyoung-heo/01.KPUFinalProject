#include "stdafx.h"
#include "PacketMgr.h"
#include "Player.h"

void process_packet(int id)
{
	/* Server Number �ش� ���� */
	CPlayer* pPlayer = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(id));

	// ��Ŷ Ÿ��
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

/* ========================��Ŷ ������========================*/
void process_recv(int id, DWORD iosize)
{
	/* Server Number �ش� ���� */
	CPlayer* pPlayer = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(id));
	
	// m_packet_start		: ó���� ������ ���� �� ��ġ (= iocp_buffer)
	// m_packet_start[0]	: ó���� ��Ŷ�� ũ��
	unsigned char p_size = pPlayer->m_packet_start[0];
	
	/* ���� ��Ŷ�� ���� ������ ��ġ ���� */
	// m_recv_start			: ������ ��Ŷ�� ���� ��ġ
	unsigned char* next_recv_ptr = pPlayer->m_recv_start + iosize;

	// ó���� ��Ŷ ũ�⺸�� �����ִ� ������ ������ ��� ��Ŷ ó�� ����
	while (p_size <= next_recv_ptr - pPlayer->m_packet_start)
	{
		// ��Ŷ ó�� (= ��Ŷ �����)
		process_packet(id);
		// ��Ŷ ó�� �� ������ ������ ó�� ���� ���� ��ġ ����
		pPlayer->m_packet_start += p_size;
		
		// recv�� �����͸� ó���� �ߴµ� ���� ó������ ���� �����͸� ������ ���� ���� �ִ�.
		// ó���ؾ� �� ��Ŷ ũ�� ����
		if (pPlayer->m_packet_start < next_recv_ptr)
			p_size = pPlayer->m_packet_start[0];
		else
			break;
	} 

	// ó������ ����(���� �ִ�) �������� ���� ��ġ 
	long long left_data = next_recv_ptr - pPlayer->m_packet_start;

	// Recv ó�� �� -> �ٽ� Recv�� �ؾ� �Ѵ�.
	// �ٽ� Recv�� �ϱ� ���� �� ������ ���� ������ Ȯ�� �� �� ������� ��� �ʱ�ȭ
	// �����ִ� ������ MIN_BUUFER ���� ������ ���� �����͸� ������ �� ������ �Ű��ش�. 
	if ((MAX_BUFFER - (next_recv_ptr - pPlayer->m_recv_over.iocp_buf)) < MIN_BUFFER)
	{
		// ���� ������ ���۸� iocp_buf�� ����
		memcpy(pPlayer->m_recv_over.iocp_buf, pPlayer->m_packet_start, left_data);
		// ó���� ������ ���� ����
		pPlayer->m_packet_start = pPlayer->m_recv_over.iocp_buf;
		// ���� ��Ŷ�� ���� ������ ������ġ ����
		next_recv_ptr = pPlayer->m_packet_start + left_data;
	}

	// �� ���� ���� ���� ���� Ȯ�� �� ���� Recv �غ��ϱ�
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

	CPlayer* pPlayer = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(id));

	pPlayer->Get_ClientLock().lock();
	if (pPlayer->Get_IsConnected())
		WSASend(pPlayer->m_sock, &send_over->wsa_buf, 1, NULL, 0, &send_over->wsa_over, NULL);
	pPlayer->Get_ClientLock().unlock();
}

void send_login_ok(int id)
{
	sc_packet_login_ok p;

	CPlayer* pPlayer = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(id));

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

	send_packet(id, &p);
}
