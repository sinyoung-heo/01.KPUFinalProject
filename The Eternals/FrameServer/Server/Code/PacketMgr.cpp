#include "stdafx.h"
#include "PacketMgr.h"
#include "Player.h"
#include "Npc.h"
#include "Monster.h"

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
		strncpy_s(pPlayer->m_ID, p->name, strlen(p->name));
		
		pPlayer->Get_ClientLock().unlock();

#ifndef DUMMY
		/* CHECK ID in Database Server */
		if (false == CDBMgr::GetInstance()->Check_ID(id, p->password))
		{
			/* ó�� ���ӿ� ������ ������� ȸ������ ��� in Database Server */
			CDBMgr::GetInstance()->Insert_NewPlayer_DB(id, p->password);
		}
#endif // DUMMY

		/* �α��� ���� ��Ŷ ���� */
		send_login_ok(id);

		/* Sector �ٽ� ��� (���� �� �̸� �� �� �ϰ�����. �������� ���� �� �� �� ���(sector�� Key�� Unique��) */
		CSectorMgr::GetInstance()->Enter_ClientInSector(id, (int)(pPlayer->m_vPos.z / SECTOR_SIZE), (int)(pPlayer->m_vPos.x / SECTOR_SIZE));

		/* �ش� �÷��̾ ��ϵǾ� �ִ� ���� ���� �����鿡�� ������ �˸� */
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
					else if (true == CObjMgr::GetInstance()->Is_Monster(obj_num))
					{
						CMonster* pMonster = static_cast<CMonster*>(CObjMgr::GetInstance()->Get_GameObject(L"MONSTER", obj_num));

						// �þ� ���� ���ٸ� �þ� ��Ͽ� ���X
						if (false == CObjMgr::GetInstance()->Is_Near(pPlayer, pMonster)) continue;
						pPlayer->v_lock.lock();
						pPlayer->view_list.insert(obj_num);
						pPlayer->v_lock.unlock();
						
						pMonster->send_Monster_enter_packet(id);
					}
				}
			}
		}
	}
	break;

	case CS_MOVE:
	{
		cs_packet_move* p = reinterpret_cast<cs_packet_move*>(pPlayer->m_packet_start);
		
		pPlayer->move_time = p->move_time;
		_vec3 vPos = _vec3(p->posX, p->posY, p->posZ);
		_vec3 vDir = _vec3(p->dirX, p->dirY, p->dirZ);

		process_move(id, vDir, vPos);
	}
	break;

	case CS_MOVE_STOP:
	{
		cs_packet_move_stop* p = reinterpret_cast<cs_packet_move_stop*>(pPlayer->m_packet_start);

		_vec3 vPos = _vec3(p->posX, p->posY, p->posZ);
		_vec3 vDir = _vec3(p->dirX, p->dirY, p->dirZ);

		process_move_stop(id, vPos, vDir);
	}
	break;

	case CS_ATTACK:
		break;
	case CS_CHAT:
		break;
	case CS_LOGOUT:
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
	strncpy_s(p.name, pNewPlayer->m_ID, strlen(pNewPlayer->m_ID));

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

	p.move_time = pPlayer->move_time;

	p.posX = pPlayer->m_vTempPos.x;
	p.posY = pPlayer->m_vTempPos.y;
	p.posZ = pPlayer->m_vTempPos.z;

	p.dirX = pPlayer->m_vDir.x;
	p.dirY = pPlayer->m_vDir.y;
	p.dirZ = pPlayer->m_vDir.z;

	send_packet(to_client, &p);
}

void send_move_stop_packet(int to_client, int id)
{
	sc_packet_move p;

	CPlayer* pPlayer = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", id));

	if (pPlayer == nullptr) return;

	p.size = sizeof(p);
	p.type = SC_PACKET_MOVE_STOP;
	p.id = id;

	p.move_time = pPlayer->move_time;

	p.posX = pPlayer->m_vPos.x;
	p.posY = pPlayer->m_vPos.y;
	p.posZ = pPlayer->m_vPos.z;

	p.dirX = pPlayer->m_vDir.x;
	p.dirY = pPlayer->m_vDir.y;
	p.dirZ = pPlayer->m_vDir.z;

	send_packet(to_client, &p);
}

