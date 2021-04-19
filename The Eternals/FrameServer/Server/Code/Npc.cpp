#include "stdafx.h"
#include "Npc.h"

CNpc::CNpc()
	:m_npcNum(0)
{
}

CNpc::~CNpc()
{
}

void CNpc::Set_AnimDuration(double arr[])
{
	for (int i = 0; i < MAX_ANI; ++i)
	{
		if (m_uiNumAniIndex > i)
			m_arrDuration[i] = arr[i];
	}
}

void CNpc::Set_AnimationKey(const _uint& uiAniKey)
{
	if (m_uiNewAniIndex != uiAniKey)
	{
		m_uiNewAniIndex = uiAniKey;
		m_fBlendingTime = 1.0f;
		m_fBlendAnimationTime = m_fAnimationTime;
	}
}

void CNpc::Ready_NPC(const _vec3& pos, const _vec3& angle, const char& type, const char& num, const char& naviType)
{
	/* NPC의 정보 초기화 */
	m_sNum += NPC_NUM_START;
	int s_num = m_sNum;

	m_naviMeshType = naviType;

	m_bIsConnect = true;
	m_bIsDead = false;

	m_vPos = pos;
	m_vTempPos = m_vPos;
	m_vDir = _vec3(0.f, 0.f, 1.f);
	m_vAngle = angle;
	m_type = type;
	m_npcNum = num;
	m_status = STATUS::ST_NONACTIVE;

	CSectorMgr::GetInstance()->Enter_ClientInSector(s_num, (int)(m_vPos.z / SECTOR_SIZE), (int)(m_vPos.x / SECTOR_SIZE));
	CObjMgr::GetInstance()->Add_GameObject(L"NPC", this, s_num);
}

int CNpc::Update_NPC(const float& fTimeDelta)
{
	if (m_bIsDead)
		return DEAD_OBJ;

	if (fTimeDelta > 1.f)
		return NO_EVENT;

	//Change_Animation(fTimeDelta);

	/* Calculate Animation frame */
	Set_AnimationKey(m_uiAnimIdx);
	Play_Animation(fTimeDelta * NPC_TYPE::TPS);

	return NO_EVENT;
}

void CNpc::active_npc()
{
	/* NPC가 활성화되어 있지 않을 경우 활성화 */
	if (m_status != ST_ACTIVE)
	{
		STATUS prev_state = m_status;
		if (true == atomic_compare_exchange_strong(&m_status, &prev_state, ST_ACTIVE))
		{
			return;
		}
	}
}

void CNpc::Play_Animation(float fTimeDelta)
{
	if (m_uiCurAniIndex >= m_uiNumAniIndex) return;

	/*__________________________________________________________________________________________________________
	[ 애니메이션이 계속 반복되도록 fmod 수행 ]
	____________________________________________________________________________________________________________*/
	if (m_uiNewAniIndex != m_uiCurAniIndex)
	{
		m_fAnimationTime = m_fBlendAnimationTime;
		m_fBlendingTime -= 0.001f * fTimeDelta;

		if (m_fBlendingTime <= 0.0f)
			m_fBlendingTime = 0.0f;
	}
	else
	{
		m_fAnimationTime += fTimeDelta;
	}

	m_fAnimationTime = (float)(fmod(m_fAnimationTime, (m_arrDuration[m_uiCurAniIndex])));

	/*__________________________________________________________________________________________________________
	[ 3DMax 상에서의 Frame 계산 ]
	____________________________________________________________________________________________________________*/
	m_ui3DMax_NumFrame = (_uint)(_3DMAX_FPS * (m_arrDuration[m_uiCurAniIndex] / Monster_Normal::TPS));
	m_ui3DMax_CurFrame = (_uint)(_3DMAX_FPS * (m_fAnimationTime / Monster_Normal::TPS));

	if (m_fBlendingTime <= 0.0f)
	{
		m_uiCurAniIndex = m_uiNewAniIndex;
		m_fAnimationTime = 0.0f;
		m_fBlendingTime = 1.f;
	}
}

bool CNpc::Is_AnimationSetEnd(const float& fTimeDelta)
{
	if ((m_fAnimationTime >= m_arrDuration[m_uiCurAniIndex] -
		static_cast<double>(Monster_Normal::TPS * ANIMA_INTERPOLATION * fTimeDelta)) &&
		(m_uiCurAniIndex == m_uiNewAniIndex))
	{
		return true;
	}

	return false;
}

void CNpc::send_NPC_enter_packet(int to_client)
{
	sc_packet_npc_enter p;

	p.size = sizeof(p);
	p.type = SC_PACKET_NPC_ENTER;
	p.id = m_sNum;

	p.o_type = m_type;
	p.npcNum = m_npcNum;
	p.naviType = m_naviMeshType;

	p.posX = m_vPos.x;
	p.posY = m_vPos.y;
	p.posZ = m_vPos.z;

	p.angleX = m_vAngle.x;
	p.angleY = m_vAngle.y;
	p.angleZ = m_vAngle.z;

	send_packet(to_client, &p);
}

DWORD CNpc::Release()
{
	return 0;
}
