#include "stdafx.h"
#include "Ai.h"
#include "Player.h"

CAi::CAi()
    :m_iLevel(0), m_iHp(0), m_iMaxHp(0), m_fSpd(0.f),
    m_iMp(0), m_iMaxMp(0), m_iMaxAtt(0), m_iMinAtt(0.f), m_bIsMove(false),
    m_bIsAttackStance(false), m_bIsPartyState(false), m_iPartyNumber(-1)
{

}

CAi::~CAi()
{
}

void CAi::Ready_AI(const char& chJob, const char& chWeaponType, const char& chStageID, const _vec3& vPos)
{
    m_sNum += AI_NUM_START;
    int s_num = m_sNum;

    m_bIsConnect        = true;
    m_bIsDead           = false;

	sprintf_s(m_ID, "AI_%03d", m_sNum);
    m_type              = chJob;
    m_chWeaponType      = chWeaponType;
    m_chStageId         = chStageID;
    m_bIsAttackStance   = true;
    m_bIsPartyState     = true;
    m_iPartyNumber      = RAID_PARTY;

    m_iLevel            = INIT_LEV;
    m_iHp               = AI_HP;
    m_iMaxHp            = AI_HP;
    m_iMp               = AI_MP;
    m_iMaxMp            = AI_MP;
    m_iMinAtt           = AI_MINATT;
    m_iMaxAtt           = AI_MAXATT;
    m_fSpd              = INIT_SPEED;

    m_vPos              = vPos;
    m_vDir              = _vec3(0.f, 0.f, 1.f);
    m_vAngle            = _vec3(0.f, 0.f, 0.f);
	m_status			= STATUS::ST_NONACTIVE;

	// Move Speed
	m_tMoveSpeedInterpolationDesc.linear_ratio = 0.0f;
	m_tMoveSpeedInterpolationDesc.v1 = Archer::MIN_SPEED;
	m_tMoveSpeedInterpolationDesc.v2 = Archer::MAX_SPEED * Archer::OTHERS_OFFSET;


    CSectorMgr::GetInstance()->Enter_ClientInSector(s_num, (int)(m_vPos.z / SECTOR_SIZE), (int)(m_vPos.x / SECTOR_SIZE));
    CObjMgr::GetInstance()->Add_GameObject(L"AI", this, s_num);
    CObjMgr::GetInstance()->Add_PartyMember(RAID_PARTY, &m_iPartyNumber, s_num);
}

void CAi::Release_AI()
{
    process_disconnect_ai();
}

int CAi::Update_AI(const float& fTimeDelta)
{
	if (fTimeDelta > 1.f)
		return NO_EVENT;

	Change_Animation(fTimeDelta);

	return NO_EVENT;
}

void CAi::active_AI()
{
	if (m_status == ST_NONACTIVE)
	{
		STATUS prev_state = m_status;
		atomic_compare_exchange_strong(&m_status, &prev_state, ST_ACTIVE);
	}
}

void CAi::Change_Animation(const float& fTimeDelta)
{
	switch (m_type)
	{
	case PC_GLADIATOR:
	{

	}
	break;

	case PC_ARCHER:
	{
		Change_Archer_Animation(fTimeDelta);
	}
	break;

	case PC_PRIEST:
	{

	}
	break;
	}
}

void CAi::Change_Archer_Animation(const float& fTimeDelta)
{
	switch (m_status)
	{

	case STATUS::ST_ACTIVE:
	{
		m_iAniIdx = Archer::ATTACK_RUN;
		process_move_ai(fTimeDelta);
	}
	break;

	case STATUS::ST_NONACTIVE:
	{
		
	}
	break;

	case STATUS::ST_CHASE:
	{
		
	}
	break;

	case STATUS::ST_ATTACK:
	{
		
	}
	break;

	case STATUS::ST_REACTION:
	{
		
	}
	break;

	case STATUS::ST_DEAD:
	{
		
	}
	break;
	}
}