void process_move(int id, const _vec3& _vDir, const _vec3& _vPos)
{
	CPlayer* pPlayer = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", id));

	if (pPlayer == nullptr) return;

	/* �ش� �÷��̾��� ���� ��ġ�� & ����� ��ġ�� */
	float ori_x, ori_y, ori_z;	
	ori_x = pPlayer->m_vPos.x;
	ori_y = pPlayer->m_vPos.y;
	ori_z = pPlayer->m_vPos.z;

	/* �ش� �÷��̾��� ���⺤�Ϳ� ��ġ���� */
	pPlayer->m_vDir = _vDir;
	pPlayer->m_vPos = _vPos;
	
	/* �ش� �÷��̾��� ���� �þ� ��� */
	pPlayer->v_lock.lock();
	unordered_set<int> old_viewlist = pPlayer->view_list;
	pPlayer->v_lock.unlock();

	/* �ش� �÷��̾��� �̷� ��ġ ��ǥ ���� -> �̷� ��ġ��ǥ�� �ӽ� ������ ���� */
	_vec2 coll_pos = _vec2(0.f);

	if (CCollisionMgr::GetInstance()->Is_DeadReckoning(pPlayer->m_vPos, pPlayer->m_vDir, &coll_pos))
	{
		pPlayer->m_vTempPos.x = coll_pos.x;
		pPlayer->m_vTempPos.y = 0.f;
		pPlayer->m_vTempPos.z = coll_pos.y;
	}

	send_move_packet(id, id);

	/* ����� ��ǥ�� ���� ���� */
	CSectorMgr::GetInstance()->Compare_exchange_Sector(id, (int)ori_z, (int)ori_x, (int)(pPlayer->m_vPos.z), (int)(pPlayer->m_vPos.x));

	/* �÷��̾� �ֺ��� ���͸� �˻� -> ������ ���� ���� ��ü���� ��ǥ ���� */
	unordered_set<pair<int, int>> nearSector;
	nearSector.reserve(5);
	CSectorMgr::GetInstance()->Get_NearSectorIndex(&nearSector, (int)pPlayer->m_vPos.x, (int)pPlayer->m_vPos.z);

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
					{
						new_viewlist.insert(obj_num);
						active_npc(obj_num);
					}
				}
				/* MONSTER�� ��� ó��*/
				else if (true == CObjMgr::GetInstance()->Is_Monster(obj_num))
				{
					CMonster* pMonster = static_cast<CMonster*>(CObjMgr::GetInstance()->Get_GameObject(L"MONSTER", obj_num));

					// �þ� ���� ���ٸ� �þ� ��Ͽ� ���X.
					if (CObjMgr::GetInstance()->Is_Near(pPlayer, pMonster))
					{
						new_viewlist.insert(obj_num);
						active_monster(obj_num);
					}
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
			else if (true == CObjMgr::GetInstance()->Is_Monster(server_num))
			{
				CMonster* pMonster = static_cast<CMonster*>(CObjMgr::GetInstance()->Get_GameObject(L"MONSTER", server_num));
				// �÷��̾�('��')���� Monster ���� ��Ŷ ����
				pMonster->send_Monster_enter_packet(id);
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

#ifdef TEST
	/* �÷��̾� �þ� ���� �ִ� ��ü ��� */
	for (int server_obj : new_viewlist)
		cout << "move" << server_obj << "�þ� ���� �����մϴ�." << endl;
#endif
}

void process_move_stop(int id, const _vec3& _vPos, const _vec3& _vDir)
{
	CPlayer* pPlayer = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", id));

	if (pPlayer == nullptr) return;

	/* �ش� �÷��̾��� ���� ��ġ�� & ����� ��ġ�� */
	float ori_x, ori_y, ori_z;

	ori_x = pPlayer->m_vPos.x;
	ori_y = pPlayer->m_vPos.y;
	ori_z = pPlayer->m_vPos.z;

	pPlayer->m_vDir = _vDir;

	/* �ش� �÷��̾��� ���� �þ� ��� */
	pPlayer->v_lock.lock();
	unordered_set<int> old_viewlist = pPlayer->view_list;
	pPlayer->v_lock.unlock();

	/* �ش� �÷��̾�κ��� ���� ���� ��ġ�� ���� */
	pPlayer->m_vPos.x = _vPos.x;
	pPlayer->m_vPos.y = _vPos.y;
	pPlayer->m_vPos.z = _vPos.z;

	send_move_stop_packet(id, id);

	/* ����� ��ǥ�� ���� ���� */
	CSectorMgr::GetInstance()->Compare_exchange_Sector(id, (int)ori_z, (int)ori_x, (int)(pPlayer->m_vPos.z), (int)(pPlayer->m_vPos.x));

	/* �÷��̾� �ֺ��� ���͸� �˻� -> ������ ���� ���� ��ü���� ��ǥ ���� */
	unordered_set<pair<int, int>> nearSector;
	nearSector.reserve(5);
	CSectorMgr::GetInstance()->Get_NearSectorIndex(&nearSector, (int)pPlayer->m_vPos.x, (int)pPlayer->m_vPos.z);

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
					{
						new_viewlist.insert(obj_num);
						active_npc(obj_num);
					}
				}
				/* MONSTER�� ��� ó��*/
				else if (true == CObjMgr::GetInstance()->Is_Monster(obj_num))
				{
					CMonster* pMonster = static_cast<CMonster*>(CObjMgr::GetInstance()->Get_GameObject(L"MONSTER", obj_num));

					// �þ� ���� ���ٸ� �þ� ��Ͽ� ���X.
					if (CObjMgr::GetInstance()->Is_Near(pPlayer, pMonster))
					{
						new_viewlist.insert(obj_num);
						active_monster(obj_num);
					}
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
					send_move_stop_packet(server_num, id);
				}
			}
			// ���� �þ߿� ���� NPC�� ��� ó��
			else if (true == CObjMgr::GetInstance()->Is_NPC(server_num))
			{
				// �÷��̾�('��')���� NPC���� ��Ŷ ����
				send_NPC_enter_packet(id, server_num);
			}
			// ���� �þ߿� ���� MONSTER�� ��� ó��
			else if (true == CObjMgr::GetInstance()->Is_Monster(server_num))
			{
				CMonster* pMonster = static_cast<CMonster*>(CObjMgr::GetInstance()->Get_GameObject(L"MONSTER", server_num));
				// �÷��̾�('��')���� Monster ���� ��Ŷ ����
				pMonster->send_Monster_enter_packet(id);
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
					send_move_stop_packet(server_num, id);
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

#ifdef TEST
	/* �÷��̾� �þ� ���� �ִ� ��ü ��� */
	for (int server_obj : new_viewlist)
		cout << "move_stop" << server_obj << "�þ� ���� �����մϴ�." << endl;
#endif
}

