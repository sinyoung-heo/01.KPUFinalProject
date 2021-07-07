#include "stdafx.h"
#include "Ai.h"
#include "Player.h"

CAi::CAi()
    :m_iLevel(0), m_iHp(0), m_iMaxHp(0), m_fSpd(0.f),
    m_iMp(0), m_iMaxMp(0), m_iMaxAtt(0), m_iMinAtt(0.f),
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