void CAi::process_move_ai(const float& fTimeDelta)
{
	/* ���� ���� */
	if (!m_bIsMove)
	{
		/* �ش� NPC�� ���� ��ġ�� */
		float ori_x, ori_y, ori_z;
		ori_x = m_vPos.x;
		ori_y = m_vPos.y;
		ori_z = m_vPos.z;

		// �����̱� �� ��ġ������ viewlist (�þ� ���� �÷��̾� ����)
		unordered_set<pair<int, int>> oldnearSector;
		oldnearSector.reserve(NEAR_SECTOR);
		CSectorMgr::GetInstance()->Get_NearSectorIndex(&oldnearSector, (int)ori_x, (int)ori_z);

		unordered_set <int> old_viewlist;

		// �̵� ��: ���� ���� ��ȸ
		for (auto& s : oldnearSector)
		{
			// ���� ���� ���� Ÿ �������� �ִ��� �˻�
			if (!(CSectorMgr::GetInstance()->Get_SectorList()[s.first][s.second].Get_ObjList().empty()))
			{
				// ������ ���� ��ȣ ����
				for (auto& obj_num : CSectorMgr::GetInstance()->Get_SectorList()[s.first][s.second].Get_ObjList())
				{
					/* ������ ��� ó�� */
					if (true == CObjMgr::GetInstance()->Is_Player(obj_num))
					{
						CPlayer* pPlayer = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", obj_num));

						// ������ ������ �þ� ��Ͽ� ����Ѵ�.
						if (!pPlayer->Get_IsConnected()) continue;

						// �þ� ���� �ִٸ� �þ� ��Ͽ� ����Ѵ�.
						if (CObjMgr::GetInstance()->Is_Near(this, pPlayer))
							old_viewlist.insert(obj_num);
					}
				}
			}
		}

		/* AI ������ ó�� */
		switch (rand() % 3)
		{
		case 0: m_vDir = _vec3(0.f, 0.f, 1.f); break;	
		case 1: m_vDir = _vec3(1.f, 0.f, 1.f); break;
		case 2: m_vDir = _vec3(-1.f, 0.f, 1.f); break;
		}

		/* �ش� AI�� �̷� ��ġ ��ǥ ���� -> �̷� ��ġ��ǥ�� �ӽ� ������ ���� */
		_vec2 coll_pos = _vec2(0.f);

		if (CCollisionMgr::GetInstance()->Is_DeadReckoning(m_vPos, m_vDir, &coll_pos, m_chStageId))
		{
			m_vTempPos.x = coll_pos.x;
			m_vTempPos.y = 0.f;
			m_vTempPos.z = coll_pos.y;

			m_bIsMove = true;
		}
		else
		{
			m_vTempPos.x = m_vPos.x;
			m_vTempPos.y = m_vPos.y;
			m_vTempPos.z = m_vPos.z;
		}

		m_vDir = m_vTempPos - m_vPos;
		m_vDir.Normalize();

		/* ����� ��ǥ�� ���� ���� */
		CSectorMgr::GetInstance()->Compare_exchange_Sector(m_sNum, (int)ori_z, (int)ori_x, (int)(m_vPos.z), (int)(m_vPos.x));

		// ������ �� ��ġ������ viewlist (�þ� ���� �÷��̾� ����)
		unordered_set <int> new_viewlist;

		unordered_set<pair<int, int>> nearSectors;
		nearSectors.reserve(5);
		CSectorMgr::GetInstance()->Get_NearSectorIndex(&nearSectors, (int)(m_vPos.x), (int)(m_vPos.z));

		// �̵� ��: ���� ���� ��ȸ -> ������ ���� �� new viewlist ���� ���
		for (auto& s : nearSectors)
		{
			// ���� ���� ���� Ÿ �������� �ִ��� �˻�
			if (!(CSectorMgr::GetInstance()->Get_SectorList()[s.first][s.second].Get_ObjList().empty()))
			{				
				for (auto& obj_num : CSectorMgr::GetInstance()->Get_SectorList()[s.first][s.second].Get_ObjList())
				{
					/* ������ ��� ó�� */
					if (true == CObjMgr::GetInstance()->Is_Player(obj_num))
					{
						CPlayer* pPlayer = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", obj_num));

						// ������ ������ �þ� ��Ͽ� ����Ѵ�.
						if (!pPlayer->Get_IsConnected()) continue;

						// �þ� ���� �ִٸ� �þ� ��Ͽ� ����Ѵ�.
						if (CObjMgr::GetInstance()->Is_Near(this, pPlayer))
							new_viewlist.insert(obj_num);
					}
				}
			}
		}

		// �̵� �� viewlist & �̵� �� viewlist �� -> �� �������� �þ� ��� ���� NPC ���� ���θ� ����.
		for (auto pl : old_viewlist)
		{
			// �̵� �Ŀ��� AI �þ� ��� ���� "pl"(server number) ������ �����ִ� ���
			if (0 < new_viewlist.count(pl))
			{
				CPlayer* pPlayer = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", pl));
				if (pPlayer != nullptr)
				{
					/* �ش� ������ �þ� ��Ͽ� ���� AI�� ������ ��� */
					pPlayer->v_lock.lock();
					if (0 < pPlayer->view_list.count(m_sNum))
					{
						pPlayer->v_lock.unlock();
						/* �ش� �������� AI�� ������ ���� ��ġ�� ���� */
						send_AI_move_packet(pl);
					}
					/* �ش� ������ �þ� ��Ͽ� ���� AI�� �������� ���� ��� */
					else
					{
						/* �ش� ������ �þ� ��Ͽ� ���� AI ��� */
						pPlayer->view_list.insert(m_sNum);
						pPlayer->v_lock.unlock();
						send_AI_enter_packet(pl);
					}
				}
			}
			// �̵� �Ŀ� AI �þ� ��� ���� "pl"(server number) ������ ���� ���
			else
			{
				CPlayer* pPlayer = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", pl));
				if (pPlayer != nullptr)
				{
					/* �ش� ������ �þ� ��Ͽ� ���� AI�� ������ ��� */
					pPlayer->v_lock.lock();
					if (0 < pPlayer->view_list.count(m_sNum))
					{
						/* �ش� ������ �þ� ��Ͽ��� ���� AI ���� */
						pPlayer->view_list.erase(m_sNum);
						pPlayer->v_lock.unlock();
						send_leave_packet(pl, m_sNum);
					}
					else
						pPlayer->v_lock.unlock();
				}
			}
		}

		// new_vielist ��ȸ -> �÷��̾��� �þ� ��Ͽ� �־�� �� ���ο� npc���� �߰�
		for (auto& pl : new_viewlist)
		{
			CPlayer* pPlayer = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", pl));
			if (pPlayer != nullptr)
			{
				pPlayer->v_lock.lock();
				if (0 == pPlayer->view_list.count(pl))
				{
					/* �� ������ �þ� ��� ���� ���� NPC�� ���� ��� -> ���� NPC ��� */
					if (0 == pPlayer->view_list.count(m_sNum))
					{
						pPlayer->view_list.insert(m_sNum);
						pPlayer->v_lock.unlock();
						send_AI_enter_packet(pl);
					}
					/* �� ������ �þ� ��� ���� ���� NPC�� ���� ��� -> ���� NPC ��ġ ���� */
					else
					{
						pPlayer->v_lock.unlock();
						send_AI_move_packet(pl);
					}
				}
				else
					pPlayer->v_lock.unlock();
			}
		}
	}
	else
	{
	if (false == CCollisionMgr::GetInstance()->Is_InMoveLimit(m_vPos, m_vTempPos, 30.f))
		{
			m_tMoveSpeedInterpolationDesc.interpolation_speed = 1.0f * Archer::OTHERS_OFFSET;
			m_tMoveSpeedInterpolationDesc.linear_ratio += m_tMoveSpeedInterpolationDesc.interpolation_speed * fTimeDelta;
			m_fSpd = LinearInterpolation(m_tMoveSpeedInterpolationDesc.v1, m_tMoveSpeedInterpolationDesc.v2, m_tMoveSpeedInterpolationDesc.linear_ratio);

			m_vDir = m_vTempPos - m_vPos;
			m_vDir.Normalize();

			m_vPos += m_vDir * m_fSpd * fTimeDelta;
		}
		else m_bIsMove = false;	
	}

}