/*============================================NPC======================================================*/
void send_NPC_enter_packet(int to_client, int new_id)
{
	sc_packet_npc_enter p;

	CNpc* pNewNPC = static_cast<CNpc*>(CObjMgr::GetInstance()->Get_GameObject(L"NPC", new_id));

	if (pNewNPC == nullptr) return;

	p.size = sizeof(p);
	p.type = SC_PACKET_NPC_ENTER;
	p.id = new_id;

	pNewNPC->Get_ClientLock().lock();
	strncpy_s(p.name, pNewNPC->m_ID, strlen(pNewNPC->m_ID));
	strncpy_s(p.naviType, pNewNPC->m_naviType, strlen(pNewNPC->m_naviType));

	pNewNPC->Get_ClientLock().unlock();

	p.o_type = pNewNPC->m_type;
	p.npc_num = pNewNPC->m_npcNum;

	p.posX = pNewNPC->m_vPos.x;
	p.posY = pNewNPC->m_vPos.y;
	p.posZ = pNewNPC->m_vPos.z;

	p.angleX = pNewNPC->m_vAngle.x;
	p.angleY = pNewNPC->m_vAngle.y;
	p.angleZ = pNewNPC->m_vAngle.z;

	send_packet(to_client, &p);
}

void send_NPC_move_packet(int to_client, int id)
{
	sc_packet_move p;

	CNpc* pNPC = static_cast<CNpc*>(CObjMgr::GetInstance()->Get_GameObject(L"NPC", id));

	if (pNPC == nullptr) return;

	p.size = sizeof(p);
	p.type = SC_PACKET_NPC_MOVE;
	p.id = id;

	p.posX = pNPC->m_vTempPos.x;
	p.posY = pNPC->m_vTempPos.y;
	p.posZ = pNPC->m_vTempPos.z;

	p.dirX = pNPC->m_vDir.x;
	p.dirY = pNPC->m_vDir.y;
	p.dirZ = pNPC->m_vDir.z;
	
	send_packet(to_client, &p);
}

