#include "stdafx.h"
#include "PacketMgr.h"
#include "Player.h"
#include "Npc.h"

/* ========================��Ŷ ó��========================*/
void process_packet(int id)
{
	/* Server Number �ش� ���� */
	CPlayer* pPlayer = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", id));

	if (pPlayer == nullptr) return;

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
		if (false == CDBMgr::GetInstance()->Check_ID(id, p->password))
		{
			/* ó�� ���ӿ� ������ ������� ȸ������ ��� in Database Server */
			CDBMgr::GetInstance()->Insert_NewPlayer_DB(id, p->password);
		}

		/* �α��� ���� ��Ŷ ���� */
		send_login_ok(id);

		/* Sector �ٽ� ��� (���� �� �̸� �� �� �ϰ�����. �������� ���� �� �� �� ���(sector�� Key�� Unique��) */
		CSectorMgr::GetInstance()->Enter_ClientInSector(id, (int)(pPlayer->m_vPos.y / SECTOR_SIZE), (int)(pPlayer->m_vPos.x / SECTOR_SIZE));

		/* �ش� �÷��̾ ��ϵǾ� �ִ� ���� ���� �����鿡�� ������ �˸� */
		unordered_set<pair<int, int>> nearSector;
		nearSector.reserve(5);
		CSectorMgr::GetInstance()->Get_NearSectorIndex(&nearSector, (int)pPlayer->m_vPos.x, (int)pPlayer->m_vPos.y);

		// ���� ���� ��ȸ
		for (auto& s : nearSector)
		{
			// ���� ���� ���� Ÿ �������� �ִ��� �˻�
			if (!(CSectorMgr::GetInstance()->Get_SectorList()[s.first][s.second].Get_ObjList().empty()))
			{
				// Ÿ ������ ���� ��ȣ ����
				for (auto obj_num : CSectorMgr::GetInstance()->Get_SectorList()[s.first][s.second].Get_ObjList())
				{
					// '��'���� ���� ��Ŷ ���� X
					if (obj_num == id) continue;

					/* Ÿ������ ��� ó�� */
					if (true == CObjMgr::GetInstance()->Is_Player(obj_num))
					{
						CPlayer* pOther = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", obj_num));

						// ������ �����鿡�Ը� ���� ���Ḧ �˸�
						if (pOther->Get_IsConnected())
						{
							// �þ� ���� ���ٸ� �þ� ��Ͽ� ���X.
							if (false == CObjMgr::GetInstance()->Is_Near(pPlayer, pOther)) continue;
							
							/* Ÿ ������ �þ߿� ���� ���� ��� '��'�� ��Ͻ�Ŵ */
							pOther->v_lock.lock();
							if (0 == pOther->view_list.count(id))
							{
								pOther->view_list.insert(id);
								pOther->v_lock.unlock();
								send_enter_packet(obj_num, id);
							}
							else pOther->v_lock.unlock();

							/* '��'�� �þ߿� Ÿ ������ ���� ��� -> Ÿ ���� ��� */
							pPlayer->v_lock.lock();
							if (0 == pPlayer->view_list.count(obj_num))
							{
								pPlayer->view_list.insert(obj_num);
								pPlayer->v_lock.unlock();
								send_enter_packet(id, obj_num);
							}
							else pPlayer->v_lock.unlock();
						}
					}
					/* NPC�� ��� ó�� */
					else if (true == CObjMgr::GetInstance()->Is_NPC(obj_num))
					{
						CNpc* pNPC = static_cast<CNpc*>(CObjMgr::GetInstance()->Get_GameObject(L"NPC", obj_num));

						// �þ� ���� ���ٸ� �þ� ��Ͽ� ���X.
						if (false == CObjMgr::GetInstance()->Is_Near(pPlayer, pNPC)) continue;
						pPlayer->v_lock.lock();
						pPlayer->view_list.insert(obj_num);
						pPlayer->v_lock.unlock();
						send_NPC_enter_packet(id, obj_num);
					}
					/* MONSTER�� ��� ó��*/
					else
					{
						int i = 0;
					}
				}
			}
		}
	}
	break;

	case CS_MOVE:
	{
		cs_packet_move* p = reinterpret_cast<cs_packet_move*>(pPlayer->m_packet_start);
		//pPlayer->move_time = p->move_time;
		process_move(id, p);
	}
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
	CPlayer* pPlayer = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", id));

	if (pPlayer == nullptr) return;
	
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

