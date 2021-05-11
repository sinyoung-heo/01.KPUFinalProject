#include "stdafx.h"
#include "ObjMgr.h"
#include "Monster.h"

IMPLEMENT_SINGLETON(CObjMgr)

CObjMgr::CObjMgr(void)
{
}

CObjMgr::~CObjMgr(void)
{
}

HRESULT CObjMgr::Init_ObjMgr()
{
	m_mapObjList[L"PLAYER"] = OBJLIST();
	m_mapObjList[L"NPC"] = OBJLIST();
	m_mapObjList[L"MONSTER"] = OBJLIST();

	return S_OK;
}

CObj* CObjMgr::Get_GameObject(wstring wstrObjTag, int server_num)
{
	objmgr_lock ol(m_mutex);

	/* map에서 찾고자 하는 OBJLIST를 Key 값을 통해 찾기 */
	auto& iter_find = m_mapObjList.find(wstrObjTag);

	/* 해당 OBJLIST를 찾지 못하였다면 NULL 반환 */
	if (iter_find == m_mapObjList.end())
		return nullptr;

	/* server number값이 있는지 탐색 */
	auto& user = iter_find->second.find(server_num);

	/* 탐색 실패 */
	if (user == iter_find->second.end())
		return nullptr;

	/* 탐색 성공 -> 객체 찾기 성공 */
	return user->second;
}

OBJLIST* CObjMgr::Get_OBJLIST(wstring wstrObjTag)
{
	objmgr_lock ol(m_mutex);

	/* map에서 찾고자 하는 OBJLIST를 key 값을 통해 찾기 */
	auto& iter_find = m_mapObjList.find(wstrObjTag);

	/* 해당 OBJLIST를 찾지 못하였다면 NULL 반환 */
	if (iter_find == m_mapObjList.end())
		return nullptr;

	return &(iter_find->second);
}

