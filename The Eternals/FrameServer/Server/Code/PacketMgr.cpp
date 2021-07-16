#include "stdafx.h"
#include "PacketMgr.h"
#include "Player.h"
#include "Npc.h"
#include "Monster.h"
#include "Ai.h"

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

		bool bIsLoadItem = false;
		// ȸ���� ��� DB �˻�
		if (p->isMember)
		{
			/* CHECK ID in Database Server */
			/* ��ȸ�� -> ȸ�� ���� */
			if (false == CDBMgr::GetInstance()->Check_ID(id, p->password))
			{
				CDBMgr::GetInstance()->Insert_NewPlayer_DB(id, p->password);
				CDBMgr::GetInstance()->ready_Equipment(id);
			}
			/* ���� ȸ���� ��� ��� �� �κ��丮 Load */
			else
			{
				bIsLoadItem = true;
				// Load Equipment
				CDBMgr::GetInstance()->Load_Equipment(id);
				// Load Inventory
				CDBMgr::GetInstance()->Load_Inventory(id);
			}
		}
		// ��ȸ���� ���
		else
		{
			pPlayer->m_type = p->o_type;
			pPlayer->m_chWeaponType = p->weapon_type;
		}

		/* �α��� ���� ��Ŷ ���� */
		pPlayer->send_login_ok();

		/* ���â �� �κ��丮 ���� ���� */
		if (bIsLoadItem)
			pPlayer->send_load_InventoryAndEquipment();
		
		/* Sector �ٽ� ��� (���� �� �̸� �� �� �ϰ�����. �������� ���� �� �� �� ���(sector�� Key�� Unique��) */
		CSectorMgr::GetInstance()->Enter_ClientInSector(id, (int)(pPlayer->m_vPos.z / SECTOR_SIZE), (int)(pPlayer->m_vPos.x / SECTOR_SIZE));

		/* �ش� �÷��̾ ��ϵǾ� �ִ� ���� ���� �����鿡�� ������ �˸� */
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
								pPlayer->send_enter_packet(obj_num);
							}
							else pOther->v_lock.unlock();

							/* '��'�� �þ߿� Ÿ ������ ���� ��� -> Ÿ ���� ��� */
							pPlayer->v_lock.lock();
							if (0 == pPlayer->view_list.count(obj_num))
							{
								pPlayer->view_list.insert(obj_num);
								pPlayer->v_lock.unlock();
								pOther->send_enter_packet(id);								
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
						pNPC->send_NPC_enter_packet(id);
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

		//// Stage Winter�� ���
		//if (pPlayer->m_chStageId == STAGE_WINTER)
		//{
		//	CMonster* pMonster = static_cast<CMonster*>(CObjMgr::GetInstance()->Get_GameObject(L"MONSTER", g_iVergosServerNum));
		//	if (pMonster->Get_Dead() == false)
		//	{
		//		pPlayer->v_lock.lock();
		//		pPlayer->view_list.insert(g_iVergosServerNum);
		//		pPlayer->v_lock.unlock();

		//		pMonster->send_Monster_enter_packet(id);
		//	}			
		//}
	}
	break;

	case CS_MOVE:
	{
		cs_packet_move* p = reinterpret_cast<cs_packet_move*>(pPlayer->m_packet_start);

		pPlayer->move_time = p->move_time;
		pPlayer->m_iAniIdx = p->animIdx;
		_vec3 vPos = _vec3(p->posX, p->posY, p->posZ);
		_vec3 vDir = _vec3(p->dirX, p->dirY, p->dirZ);

		process_move(id, vDir, vPos);
	}
	break;

	case CS_MOVE_STOP:
	{
		cs_packet_move_stop* p = reinterpret_cast<cs_packet_move_stop*>(pPlayer->m_packet_start);

		pPlayer->m_iAniIdx = p->animIdx;
		_vec3 vPos = _vec3(p->posX, p->posY, p->posZ);
		_vec3 vDir = _vec3(p->dirX, p->dirY, p->dirZ);

		process_move_stop(id, vPos, vDir);
	}
	break;

	case CS_STANCE_CHANGE:
	{
		cs_packet_stance_change* p = reinterpret_cast<cs_packet_stance_change*>(pPlayer->m_packet_start);

		pPlayer->m_iAniIdx = p->animIdx;
		pPlayer->m_bIsAttackStance = p->is_stance_attack;
		process_stance_change(id, p->is_stance_attack);
	}
	break;

	case CS_ATTACK:
	{
		cs_packet_attack* p = reinterpret_cast<cs_packet_attack*>(pPlayer->m_packet_start);

		int		iAniIdx = p->animIdx;
		_vec3	vPos = _vec3(p->posX, p->posY, p->posZ);
		_vec3	vDir = _vec3(p->dirX, p->dirY, p->dirZ);
		float	fEndAngleY = p->end_angleY;

		process_attack(id, vDir, vPos, iAniIdx, fEndAngleY); // ��ų or �⺻ ���� �ִϸ��̼� �ε��� �ο� �ʿ���
	}
	break;

	case CS_ATTACK_STOP:
	{
		cs_packet_attack* p = reinterpret_cast<cs_packet_attack*>(pPlayer->m_packet_start);

		_vec3 vPos = _vec3(p->posX, p->posY, p->posZ);
		_vec3 vDir = _vec3(p->dirX, p->dirY, p->dirZ);

		process_attack_stop(id, vDir, vPos, p->animIdx);
	}
	break;

	case CS_CHAT:
	{
		cs_packet_chat* p = reinterpret_cast<cs_packet_chat*>(pPlayer->m_packet_start);

		process_chat(id, p->message);
	}
	break;

	case CS_LOGOUT:
	{
		process_disconnect(id);
	}
	break;

	case CS_COLLIDE:
	{
		cs_packet_player_collision* p = reinterpret_cast<cs_packet_player_collision*>(pPlayer->m_packet_start);
		process_collide(id, p->col_id, p->damage);
	}
	break;

	case CS_COLLIDE_MONSTER:
	{
		cs_packet_player_collision* p = reinterpret_cast<cs_packet_player_collision*>(pPlayer->m_packet_start);

		CMonster* pMonster = static_cast<CMonster*>(CObjMgr::GetInstance()->Get_GameObject(L"MONSTER", p->col_id));
		if (nullptr == pMonster) return;

		if (pMonster->m_monNum == MON_VERGOS)
			pMonster->Hurt_Vergos(id, p->damage, p->affect);
		else
			pMonster->Hurt_Monster(id, p->damage, p->affect);
	}
	break;

	case CS_STAGE_CHANGE:
	{
		cs_packet_stage_change* p = reinterpret_cast<cs_packet_stage_change*>(pPlayer->m_packet_start);
		process_stage_change(id, p->stage_id);
	}
	break;

	case CS_SUGGEST_PARTY:
	{
		cs_packet_suggest_party* p = reinterpret_cast<cs_packet_suggest_party*>(pPlayer->m_packet_start);
		process_suggest_party(id, p->member_id);
	}
	break;

	case CS_RESPOND_PARTY:
	{
		cs_packet_respond_party* p = reinterpret_cast<cs_packet_respond_party*>(pPlayer->m_packet_start);
		process_respond_party(p->result, p->suggester_id, id);
	}
	break;

	case CS_JOIN_PARTY:
	{
		cs_packet_suggest_party* p = reinterpret_cast<cs_packet_suggest_party*>(pPlayer->m_packet_start);
		process_join_party(id, p->member_id);
	}
	break;

	case CS_DECIDE_PARTY:
	{
		cs_packet_respond_party* p = reinterpret_cast<cs_packet_respond_party*>(pPlayer->m_packet_start);
		process_decide_party(p->result, p->suggester_id, id);
	}
	break;

	case CS_LEAVE_PARTY:
	{
		cs_packet_suggest_party* p = reinterpret_cast<cs_packet_suggest_party*>(pPlayer->m_packet_start);
		process_leave_party(id);
	}
	break;

	case CS_ADD_ITEM:
	{
		cs_packet_manage_inventory* p = reinterpret_cast<cs_packet_manage_inventory*>(pPlayer->m_packet_start);
		process_add_item(id, p->itemType, p->itemName);
	}
	break;

	case CS_DELETE_ITEM:
	{
		cs_packet_manage_inventory* p = reinterpret_cast<cs_packet_manage_inventory*>(pPlayer->m_packet_start);
		process_delete_item(id, p->itemType, p->itemName);
	}
	break;

	case CS_EQUIP_ITEM:
	{
		cs_packet_manage_inventory* p = reinterpret_cast<cs_packet_manage_inventory*>(pPlayer->m_packet_start);
		process_equip_item(id, p->itemSlotType, p->itemType, p->itemName);
	}
	break;

	case CS_UNEQUIP_ITEM:
	{
		cs_packet_manage_inventory* p = reinterpret_cast<cs_packet_manage_inventory*>(pPlayer->m_packet_start);
		process_unequip_item(id, p->itemSlotType, p->itemType, p->itemName);
	}
	break;

	case CS_SHOP:
	{
		cs_packet_shop* p = reinterpret_cast<cs_packet_shop*>(pPlayer->m_packet_start);		
		process_shopping(id, p);
	}
	break;

	case CS_BUFF:
	{
		cs_packet_attack* p = reinterpret_cast<cs_packet_attack*>(pPlayer->m_packet_start);
		process_buff(id, p);
	}
	break;

	case CS_DRINK_POTION:
	{
		cs_packet_potion* p = reinterpret_cast<cs_packet_potion*>(pPlayer->m_packet_start);
		process_use_potion(id, p->bIsPotionHP);
	}
	break;

	case CS_END_CINEMA:
	{
		cs_packet_potion* p = reinterpret_cast<cs_packet_potion*>(pPlayer->m_packet_start);
		process_cinema_end(id);
	}
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
	if (pPlayer != nullptr && pPlayer->Get_IsConnected())
	{
		pPlayer->Get_ClientLock().lock();
		WSASend(pPlayer->m_sock, &send_over->wsa_buf, 1, NULL, 0, &send_over->wsa_over, NULL);
		pPlayer->Get_ClientLock().unlock();
	}	
}