void CAi::process_disconnect_ai()
{
	/* sector���� �ش� �÷��̾� ����� */
	CSectorMgr::GetInstance()->Leave_ClientInSector(m_sNum, (int)(m_vPos.z / SECTOR_SIZE), (int)(m_vPos.x / SECTOR_SIZE));

	/* ��Ƽ�� ���ԵǾ� �ִٸ� ��Ƽ Ż�� */
	if (m_bIsPartyState == true)
		process_leave_party_ai();

	/* �ش� �÷��̾ ��ϵǾ� �ִ� ���� ���� �����鿡�� ���� ���Ḧ �˸� */
	unordered_set<pair<int, int>> nearSector;
	nearSector.reserve(NEAR_SECTOR);
	CSectorMgr::GetInstance()->Get_NearSectorIndex(&nearSector, (int)m_vPos.x, (int)m_vPos.z);

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
				if (obj_num == m_sNum) continue;

				CPlayer* pOther = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", obj_num));

				// ������ �����鿡�Ը� ���� ���Ḧ �˸�
				if (pOther->Get_IsConnected())
				{
					/* Ÿ ������ �þ� ��� ���� '��'�� �ִٸ� ����� */
					pOther->v_lock.lock();
					if (0 != pOther->view_list.count(m_sNum))
					{
						pOther->view_list.erase(m_sNum);
						pOther->v_lock.unlock();

						/* Ÿ �������� ���� ���� ��Ŷ ���� */
#ifdef TEST
						cout << obj_num << "�Կ���" << id << "�� ������ ����" << endl;
#endif					
						send_leave_packet(obj_num, m_sNum);
					}
					else pOther->v_lock.unlock();
				}
			}
		}
	}


	m_bIsConnect	= false;
	m_bIsDead		= false;
	m_vPos			= _vec3(0.f, 0.f, 0.f);
	m_vDir			= _vec3(0.f, 0.f, 0.f);
	m_ID[0]			= 0;
	m_type			= 0;
	m_chStageId		= STAGE_WINTER;
	m_bIsPartyState = false;
	m_iPartyNumber	= INIT_PARTY_NUMBER;
	m_status		= STATUS::ST_END;

	CObjPoolMgr::GetInstance()->return_Object(L"AI", this);
	CObjMgr::GetInstance()->Delete_GameObject(L"AI", this);
}