void CObjMgr::Create_StageBeachMonster()
{
	CMonster* pNew = nullptr;
	int s_num = -1;

	/*	________________________________________________________________________________
										CRAB MONSTER
		________________________________________________________________________________*/
	// MONSTER - crab 1
	//pNew = static_cast<CMonster*>(CObjPoolMgr::GetInstance()->use_Object(L"MONSTER"));

	//if (pNew)
	//{
	//	/* NPC의 정보 초기화 */
	//	pNew->Ready_Monster(_vec3(388.235f, 0.f, 142.977f), _vec3(0.f, 0.0f, 0.f), MON_NORMAL, MON_CRAB, STAGE_BEACH, CRAB_HP, CRAB_ATT, CRAB_EXP, CRAB_SPD);
	//	pNew->Set_NumAnimation(Crab::NUM_ANIMATION);
	//	pNew->Set_AnimDuration(Crab::duration);
	//}
	//else return;

	// MONSTER - crab 2
	pNew = static_cast<CMonster*>(CObjPoolMgr::GetInstance()->use_Object(L"MONSTER"));

	if (pNew)
	{
		/* NPC의 정보 초기화 */
		pNew->Ready_Monster(_vec3(406.363f, 0.f, 135.247f), _vec3(0.f, 0.0f, 0.f), MON_NORMAL, MON_CRAB, STAGE_BEACH, CRAB_HP, CRAB_ATT, CRAB_EXP, CRAB_SPD);
		pNew->Set_NumAnimation(Crab::NUM_ANIMATION);
		pNew->Set_AnimDuration(Crab::duration);
	}
	else return;

	// MONSTER - crab 3
	//pNew = static_cast<CMonster*>(CObjPoolMgr::GetInstance()->use_Object(L"MONSTER"));

	//if (pNew)
	//{
	//	/* NPC의 정보 초기화 */
	//	pNew->Ready_Monster(_vec3(410.387f, 0.f, 136.93f), _vec3(0.f, 0.0f, 0.f), MON_NORMAL, MON_CRAB, STAGE_BEACH, CRAB_HP, CRAB_ATT, CRAB_EXP, CRAB_SPD);
	//	pNew->Set_NumAnimation(Crab::NUM_ANIMATION);
	//	pNew->Set_AnimDuration(Crab::duration);
	//}
	//else return;

	// MONSTER - crab 4
	pNew = static_cast<CMonster*>(CObjPoolMgr::GetInstance()->use_Object(L"MONSTER"));

	if (pNew)
	{
		/* NPC의 정보 초기화 */
		pNew->Ready_Monster(_vec3(393.557f, 0.f, 134.746f), _vec3(0.f, 0.0f, 0.f), MON_NORMAL, MON_CRAB, STAGE_BEACH, CRAB_HP, CRAB_ATT, CRAB_EXP, CRAB_SPD);
		pNew->Set_NumAnimation(Crab::NUM_ANIMATION);
		pNew->Set_AnimDuration(Crab::duration);
	}
	else return;

	// MONSTER - crab 5
	//pNew = static_cast<CMonster*>(CObjPoolMgr::GetInstance()->use_Object(L"MONSTER"));

	//if (pNew)
	//{
	//	/* NPC의 정보 초기화 */
	//	pNew->Ready_Monster(_vec3(387.697f, 0.f, 126.109f), _vec3(0.f, 0.0f, 0.f), MON_NORMAL, MON_CRAB, STAGE_BEACH, CRAB_HP, CRAB_ATT, CRAB_EXP, CRAB_SPD);
	//	pNew->Set_NumAnimation(Crab::NUM_ANIMATION);
	//	pNew->Set_AnimDuration(Crab::duration);
	//}
	//else return;

	// MONSTER - crab 6
	pNew = static_cast<CMonster*>(CObjPoolMgr::GetInstance()->use_Object(L"MONSTER"));

	if (pNew)
	{
		/* NPC의 정보 초기화 */
		pNew->Ready_Monster(_vec3(391.849f, 0.f, 135.497f), _vec3(0.f, 0.0f, 0.f), MON_NORMAL, MON_CRAB, STAGE_BEACH, CRAB_HP, CRAB_ATT, CRAB_EXP, CRAB_SPD);
		pNew->Set_NumAnimation(Crab::NUM_ANIMATION);
		pNew->Set_AnimDuration(Crab::duration);
	}
	else return;

	// MONSTER - crab 7
	pNew = static_cast<CMonster*>(CObjPoolMgr::GetInstance()->use_Object(L"MONSTER"));

	if (pNew)
	{
		/* NPC의 정보 초기화 */
		pNew->Ready_Monster(_vec3(380.019f, 0.f, 153.189f), _vec3(0.f, 0.0f, 0.f), MON_NORMAL, MON_CRAB, STAGE_BEACH, CRAB_HP, CRAB_ATT, CRAB_EXP, CRAB_SPD);
		pNew->Set_NumAnimation(Crab::NUM_ANIMATION);
		pNew->Set_AnimDuration(Crab::duration);
	}
	else return;

	// MONSTER - crab 8
	//pNew = static_cast<CMonster*>(CObjPoolMgr::GetInstance()->use_Object(L"MONSTER"));

	//if (pNew)
	//{
	//	/* NPC의 정보 초기화 */
	//	pNew->Ready_Monster(_vec3(385.967f, 0.f, 154.236f), _vec3(0.f, 0.0f, 0.f), MON_NORMAL, MON_CRAB, STAGE_BEACH, CRAB_HP, CRAB_ATT, CRAB_EXP, CRAB_SPD);
	//	pNew->Set_NumAnimation(Crab::NUM_ANIMATION);
	//	pNew->Set_AnimDuration(Crab::duration);
	//}
	//else return;

	// MONSTER - crab 9
	pNew = static_cast<CMonster*>(CObjPoolMgr::GetInstance()->use_Object(L"MONSTER"));

	if (pNew)
	{
		/* NPC의 정보 초기화 */
		pNew->Ready_Monster(_vec3(394.258f, 0.f, 165.382f), _vec3(0.f, 0.0f, 0.f), MON_NORMAL, MON_CRAB, STAGE_BEACH, CRAB_HP, CRAB_ATT, CRAB_EXP, CRAB_SPD);
		pNew->Set_NumAnimation(Crab::NUM_ANIMATION);
		pNew->Set_AnimDuration(Crab::duration);
	}
	else return;

	/*	________________________________________________________________________________
										DRAWN SAILOR MONSTER
		________________________________________________________________________________*/

	// MONSTER - drowned sailor 1
	//pNew = static_cast<CMonster*>(CObjPoolMgr::GetInstance()->use_Object(L"MONSTER"));

	//if (pNew)
	//{
	//	/* NPC의 정보 초기화 */
	//	pNew->Ready_Monster(_vec3(381.223f, 0.f, 181.083f), _vec3(0.f, 0.0f, 0.f), MON_NORMAL, MON_SAILOR, STAGE_BEACH, SAILOR_HP, SAILOR_ATT, SAILOR_EXP, SAILOR_SPD);
	//	pNew->Set_NumAnimation(DrownedSailor::NUM_ANIMATION);
	//	pNew->Set_AnimDuration(DrownedSailor::duration);
	//}
	//else return;

	// MONSTER - drowned sailor 2
	pNew = static_cast<CMonster*>(CObjPoolMgr::GetInstance()->use_Object(L"MONSTER"));

	if (pNew)
	{
		/* NPC의 정보 초기화 */
		pNew->Ready_Monster(_vec3(358.218f, 0.f, 154.457f), _vec3(0.f, 0.0f, 0.f), MON_NORMAL, MON_SAILOR, STAGE_BEACH, SAILOR_HP, SAILOR_ATT, SAILOR_EXP, SAILOR_SPD);
		pNew->Set_NumAnimation(DrownedSailor::NUM_ANIMATION);
		pNew->Set_AnimDuration(DrownedSailor::duration);
	}
	else return;

	// MONSTER - drowned sailor 3
	pNew = static_cast<CMonster*>(CObjPoolMgr::GetInstance()->use_Object(L"MONSTER"));

	if (pNew)
	{
		/* NPC의 정보 초기화 */
		pNew->Ready_Monster(_vec3(355.212f, 0.f, 165.113f), _vec3(0.f, 0.0f, 0.f), MON_NORMAL, MON_SAILOR, STAGE_BEACH, SAILOR_HP, SAILOR_ATT, SAILOR_EXP, SAILOR_SPD);
		pNew->Set_NumAnimation(DrownedSailor::NUM_ANIMATION);
		pNew->Set_AnimDuration(DrownedSailor::duration);
	}
	else return;

	// MONSTER - drowned sailor 4
	//pNew = static_cast<CMonster*>(CObjPoolMgr::GetInstance()->use_Object(L"MONSTER"));

	//if (pNew)
	//{
	//	/* NPC의 정보 초기화 */
	//	pNew->Ready_Monster(_vec3(361.711f, 0.f, 158.886f), _vec3(0.f, 0.0f, 0.f), MON_NORMAL, MON_SAILOR, STAGE_BEACH, SAILOR_HP, SAILOR_ATT, SAILOR_EXP, SAILOR_SPD);
	//	pNew->Set_NumAnimation(DrownedSailor::NUM_ANIMATION);
	//	pNew->Set_AnimDuration(DrownedSailor::duration);
	//}
	//else return;

	// MONSTER - drowned sailor 5
	pNew = static_cast<CMonster*>(CObjPoolMgr::GetInstance()->use_Object(L"MONSTER"));

	if (pNew)
	{
		/* NPC의 정보 초기화 */
		pNew->Ready_Monster(_vec3(354.653f, 0.f, 142.572f), _vec3(0.f, 0.0f, 0.f), MON_NORMAL, MON_SAILOR, STAGE_BEACH, SAILOR_HP, SAILOR_ATT, SAILOR_EXP, SAILOR_SPD);
		pNew->Set_NumAnimation(DrownedSailor::NUM_ANIMATION);
		pNew->Set_AnimDuration(DrownedSailor::duration);
	}
	else return;

	// MONSTER - drowned sailor 6
	pNew = static_cast<CMonster*>(CObjPoolMgr::GetInstance()->use_Object(L"MONSTER"));

	if (pNew)
	{
		/* NPC의 정보 초기화 */
		pNew->Ready_Monster(_vec3(359.64f, 0.f, 167.906f), _vec3(0.f, 0.0f, 0.f), MON_NORMAL, MON_SAILOR, STAGE_BEACH, SAILOR_HP, SAILOR_ATT, SAILOR_EXP, SAILOR_SPD);
		pNew->Set_NumAnimation(DrownedSailor::NUM_ANIMATION);
		pNew->Set_AnimDuration(DrownedSailor::duration);
	}
	else return;

	// MONSTER - drowned sailor 7
	//pNew = static_cast<CMonster*>(CObjPoolMgr::GetInstance()->use_Object(L"MONSTER"));

	//if (pNew)
	//{
	//	/* NPC의 정보 초기화 */
	//	pNew->Ready_Monster(_vec3(353.29f, 0.f, 177.823f), _vec3(0.f, 0.0f, 0.f), MON_NORMAL, MON_SAILOR, STAGE_BEACH, SAILOR_HP, SAILOR_ATT, SAILOR_EXP, SAILOR_SPD);
	//	pNew->Set_NumAnimation(DrownedSailor::NUM_ANIMATION);
	//	pNew->Set_AnimDuration(DrownedSailor::duration);
	//}
	//else return;

	// MONSTER - drowned sailor 8
	pNew = static_cast<CMonster*>(CObjPoolMgr::GetInstance()->use_Object(L"MONSTER"));

	if (pNew)
	{
		/* NPC의 정보 초기화 */
		pNew->Ready_Monster(_vec3(378.303f, 0.f, 171.636f), _vec3(0.f, 0.0f, 0.f), MON_NORMAL, MON_SAILOR, STAGE_BEACH, SAILOR_HP, SAILOR_ATT, SAILOR_EXP, SAILOR_SPD);
		pNew->Set_NumAnimation(DrownedSailor::NUM_ANIMATION);
		pNew->Set_AnimDuration(DrownedSailor::duration);
	}
	else return;

	/*	________________________________________________________________________________
									CRAFTY ARCHNE MONSTER
	________________________________________________________________________________*/
	pNew = static_cast<CMonster*>(CObjPoolMgr::GetInstance()->use_Object(L"MONSTER"));
	
	if (pNew)
	{
		/* NPC의 정보 초기화 */
		pNew->Ready_Monster(_vec3(403.297f, 0.f, 185.56f), _vec3(0.f, 0.0f, 0.f), MON_NORMAL, MON_ARACHNE, STAGE_BEACH, ARCHNE_HP, ARCHNE_ATT, ARCHNE_EXP, ARCHNE_SPD);
		pNew->Set_NumAnimation(CraftyArachne::NUM_ANIMATION);
		pNew->Set_AnimDuration(CraftyArachne::duration);
	}
	else return;
}