/*===========================================PLAYER=====================================================*/
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

	if (pPlayer == nullptr) return;

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

void send_leave_packet(int to_client, int leave_id)
{
	sc_packet_leave p;

	p.size = sizeof(p);
	p.type = SC_PACKET_LEAVE;
	p.id = leave_id;

	send_packet(to_client, &p);
}

void send_move_packet(int to_client, int id)
{
	sc_packet_move p;

	CPlayer* pPlayer = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", id));

	if (pPlayer == nullptr) return;

	p.size = sizeof(p);
	p.type = SC_PACKET_MOVE;
	p.id = id;

	//p.move_time = pNewPlayer->move_time;

	p.posX = pPlayer->m_vPos.x;
	p.posY = pPlayer->m_vPos.y;
	p.posZ = pPlayer->m_vPos.z;

	p.dirX = pPlayer->m_vDir.x;
	p.dirY = pPlayer->m_vDir.y;
	p.dirZ = pPlayer->m_vDir.z;

	send_packet(to_client, &p);
}

void process_move(int id, cs_packet_move* info)
{
	CPlayer* pPlayer = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", id));

	if (pPlayer == nullptr) return;

	/* �ش� �÷��̾��� ���� ��ġ�� */
	float ori_x, ori_y, ori_z;
	ori_x = pPlayer->m_vPos.x;
	ori_y = pPlayer->m_vPos.y;
	ori_z = pPlayer->m_vPos.z;

	/* �ش� �÷��̾��� ���� �þ� ��� */
	pPlayer->v_lock.lock();
	unordered_set<int> old_viewlist = pPlayer->view_list;
	pPlayer->v_lock.unlock();

	/* �ش� �÷��̾�κ��� ���� ����� ��ġ�� ���� */
	pPlayer->m_vPos.x = info->posX;
	pPlayer->m_vPos.y = info->posY;
	pPlayer->m_vPos.z = info->posZ;

	pPlayer->m_vDir.x = info->dirX;
	pPlayer->m_vDir.y = info->dirY;
	pPlayer->m_vDir.z = info->dirZ;

	/* ����� ��ǥ�� ���� ���� */
	CSectorMgr::GetInstance()->Compare_exchange_Sector(id, (int)ori_y, (int)ori_x, (int)(pPlayer->m_vPos.y), (int)(pPlayer->m_vPos.x));


	/* �÷��̾� �ֺ��� ���͸� �˻� -> ������ ���� ���� ��ü���� ��ǥ ���� */
	unordered_set<pair<int, int>> nearSector;
	nearSector.reserve(5);
	CSectorMgr::GetInstance()->Get_NearSectorIndex(&nearSector, (int)pPlayer->m_vPos.x, (int)pPlayer->m_vPos.y);

	// �ش� �÷��̾��� ���ŵ� �þ� ���
	unordered_set<int> new_viewlist;

	// ���� ���� ��ȸ
	for (auto& s : nearSector)
	{
		// ���� ���� ���� Ÿ �������� �ִ��� �˻�
		if (!(CSectorMgr::GetInstance()->Get_SectorList()[s.first][s.second].Get_ObjList().empty()))
		{
			// Ÿ ������ ���� ��ȣ ����
			for (auto obj_num : CSectorMgr::GetInstance()->Get_SectorList()[s.first][s.second].Get_ObjList())
			{
				/* Ÿ������ ��� ó�� */
				if (obj_num == id) continue;
				if (true == CObjMgr::GetInstance()->Is_Player(obj_num))
				{
					CPlayer* pOther = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", obj_num));
					
					// ������ ������ �þ� ��Ͽ� ����Ѵ�.
					if (!pOther->Get_IsConnected()) continue;
					
					// �þ� ���� �ִٸ� �þ� ��Ͽ� ����Ѵ�.
					if (CObjMgr::GetInstance()->Is_Near(pPlayer, pOther))
						new_viewlist.insert(obj_num);			
				}
				/* NPC�� ��� ó�� */
				else if (true == CObjMgr::GetInstance()->Is_NPC(obj_num))
				{
					CNpc* pNPC = static_cast<CNpc*>(CObjMgr::GetInstance()->Get_GameObject(L"NPC", obj_num));

					// �þ� ���� ���ٸ� �þ� ��Ͽ� ���X.
					if (CObjMgr::GetInstance()->Is_Near(pPlayer, pNPC))
						new_viewlist.insert(obj_num);				
				}
				/* MONSTER�� ��� ó��*/
				else
				{
					int i = 0;
				}
			}
		}
	}

	/* ���ο� �þ� ��� ���� ��ü ó�� */
	for (int server_num : new_viewlist)
	{
		// �÷��̾� �þ� ��Ͽ� ���� ���� ��ü ó�� (���� �þ� ��Ͽ� ���ٸ�)
		if (0 == old_viewlist.count(server_num))
		{
			// �÷��̾� �þ� ��Ͽ� �߰�
			pPlayer->v_lock.lock();
			pPlayer->view_list.insert(server_num);
			pPlayer->v_lock.unlock();

			// ���ο� Ÿ������ �þ� ó��
			if (true == CObjMgr::GetInstance()->Is_Player(server_num))
			{
				// �÷��̾�('��')���� ���ο� ���� ���� ��Ŷ ����
				send_enter_packet(id, server_num);

				CPlayer* pOther = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", server_num));

				// Ÿ ������ �þ� ��� ó��
				pOther->v_lock.lock();
				// Ÿ ������ �þ� ��Ͽ� '��'�� ���� ���� ���
				if (0 == pOther->view_list.count(id))
				{
					pOther->view_list.insert(id);
					pOther->v_lock.unlock();
					send_enter_packet(server_num, id);
				}
				else
				{
					pOther->v_lock.unlock();
					send_move_packet(server_num, id);
				}
			}
			// ���� �þ߿� ���� NPC�� ��� ó��
			else if (true == CObjMgr::GetInstance()->Is_NPC(server_num))
			{
				// �÷��̾�('��')���� NPC���� ��Ŷ ����
				send_NPC_enter_packet(id, server_num);
			}
			// ���� �þ߿� ���� MONSTER�� ��� ó��
			else
			{
				int i = 0;
			}
		}
		// �÷��̾� �þ� ��Ͽ� ��� �ִ� ��ü ó��
		else
		{
			// Ÿ ���� ó��
			if (true == CObjMgr::GetInstance()->Is_Player(server_num))
			{
				CPlayer* pOther = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", server_num));

				// Ÿ ������ �þ� ��� ó��
				pOther->v_lock.lock();
				// Ÿ ������ �þ� ��Ͽ� '��'�� ��� �ִ� ���
				if (0 != pOther->view_list.count(id))
				{
					pOther->v_lock.unlock();
					send_move_packet(server_num, id);
				}
				// Ÿ ������ �þ� ��Ͽ� '��'�� ���� ���� ���
				else
				{
					pOther->view_list.insert(id);
					pOther->v_lock.unlock();
					send_enter_packet(server_num, id);
				}
			}
		} // ���濡�� ���� ���� ���� ���� .... ���߿�
	}

	/* ���� �þ� ��Ͽ��� ����� ��ü ó�� */
	for (int s_num : old_viewlist)
	{
		// ���ŵ� �þ� ��Ͽ� ���� ��ü�� ���
		if (0 == new_viewlist.count(s_num))
		{
			pPlayer->v_lock.lock();
			pPlayer->view_list.erase(s_num);
			pPlayer->v_lock.unlock();
			send_leave_packet(id, s_num);

			// Ÿ ���� ó��
			if (true == CObjMgr::GetInstance()->Is_Player(s_num))
			{
				CPlayer* pOther = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", s_num));

				// Ÿ ������ �þ� ��� ó��
				pOther->v_lock.lock();

				// Ÿ ������ �þ߿� '��'�� �ִٸ� ����
				if (0 != pOther->view_list.count(id))
				{
					pOther->view_list.erase(id);
					pOther->v_lock.unlock();
					send_leave_packet(s_num, id);
				}
				else pOther->v_lock.unlock();
			}
		}
	}
}

/*============================================NPC======================================================*/
void send_NPC_enter_packet(int to_client, int new_id)
{
	sc_packet_enter p;

	CNpc* pNewPlayer = static_cast<CNpc*>(CObjMgr::GetInstance()->Get_GameObject(L"NPC", new_id));

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