void CAi::process_leave_party_ai()
{
	if (!m_bIsPartyState) return;

	// ��Ƽ �������鿡�� ��Ƽ Ż�� �˸�
	for (auto& p : *CObjMgr::GetInstance()->Get_PARTYLIST(m_iPartyNumber))
	{
		if (p != m_sNum && CObjMgr::GetInstance()->Is_Player(p))
		{
			// Ż�� ��� ���� -> ���� ������
			send_leave_party_ai(p);
		}
	}

	// �ش� ������ ��Ƽ ���� �ʱ�ȭ
	CObjMgr::GetInstance()->Leave_Party(&m_iPartyNumber, m_sNum);
	m_bIsPartyState = false;
}

void CAi::send_AI_enter_packet(const int& to_client)
{
	sc_packet_enter p;

	p.size				= sizeof(p);
	p.type				= SC_PACKET_ENTER;
	p.id				= m_sNum;

	c_lock.lock();
	strncpy_s(p.name, m_ID, strlen(m_ID));
	c_lock.unlock();

	p.o_type			= m_type;
	p.weaponType		= m_chWeaponType;
	p.stageID			= m_chStageId;
	p.is_stance_attack	= m_bIsAttackStance;
	p.is_party_state	= m_bIsPartyState;

	p.posX				= m_vPos.x;
	p.posY				= m_vPos.y;
	p.posZ				= m_vPos.z;

	p.dirX				= m_vDir.x;
	p.dirY				= m_vDir.y;
	p.dirZ				= m_vDir.z;
	
	send_packet(to_client, &p);
}

void CAi::send_AI_move_packet(const int& to_client)
{
	sc_packet_move p;

	p.size		= sizeof(p);
	p.type		= SC_PACKET_MOVE;
	p.id		= m_sNum;

	p.spd		= m_fSpd;
	p.animIdx	= m_iAniIdx;
	p.move_time = move_time;

	p.posX		= m_vTempPos.x;
	p.posY		= m_vTempPos.y;
	p.posZ		= m_vTempPos.z;

	p.dirX		= m_vDir.x;
	p.dirY		= m_vDir.y;
	p.dirZ		= m_vDir.z;

	send_packet(to_client, &p);
}

void CAi::send_leave_party_ai(const int& to_client)
{
	sc_packet_suggest_party p;

	p.size = sizeof(p);
	p.type = SC_PACKET_LEAVE_PARTY;
	p.id = m_sNum;

	send_packet(to_client, &p);
}

DWORD CAi::Release()
{
    return 0;
}