void send_leave_packet(const int& to_client, const int& leave_id)
{
	sc_packet_leave p;

	p.size	= sizeof(p);
	p.type	= SC_PACKET_LEAVE;
	p.id	= leave_id;

	send_packet(to_client, &p);
}

void send_suggest_party(const int& to_client, const int& id)
{
	sc_packet_suggest_party p;

	p.size	= sizeof(p);
	p.type	= SC_PACKET_SUGGEST_PARTY;
	p.id	= id;

	send_packet(to_client, &p);
}

void send_reject_party(const int& to_client, const int& id)
{
	sc_packet_chat p;

	p.size	= sizeof(p);
	p.type	= SC_PACKET_REJECT_PARTY;
	p.id	= id;

	strncpy_s(p.name, "0", strlen("0"));
	strncpy_s(p.message, "��Ƽ ������ �����Ͽ����ϴ�.", strlen("��Ƽ ������ �����Ͽ����ϴ�."));

	send_packet(to_client, &p);
}

void send_join_party(const int& to_client, const int& id)
{
	sc_packet_suggest_party p;

	p.size	= sizeof(p);
	p.type	= SC_PACKET_JOIN_PARTY;
	p.id	= id;

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

	if (CCollisionMgr::GetInstance()->Is_DeadReckoning(pPlayer->m_vPos, pPlayer->m_vDir, &coll_pos, pPlayer->m_chStageId))
	{
		pPlayer->m_vTempPos.x = coll_pos.x;
		pPlayer->m_vTempPos.y = 0.f;
		pPlayer->m_vTempPos.z = coll_pos.y;
	}

	pPlayer->send_move_packet(id);

	/* ����� ��ǥ�� ���� ���� */
	CSectorMgr::GetInstance()->Compare_exchange_Sector(id, (int)ori_z, (int)ori_x, (int)(pPlayer->m_vPos.z), (int)(pPlayer->m_vPos.x));

	/* �÷��̾� �ֺ��� ���͸� �˻� -> ������ ���� ���� ��ü���� ��ǥ ���� */
	unordered_set<pair<int, int>> nearSector;
	nearSector.reserve(NEAR_SECTOR);
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
						pNPC->wakeUp_npc();
					}
				}
				/* MONSTER�� ��� ó��*/
				else if (true == CObjMgr::GetInstance()->Is_Monster(obj_num))
				{
					CMonster* pMonster = static_cast<CMonster*>(CObjMgr::GetInstance()->Get_GameObject(L"MONSTER", obj_num));
					if (pMonster->Get_Dead() == true) continue;

					// �þ� ���� ���ٸ� �þ� ��Ͽ� ���X.
					if (CObjMgr::GetInstance()->Is_Near(pPlayer, pMonster))
					{
						new_viewlist.insert(obj_num);
						pMonster->active_monster();
					}
				}
			}
		}
	}

	// Stage Winter�� ���
	if (pPlayer->m_chStageId == STAGE_WINTER)
	{
		CMonster* pMonster = static_cast<CMonster*>(CObjMgr::GetInstance()->Get_GameObject(L"MONSTER", g_iVergosServerNum));
		if (pMonster != nullptr && pMonster->Get_Dead() == false)
		{
			new_viewlist.insert(g_iVergosServerNum);
		}

		auto& iter_begin = CObjMgr::GetInstance()->Get_OBJLIST(L"AI")->begin();
		auto& iter_end = CObjMgr::GetInstance()->Get_OBJLIST(L"AI")->end();
		for (iter_begin; iter_begin != iter_end; ++iter_begin)
		{
			new_viewlist.insert(iter_begin->second->m_sNum);
		}
	}

	/* ���ο� �þ� ��� ���� ��ü ó�� */
	for (const int& server_num : new_viewlist)
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
				CPlayer* pOther = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", server_num));
				
				// �÷��̾�('��')���� ���ο� ���� ���� ��Ŷ ����
				pOther->send_enter_packet(id);

				// Ÿ ������ �þ� ��� ó��
				pOther->v_lock.lock();
				// Ÿ ������ �þ� ��Ͽ� '��'�� ���� ���� ���
				if (0 == pOther->view_list.count(id))
				{
					pOther->view_list.insert(id);
					pOther->v_lock.unlock();
					pPlayer->send_enter_packet(server_num);
				}
				else
				{
					pOther->v_lock.unlock();
					pPlayer->send_move_packet(server_num);
				}
			}
			// ���� �þ߿� ���� NPC�� ��� ó��
			else if (true == CObjMgr::GetInstance()->Is_NPC(server_num))
			{
				// �÷��̾�('��')���� NPC���� ��Ŷ ����
				CNpc* pNPC = static_cast<CNpc*>(CObjMgr::GetInstance()->Get_GameObject(L"NPC", server_num));
				pNPC->send_NPC_enter_packet(id);
			}
			// ���� �þ߿� ���� MONSTER�� ��� ó��
			else if (true == CObjMgr::GetInstance()->Is_Monster(server_num))
			{
				// �÷��̾�('��')���� Monster ���� ��Ŷ ����
				CMonster* pMonster = static_cast<CMonster*>(CObjMgr::GetInstance()->Get_GameObject(L"MONSTER", server_num));
				pMonster->send_Monster_enter_packet(id);
			}
			// ���� �þ߿� ���� AI�� ��� ó��
			else if (true == CObjMgr::GetInstance()->Is_AI(server_num))
			{
				// �÷��̾�('��')���� AI ���� ��Ŷ ����
				CAi* pAi = static_cast<CAi*>(CObjMgr::GetInstance()->Get_GameObject(L"AI", server_num));
				pAi->send_AI_enter_packet(id);
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
					pPlayer->send_move_packet(server_num);
				}
				// Ÿ ������ �þ� ��Ͽ� '��'�� ���� ���� ���
				else
				{
					pOther->view_list.insert(id);
					pOther->v_lock.unlock();
					pPlayer->send_enter_packet(server_num);
				}
			}
		} 
	}

	/* ���� �þ� ��Ͽ��� ����� ��ü ó�� */
	for (const int& s_num : old_viewlist)
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

	/* �ش� �÷��̾��� ���� �þ� ��� */
	pPlayer->v_lock.lock();
	unordered_set<int> old_viewlist = pPlayer->view_list;
	pPlayer->v_lock.unlock();

	/* �ش� �÷��̾�κ��� ���� ���� ��ġ�� ���� */
	pPlayer->m_vDir = _vDir;
	pPlayer->m_vPos = _vPos;

	pPlayer->send_move_stop_packet(id);

	/* ����� ��ǥ�� ���� ���� */
	CSectorMgr::GetInstance()->Compare_exchange_Sector(id, (int)ori_z, (int)ori_x, (int)(pPlayer->m_vPos.z), (int)(pPlayer->m_vPos.x));

	/* �÷��̾� �ֺ��� ���͸� �˻� -> ������ ���� ���� ��ü���� ��ǥ ���� */
	unordered_set<pair<int, int>> nearSector;
	nearSector.reserve(NEAR_SECTOR);
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
				if (obj_num == id) continue;
				/* Ÿ������ ��� ó�� */
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
						pNPC->wakeUp_npc();
					}
				}
				/* MONSTER�� ��� ó��*/
				else if (true == CObjMgr::GetInstance()->Is_Monster(obj_num))
				{
					CMonster* pMonster = static_cast<CMonster*>(CObjMgr::GetInstance()->Get_GameObject(L"MONSTER", obj_num));
					if (pMonster->Get_Dead() == true) continue;

					// �þ� ���� ���ٸ� �þ� ��Ͽ� ���X.
					if (CObjMgr::GetInstance()->Is_Near(pPlayer, pMonster))
					{
						new_viewlist.insert(obj_num);
						pMonster->active_monster();
					}
				}
			}
		}
	}

	// Stage Winter�� ���
	if (pPlayer->m_chStageId == STAGE_WINTER)
	{
		CMonster* pMonster = static_cast<CMonster*>(CObjMgr::GetInstance()->Get_GameObject(L"MONSTER", g_iVergosServerNum));
		if (pMonster != nullptr && pMonster->Get_Dead() == false)
		{
			new_viewlist.insert(g_iVergosServerNum);
		}

		auto& iter_begin = CObjMgr::GetInstance()->Get_OBJLIST(L"AI")->begin();
		auto& iter_end = CObjMgr::GetInstance()->Get_OBJLIST(L"AI")->end();
		for (iter_begin; iter_begin != iter_end; ++iter_begin)
		{
			new_viewlist.insert(iter_begin->second->m_sNum);
		}
	}

	/* ���ο� �þ� ��� ���� ��ü ó�� */
	for (const int& server_num : new_viewlist)
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
				CPlayer* pOther = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", server_num));
				
				// �÷��̾�('��')���� ���ο� ���� ���� ��Ŷ ����
				pOther->send_enter_packet(id);

				// Ÿ ������ �þ� ��� ó��
				pOther->v_lock.lock();
				// Ÿ ������ �þ� ��Ͽ� '��'�� ���� ���� ���
				if (0 == pOther->view_list.count(id))
				{
					pOther->view_list.insert(id);
					pOther->v_lock.unlock();
					pPlayer->send_enter_packet(server_num);
				}
				else
				{
					pOther->v_lock.unlock();
					pPlayer->send_move_stop_packet(server_num);
				}
			}
			// ���� �þ߿� ���� NPC�� ��� ó��
			else if (true == CObjMgr::GetInstance()->Is_NPC(server_num))
			{
				// �÷��̾�('��')���� NPC���� ��Ŷ ����
				CNpc* pNPC = static_cast<CNpc*>(CObjMgr::GetInstance()->Get_GameObject(L"NPC", server_num));
				pNPC->send_NPC_enter_packet(id);
			}
			// ���� �þ߿� ���� MONSTER�� ��� ó��
			else if (true == CObjMgr::GetInstance()->Is_Monster(server_num))
			{
				// �÷��̾�('��')���� Monster ���� ��Ŷ ����
				CMonster* pMonster = static_cast<CMonster*>(CObjMgr::GetInstance()->Get_GameObject(L"MONSTER", server_num));
				pMonster->send_Monster_enter_packet(id);
			}
			// ���� �þ߿� ���� AI�� ��� ó��
			else if (true == CObjMgr::GetInstance()->Is_AI(server_num))
			{
				// �÷��̾�('��')���� Monster ���� ��Ŷ ����
				CAi* pAi = static_cast<CAi*>(CObjMgr::GetInstance()->Get_GameObject(L"AI", server_num));
				pAi->send_AI_enter_packet(id);
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
					pPlayer->send_move_stop_packet(server_num);
				}
				// Ÿ ������ �þ� ��Ͽ� '��'�� ���� ���� ���
				else
				{
					pOther->view_list.insert(id);
					pOther->v_lock.unlock();
					pPlayer->send_enter_packet(server_num);
				}
			}
		}
	}

	/* ���� �þ� ��Ͽ��� ����� ��ü ó�� */
	for (const int& s_num : old_viewlist)
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