bool CObjMgr::Is_Player(int server_num)
{
	objmgr_lock ol(m_mutex);

	/* PLAYER ObjList 에서 찾고자 하는 OBJLIST를 key 값을 통해 찾기 */
	auto& iter_find = m_mapObjList[L"PLAYER"].find(server_num);

	/* 해당 OBJLIST를 찾지 못하였다면 NULL 반환 */
	if (iter_find == m_mapObjList[L"PLAYER"].end())
		return false;

	return true;
}

bool CObjMgr::Is_NPC(int server_num)
{
	objmgr_lock ol(m_mutex);

	/* NPC ObjList 에서 찾고자 하는 OBJLIST를 key 값을 통해 찾기 */
	auto& iter_find = m_mapObjList[L"NPC"].find(server_num);

	/* 해당 OBJLIST를 찾지 못하였다면 NULL 반환 */
	if (iter_find == m_mapObjList[L"NPC"].end())
		return false;

	return true;
}

bool CObjMgr::Is_Monster(int server_num)
{
	objmgr_lock ol(m_mutex);

	/* NPC ObjList 에서 찾고자 하는 OBJLIST를 key 값을 통해 찾기 */
	auto& iter_find = m_mapObjList[L"MONSTER"].find(server_num);

	/* 해당 OBJLIST를 찾지 못하였다면 NULL 반환 */
	if (iter_find == m_mapObjList[L"MONSTER"].end())
		return false;

	return true;
}

