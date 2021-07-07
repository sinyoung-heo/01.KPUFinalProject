#include "stdafx.h"
#include "ObjMgr.h"
#include "Monster.h"
#include "Npc.h"
#include "Ai.h"

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
	m_mapObjList[L"AI"] = OBJLIST();

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

PARTYLIST* CObjMgr::Get_PARTYLIST(const int& party_num)
{
	objmgr_lock ol(m_mutex);

	/* map에서 찾고자 하는 OBJLIST를 key 값을 통해 찾기 */
	auto& iter_find = m_mapPartyList.find(party_num);

	/* 해당 OBJLIST를 찾지 못하였다면 NULL 반환 */
	if (iter_find == m_mapPartyList.end())
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
	pNew = static_cast<CMonster*>(CObjPoolMgr::GetInstance()->use_Object(L"MONSTER"));

	if (pNew)
	{
		/* NPC의 정보 초기화 */
		pNew->Ready_Monster(_vec3(388.235f, 0.f, 142.977f), _vec3(0.f, 0.0f, 0.f), MON_NORMAL, MON_CRAB, STAGE_BEACH, CRAB_HP, CRAB_ATT, CRAB_EXP, CRAB_SPD);
		pNew->Set_NumAnimation(Crab::NUM_ANIMATION);
		pNew->Set_AnimDuration(Crab::duration);
	}
	else return;

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
	pNew = static_cast<CMonster*>(CObjPoolMgr::GetInstance()->use_Object(L"MONSTER"));

	if (pNew)
	{
		/* NPC의 정보 초기화 */
		pNew->Ready_Monster(_vec3(410.387f, 0.f, 136.93f), _vec3(0.f, 0.0f, 0.f), MON_NORMAL, MON_CRAB, STAGE_BEACH, CRAB_HP, CRAB_ATT, CRAB_EXP, CRAB_SPD);
		pNew->Set_NumAnimation(Crab::NUM_ANIMATION);
		pNew->Set_AnimDuration(Crab::duration);
	}
	else return;

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
	pNew = static_cast<CMonster*>(CObjPoolMgr::GetInstance()->use_Object(L"MONSTER"));

	if (pNew)
	{
		/* NPC의 정보 초기화 */
		pNew->Ready_Monster(_vec3(387.697f, 0.f, 126.109f), _vec3(0.f, 0.0f, 0.f), MON_NORMAL, MON_CRAB, STAGE_BEACH, CRAB_HP, CRAB_ATT, CRAB_EXP, CRAB_SPD);
		pNew->Set_NumAnimation(Crab::NUM_ANIMATION);
		pNew->Set_AnimDuration(Crab::duration);
	}
	else return;

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
	pNew = static_cast<CMonster*>(CObjPoolMgr::GetInstance()->use_Object(L"MONSTER"));

	if (pNew)
	{
		/* NPC의 정보 초기화 */
		pNew->Ready_Monster(_vec3(385.967f, 0.f, 154.236f), _vec3(0.f, 0.0f, 0.f), MON_NORMAL, MON_CRAB, STAGE_BEACH, CRAB_HP, CRAB_ATT, CRAB_EXP, CRAB_SPD);
		pNew->Set_NumAnimation(Crab::NUM_ANIMATION);
		pNew->Set_AnimDuration(Crab::duration);
	}
	else return;

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
	pNew = static_cast<CMonster*>(CObjPoolMgr::GetInstance()->use_Object(L"MONSTER"));

	if (pNew)
	{
		/* NPC의 정보 초기화 */
		pNew->Ready_Monster(_vec3(381.223f, 0.f, 181.083f), _vec3(0.f, 0.0f, 0.f), MON_NORMAL, MON_SAILOR, STAGE_BEACH, SAILOR_HP, SAILOR_ATT, SAILOR_EXP, SAILOR_SPD);
		pNew->Set_NumAnimation(DrownedSailor::NUM_ANIMATION);
		pNew->Set_AnimDuration(DrownedSailor::duration);
	}
	else return;

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
	pNew = static_cast<CMonster*>(CObjPoolMgr::GetInstance()->use_Object(L"MONSTER"));

	if (pNew)
	{
		/* NPC의 정보 초기화 */
		pNew->Ready_Monster(_vec3(361.711f, 0.f, 158.886f), _vec3(0.f, 0.0f, 0.f), MON_NORMAL, MON_SAILOR, STAGE_BEACH, SAILOR_HP, SAILOR_ATT, SAILOR_EXP, SAILOR_SPD);
		pNew->Set_NumAnimation(DrownedSailor::NUM_ANIMATION);
		pNew->Set_AnimDuration(DrownedSailor::duration);
	}
	else return;

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
	pNew = static_cast<CMonster*>(CObjPoolMgr::GetInstance()->use_Object(L"MONSTER"));

	if (pNew)
	{
		/* NPC의 정보 초기화 */
		pNew->Ready_Monster(_vec3(353.29f, 0.f, 177.823f), _vec3(0.f, 0.0f, 0.f), MON_NORMAL, MON_SAILOR, STAGE_BEACH, SAILOR_HP, SAILOR_ATT, SAILOR_EXP, SAILOR_SPD);
		pNew->Set_NumAnimation(DrownedSailor::NUM_ANIMATION);
		pNew->Set_AnimDuration(DrownedSailor::duration);
	}
	else return;

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

void CObjMgr::Create_StageWinterMonster()
{
	CMonster* pNew = nullptr;
	int s_num = -1;

	/*	________________________________________________________________________________
										 집단 1
	________________________________________________________________________________*/
	// MONSTER - Monkey 1
	float fAngle = 0.f;
	for (int i = 0; i < 10; ++i, fAngle += 30.f)
	{
		pNew = static_cast<CMonster*>(CObjPoolMgr::GetInstance()->use_Object(L"MONSTER"));
		if (pNew)
		{
			
			float fX = 380.f + cosf(fAngle * PI / 180.f) * 10.f;
			float fY = 400.f - sinf(fAngle * PI / 180.f) * 10.f;
			pNew->Ready_Monster(_vec3(fX, 0.f, fY), _vec3(0.f, 0.0f, 0.f), MON_NORMAL, MON_MONKEY, STAGE_WINTER, MONKEY_HP, MONKEY_ATT, MONKEY_EXP, MONKEY_SPD);
			pNew->Set_NumAnimation(Monkey::NUM_ANIMATION);
			pNew->Set_AnimDuration(Monkey::duration);
		}
	}

	// MONSTER - Giant Monkey
	pNew = static_cast<CMonster*>(CObjPoolMgr::GetInstance()->use_Object(L"MONSTER"));
	if (pNew)
	{
		/* NPC의 정보 초기화 */
		pNew->Ready_Monster(_vec3(380.f, 0.f, 400.f), _vec3(0.f, 0.0f, 0.f), MON_NORMAL, MON_GMONKEY, STAGE_WINTER, GIANTMONKEY_HP, GIANTMONKEY_ATT, GIANTMONKEY_EXP, GIANTMONKEY_SPD);
		pNew->Set_NumAnimation(GiantMonkey::NUM_ANIMATION);
		pNew->Set_AnimDuration(GiantMonkey::duration);
	}
	else return;
}

void CObjMgr::Create_AiPlayer()
{
	// 레이드 파티 생성
	m_mapPartyList[RAID_PARTY] = PARTYLIST();

	CAi* pNew = nullptr;
	int s_num = -1;

	// ARCHER 1
	pNew = static_cast<CAi*>(CObjPoolMgr::GetInstance()->use_Object(L"AI"));
	if (pNew)
	{
		pNew->Ready_AI(PC_ARCHER, Bow27_SM, STAGE_WINTER, _vec3(STAGE_WINTER_X, 0.f, STAGE_WINTER_Z));
	}
	else return;
}

void CObjMgr::Create_StageVelikaNPC()
{

	CNpc* pNew = nullptr;
	int s_num = -1;

	/* ______________________________________________________________________________________________________*/
	/*											SCENE - VELIKA TOWN											 */
	/* ______________________________________________________________________________________________________*/

	// NPC - Merchant 1
	pNew = static_cast<CNpc*>(CObjPoolMgr::GetInstance()->use_Object(L"NPC"));

	if (pNew)
	{
		/* NPC의 정보 초기화 */
		pNew->Ready_NPC(_vec3(143.0f, 0.f, 79.0f), _vec3(0.f, -45.f, 0.f), NPC_MERCHANT, NPC_BARAKA_MERCHANT, STAGE_VELIKA);
		pNew->Set_NumAnimation(Baraka_M_Merchant::NUM_ANIMATION);
		pNew->Set_AnimDuration(Baraka_M_Merchant::duration);
	}
	else return;

	// NPC - Merchant 2
	pNew = static_cast<CNpc*>(CObjPoolMgr::GetInstance()->use_Object(L"NPC"));

	if (pNew)
	{
		/* NPC의 정보 초기화 */
		pNew->Ready_NPC(_vec3(151.0f, 0.f, 90.0f), _vec3(0.f, -45.f, 0.f), NPC_MERCHANT, NPC_POPORI_MERCHANT, STAGE_VELIKA);
		pNew->Set_NumAnimation(Popori_M_Merchant::NUM_ANIMATION);
		pNew->Set_AnimDuration(Popori_M_Merchant::duration);
	}
	else return;

	// NPC - Merchant 3
	pNew = static_cast<CNpc*>(CObjPoolMgr::GetInstance()->use_Object(L"NPC"));

	if (pNew)
	{
		/* NPC의 정보 초기화 */
		pNew->Ready_NPC(_vec3(154.0f, 0.f, 103.0f), _vec3(0.f, -90.f, 0.f), NPC_MERCHANT, NPC_BARAKA_MYSTELLIUM, STAGE_VELIKA);
		pNew->Set_NumAnimation(Baraka_M_Mystellium::NUM_ANIMATION);
		pNew->Set_AnimDuration(Baraka_M_Mystellium::duration);
	}
	else return;

	// NPC Assistant - Popori Boy
	pNew = static_cast<CNpc*>(CObjPoolMgr::GetInstance()->use_Object(L"NPC"));

	if (pNew)
	{
		/* NPC의 정보 초기화 */
		pNew->Ready_NPC(_vec3(148.5f, 1.f, 88.5f), _vec3(0.f, -90.f, 0.f), NPC_NONMOVE, NPC_POPORI_BOY, STAGE_VELIKA);
		pNew->Set_NumAnimation(Popori_boy::NUM_ANIMATION);
		pNew->Set_AnimDuration(Popori_boy::duration);
	}
	else return;

	// NPC Assistant - Popori Boy
	pNew = static_cast<CNpc*>(CObjPoolMgr::GetInstance()->use_Object(L"NPC"));

	if (pNew)
	{
		/* NPC의 정보 초기화 */
		pNew->Ready_NPC(_vec3(148.8f, 1.f, 89.0f), _vec3(0.f, -90.f, 0.f), NPC_NONMOVE, NPC_POPORI_BOY, STAGE_VELIKA);
		pNew->Set_NumAnimation(Popori_boy::NUM_ANIMATION);
		pNew->Set_AnimDuration(Popori_boy::duration);
	}
	else return;

	// NPC - Quest 1
	pNew = static_cast<CNpc*>(CObjPoolMgr::GetInstance()->use_Object(L"NPC"));

	if (pNew)
	{
		/* NPC의 정보 초기화 */
		pNew->Ready_NPC(_vec3(104.1f, 0.f, 95.0f), _vec3(0.f, -180.f, 0.f), NPC_QUEST, NPC_CASTANIC_LSMITH, STAGE_VELIKA);
		pNew->Set_NumAnimation(Castanic_M_Lsmith::NUM_ANIMATION);
		pNew->Set_AnimDuration(Castanic_M_Lsmith::duration);
	}
	else return;

	// NPC - Baraka_M_Extractor (문지기)
	pNew = static_cast<CNpc*>(CObjPoolMgr::GetInstance()->use_Object(L"NPC"));

	if (pNew)
	{
		/* NPC의 정보 초기화 */
		pNew->Ready_NPC(_vec3(122.6f, 0.f, 25.0f), _vec3(0.f, 0.f, 0.f), NPC_NONMOVE, NPC_BARAKA_EXTRACTOR, STAGE_VELIKA);
		pNew->Set_NumAnimation(Baraka_M_Extractor::NUM_ANIMATION);
		pNew->Set_AnimDuration(Baraka_M_Extractor::duration);
	}
	else return;

	// NPC - Baraka_M_Extractor (문지기)
	pNew = static_cast<CNpc*>(CObjPoolMgr::GetInstance()->use_Object(L"NPC"));

	if (pNew)
	{
		/* NPC의 정보 초기화 */
		pNew->Ready_NPC(_vec3(138.2f, 0.f, 25.0f), _vec3(0.f, 0.f, 0.f), NPC_NONMOVE, NPC_BARAKA_EXTRACTOR, STAGE_VELIKA);
		pNew->Set_NumAnimation(Baraka_M_Extractor::NUM_ANIMATION);
		pNew->Set_AnimDuration(Baraka_M_Extractor::duration);
	}
	else return;

	// NPC - Baraka_M_Extractor (문지기)
	pNew = static_cast<CNpc*>(CObjPoolMgr::GetInstance()->use_Object(L"NPC"));

	if (pNew)
	{
		/* NPC의 정보 초기화 */
		pNew->Ready_NPC(_vec3(110.158f, 0.f, 206.25f), _vec3(0.f, -180.f, 0.f), NPC_NONMOVE, NPC_BARAKA_EXTRACTOR, STAGE_VELIKA);
		pNew->Set_NumAnimation(Baraka_M_Extractor::NUM_ANIMATION);
		pNew->Set_AnimDuration(Baraka_M_Extractor::duration);
	}
	else return;

	// NPC - Baraka_M_Extractor (문지기)
	pNew = static_cast<CNpc*>(CObjPoolMgr::GetInstance()->use_Object(L"NPC"));

	if (pNew)
	{
		/* NPC의 정보 초기화 */
		pNew->Ready_NPC(_vec3(120.58f, 0.f, 206.25f), _vec3(0.f, 180.f, 0.f), NPC_NONMOVE, NPC_BARAKA_EXTRACTOR, STAGE_VELIKA);
		pNew->Set_NumAnimation(Baraka_M_Extractor::NUM_ANIMATION);
		pNew->Set_AnimDuration(Baraka_M_Extractor::duration);
	}
	else return;

	// NPC - Walker (Chicken)
	pNew = static_cast<CNpc*>(CObjPoolMgr::GetInstance()->use_Object(L"NPC"));

	if (pNew)
	{
		/* NPC의 정보 초기화 */
		pNew->Ready_NPC(_vec3(114.773f, 0.f, 67.6f), _vec3(0.f, 0.f, 0.f), NPC_MOVE, NPC_CHICKEN, STAGE_VELIKA);
		pNew->Set_NumAnimation(Chicken::NUM_ANIMATION);
		pNew->Set_AnimDuration(Chicken::duration);
	}
	else return;

	// NPC - Walker (Cat)
	pNew = static_cast<CNpc*>(CObjPoolMgr::GetInstance()->use_Object(L"NPC"));

	if (pNew)
	{
		/* NPC의 정보 초기화 */
		pNew->Ready_NPC(_vec3(87.91f, 0.f, 84.52f), _vec3(0.f, 0.f, 0.f), NPC_MOVE, NPC_CAT, STAGE_VELIKA);
		pNew->Set_NumAnimation(Cat::NUM_ANIMATION);
		pNew->Set_AnimDuration(Cat::duration);
	}
	else return;

	// NPC - Walker (Aman_boy)
	pNew = static_cast<CNpc*>(CObjPoolMgr::GetInstance()->use_Object(L"NPC"));

	if (pNew)
	{
		/* NPC의 정보 초기화 */
		pNew->Ready_NPC(_vec3(103.84f, 0.f, 125.85f), _vec3(0.f, 0.f, 0.f), NPC_MOVE, NPC_AMAN_BOY, STAGE_VELIKA);
		pNew->Set_NumAnimation(Aman_boy::NUM_ANIMATION);
		pNew->Set_AnimDuration(Aman_boy::duration);
	}
	else return;

	// NPC - Walker (Chicken)
	pNew = static_cast<CNpc*>(CObjPoolMgr::GetInstance()->use_Object(L"NPC"));

	if (pNew)
	{
		/* NPC의 정보 초기화 */
		pNew->Ready_NPC(_vec3(118.9f, 0.f, 159.6f), _vec3(0.f, 0.f, 0.f), NPC_MOVE, NPC_CHICKEN, STAGE_VELIKA);
		pNew->Set_NumAnimation(Chicken::NUM_ANIMATION);
		pNew->Set_AnimDuration(Chicken::duration);
	}
	else return;

	// NPC - Walker (Cat)
	pNew = static_cast<CNpc*>(CObjPoolMgr::GetInstance()->use_Object(L"NPC"));

	if (pNew)
	{
		/* NPC의 정보 초기화 */
		pNew->Ready_NPC(_vec3(89.02f, 0.f, 165.f), _vec3(0.f, 0.f, 0.f), NPC_MOVE, NPC_CAT, STAGE_VELIKA);
		pNew->Set_NumAnimation(Cat::NUM_ANIMATION);
		pNew->Set_AnimDuration(Cat::duration);
	}
	else return;

	// NPC - Walker (Chicken)
	pNew = static_cast<CNpc*>(CObjPoolMgr::GetInstance()->use_Object(L"NPC"));

	if (pNew)
	{
		/* NPC의 정보 초기화 */
		pNew->Ready_NPC(_vec3(108.9f, 0.f, 155.6f), _vec3(0.f, 0.f, 0.f), NPC_MOVE, NPC_CHICKEN, STAGE_VELIKA);
		pNew->Set_NumAnimation(Chicken::NUM_ANIMATION);
		pNew->Set_AnimDuration(Chicken::duration);
	}
	else return;

	// NPC - Walker (Cat)
	pNew = static_cast<CNpc*>(CObjPoolMgr::GetInstance()->use_Object(L"NPC"));

	if (pNew)
	{
		/* NPC의 정보 초기화 */
		pNew->Ready_NPC(_vec3(129.02f, 0.f, 143.f), _vec3(0.f, 0.f, 0.f), NPC_MOVE, NPC_CAT, STAGE_VELIKA);
		pNew->Set_NumAnimation(Cat::NUM_ANIMATION);
		pNew->Set_AnimDuration(Cat::duration);
	}
	else return;

	// NPC - Walker (Aman_boy)
	pNew = static_cast<CNpc*>(CObjPoolMgr::GetInstance()->use_Object(L"NPC"));

	if (pNew)
	{
		/* NPC의 정보 초기화 */
		pNew->Ready_NPC(_vec3(63.5f, 0.f, 146.3f), _vec3(0.f, 0.f, 0.f), NPC_MOVE, NPC_AMAN_BOY, STAGE_VELIKA);
		pNew->Set_NumAnimation(Aman_boy::NUM_ANIMATION);
		pNew->Set_AnimDuration(Aman_boy::duration);
	}
	else return;

	// NPC - Walker (Aman_boy)
	pNew = static_cast<CNpc*>(CObjPoolMgr::GetInstance()->use_Object(L"NPC"));

	if (pNew)
	{
		/* NPC의 정보 초기화 */
		pNew->Ready_NPC(_vec3(175.4f, 0.f, 53.3f), _vec3(0.f, 0.f, 0.f), NPC_MOVE, NPC_AMAN_BOY, STAGE_VELIKA);
		pNew->Set_NumAnimation(Aman_boy::NUM_ANIMATION);
		pNew->Set_AnimDuration(Aman_boy::duration);
	}
	else return;

	// NPC - Walker (Aman_boy)
	pNew = static_cast<CNpc*>(CObjPoolMgr::GetInstance()->use_Object(L"NPC"));

	if (pNew)
	{
		/* NPC의 정보 초기화 */
		pNew->Ready_NPC(_vec3(147.02f, 0.f, 59.3f), _vec3(0.f, 0.f, 0.f), NPC_MOVE, NPC_AMAN_BOY, STAGE_VELIKA);
		pNew->Set_NumAnimation(Aman_boy::NUM_ANIMATION);
		pNew->Set_AnimDuration(Aman_boy::duration);
	}
	else return;

	// NPC - Walker (Aman_boy)
	pNew = static_cast<CNpc*>(CObjPoolMgr::GetInstance()->use_Object(L"NPC"));

	if (pNew)
	{
		/* NPC의 정보 초기화 */
		pNew->Ready_NPC(_vec3(131.25f, 0.f, 57.98f), _vec3(0.f, 0.f, 0.f), NPC_MOVE, NPC_AMAN_BOY, STAGE_VELIKA);
		pNew->Set_NumAnimation(Aman_boy::NUM_ANIMATION);
		pNew->Set_AnimDuration(Aman_boy::duration);
	}
	else return;

	// NPC - Walker (Human Boy)
	pNew = static_cast<CNpc*>(CObjPoolMgr::GetInstance()->use_Object(L"NPC"));

	if (pNew)
	{
		/* NPC의 정보 초기화 */
		pNew->Ready_NPC(_vec3(88.16f, 0.f, 143.19f), _vec3(0.f, -180.f, 0.f), NPC_MOVE, NPC_HUMAN_BOY, STAGE_VELIKA);
		pNew->Set_NumAnimation(Human_boy::NUM_ANIMATION);
		pNew->Set_AnimDuration(Human_boy::duration);
	}
	else return;

	// NPC - Walker (Human girl)
	pNew = static_cast<CNpc*>(CObjPoolMgr::GetInstance()->use_Object(L"NPC"));

	if (pNew)
	{
		/* NPC의 정보 초기화 */
		pNew->Ready_NPC(_vec3(100.13f, 0.f, 138.62f), _vec3(0.f, -180.f, 0.f), NPC_MOVE, NPC_HUMAN_GIRL, STAGE_VELIKA);
		pNew->Set_NumAnimation(Human_girl::NUM_ANIMATION);
		pNew->Set_AnimDuration(Human_girl::duration);
	}
	else return;

	// NPC - Walker (Highelf girl)
	pNew = static_cast<CNpc*>(CObjPoolMgr::GetInstance()->use_Object(L"NPC"));

	if (pNew)
	{
		/* NPC의 정보 초기화 */
		pNew->Ready_NPC(_vec3(142.43f, 0.f, 125.3f), _vec3(0.f, 0.f, 0.f), NPC_MOVE, NPC_HIGHELF_GIRL, STAGE_VELIKA);
		pNew->Set_NumAnimation(Highelf_girl::NUM_ANIMATION);
		pNew->Set_AnimDuration(Highelf_girl::duration);
	}
	else return;

	// NPC - Villigers
	pNew = static_cast<CNpc*>(CObjPoolMgr::GetInstance()->use_Object(L"NPC"));

	if (pNew)
	{
		/* NPC의 정보 초기화 */
		pNew->Ready_NPC(_vec3(56.30f, 0.f, 83.18f), _vec3(0.f, 15.f, 0.f), NPC_NONMOVE, NPC_VILLAGERS, STAGE_VELIKA);
		pNew->Set_NumAnimation(NPC_Villagers::NUM_ANIMATION);
		pNew->Set_AnimDuration(NPC_Villagers::duration);
	}
	else return;

	// NPC - Villigers
	pNew = static_cast<CNpc*>(CObjPoolMgr::GetInstance()->use_Object(L"NPC"));

	if (pNew)
	{
		/* NPC의 정보 초기화 */
		pNew->Ready_NPC(_vec3(79.52f, 0.f, 57.38f), _vec3(0.f, 0.f, 0.f), NPC_NONMOVE, NPC_VILLAGERS, STAGE_VELIKA);
		pNew->Set_NumAnimation(NPC_Villagers::NUM_ANIMATION);
		pNew->Set_AnimDuration(NPC_Villagers::duration);
	}
	else return;

	// NPC - Villigers
	pNew = static_cast<CNpc*>(CObjPoolMgr::GetInstance()->use_Object(L"NPC"));

	if (pNew)
	{
		/* NPC의 정보 초기화 */
		pNew->Ready_NPC(_vec3(189.77f, 0.f, 94.47f), _vec3(0.f, -75.f, 0.f), NPC_NONMOVE, NPC_VILLAGERS, STAGE_VELIKA);
		pNew->Set_NumAnimation(NPC_Villagers::NUM_ANIMATION);
		pNew->Set_AnimDuration(NPC_Villagers::duration);
	}
	else return;

	// NPC - Children (Human Boy)
	pNew = static_cast<CNpc*>(CObjPoolMgr::GetInstance()->use_Object(L"NPC"));

	if (pNew)
	{
		/* NPC의 정보 초기화 */
		pNew->Ready_NPC(_vec3(109.4f, 0.f, 83.9f), _vec3(0.f, -180.f, 0.f), NPC_NONMOVE, NPC_HUMAN_BOY, STAGE_VELIKA);
		pNew->Set_NumAnimation(Human_boy::NUM_ANIMATION);
		pNew->Set_AnimDuration(Human_boy::duration);
	}
	else return;

	// NPC - Children (Human girl)
	pNew = static_cast<CNpc*>(CObjPoolMgr::GetInstance()->use_Object(L"NPC"));

	if (pNew)
	{
		/* NPC의 정보 초기화 */
		pNew->Ready_NPC(_vec3(109.6f, 0.f, 81.87f), _vec3(0.f, 0.f, 0.f), NPC_NONMOVE, NPC_HUMAN_GIRL, STAGE_VELIKA);
		pNew->Set_NumAnimation(Human_girl::NUM_ANIMATION);
		pNew->Set_AnimDuration(Human_girl::duration);
	}
	else return;

	// NPC - Children (Human girl)
	pNew = static_cast<CNpc*>(CObjPoolMgr::GetInstance()->use_Object(L"NPC"));

	if (pNew)
	{
		/* NPC의 정보 초기화 */
		pNew->Ready_NPC(_vec3(91.13f, 0.f, 104.78f), _vec3(0.f, -180.f, 0.f), NPC_NONMOVE, NPC_HUMAN_GIRL, STAGE_VELIKA);
		pNew->Set_NumAnimation(Human_girl::NUM_ANIMATION);
		pNew->Set_AnimDuration(Human_girl::duration);
	}
	else return;

	// NPC - Children (Highelf girl)
	pNew = static_cast<CNpc*>(CObjPoolMgr::GetInstance()->use_Object(L"NPC"));

	if (pNew)
	{
		/* NPC의 정보 초기화 */
		pNew->Ready_NPC(_vec3(91.33f, 0.f, 103.3f), _vec3(0.f, 0.f, 0.f), NPC_NONMOVE, NPC_HIGHELF_GIRL, STAGE_VELIKA);
		pNew->Set_NumAnimation(Highelf_girl::NUM_ANIMATION);
		pNew->Set_AnimDuration(Highelf_girl::duration);
	}
	else return;

	// NPC - Children (Highelf girl)
	pNew = static_cast<CNpc*>(CObjPoolMgr::GetInstance()->use_Object(L"NPC"));

	if (pNew)
	{
		/* NPC의 정보 초기화 */
		pNew->Ready_NPC(_vec3(176.3f, 0.f, 118.5f), _vec3(0.f, -180.f, 0.f), NPC_NONMOVE, NPC_HIGHELF_GIRL, STAGE_VELIKA);
		pNew->Set_NumAnimation(Highelf_girl::NUM_ANIMATION);
		pNew->Set_AnimDuration(Highelf_girl::duration);
	}
	else return;

	// NPC - Children (Human Boy)
	pNew = static_cast<CNpc*>(CObjPoolMgr::GetInstance()->use_Object(L"NPC"));

	if (pNew)
	{
		/* NPC의 정보 초기화 */
		pNew->Ready_NPC(_vec3(177.821f, 0.f, 118.03f), _vec3(0.f, -90.f, 0.f), NPC_NONMOVE, NPC_HUMAN_BOY, STAGE_VELIKA);
		pNew->Set_NumAnimation(Human_boy::NUM_ANIMATION);
		pNew->Set_AnimDuration(Human_boy::duration);
	}
	else return;

	// NPC - Children (Human Boy)
	pNew = static_cast<CNpc*>(CObjPoolMgr::GetInstance()->use_Object(L"NPC"));

	if (pNew)
	{
		/* NPC의 정보 초기화 */
		pNew->Ready_NPC(_vec3(177.3f, 0.f, 116.5f), _vec3(0.f, 0.f, 0.f), NPC_NONMOVE, NPC_HUMAN_BOY, STAGE_VELIKA);
		pNew->Set_NumAnimation(Human_boy::NUM_ANIMATION);
		pNew->Set_AnimDuration(Human_boy::duration);
	}
	else return;

	// NPC - Children (Human Boy)
	pNew = static_cast<CNpc*>(CObjPoolMgr::GetInstance()->use_Object(L"NPC"));

	if (pNew)
	{
		/* NPC의 정보 초기화 */
		pNew->Ready_NPC(_vec3(193.17f, 0.f, 68.65f), _vec3(0.f, -180.f, 0.f), NPC_NONMOVE, NPC_HUMAN_BOY, STAGE_VELIKA);
		pNew->Set_NumAnimation(Human_boy::NUM_ANIMATION);
		pNew->Set_AnimDuration(Human_boy::duration);
	}
	else return;

	// NPC - Children (Highelf girl)
	pNew = static_cast<CNpc*>(CObjPoolMgr::GetInstance()->use_Object(L"NPC"));

	if (pNew)
	{
		/* NPC의 정보 초기화 */
		pNew->Ready_NPC(_vec3(192.98f, 0.f, 66.44f), _vec3(0.f, 0.f, 0.f), NPC_NONMOVE, NPC_HIGHELF_GIRL, STAGE_VELIKA);
		pNew->Set_NumAnimation(Highelf_girl::NUM_ANIMATION);
		pNew->Set_AnimDuration(Highelf_girl::duration);
	}
	else return;

	// NPC - Children (Human girl)
	pNew = static_cast<CNpc*>(CObjPoolMgr::GetInstance()->use_Object(L"NPC"));

	if (pNew)
	{
		/* NPC의 정보 초기화 */
		pNew->Ready_NPC(_vec3(192.08f, 0.f, 67.51f), _vec3(0.f, 90.f, 0.f), NPC_NONMOVE, NPC_HUMAN_GIRL, STAGE_VELIKA);
		pNew->Set_NumAnimation(Human_girl::NUM_ANIMATION);
		pNew->Set_AnimDuration(Human_girl::duration);
	}
	else return;

	// NPC - Children (Human Boy)
	pNew = static_cast<CNpc*>(CObjPoolMgr::GetInstance()->use_Object(L"NPC"));

	if (pNew)
	{
		/* NPC의 정보 초기화 */
		pNew->Ready_NPC(_vec3(123.95f, 0.f, 81.344f), _vec3(0.f, -180.f, 0.f), NPC_NONMOVE, NPC_HUMAN_BOY, STAGE_VELIKA);
		pNew->Set_NumAnimation(Human_boy::NUM_ANIMATION);
		pNew->Set_AnimDuration(Human_boy::duration);
	}
	else return;

	// NPC - Children (Highelf girl)
	pNew = static_cast<CNpc*>(CObjPoolMgr::GetInstance()->use_Object(L"NPC"));

	if (pNew)
	{
		/* NPC의 정보 초기화 */
		pNew->Ready_NPC(_vec3(123.73f, 0.f, 79.61f), _vec3(0.f, 0.f, 0.f), NPC_NONMOVE, NPC_HIGHELF_GIRL, STAGE_VELIKA);
		pNew->Set_NumAnimation(Highelf_girl::NUM_ANIMATION);
		pNew->Set_AnimDuration(Highelf_girl::duration);
	}
	else return;

	// NPC - Children (Human girl)
	pNew = static_cast<CNpc*>(CObjPoolMgr::GetInstance()->use_Object(L"NPC"));

	if (pNew)
	{
		/* NPC의 정보 초기화 */
		pNew->Ready_NPC(_vec3(122.52f, 0.f, 80.5f), _vec3(0.f, 90.f, 0.f), NPC_NONMOVE, NPC_HUMAN_GIRL, STAGE_VELIKA);
		pNew->Set_NumAnimation(Human_girl::NUM_ANIMATION);
		pNew->Set_AnimDuration(Human_girl::duration);
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

bool CObjMgr::Is_AI(int server_num)
{
	objmgr_lock ol(m_mutex);

	/* NPC ObjList 에서 찾고자 하는 OBJLIST를 key 값을 통해 찾기 */
	auto& iter_find = m_mapObjList[L"AI"].find(server_num);

	/* 해당 OBJLIST를 찾지 못하였다면 NULL 반환 */
	if (iter_find == m_mapObjList[L"AI"].end())
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

HRESULT CObjMgr::Create_Party(int* iPartyNumber, const int& server_num)
{
	if (*iPartyNumber == INIT_PARTY_NUMBER)
	{
		int party_num = m_mapPartyList.size();

		/* map에서 찾고자 하는 PARTYLIST를 key 값을 통해 찾기 */
		auto& iter_find = m_mapPartyList.find(party_num);

		/* 해당 PARTYLIST가 없다면, 임시 PARTYLIST 생성한 후 오브젝트 삽입 */
		if (iter_find == m_mapPartyList.end())
			m_mapPartyList[party_num] = PARTYLIST();

		m_mapPartyList[party_num].insert(server_num);
		*iPartyNumber = party_num;

		return S_OK;
	}
	
	return E_FAIL;
}

HRESULT CObjMgr::Add_PartyMember(const int& iPartyNumber, int* responderPartyNum, const int& server_num)
{
	/* map에서 찾고자 하는 PARTYLIST를 key 값을 통해 찾기 */
	auto& iter_find = m_mapPartyList.find(iPartyNumber);

	/* 해당 PARTYLIST가 없다면, FAIL */
	if (iter_find == m_mapPartyList.end())
		return E_FAIL;

	m_mapPartyList[iPartyNumber].insert(server_num);
	*responderPartyNum = iPartyNumber;

	return S_OK;
}

HRESULT CObjMgr::Leave_Party(int* iPartyNumber, const int& server_num)
{
	/* map에서 찾고자 하는 PARTYLIST를 key 값을 통해 찾기 */
	auto& iter_find = m_mapPartyList.find(*iPartyNumber);

	/* 해당 PARTYLIST가 없다면, FAIL */
	if (iter_find == m_mapPartyList.end())
		return E_FAIL;

	/* 해당 파티에서 해당 유저 삭제 */
	m_mapPartyList[*iPartyNumber].erase(server_num);

	/* 해당 파티 잔여 인원이 0명이라면 파티 소멸 */
	if (m_mapPartyList[*iPartyNumber].size() < 1)
	{
		m_mapPartyList.erase(*iPartyNumber);
	}

	//Print_PartyInfo(*iPartyNumber);

	*iPartyNumber = INIT_PARTY_NUMBER;

	return S_OK;
}

void CObjMgr::Print_PartyInfo(const int& iPartyNumber)
{
	/* map에서 찾고자 하는 PARTYLIST를 key 값을 통해 찾기 */
	auto& iter_find = m_mapPartyList.find(iPartyNumber);

	/* 해당 OBJLIST가 없다면, FAIL */
	if (iter_find == m_mapPartyList.end())
		return;

	cout << iPartyNumber << "번 파티 멤버 수: " << m_mapPartyList[iPartyNumber].size();
	cout << "파티 멤버 서버 넘버: ";
	for (auto& p : m_mapPartyList[iPartyNumber])
	{
		cout << p << ", ";
	}
	cout << endl;
}

void CObjMgr::Release()
{
	for (auto& o_list : m_mapObjList)
	{	
		o_list.second.clear();
	}

	for (auto& p_list : m_mapPartyList)
	{
		p_list.second.clear();
	}
}