void process_collide(int id, int colID, int damage)
{
	CPlayer* pPlayer = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", id));
	if (pPlayer == nullptr) return;

	/* COLLIDE PLAYER - NPC */
	if (colID >= NPC_NUM_START && colID < MON_NUM_START) {}

	/* COLLIDE PLAYER - MONSTER */
	else if (colID >= MON_NUM_START)
	{
		CMonster* pMonster = static_cast<CMonster*>(CObjMgr::GetInstance()->Get_GameObject(L"MONSTER", colID));
		if (nullptr == pMonster) return;

		/* Decrease Player HP */
		if (pPlayer->m_iHp > ZERO_HP)
		{
			pPlayer->m_iHp -= damage;
			if (pPlayer->m_iHp <= ZERO_HP)
				pPlayer->m_iHp = 0;
		}
		else
		{
			/* Player Dead */
			pPlayer->m_iHp = ZERO_HP;
			//pPlayer->Set_IsDead(true);
		}

		/* �ش� �÷��̾��� �þ� ��� */
		pPlayer->v_lock.lock();
		unordered_set<int> viewlist = pPlayer->view_list;
		pPlayer->v_lock.unlock();

		/* �ش� �������� �ٲ� stat ���� */
		pPlayer->send_player_stat(id);

		/* �ش� ������ ��Ƽ�� ���ԵǾ� �ִ� ������ ��� ��Ƽ������ ���� */
		if (pPlayer->m_bIsPartyState)
		{
			for (auto& p : *CObjMgr::GetInstance()->Get_PARTYLIST(pPlayer->m_iPartyNumber))
			{
				if (p == id) continue;
				pPlayer->send_update_party(p);
			}
		}
	}
}

void process_attack(int id, const _vec3& _vDir, const _vec3& _vPos, int aniIdx, float end_angleY)
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

	if (CCollisionMgr::GetInstance()->Is_DeadReckoning(pPlayer->m_vPos, pPlayer->m_vDir, &coll_pos, pPlayer->m_chStageId))
	{	
		pPlayer->m_vTempPos.x = coll_pos.x;
		pPlayer->m_vTempPos.y = 0.f;
		pPlayer->m_vTempPos.z = coll_pos.y;

		if(pPlayer->m_type == PC_ARCHER)
		{
			if (aniIdx != Archer::BACK_DASH && aniIdx != Archer::ESCAPING_BOMB && aniIdx != Archer::CHARGE_ARROW_SHOT)
			{
				pPlayer->m_vTempPos.x = pPlayer->m_vPos.x;
				pPlayer->m_vTempPos.y = pPlayer->m_vPos.y;
				pPlayer->m_vTempPos.z = pPlayer->m_vPos.z;
			}		
		}	
	}

	pPlayer->send_attack_packet(id, aniIdx, end_angleY);

	/* ����� ��ǥ�� ���� ���� */
	CSectorMgr::GetInstance()->Compare_exchange_Sector(id, (int)ori_z, (int)ori_x, (int)(pPlayer->m_vPos.z), (int)(pPlayer->m_vPos.x));

	/* �÷��̾� �ֺ��� ���͸� �˻� -> ������ ���� ���� ��ü���� ��ǥ ���� */
	unordered_set<pair<int, int>> nearSector;
	nearSector.reserve(NEAR_SECTOR);
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
			for (auto& obj_num : CSectorMgr::GetInstance()->Get_SectorList()[s.first][s.second].Get_ObjList())
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
						pNPC->wakeUp_npc();
					}
				}
				/* MONSTER�� ��� ó��*/
				else if (true == CObjMgr::GetInstance()->Is_Monster(obj_num))
				{
					CMonster* pMonster = static_cast<CMonster*>(CObjMgr::GetInstance()->Get_GameObject(L"MONSTER", obj_num));
					if (pMonster->Get_Dead() == true) continue;

					// �þ� ���� ���ٸ� �þ� ��Ͽ� ���X.
					if (CObjMgr::GetInstance()->Is_Near(pPlayer, pMonster))
					{
						new_viewlist.insert(obj_num);
						pMonster->active_monster();
					}
				}
			}
		}
	}

	// Stage Winter�� ���
	if (pPlayer->m_chStageId == STAGE_WINTER)
	{
		CMonster* pMonster = static_cast<CMonster*>(CObjMgr::GetInstance()->Get_GameObject(L"MONSTER", g_iVergosServerNum));
		if (pMonster != nullptr && pMonster->Get_Dead() == false)
		{
			new_viewlist.insert(g_iVergosServerNum);
		}

		auto& iter_begin = CObjMgr::GetInstance()->Get_OBJLIST(L"AI")->begin();
		auto& iter_end = CObjMgr::GetInstance()->Get_OBJLIST(L"AI")->end();
		for (iter_begin; iter_begin != iter_end; ++iter_begin)
		{
			new_viewlist.insert(iter_begin->second->m_sNum);
		}
	}

	/* ���ο� �þ� ��� ���� ��ü ó�� */
	for (const int& server_num : new_viewlist)
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
				CPlayer* pOther = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", server_num));

				// �÷��̾�('��')���� ���ο� ���� ���� ��Ŷ ����
				pOther->send_enter_packet(id);

				// Ÿ ������ �þ� ��� ó��
				pOther->v_lock.lock();
				// Ÿ ������ �þ� ��Ͽ� '��'�� ���� ���� ���
				if (0 == pOther->view_list.count(id))
				{
					pOther->view_list.insert(id);
					pOther->v_lock.unlock();
					pPlayer->send_enter_packet(server_num);
				}
				else
				{
					pOther->v_lock.unlock();
					pPlayer->send_attack_packet(server_num, aniIdx, end_angleY);
				}
			}
			// ���� �þ߿� ���� NPC�� ��� ó��
			else if (true == CObjMgr::GetInstance()->Is_NPC(server_num))
			{
				// �÷��̾�('��')���� NPC���� ��Ŷ ����
				CNpc* pNPC = static_cast<CNpc*>(CObjMgr::GetInstance()->Get_GameObject(L"NPC", server_num));
				pNPC->send_NPC_enter_packet(id);
			}
			// ���� �þ߿� ���� MONSTER�� ��� ó��
			else if (true == CObjMgr::GetInstance()->Is_Monster(server_num))
			{
				// �÷��̾�('��')���� Monster ���� ��Ŷ ����
				CMonster* pMonster = static_cast<CMonster*>(CObjMgr::GetInstance()->Get_GameObject(L"MONSTER", server_num));
				pMonster->send_Monster_enter_packet(id);
			}
			// ���� �þ߿� ���� AI�� ��� ó��
			else if (true == CObjMgr::GetInstance()->Is_AI(server_num))
			{				
				CAi* pAi = static_cast<CAi*>(CObjMgr::GetInstance()->Get_GameObject(L"AI", server_num));
				pAi->send_AI_enter_packet(id);
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
					pPlayer->send_attack_packet(server_num, aniIdx, end_angleY);
				}
				// Ÿ ������ �þ� ��Ͽ� '��'�� ���� ���� ���
				else
				{
					pOther->view_list.insert(id);
					pOther->v_lock.unlock();
					pPlayer->send_enter_packet(server_num);
				}
			}
		} 
	}

	/* ���� �þ� ��Ͽ��� ����� ��ü ó�� */
	for (const int& s_num : old_viewlist)
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

	/* ��ų Ÿ�� Ȯ�� -> �ش� ���� MP ���� & �ɷ�ġ ȹ�� */
	if (pPlayer->m_type == PC_GLADIATOR)
	{
		switch (aniIdx)
		{
		case Gladiator::STINGER_BLADE:
		{
			pPlayer->m_iMp -= Gladiator::AMOUNT_STINGER_BLADE;
		}
		break;
		case Gladiator::CUTTING_SLASH:
		{
			pPlayer->m_iMp -= Gladiator::AMOUNT_CUTTING_SLASH;
		}
		break;
		case Gladiator::JAW_BREAKER:
		{
			pPlayer->m_iMp -= Gladiator::AMOUNT_JAW_BREAKER;
		}
		break;
		case Gladiator::GAIA_CRUSH1:
		{
			pPlayer->m_iMp -= Gladiator::AMOUNT_GAIA_CRUSH1;
		}
		break;
		case Gladiator::DRAW_SWORD_CHARGE:
		{
			pPlayer->m_iMp -= Gladiator::AMOUNT_DRAW_SWORD_CHARGE;
		}
		break;
		}
	}
	else if (pPlayer->m_type == PC_ARCHER)
	{
		switch (aniIdx)
		{
		case Archer::RAPID_SHOT1:
		{
			pPlayer->m_iMp -= Archer::AMOUNT_RAPID_SHOT1;
		}
		break;
		case Archer::ESCAPING_BOMB:
		{
			pPlayer->m_iMp -= Archer::AMOUNT_ESCAPING_BOMB;
		}
		break;
		case Archer::ARROW_SHOWER_START:
		{
			pPlayer->m_iMp -= Archer::AMOUNT_ARROW_SHOWER_START;
		}
		break;
		case Archer::ARROW_FALL_START:
		{
			pPlayer->m_iMp -= Archer::AMOUNT_ARROW_FALL_START;
		}
		break;
		case Archer::CHARGE_ARROW_START:
		{
			pPlayer->m_iMp -= Archer::AMOUNT_CHARGE_ARROW_START;
		}
		break;
		}
	}

	if (pPlayer->m_iMp <= ZERO_HP)
		pPlayer->m_iMp = ZERO_HP;

	/* �ش� ���� �ɷ�ġ ������Ʈ */
	pPlayer->send_consume_point(id);

	/* ��Ƽ Ȱ�� ���� ��� */
	if (pPlayer->m_bIsPartyState)
	{
		for (auto& member : *CObjMgr::GetInstance()->Get_PARTYLIST(pPlayer->m_iPartyNumber))
		{
			CPlayer* pOther = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", member));
			if (pOther == nullptr || !pOther->m_bIsConnect || !pOther->m_bIsPartyState || member == id) continue;

			// ���� �ɷ�ġ ����
			pPlayer->send_update_party(member);
		}
	}