bool CObjMgr::Is_Near(const CObj* me, const CObj* other)
{
	float dist = (other->m_vPos.x - me->m_vPos.x) * (other->m_vPos.x - me->m_vPos.x);
	dist += (other->m_vPos.y - me->m_vPos.y) * (other->m_vPos.y - me->m_vPos.y);
	dist += (other->m_vPos.z - me->m_vPos.z) * (other->m_vPos.z - me->m_vPos.z);

	return dist <= VIEW_LIMIT * VIEW_LIMIT;
}

bool CObjMgr::Is_Monster_Target(const CObj* me, const CObj* other)
{
	float dist = (other->m_vPos.x - me->m_vPos.x) * (other->m_vPos.x - me->m_vPos.x);
	dist += (other->m_vPos.y - me->m_vPos.y) * (other->m_vPos.y - me->m_vPos.y);
	dist += (other->m_vPos.z - me->m_vPos.z) * (other->m_vPos.z - me->m_vPos.z);

	return dist <= CHASE_RANGE * CHASE_RANGE;
}

bool CObjMgr::Is_Monster_AttackTarget(const CObj* me, const CObj* other, const int& range)
{
	float dist = (other->m_vPos.x - me->m_vPos.x) * (other->m_vPos.x - me->m_vPos.x);
	dist += (other->m_vPos.y - me->m_vPos.y) * (other->m_vPos.y - me->m_vPos.y);
	dist += (other->m_vPos.z - me->m_vPos.z) * (other->m_vPos.z - me->m_vPos.z);

	return dist <= range * range;
}