void random_move_npc(int id)
{
	CNpc* pNPC = static_cast<CNpc*>(CObjMgr::GetInstance()->Get_GameObject(L"NPC", id));

	if (pNPC == nullptr) return;

	/* �ش� NPC�� ���� ��ġ�� */
	float ori_x, ori_y, ori_z;
	ori_x = pNPC->m_vPos.x;
	ori_y = pNPC->m_vPos.y;
	ori_z = pNPC->m_vPos.z;

	// �����̱� �� ��ġ������ viewlist (�þ� ���� �÷��̾� ����)
	unordered_set<pair<int, int>> oldnearSector;
	oldnearSector.reserve(5);
	CSectorMgr::GetInstance()->Get_NearSectorIndex(&oldnearSector, (int)ori_x, (int)ori_z);

	unordered_set <int> old_viewlist; 

	// �̵� ��: ���� ���� ��ȸ
	for (auto& s : oldnearSector)
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
					CPlayer* pPlayer = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", obj_num));

					// ������ ������ �þ� ��Ͽ� ����Ѵ�.
					if (!pPlayer->Get_IsConnected()) continue;

					// �þ� ���� �ִٸ� �þ� ��Ͽ� ����Ѵ�.
					if (CObjMgr::GetInstance()->Is_Near(pNPC, pPlayer))
						old_viewlist.insert(obj_num);
				}
			}
		}
	}

	/* NPC ������ ó�� */
	switch (rand() % 8)
	{
	case 0: pNPC->m_vDir = _vec3(0.f, 0.f, 1.f); break;
	case 1: pNPC->m_vDir = _vec3(0.f, 0.f, -1.f); break;
	case 2: pNPC->m_vDir = _vec3(1.f, 0.f, 0.f); break;
	case 3: pNPC->m_vDir = _vec3(1.f, 0.f, 1.f); break;
	case 4: pNPC->m_vDir = _vec3(1.f, 0.f, -1.f); break;
	case 5: pNPC->m_vDir = _vec3(-1.f, 0.f, 0.f); break;
	case 6: pNPC->m_vDir = _vec3(-1.f, 0.f, 1.f); break;
	case 7: pNPC->m_vDir = _vec3(-1.f, 0.f, -1.f); break;
	}

	/* �ش� NPC�� �̷� ��ġ ��ǥ ���� -> �̷� ��ġ��ǥ�� �ӽ� ������ ���� */
	pNPC->m_vTempPos += pNPC->m_vDir * 3.f;

	/* NaviMesh�� ��� ��� ������ X */
	if (CNaviMesh::GetInstance()->Get_CurrentPositionCellIndex(pNPC->m_vTempPos) == -1)
	{
		pNPC->m_vTempPos = pNPC->m_vPos;
		add_timer(id, OPMODE::OP_RANDOM_MOVE_NPC, system_clock::now() + 15s);
		return;
	}

	/* ����� ��ǥ�� ���� ���� */
	CSectorMgr::GetInstance()->Compare_exchange_Sector(id, (int)ori_z, (int)ori_x, (int)(pNPC->m_vPos.z), (int)(pNPC->m_vPos.x));

	// ������ �� ��ġ������ viewlist (�þ� ���� �÷��̾� ����)
	unordered_set <int> new_viewlist;

	unordered_set<pair<int, int>> nearSectors;
	nearSectors.reserve(5);
	CSectorMgr::GetInstance()->Get_NearSectorIndex(&nearSectors, (int)(pNPC->m_vPos.x), (int)(pNPC->m_vPos.z));

	// �̵� ��: ���� ���� ��ȸ -> ������ ���� �� new viewlist ���� ���
	for (auto& s : nearSectors)
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
					CPlayer* pPlayer = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", obj_num));

					// ������ ������ �þ� ��Ͽ� ����Ѵ�.
					if (!pPlayer->Get_IsConnected()) continue;

					// �þ� ���� �ִٸ� �þ� ��Ͽ� ����Ѵ�.
					if (CObjMgr::GetInstance()->Is_Near(pNPC, pPlayer))
						new_viewlist.insert(obj_num);
				}
			}
		}
	}

	/* ������ �� ���� NPC�� �þ� ��� �ȿ� ������ �ִ� ��� -> true */
	bool isInContinue = false; 

	// �̵� �� viewlist & �̵� �� viewlist �� -> �� �������� �þ� ��� ���� NPC ���� ���θ� ����.
	for (auto pl : old_viewlist)
	{
		// �̵� �Ŀ��� NPC �þ� ��� ���� "pl"(server number) ������ �����ִ� ���
		if (0 < new_viewlist.count(pl))
		{
			// ���� NPC�� ��� � ������ �þ� ��Ͽ� �־�� ��.
			isInContinue = true;

			CPlayer* pPlayer = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", pl));
			if (pPlayer != nullptr)
			{
				/* �ش� ������ �þ� ��Ͽ� ���� NPC�� ������ ��� */
				pPlayer->v_lock.lock();
				if (0 < pPlayer->view_list.count(id))
				{
					pPlayer->v_lock.unlock();
					/* �ش� �������� NPC�� ������ ���� ��ġ�� ���� */
					send_NPC_move_packet(pl, id);
				}
				/* �ش� ������ �þ� ��Ͽ� ���� NPC�� �������� ���� ��� */
				else
				{
					/* �ش� ������ �þ� ��Ͽ� ���� NPC ��� */
					pPlayer->view_list.insert(id);
					pPlayer->v_lock.unlock();
					send_NPC_enter_packet(pl, id);

					/* �ش� �������� NPC�� ������ ���� ��ġ�� ���� */
					send_NPC_move_packet(pl, id);
				}
			}
		}
		// �̵� �Ŀ� NPC �þ� ��� ���� "pl"(server number) ������ ���� ���
		else
		{
			CPlayer* pPlayer = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", pl));
			if (pPlayer != nullptr)
			{
				/* �ش� ������ �þ� ��Ͽ� ���� NPC�� ������ ��� */
				pPlayer->v_lock.lock();
				if (0 < pPlayer->view_list.count(id))
				{
					/* �ش� ������ �þ� ��Ͽ��� ���� NPC ���� */
					pPlayer->view_list.erase(id);
					pPlayer->v_lock.unlock();
					send_leave_packet(pl, id);
				}
				else
					pPlayer->v_lock.unlock();
			}
		}
	}

	// new_vielist ��ȸ -> �÷��̾��� �þ� ��Ͽ� �־�� �� ���ο� npc���� �߰�
	for (auto pl : new_viewlist)
	{
		CPlayer* pPlayer = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", pl));
		if (pPlayer != nullptr)
		{
			pPlayer->v_lock.lock();			
			if (0 == pPlayer->view_list.count(pl))
			{
				/* �� ������ �þ� ��� ���� ���� NPC�� ���� ��� -> ���� NPC ��� */
				if (0 == pPlayer->view_list.count(id))
				{
					pPlayer->view_list.insert(id);
					pPlayer->v_lock.unlock();
					send_NPC_enter_packet(pl, id);
				}
				/* �� ������ �þ� ��� ���� ���� NPC�� ���� ��� -> ���� NPC ��ġ ���� */
				else
				{
					pPlayer->v_lock.unlock();
					send_NPC_move_packet(pl, id);
				}
			}
			else
				pPlayer->v_lock.unlock();
		}
		
	}

	/* ���� ������ ��� ���� */
	pNPC->m_vPos = pNPC->m_vTempPos;
	add_timer(id, OPMODE::OP_RANDOM_MOVE_NPC, system_clock::now() + 15s);
}