#ifdef TEST
	/* �÷��̾� �þ� ���� �ִ� ��ü ��� */
	for (int server_obj : new_viewlist)
		cout << "attack" << server_obj << "�þ� ���� �����մϴ�." << endl;
#endif
}

void process_attack_stop(int id, const _vec3& _vDir, const _vec3& _vPos, int aniIdx)
{
	CPlayer* pPlayer = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", id));

	if (pPlayer == nullptr) return;

	/* �ش� �÷��̾��� ���� ��ġ�� & ����� ��ġ�� */
	float ori_x, ori_y, ori_z;

	ori_x = pPlayer->m_vPos.x;
	ori_y = pPlayer->m_vPos.y;
	ori_z = pPlayer->m_vPos.z;

	/* �ش� �÷��̾��� ���� �þ� ��� */
	pPlayer->v_lock.lock();
	unordered_set<int> old_viewlist = pPlayer->view_list;
	pPlayer->v_lock.unlock();

	/* �ش� �÷��̾�κ��� ���� ���� ��ġ�� ���� */
	pPlayer->m_vDir = _vDir;
	pPlayer->m_vPos = _vPos;

	pPlayer->send_attack_stop_packet(id, aniIdx);

	/* ����� ��ǥ�� ���� ���� */
	CSectorMgr::GetInstance()->Compare_exchange_Sector(id, (int)ori_z, (int)ori_x, (int)(pPlayer->m_vPos.z), (int)(pPlayer->m_vPos.x));

	/* �÷��̾� �ֺ��� ���͸� �˻� -> ������ ���� ���� ��ü���� ��ǥ ���� */
	unordered_set<pair<int, int>> nearSector;
	nearSector.reserve(NEAR_SECTOR);
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
			for (auto& obj_num : CSectorMgr::GetInstance()->Get_SectorList()[s.first][s.second].Get_ObjList())
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
						pNPC->wakeUp_npc();
					}
				}
				/* MONSTER�� ��� ó��*/
				else if (true == CObjMgr::GetInstance()->Is_Monster(obj_num))
				{
					CMonster* pMonster = static_cast<CMonster*>(CObjMgr::GetInstance()->Get_GameObject(L"MONSTER", obj_num));
					if (pMonster->Get_Dead() == true) continue;

					// �þ� ���� ���ٸ� �þ� ��Ͽ� ���X.
					if (CObjMgr::GetInstance()->Is_Near(pPlayer, pMonster))
					{
						new_viewlist.insert(obj_num);
						pMonster->active_monster();
					}
				}
			}
		}
	}

	// Stage Winter�� ���
	if (pPlayer->m_chStageId == STAGE_WINTER)
	{
		CMonster* pMonster = static_cast<CMonster*>(CObjMgr::GetInstance()->Get_GameObject(L"MONSTER", g_iVergosServerNum));
		if (pMonster != nullptr && pMonster->Get_Dead() == false)
		{
			new_viewlist.insert(g_iVergosServerNum);
		}

		auto& iter_begin = CObjMgr::GetInstance()->Get_OBJLIST(L"AI")->begin();
		auto& iter_end = CObjMgr::GetInstance()->Get_OBJLIST(L"AI")->end();
		for (iter_begin; iter_begin != iter_end; ++iter_begin)
		{
			new_viewlist.insert(iter_begin->second->m_sNum);
		}
	}

	/* ���ο� �þ� ��� ���� ��ü ó�� */
	for (const int& server_num : new_viewlist)
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
				CPlayer* pOther = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", server_num));
				
				// �÷��̾�('��')���� ���ο� ���� ���� ��Ŷ ����
				pOther->send_enter_packet(id);

				// Ÿ ������ �þ� ��� ó��
				pOther->v_lock.lock();
				// Ÿ ������ �þ� ��Ͽ� '��'�� ���� ���� ���
				if (0 == pOther->view_list.count(id))
				{
					pOther->view_list.insert(id);
					pOther->v_lock.unlock();
					pPlayer->send_enter_packet(server_num);
				}
				else
				{
					pOther->v_lock.unlock();
					pPlayer->send_attack_stop_packet(server_num, aniIdx);
				}
			}
			// ���� �þ߿� ���� NPC�� ��� ó��
			else if (true == CObjMgr::GetInstance()->Is_NPC(server_num))
			{
				// �÷��̾�('��')���� NPC���� ��Ŷ ����
				CNpc* pNPC = static_cast<CNpc*>(CObjMgr::GetInstance()->Get_GameObject(L"NPC", server_num));
				pNPC->send_NPC_enter_packet(id);
			}
			// ���� �þ߿� ���� MONSTER�� ��� ó��
			else if (true == CObjMgr::GetInstance()->Is_Monster(server_num))
			{
				// �÷��̾�('��')���� Monster ���� ��Ŷ ����
				CMonster* pMonster = static_cast<CMonster*>(CObjMgr::GetInstance()->Get_GameObject(L"MONSTER", server_num));
				pMonster->send_Monster_enter_packet(id);
			}
			// ���� �þ߿� ���� AI�� ��� ó��
			else if (true == CObjMgr::GetInstance()->Is_AI(server_num))
			{
				// �÷��̾�('��')���� Monster ���� ��Ŷ ����
				CAi* pAi = static_cast<CAi*>(CObjMgr::GetInstance()->Get_GameObject(L"AI", server_num));
				pAi->send_AI_enter_packet(id);
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
					pPlayer->send_attack_stop_packet(server_num, aniIdx);
				}
				// Ÿ ������ �þ� ��Ͽ� '��'�� ���� ���� ���
				else
				{
					pOther->view_list.insert(id);
					pOther->v_lock.unlock();
					pPlayer->send_enter_packet(server_num);
				}
			}
		}
	}

	/* ���� �þ� ��Ͽ��� ����� ��ü ó�� */
	for (const int& s_num : old_viewlist)
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
		cout << "attck_stop" << server_obj << "�þ� ���� �����մϴ�." << endl;