HRESULT CObjMgr::Add_GameObject(wstring wstrObjTag, CObj* pObj, int server_num)
{
	if (pObj != nullptr)
	{
		/* map에서 찾고자 하는 OBJLIST를 key 값을 통해 찾기 */
		auto& iter_find = m_mapObjList.find(wstrObjTag);

		/* 해당 OBJLIST가 없다면, 임시 OBJLIST 생성한 후 오브젝트 삽입 */
		if (iter_find == m_mapObjList.end())
			m_mapObjList[wstrObjTag] = OBJLIST();
		
		m_mapObjList[wstrObjTag][server_num] = pObj;
	}

	return S_OK;
}

HRESULT CObjMgr::Delete_GameObject(wstring wstrObjTag, CObj* pObj)
{
	if (pObj != nullptr)
	{
		/* map에서 찾고자 하는 OBJLIST를 key 값을 통해 찾기 */
		auto& iter_find = m_mapObjList.find(wstrObjTag);

		/* 해당 OBJLIST가 없다면, 임시 OBJLIST 생성한 후 오브젝트 삽입 */
		if (iter_find == m_mapObjList.end())
			return E_FAIL;

		/* server number 값이 있는지 탐색 */
		auto& user = iter_find->second.find(pObj->m_sNum);
		
		/* 탐색 실패 */
		if (user == iter_find->second.end())
			return E_FAIL;

		/* 3.탐색 성공 -> 삭제 */
		if (user->second)
		{
			iter_find->second.erase(pObj->m_sNum);
			return S_OK;
		}
	}

	return S_OK;
}

HRESULT CObjMgr::Delete_OBJLIST(wstring wstrObjTag)
{
	m_mapObjList[wstrObjTag].clear();
	return S_OK;
}

void CObjMgr::Release()
{
	for (auto& o_list : m_mapObjList)
	{	
		o_list.second.clear();
	}
}