void active_npc(int id)
{
	CNpc* pNPC = static_cast<CNpc*>(CObjMgr::GetInstance()->Get_GameObject(L"NPC", id));
	
	if (nullptr == pNPC) return;
	
	/* NPC�� Ȱ��ȭ�Ǿ� ���� ���� ��� Ȱ��ȭ */
	if (pNPC->m_status != ST_ACTIVE)
	{
		STATUS prev_state = pNPC->m_status;
		if (true == atomic_compare_exchange_strong(&pNPC->m_status, &prev_state, ST_ACTIVE))
		{
			switch (pNPC->m_npcNum)
			{
			case NPC_NORMAL:
				add_timer(id, OP_RANDOM_MOVE_NPC, system_clock::now() + 1s);
				break;
			case NPC_MERCHANT:
				break;
			case NPC_QUEST:
				break;
			case NPC_BG:
				break;
			}
		
		}
	}
}
/*============================================MONSTER======================================================*/
void active_monster(int id)
{
	CMonster* pMonster = static_cast<CMonster*>(CObjMgr::GetInstance()->Get_GameObject(L"MONSTER", id));

	if (nullptr == pMonster) return;

	/* Monster�� Ȱ��ȭ�Ǿ� ���� ���� ��� Ȱ��ȭ */
	if (pMonster->m_status != ST_ACTIVE)
	{
		STATUS prev_state = pMonster->m_status;
		atomic_compare_exchange_strong(&pMonster->m_status, &prev_state, ST_ACTIVE);	
	}
}

void nonActive_monster(int id)
{
	CMonster* pMonster = static_cast<CMonster*>(CObjMgr::GetInstance()->Get_GameObject(L"MONSTER", id));

	if (nullptr == pMonster) return;

	if (pMonster->m_status != ST_NONACTIVE)
	{
		STATUS prev_state = pMonster->m_status;
		atomic_compare_exchange_strong(&pMonster->m_status, &prev_state, ST_NONACTIVE);
	}
}

/*===========================================FUNC====================================================*/
void add_timer(int obj_id, OPMODE ev_type, system_clock::time_point t)
{
	g_timer_lock.lock();
	g_timer_queue.emplace(obj_id, t, ev_type);
	g_timer_lock.unlock();
}

bool CAS(atomic<STATUS>* addr, STATUS* old_v, STATUS new_v)
{
	return atomic_compare_exchange_strong(addr, old_v, new_v);
}