#endif
}

void process_buff(const int& id, cs_packet_attack* p)
{
	CPlayer* pPlayer = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", id));
	if (pPlayer == nullptr) return;

	/* �ش� �÷��̾��� ���� ��ġ�� & ����� ��ġ�� */
	float ori_x, ori_y, ori_z;
	ori_x = pPlayer->m_vPos.x;
	ori_y = pPlayer->m_vPos.y;
	ori_z = pPlayer->m_vPos.z;

	/* �ش� �÷��̾��� ���⺤�Ϳ� ��ġ���� */
	pPlayer->m_vPos = _vec3(p->posX, p->posY, p->posZ);
	pPlayer->m_vDir = _vec3(p->dirX, p->dirY, p->dirZ);

	/* �ش� �÷��̾��� ���� �þ� ��� */
	pPlayer->v_lock.lock();
	unordered_set<int> old_viewlist = pPlayer->view_list;
	pPlayer->v_lock.unlock();

	/* �ش� �÷��̾� ���� ���� ��ġ */
	pPlayer->m_vTempPos.x = pPlayer->m_vPos.x;
	pPlayer->m_vTempPos.y = pPlayer->m_vPos.y;
	pPlayer->m_vTempPos.z = pPlayer->m_vPos.z;

	/* ����� ��ǥ�� ���� ���� */
	CSectorMgr::GetInstance()->Compare_exchange_Sector(id, (int)ori_z, (int)ori_x, (int)(pPlayer->m_vPos.z), (int)(pPlayer->m_vPos.x));

	/* �÷��̾� �ֺ��� ���͸� �˻� -> ������ ���� ���� ��ü���� ��ǥ ���� */
	unordered_set<pair<int, int>> nearSector;
	nearSector.reserve(NEAR_SECTOR);
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
			for (auto& obj_num : CSectorMgr::GetInstance()->Get_SectorList()[s.first][s.second].Get_ObjList())
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
						pNPC->wakeUp_npc();
					}
				}
				/* MONSTER�� ��� ó��*/
				else if (true == CObjMgr::GetInstance()->Is_Monster(obj_num))
				{
					CMonster* pMonster = static_cast<CMonster*>(CObjMgr::GetInstance()->Get_GameObject(L"MONSTER", obj_num));
					if (pMonster->Get_Dead() == true) continue;

					// �þ� ���� ���ٸ� �þ� ��Ͽ� ���X.
					if (CObjMgr::GetInstance()->Is_Near(pPlayer, pMonster))
					{
						new_viewlist.insert(obj_num);
						pMonster->active_monster();
					}
				}
			}
		}
	}

	// Stage Winter�� ���
	if (pPlayer->m_chStageId == STAGE_WINTER)
	{
		CMonster* pMonster = static_cast<CMonster*>(CObjMgr::GetInstance()->Get_GameObject(L"MONSTER", g_iVergosServerNum));
		if (pMonster != nullptr && pMonster->Get_Dead() == false)
		{
			new_viewlist.insert(g_iVergosServerNum);
		}

		auto& iter_begin = CObjMgr::GetInstance()->Get_OBJLIST(L"AI")->begin();
		auto& iter_end = CObjMgr::GetInstance()->Get_OBJLIST(L"AI")->end();
		for (iter_begin; iter_begin != iter_end; ++iter_begin)
		{
			new_viewlist.insert(iter_begin->second->m_sNum);
		}
	}

	/* ���ο� �þ� ��� ���� ��ü ó�� */
	for (const int& server_num : new_viewlist)
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
				CPlayer* pOther = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", server_num));
				
				// �÷��̾�('��')���� ���ο� ���� ���� ��Ŷ ����
				pOther->send_enter_packet(id);

				// Ÿ ������ �þ� ��� ó��
				pOther->v_lock.lock();
				// Ÿ ������ �þ� ��Ͽ� '��'�� ���� ���� ���
				if (0 == pOther->view_list.count(id))
				{
					pOther->view_list.insert(id);
					pOther->v_lock.unlock();
					pPlayer->send_enter_packet(server_num);
				}
				else
				{
					pOther->v_lock.unlock();
					pPlayer->send_attack_packet(server_num, p->animIdx, p->end_angleY);
				}
			}
			// ���� �þ߿� ���� NPC�� ��� ó��
			else if (true == CObjMgr::GetInstance()->Is_NPC(server_num))
			{
				// �÷��̾�('��')���� NPC���� ��Ŷ ����
				CNpc* pNPC = static_cast<CNpc*>(CObjMgr::GetInstance()->Get_GameObject(L"NPC", server_num));
				pNPC->send_NPC_enter_packet(id);
			}
			// ���� �þ߿� ���� MONSTER�� ��� ó��
			else if (true == CObjMgr::GetInstance()->Is_Monster(server_num))
			{
				// �÷��̾�('��')���� Monster ���� ��Ŷ ����
				CMonster* pMonster = static_cast<CMonster*>(CObjMgr::GetInstance()->Get_GameObject(L"MONSTER", server_num));
				pMonster->send_Monster_enter_packet(id);
			}
			// ���� �þ߿� ���� AI�� ��� ó��
			else if (true == CObjMgr::GetInstance()->Is_Monster(server_num))
			{
				CAi* pAi = static_cast<CAi*>(CObjMgr::GetInstance()->Get_GameObject(L"AI", server_num));
				pAi->send_AI_enter_packet(id);
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
					pPlayer->send_attack_packet(server_num, p->animIdx, p->end_angleY);
				}
				// Ÿ ������ �þ� ��Ͽ� '��'�� ���� ���� ���
				else
				{
					pOther->view_list.insert(id);
					pOther->v_lock.unlock();
					pPlayer->send_enter_packet(server_num);
				}
			}
		} 
	}

	/* ���� �þ� ��Ͽ��� ����� ��ü ó�� */
	for (const int& s_num : old_viewlist)
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

	/* ���� Ÿ�� Ȯ�� -> �ش� ���� MP ���� & �ɷ�ġ ȹ�� */
	switch (p->animIdx)
	{

	// �̵� �ӵ�
	case Priest::AURA_ON:
	{
		pPlayer->m_iMp -= Priest::AMOUNT_AURA;
	}
	break;
	// �ǵ�
	case Priest::PURIFY:
	{
		pPlayer->m_iMp -= Priest::AMOUNT_PURIFY;
	}
	break;

	case Priest::HEAL_START:
	{
		pPlayer->m_iMp -= Priest::AMOUNT_HEAL;

		pPlayer->m_iHp += (int)((float)pPlayer->m_iMaxHp * Priest::PLUS_HP / PERCENT);

		if (pPlayer->m_iHp >= pPlayer->m_iMaxHp)
			pPlayer->m_iHp = pPlayer->m_iMaxHp;
	}
	break;

	case Priest::MP_CHARGE_START:
	{
		pPlayer->m_iMp += (int)((float)pPlayer->m_iMaxMp * Priest::PLUS_MP / PERCENT);

		if (pPlayer->m_iMp >= pPlayer->m_iMaxMp)
			pPlayer->m_iMp = pPlayer->m_iMaxMp;
	}
	break;
	}

	/* �ش� ���� �ɷ�ġ ������Ʈ */
	pPlayer->send_consume_point(id);

	/* ��Ƽ Ȱ�� ���� ��� */
	if (pPlayer->m_bIsPartyState)
	{
		for (auto& member : *CObjMgr::GetInstance()->Get_PARTYLIST(pPlayer->m_iPartyNumber))
		{
			CPlayer* pOther = nullptr;
			if (member >= AI_NUM_START)
			{
				pOther = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"AI", member));
				if (pOther == nullptr || !pOther->m_bIsConnect || !pOther->m_bIsPartyState) continue;
			}
			else
			{
				pOther = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", member));
				if (pOther == nullptr || !pOther->m_bIsConnect || !pOther->m_bIsPartyState) continue;
			}

			if (member != id)
			{
				switch (p->animIdx)
				{
				case Priest::HEAL_START:
				{
					pOther->m_iHp += (int)((float)pOther->m_iMaxHp * Priest::PLUS_HP / PERCENT);

					if (pOther->m_iHp >= pOther->m_iMaxHp)
						pOther->m_iHp = pOther->m_iMaxHp;
				}
				break;

				case Priest::MP_CHARGE_START:
				{
					pOther->m_iMp += (int)((float)pOther->m_iMaxMp * Priest::PLUS_MP / PERCENT);

					if (pOther->m_iMp >= pOther->m_iMaxMp)
						pOther->m_iMp = pOther->m_iMaxMp;
				}
				break;
				}
			}	
			// ��Ƽ�� ���� �ɷ�ġ ����
			if (member < AI_NUM_START)
				pPlayer->send_buff_stat(member, p->animIdx, pOther->m_iHp, pOther->m_iMaxHp, pOther->m_iMp, pOther->m_iMaxMp);
		}
	}
}

void process_stance_change(int id, const bool& stance)
{
	CPlayer* pPlayer = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", id));

	if (pPlayer == nullptr) return;

	/* �ش� �÷��̾��� ���� �þ� ��� */
	pPlayer->v_lock.lock();
	unordered_set<int> old_viewlist = pPlayer->view_list;
	pPlayer->v_lock.unlock();

	for (const int& server_num : old_viewlist)
	{
		if (id == server_num) continue;
		if (true == CObjMgr::GetInstance()->Is_Player(server_num))
		{
			CPlayer* pOther = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", server_num));
			if (pOther == nullptr) continue;
			if (!pOther->m_bIsConnect) continue;

			pPlayer->send_player_stance_change(server_num, stance);
		}
	}
}

void process_stage_change(int id, const char& stage_id)
{
	CPlayer* pPlayer = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", id));
	if (pPlayer == nullptr) return;

	/* �ش� �÷��̾��� ���� ��ġ�� & ����� ��ġ�� */
	float ori_x, ori_y, ori_z;
	ori_x = pPlayer->m_vPos.x;
	ori_y = pPlayer->m_vPos.y;
	ori_z = pPlayer->m_vPos.z;

	// Set Position
	switch (stage_id)
	{
	case STAGE_VELIKA:
	{
		pPlayer->m_vPos = _vec3(STAGE_VELIKA_X, 0.0f, STAGE_VELIKA_Z);
		
		if (pPlayer->m_chStageId == STAGE_WINTER)
			process_raid_end(id);
	}
	break;

	case STAGE_BEACH:
	{
		pPlayer->m_vPos = _vec3(STAGE_BEACH_X, 0.0f, STAGE_BEACH_Z);
	}
	break;

	case STAGE_WINTER:
	{
		pPlayer->m_vPos = _vec3(STAGE_WINTER_X, 0.0f, STAGE_WINTER_Z);
	}
	break;
	}

	// Set StageID
	pPlayer->m_chStageId = stage_id;

	// Send Packet
	pPlayer->send_player_stage_change(id);

	/* �ش� �÷��̾��� ���� �þ� ��� */
	pPlayer->v_lock.lock();
	unordered_set<int> old_viewlist = pPlayer->view_list;
	pPlayer->v_lock.unlock();

	for (const int& server_num : old_viewlist)
	{
		if (id == server_num) continue;
		if (true == CObjMgr::GetInstance()->Is_Player(server_num))
		{
			CPlayer* pOther = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", server_num));
			if (pOther == nullptr) continue;
			if (!pOther->m_bIsConnect) continue;

			pPlayer->send_player_stage_change(server_num);
		}
	}

	/* ����� ��ǥ�� ���� ���� */
	CSectorMgr::GetInstance()->Compare_exchange_Sector(id, (int)ori_z, (int)ori_x, (int)(pPlayer->m_vPos.z), (int)(pPlayer->m_vPos.x));
}

void process_suggest_party(const int& suggester_id, const int& others_id)
{
	if (true == CObjMgr::GetInstance()->Is_Player(others_id))
	{
		CPlayer* pOther = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", others_id));
		if (pOther == nullptr) return;
		if (!pOther->m_bIsConnect) return;
		if (pOther->m_bIsPartyState == true) return;

		send_suggest_party(others_id, suggester_id);
	}
}

void process_respond_party(const bool& result, const int& suggester_id, const int& responder_id)
{
	// ���� ����
	if (result == false)
	{
		// ���� �޽��� ����
		send_reject_party(suggester_id, responder_id);
	}
	// ���� ����
	else
	{
		// 1. ��Ƽ�ʴ��ڰ� ������ ��Ƽ�� ���� ���
		CPlayer* pSuggester = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", suggester_id));
		if (pSuggester == nullptr) return;
		if (!pSuggester->m_bIsConnect) return;

		CPlayer* pResponder = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", responder_id));
		if (pResponder == nullptr) return;
		if (!pResponder->m_bIsConnect) return;

		if (pSuggester->m_bIsPartyState == false)
		{
			// 1-1. ���ο� ��Ƽ ����
			CObjMgr::GetInstance()->Create_Party(&pSuggester->m_iPartyNumber, suggester_id);

			// 1-2. ���ο� ��Ƽ ���� �� ��� �ʴ�
			CObjMgr::GetInstance()->Add_PartyMember(pSuggester->m_iPartyNumber, &pResponder->m_iPartyNumber, responder_id);

			// 1-3. �ʴ��� ��Ƽ ���� ���·� ����
			pSuggester->m_bIsPartyState = true;

			// 1-4. ���ο� ��� ��Ƽ ���� ���·� ����
			pResponder->m_bIsPartyState = true;
		}
		// 2. ��Ƽ�ʴ��ڰ� ������ ��Ƽ�� ���� ���
		else
		{
			// 2-1. ���ο� ��� �ʴ�
			CObjMgr::GetInstance()->Add_PartyMember(pSuggester->m_iPartyNumber, &pResponder->m_iPartyNumber, responder_id);

			// 2-2. ���ο� ��� ��Ƽ ���� ���·� ����
			pResponder->m_bIsPartyState = true;
		}

		// 3. ��Ƽ�������鿡�� ���ο� ��Ƽ��� ���� ����
		for (auto& p : *CObjMgr::GetInstance()->Get_PARTYLIST(pSuggester->m_iPartyNumber))
		{
			if (p != responder_id)
			{
				CPlayer* pMember = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", p));
				if (pMember == nullptr) return;
				if (!pMember->m_bIsConnect) return;

				// ���ο� ��� ���� -> ���� ������
				pResponder->send_enter_party(p);
				// ���� ������ ���� -> ���ο� ���
				pMember->send_enter_party(responder_id);
			}
		}
	}
}

void process_join_party(const int& joinner_id, const int& others_id)
{
	if (true == CObjMgr::GetInstance()->Is_Player(others_id))
	{
		CPlayer* pOther = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", others_id));
		if (pOther == nullptr) return;
		if (!pOther->m_bIsConnect) return;
		if (!pOther->m_bIsPartyState) return;

		send_join_party(others_id, joinner_id);
	}
}

void process_decide_party(const bool& result, const int& joinner_id, const int& responder_id)
{
	// ���� ��û ����
	if (result == false)
	{
		// ���� �޽��� ����
		send_reject_party(joinner_id, responder_id);
	}
	// ���� ��û ����
	else
	{
		CPlayer* pJoinner = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", joinner_id));
		if (pJoinner == nullptr) return;
		if (!pJoinner->m_bIsConnect) return;

		CPlayer* pResponder = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", responder_id));
		if (pResponder == nullptr) return;
		if (!pResponder->m_bIsConnect) return;

		// 1-1. ���ο� ��� ���� ���� -> ��Ƽ�� ��Ͽ� ����
		CObjMgr::GetInstance()->Add_PartyMember(pResponder->m_iPartyNumber, &pJoinner->m_iPartyNumber, joinner_id);

		// 1-2. ���ο� ��� ��Ƽ ���� ���·� ����
		pJoinner->m_bIsPartyState = true;

		// 2. ��Ƽ�������鿡�� ���ο� ��Ƽ��� ���� ����
		for (auto& p : *CObjMgr::GetInstance()->Get_PARTYLIST(pResponder->m_iPartyNumber))
		{
			if (p != joinner_id)
			{
				CPlayer* pMember = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", p));
				if (pMember == nullptr) return;
				if (!pMember->m_bIsConnect) return;

				// ���ο� ��� ���� -> ���� ������
				pJoinner->send_enter_party(p);
				// ���� ������ ���� -> ���ο� ���
				pMember->send_enter_party(joinner_id);
			}
		}
	}
}

void process_leave_party(const int& id)
{
	CPlayer* pUser = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", id));
	if (pUser == nullptr) return;
	if (!pUser->m_bIsConnect) return;
	if (!pUser->m_bIsPartyState) return;

	// ��Ƽ �������鿡�� ��Ƽ Ż�� �˸�
	for (auto& p : *CObjMgr::GetInstance()->Get_PARTYLIST(pUser->m_iPartyNumber))
	{
		if (CObjMgr::GetInstance()->Is_Player(p) == true && p != id)
		{
			// Ż�� ��� ���� -> ���� ������
			pUser->send_leave_party(p);
		}
	}

	// �ش� ������ ��Ƽ ���� �ʱ�ȭ
	CObjMgr::GetInstance()->Leave_Party(&pUser->m_iPartyNumber, id);
	pUser->m_bIsPartyState = false;
}

void process_disconnect(const int& id)
{
	CPlayer* pPlayer = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", id));

	if (pPlayer == nullptr || pPlayer->m_bIsConnect == false) return;

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

	pPlayer->Set_IsConnected(false);
	closesocket(pPlayer->m_sock);
	pPlayer->m_sock = 0;
	pPlayer->m_vPos = _vec3(0.f, 0.f, 0.f);
	pPlayer->m_vDir = _vec3(0.f, 0.f, 0.f);
	pPlayer->m_ID[0] = 0;
	pPlayer->m_type = 0;
	pPlayer->m_chStageId = STAGE_VELIKA;
	pPlayer->m_bIsPartyState = false;
	pPlayer->m_iPartyNumber = INIT_PARTY_NUMBER;
	pPlayer->m_iMoney = INIT_MONEY;
	pPlayer->view_list.clear();
	pPlayer->Release_Inventory();
	pPlayer->Release_Equipment();
	pPlayer->Get_ClientLock().unlock();

	CObjPoolMgr::GetInstance()->return_Object(L"PLAYER", pPlayer);
	CObjMgr::GetInstance()->Delete_GameObject(L"PLAYER", pPlayer);
}

void process_chat(const int& id, const char* buffer)
{
	CPlayer* pPlayer = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", id));
	if (pPlayer == nullptr) return;

	auto iter_begin = CObjMgr::GetInstance()->Get_OBJLIST(L"PLAYER")->begin();
	auto iter_end = CObjMgr::GetInstance()->Get_OBJLIST(L"PLAYER")->end();

	int tempSize = 0;
	Check_Korean(buffer, &tempSize);

	for (iter_begin; iter_begin != iter_end; ++iter_begin)
	{
		pPlayer->send_chat(iter_begin->second->m_sNum, buffer, tempSize);
	}
}

void process_add_item(const int& id, const char& chItemType, const char& chName)
{
	CPlayer* pUser = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", id));
	if (pUser == nullptr) return;
	if (!pUser->m_bIsConnect) return;

	int itemNumber = CItemMgr::GetInstance()->Find_ItemNumber(chItemType, chName);
	if (pUser->Add_Item(itemNumber, static_cast<ITEM>(chItemType)))
	{
		pUser->send_update_inventory(id, chItemType, chName, pUser->Get_ItemCount(itemNumber, static_cast<ITEM>(chItemType)), true);
	}
}

void process_delete_item(const int& id, const char& chItemType, const char& chName)
{
	CPlayer* pUser = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", id));
	if (pUser == nullptr) return;
	if (!pUser->m_bIsConnect) return;

	int itemNumber = CItemMgr::GetInstance()->Find_ItemNumber(chItemType, chName);
	if (pUser->Delete_Item(itemNumber, static_cast<ITEM>(chItemType)))
	{
		pUser->send_update_inventory(id, chItemType, chName, pUser->Get_ItemCount(itemNumber, static_cast<ITEM>(chItemType)), false);
	}
}

void process_equip_item(const int& id, const char& chItemSlotType, const char& chItemType, const char& chName)
{
	CPlayer* pUser = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", id));
	if (pUser == nullptr) return;
	if (!pUser->m_bIsConnect) return;

	int originItemNumber = CItemMgr::GetInstance()->Find_ItemNumber(chItemType, pUser->Get_Equipment(chItemSlotType));
	if (originItemNumber != -1)
	{
		pUser->m_iMaxAtt -= CItemMgr::GetInstance()->Get_Item(originItemNumber).iAtt;
		pUser->m_iMinAtt -= CItemMgr::GetInstance()->Get_Item(originItemNumber).iAtt;
		pUser->m_iHp	 -= CItemMgr::GetInstance()->Get_Item(originItemNumber).iHp;
		pUser->m_iMaxHp  -= CItemMgr::GetInstance()->Get_Item(originItemNumber).iHp;
		pUser->m_iMp	 -= CItemMgr::GetInstance()->Get_Item(originItemNumber).iMp;
		pUser->m_iMaxMp  -= CItemMgr::GetInstance()->Get_Item(originItemNumber).iMp;
	}

	pUser->Equip_Item(chName, chItemSlotType);

	// �ɷ�ġ ����
	int itemNumber = CItemMgr::GetInstance()->Find_ItemNumber(chItemType, chName);
	pUser->m_iMaxAtt	+= CItemMgr::GetInstance()->Get_Item(itemNumber).iAtt;
	pUser->m_iMinAtt	+= CItemMgr::GetInstance()->Get_Item(itemNumber).iAtt;
	pUser->m_iHp		+= CItemMgr::GetInstance()->Get_Item(itemNumber).iHp;
	pUser->m_iMaxHp		+= CItemMgr::GetInstance()->Get_Item(itemNumber).iHp;
	pUser->m_iMp		+= CItemMgr::GetInstance()->Get_Item(itemNumber).iMp;
	pUser->m_iMaxMp		+= CItemMgr::GetInstance()->Get_Item(itemNumber).iMp;

	pUser->send_update_equipment(id, chItemSlotType, true);
}

void process_unequip_item(const int& id, const char& chItemSlotType, const char& chItemType, const char& chName)
{
	CPlayer* pUser = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", id));
	if (pUser == nullptr) return;
	if (!pUser->m_bIsConnect) return;

	pUser->Unequip_Item(chName, chItemSlotType);

	// �ɷ�ġ ����
	int itemNumber = CItemMgr::GetInstance()->Find_ItemNumber(chItemType, chName);
	pUser->m_iMaxAtt	-= CItemMgr::GetInstance()->Get_Item(itemNumber).iAtt;
	pUser->m_iMinAtt	-= CItemMgr::GetInstance()->Get_Item(itemNumber).iAtt;
	pUser->m_iHp		-= CItemMgr::GetInstance()->Get_Item(itemNumber).iHp;
	pUser->m_iMaxHp		-= CItemMgr::GetInstance()->Get_Item(itemNumber).iHp;
	pUser->m_iMp		-= CItemMgr::GetInstance()->Get_Item(itemNumber).iMp;
	pUser->m_iMaxMp		-= CItemMgr::GetInstance()->Get_Item(itemNumber).iMp;

	pUser->send_update_equipment(id, chItemSlotType, false);
}

void process_shopping(const int& id, cs_packet_shop* p)
{
	CPlayer* pUser = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", id));
	if (pUser == nullptr) return;
	if (!pUser->m_bIsConnect) return;

	/* ���� */

	// 1. ��ü ���� �ݾ� ��� �� ��ü ���� ������ ���� ���
	int allItemCost = 0;
	int allItemCount = 0;

	for (int i = 0; i < SHOP_SLOT; ++i)
	{
		if (p->buyItemType[i] == -1 || p->buyItemName[i] == -1) continue;

		int itemNumber = CItemMgr::GetInstance()->Find_ItemNumber(p->buyItemType[i], p->buyItemType[i]);
		GAMEITEM item = CItemMgr::GetInstance()->Get_Item(itemNumber);

		/* 2. HP/MP ���� ������ ���� ��� */
		if (p->buyItemType[i] == ItemType_Potion && p->buyItemName[i] == Potion_HP)
		{
			if (pUser->Is_inInventory(30, ITEM::ITEM_ETC))	continue;
			else allItemCount += 1;
		}
		else if (p->buyItemType[i] == ItemType_Potion && p->buyItemName[i] == Potion_MP)
		{
			if (pUser->Is_inInventory(31, ITEM::ITEM_ETC))	continue;
			else allItemCount += 1;
		}
		else
			allItemCount += p->buyItemCount[i];

		allItemCost += item.iCost * p->buyItemCount[i];
	}

	/* ���� ����*/
	if (allItemCost > pUser->m_iMoney || pUser->Is_Full_Inventory(allItemCount) == true)
	{
		//cout << "�������� �����ϰų� �κ��丮�� ���� �� ������ �� �����ϴ�." << endl;
		return;
	}

	// 3. ���� �Ϸ�
	for (int i = 0; i < SHOP_SLOT; ++i)
	{
		if (p->buyItemType[i] == -1 || p->buyItemName[i] == -1) continue;

		int itemNumber = CItemMgr::GetInstance()->Find_ItemNumber(p->buyItemType[i], p->buyItemName[i]);
		GAMEITEM item = CItemMgr::GetInstance()->Get_Item(itemNumber);
		
		if (pUser->Buy_Item(itemNumber, static_cast<ITEM>(p->buyItemType[i]), p->buyItemCount[i]))
		{
			pUser->m_iMoney -= item.iCost * p->buyItemCount[i];

			pUser->send_update_inventory(id,
										 p->buyItemType[i],
										 p->buyItemName[i],
										 p->buyItemCount[i],
										 true);
		}	
	}

	/* �Ǹ� */
	for (int i = 0; i < SHOP_SLOT; ++i)
	{
		if (p->sellItemType[i] == -1 || p->sellItemName[i] == -1) continue;

		int itemNumber	= CItemMgr::GetInstance()->Find_ItemNumber(p->sellItemType[i], p->sellItemName[i]);
		GAMEITEM item	= CItemMgr::GetInstance()->Get_Item(itemNumber);

		/* �ش� ������ �κ��丮�� �ش� �������� ����� �ִ��� Ȯ�� */
		if (pUser->Is_inInventory(itemNumber, static_cast<ITEM>(p->sellItemType[i]), p->sellItemCount[i]))
		{
			if (pUser->Sell_Item(itemNumber, static_cast<ITEM>(p->sellItemType[i]), p->sellItemCount[i]))
			{
				pUser->m_iMoney += item.iCost * p->sellItemCount[i];	
			}		
		}
	}

	pUser->send_user_money();
}

void process_load_equipment(const int& id, const char& chItemSlotType, const char& chItemType, const char& chName)
{
	CPlayer* pUser = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", id));
	if (pUser == nullptr) return;
	if (!pUser->m_bIsConnect) return;

	// �ɷ�ġ ����
	int itemNumber = CItemMgr::GetInstance()->Find_ItemNumber(chItemType, chName);
	pUser->m_iMaxAtt	+= CItemMgr::GetInstance()->Get_Item(itemNumber).iAtt;
	pUser->m_iMinAtt	+= CItemMgr::GetInstance()->Get_Item(itemNumber).iAtt;
	pUser->m_iHp		+= CItemMgr::GetInstance()->Get_Item(itemNumber).iHp;
	pUser->m_iMaxHp		+= CItemMgr::GetInstance()->Get_Item(itemNumber).iHp;
	pUser->m_iMp		+= CItemMgr::GetInstance()->Get_Item(itemNumber).iMp;
	pUser->m_iMaxMp		+= CItemMgr::GetInstance()->Get_Item(itemNumber).iMp;
}

void process_logoutForEquipment(const int& id, const char& chItemSlotType, const char& chItemType, const char& chName)
{
	CPlayer* pUser = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", id));
	if (pUser == nullptr) return;
	if (!pUser->m_bIsConnect) return;

	// �ɷ�ġ ����
	int itemNumber = CItemMgr::GetInstance()->Find_ItemNumber(chItemType, chName);
	pUser->m_iMaxAtt	-= CItemMgr::GetInstance()->Get_Item(itemNumber).iAtt;
	pUser->m_iMinAtt	-= CItemMgr::GetInstance()->Get_Item(itemNumber).iAtt;
	pUser->m_iHp		-= CItemMgr::GetInstance()->Get_Item(itemNumber).iHp;
	pUser->m_iMaxHp		-= CItemMgr::GetInstance()->Get_Item(itemNumber).iHp;
	pUser->m_iMp		-= CItemMgr::GetInstance()->Get_Item(itemNumber).iMp;
	pUser->m_iMaxMp		-= CItemMgr::GetInstance()->Get_Item(itemNumber).iMp;
}

void process_use_potion(const int& id, const bool& bIsPotionHP)
{
	CPlayer* pUser = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", id));
	if (pUser == nullptr) return;
	if (!pUser->m_bIsConnect) return;
	
	// HP Potion
	if (bIsPotionHP)
	{	
		if (pUser->Delete_Item(30, ITEM::ITEM_ETC))
		{
			pUser->m_iHp += Item_Potion_HP_HP * pUser->m_iMaxHp / 100;
			if (pUser->m_iHp >= pUser->m_iMaxHp)
				pUser->m_iHp = pUser->m_iMaxHp;

			pUser->send_drink_potion(id, pUser->m_iHp, ItemType_Potion, Potion_HP, pUser->Get_ItemCount(30, ITEM::ITEM_ETC), false);	
		}
	}
	// MP Potion
	else
	{
		if (pUser->Delete_Item(31, ITEM_ETC))
		{
			pUser->m_iMp += Item_Potion_MP_MP * pUser->m_iMaxMp / 100;
			if (pUser->m_iMp >= pUser->m_iMaxMp)
				pUser->m_iMp = pUser->m_iMaxMp;

			pUser->send_drink_potion(id, pUser->m_iMp, ItemType_Potion, Potion_MP, pUser->Get_ItemCount(31, ITEM::ITEM_ETC), false);
		}
	}

	/* �ش� ������ ��Ƽ�� ���ԵǾ� �ִ� ������ ��� ��Ƽ������ ���� */
	if (pUser->m_bIsPartyState)
	{
		for (auto& p : *CObjMgr::GetInstance()->Get_PARTYLIST(pUser->m_iPartyNumber))
		{
			if (p == id) continue;
			pUser->send_update_party(p);
		}
	}
}

void process_cinema_end(const int& id)
{
	CPlayer* pPlayer = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", id));
	if (pPlayer == nullptr) return;

	if (pPlayer->m_chStageId == STAGE_WINTER)
	{
		CObjMgr::GetInstance()->Add_RaidList(id);

		/* ���� ��Ƽ Ż�� -> ���̵� ��Ƽ ���� */
		process_leave_party(id);

		CObjMgr::GetInstance()->Add_PartyMember(RAID_PARTY, &pPlayer->m_iPartyNumber, id);
		pPlayer->m_bIsPartyState = true;

		// ��Ƽ�������鿡�� ���ο� ��Ƽ��� ���� ����
		for (auto& p : *CObjMgr::GetInstance()->Get_PARTYLIST(RAID_PARTY))
		{
			if (p != id)
			{
				if (CObjMgr::GetInstance()->Is_Player(p) == true)
				{
					CPlayer* pMember = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", p));
					if (pMember == nullptr) continue;
					if (!pMember->m_bIsConnect) continue;

					// ���ο� ��� ���� -> ���� ������			
					pPlayer->send_enter_party(p);
					// ���� ������ ���� -> ���ο� ���
					pMember->send_enter_party(id);
				}
				else if (CObjMgr::GetInstance()->Is_AI(p) == true)
				{
					CAi* pMember = static_cast<CAi*>(CObjMgr::GetInstance()->Get_GameObject(L"AI", p));
					if (pMember != nullptr)
					{
						// ���� ������ ���� -> ���ο� ���
						pMember->send_enter_party(id);
					}
				}
			}
		}
	}

	/* AI & Vergos Connect */
	if (CObjMgr::GetInstance()->Get_RAIDLIST()->size() >= RAID_MINIMUM)
	{
		CMonster* pMonster = static_cast<CMonster*>(CObjMgr::GetInstance()->Get_GameObject(L"MONSTER", g_iVergosServerNum));
		if (pMonster != nullptr && pMonster->Get_IsConnected() == false)
		{
			add_timer(pMonster->m_sNum, OP_MODE_RAID_START, system_clock::now() + 10s);
		}
	}
}

void process_raid_end(const int& id)
{
	CPlayer* pPlayer = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", id));
	if (pPlayer == nullptr) return;
	if (pPlayer->m_chStageId != STAGE_WINTER) return;
	
	/* Vergos & AI �þ߿��� ���� */
	pPlayer->v_lock.lock();
	if (pPlayer->view_list.count(g_iVergosServerNum) != 0)
	{
		pPlayer->view_list.erase(g_iVergosServerNum);
		send_leave_packet(id, g_iVergosServerNum);
	}

	auto& iter_begin = CObjMgr::GetInstance()->Get_OBJLIST(L"AI")->begin();
	auto& iter_end = CObjMgr::GetInstance()->Get_OBJLIST(L"AI")->end();
	for (iter_begin; iter_begin != iter_end; ++iter_begin)
	{
		pPlayer->view_list.erase(iter_begin->second->m_sNum);
		send_leave_packet(id, iter_begin->second->m_sNum);
	}
	pPlayer->v_lock.unlock();

	/* ���̵� ��Ƽ Ż�� */
	process_leave_party(id);

	/* ���̵� ��� ��Ͽ��� Ż�� */
	CObjMgr::GetInstance()->Leave_RaidList(id);
	
	/* AI & Vergos Reset */
	if (CObjMgr::GetInstance()->Get_RAIDLIST()->size() < 1)
	{
		add_timer(g_iVergosServerNum, OP_MODE_RAID_END, system_clock::now() + 1s);
	}
}

void process_hurt_ai()
{
	auto& iter_begin = CObjMgr::GetInstance()->Get_OBJLIST(L"AI")->begin();
	auto& iter_end = CObjMgr::GetInstance()->Get_OBJLIST(L"AI")->end();
	for (iter_begin; iter_begin != iter_end; ++iter_begin)
	{
		if (iter_begin->second->Get_IsConnected() == false) continue;
		static_cast<CAi*>(iter_begin->second)->Hurt_AI();
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

void Check_Korean(const char* text, int* len)
{
	// �ѱ��� �� ���ڴ� 2Byte -> �� Byte�� �ֻ��� bit�� 1�̴�.
	for (int i = 0; i < MAX_STR_LEN; ++i)
	{
		if (text[i] == '\0') return;

		// �ѱ��� �ƴ� ���
		if ((text[i] & 0x80) != 0x80)
			*len += 1;
		// �ѱ��� ���
		else if ((text[i] & 0x80) == 0x80)
			*len += 2;
	}
}
