#include "stdafx.h"
#include "CinemaMgr.h"
#include "InstancePoolMgr.h"
#include "ObjectMgr.h"
#include "Lakan.h"
#include "PrionBerserker.h"
#include "PrionBerserkerBoss.h"
#include "CinemaVergos.h"
#include <iterator>
#include <random>
#include "DirectInput.h"

IMPLEMENT_SINGLETON(CCinemaMgr)

CCinemaMgr::CCinemaMgr()
{
	m_pObjectMgr = Engine::CObjectMgr::Get_Instance();
}

void CCinemaMgr::Set_IsUpdateVergos(const _bool& bIsUpdate)
{
	if (nullptr != m_pVergos)
		static_cast<CCinemaVergos*>(m_pVergos)->Set_IsUpdate(bIsUpdate);
}

HRESULT CCinemaMgr::Ready_CinemaCharacter()
{
	/* CINEMATIC MONSTER */

	// Lakan
	Engine::CGameObject* pGameObj = nullptr;
	for (int z = 0; z < LINEUP_Z; ++z)
	{
		for (int x = 0; x < LINEUP_X; ++x)
		{
			pGameObj = Pop_Instance(CInstancePoolMgr::Get_Instance()->Get_MonsterLakanPool());

			if (nullptr != pGameObj)
			{
				pGameObj->Get_Transform()->m_vScale = _vec3(0.1f);
				pGameObj->Get_Transform()->m_vAngle = _vec3(0.f);
				pGameObj->Get_Transform()->m_vPos = _vec3(350.f + (x * 8), 0.f, 340.f + (z * 6));
				pGameObj->Set_State(0);
				
				m_pObjectMgr->Add_GameObject(L"Layer_GameObject", L"LAKAN", pGameObj);
				arrLakan[z][x] = pGameObj;
				m_vecLakan.emplace_back(pGameObj);
				static_cast<CLakan*>(pGameObj)->Set_OriginPos(pGameObj->Get_Transform()->m_vPos);

				if (z == 0)
					m_vecAnimationLakan.emplace_back(pGameObj);

				if (z == LINEUP_Z - 1 && x == LINEUP_X / 2)
					m_pCenterLakan = pGameObj;
			}
		}
	}

	// PrionBerserker
	for (int z = 0; z < LINEUP_Z; ++z)
	{
		for (int x = 0; x < LINEUP_X; ++x)
		{
			pGameObj = Pop_Instance(CInstancePoolMgr::Get_Instance()->Get_MonsterPrionBerserkerPool());

			if (nullptr != pGameObj)
			{
				pGameObj->Get_Transform()->m_vScale = _vec3(0.1f);
				pGameObj->Get_Transform()->m_vAngle = _vec3(0.f, -180.f, 0.f);
				if (z == 0)
				{
					if (x % 2 == 0)
					{
						pGameObj->Get_Transform()->m_vPos = _vec3(345.f + (x * 10), 0.f, 425.f + (z * 6));
						static_cast<CPrionBerserker*>(pGameObj)->Set_OriginPos(pGameObj->Get_Transform()->m_vPos);
					}
					else
					{
						pGameObj->Get_Transform()->m_vPos = _vec3(345.f + (x * 10), 0.f, 425.f + (z * 5));
						static_cast<CPrionBerserker*>(pGameObj)->Set_OriginPos(pGameObj->Get_Transform()->m_vPos);
					}
				}
				else if (z == 1)
				{
					if (x % 2 == 0)
					{
						pGameObj->Get_Transform()->m_vPos = _vec3(320.f + (x * 15), 0.f, 427.f + (z * 8));
						static_cast<CPrionBerserker*>(pGameObj)->Set_OriginPos(pGameObj->Get_Transform()->m_vPos);
					}
					else
					{
						pGameObj->Get_Transform()->m_vPos = _vec3(320.f + (x * 15), 0.f, 427.f + (z * 6));
						static_cast<CPrionBerserker*>(pGameObj)->Set_OriginPos(pGameObj->Get_Transform()->m_vPos);
					}
				}
				else if (z == 2)
				{
					if (x % 2 == 0)
					{
						pGameObj->Get_Transform()->m_vPos = _vec3(335.f + (x * 12), 0.f, 425.f + (z * 9));
						static_cast<CPrionBerserker*>(pGameObj)->Set_OriginPos(pGameObj->Get_Transform()->m_vPos);
					}
					else
					{
						pGameObj->Get_Transform()->m_vPos = _vec3(335.f + (x * 12), 0.f, 425.f + (z * 8));
						static_cast<CPrionBerserker*>(pGameObj)->Set_OriginPos(pGameObj->Get_Transform()->m_vPos);
					}
				}
				else if (z == 3)
				{
					if (x % 2 == 0)
					{
						pGameObj->Get_Transform()->m_vPos = _vec3(347.f + (x * 9), 0.f, 425.f + (z * 9));
						static_cast<CPrionBerserker*>(pGameObj)->Set_OriginPos(pGameObj->Get_Transform()->m_vPos);
					}
					else
					{
						pGameObj->Get_Transform()->m_vPos = _vec3(347.f + (x * 9), 0.f, 425.f + (z * 8));
						static_cast<CPrionBerserker*>(pGameObj)->Set_OriginPos(pGameObj->Get_Transform()->m_vPos);
					}
				}
				else if (z == 4)
				{
					if (x % 2 == 0)
					{
						pGameObj->Get_Transform()->m_vPos = _vec3(325.f + (x * 14), 0.f, 425.f + (z * 9));
						static_cast<CPrionBerserker*>(pGameObj)->Set_OriginPos(pGameObj->Get_Transform()->m_vPos);
					}
					else
					{
						pGameObj->Get_Transform()->m_vPos = _vec3(325.f + (x * 14), 0.f, 425.f + (z * 8));
						static_cast<CPrionBerserker*>(pGameObj)->Set_OriginPos(pGameObj->Get_Transform()->m_vPos);
					}
				}
				
				pGameObj->Set_State(0);
				m_pObjectMgr->Add_GameObject(L"Layer_GameObject", L"PRIONBERSERKER", pGameObj);
				arrPrionBerserker[z][x] = pGameObj;
				m_vecPrionBerserker.emplace_back(pGameObj);
				m_vecAnimationPrionBerserker.emplace_back(pGameObj);

				if (z == 0)
					m_vecAnimationPrionBerserker.emplace_back(pGameObj);
			}
		}

	}

	// PrionBerserkerBoss
	pGameObj = Pop_Instance(CInstancePoolMgr::Get_Instance()->Get_MonsterPrionBerserkerBossPool());

	if (nullptr != pGameObj)
	{
		pGameObj->Get_Transform()->m_vScale = _vec3(0.2f);
		pGameObj->Get_Transform()->m_vAngle = _vec3(0.f, -180.f, 0.f);
		pGameObj->Get_Transform()->m_vPos   = _vec3(387.f, 0.f, 415.f);
		pGameObj->Set_State(0);
		static_cast<CPrionBerserkerBoss*>(pGameObj)->Set_OriginPos(pGameObj->Get_Transform()->m_vPos);
		m_pObjectMgr->Add_GameObject(L"Layer_GameObject", L"PRIONBERSERKERBOSS", pGameObj);
		m_pPrionBerserkerBoss = pGameObj;
	}

	// Cinema Vergos
	pGameObj = Pop_Instance(CInstancePoolMgr::Get_Instance()->Get_MonsterCinemaVergosPool());

	if (nullptr != pGameObj)
	{
		pGameObj->Get_Transform()->m_vScale = _vec3(0.05f);
		// pGameObj->Get_Transform()->m_vAngle = _vec3(35.f, -180.f, 0.f);
		pGameObj->Get_Transform()->m_vAngle = _vec3(0.0f, -180.f, 0.f);
		pGameObj->Get_Transform()->m_vPos = _vec3(260.f, 98.0f, 570.f);
		pGameObj->Set_State(0);
		m_pObjectMgr->Add_GameObject(L"Layer_GameObject", L"CINEMAVERGOS", pGameObj);
		m_pVergos = pGameObj;
	}

	for (auto& pObj : m_vecAnimationLakan)
	{
		random_device					rd;
		default_random_engine			dre{ rd() };
		uniform_int_distribution<_int>	uid{ 0, 6400 };

		static_cast<CLakan*>(pObj)->Get_MeshComponent()->Set_AnimationTime(_float(uid(dre)));
	}

	for (auto& pObj : m_vecAnimationPrionBerserker)
	{
		random_device					rd;
		default_random_engine			dre{ rd() };
		uniform_int_distribution<_int>	uid{ 0, 9600 };

		static_cast<CPrionBerserker*>(pObj)->Get_MeshComponent()->Set_AnimationTime(_float(uid(dre)));
	}

	random_device	rd;
	mt19937			g(rd());
	shuffle(m_vecLakan.begin(), m_vecLakan.end(), g);
	shuffle(m_vecPrionBerserker.begin(), m_vecPrionBerserker.end(), g);

	m_fLakanAnimationSpeed = 0.5f;
	m_fPrionBerserkerAnimationSpeed = 0.5f;

    return S_OK;
}

void CCinemaMgr::Update_Animation(const _float& fTimeDelta)
{
	for (auto& pObj : m_vecAnimationLakan)
		static_cast<CLakan*>(pObj)->Get_MeshComponent()->Play_Animation(fTimeDelta * TPS * m_fLakanAnimationSpeed);

	for (auto& pObj : m_vecLakan)
	{
		_uint uiAnimationObjIdx = static_cast<CLakan*>(pObj)->Get_AnimationObjectIdx();
		vector<Engine::VECTOR_SKINNING_MATRIX> vecSkinningMatrix = *(static_cast<CLakan*>(m_vecAnimationLakan[uiAnimationObjIdx])->Get_MeshComponent()->Get_AniCtrl()->Get_VecSkinningMatrix());

		pObj->Set_MoveStop(m_vecAnimationLakan[uiAnimationObjIdx]->Get_MoveStop());
		static_cast<CLakan*>(pObj)->Get_MeshComponent()->Set_VecSkinningMatrix(vecSkinningMatrix);
	}

	for (auto& pObj : m_vecAnimationPrionBerserker)
		static_cast<CPrionBerserker*>(pObj)->Get_MeshComponent()->Play_Animation(fTimeDelta * TPS * m_fPrionBerserkerAnimationSpeed, static_cast<CPrionBerserker*>(pObj)->Get_IsRepeatAnimation());

	for (auto& pObj : m_vecPrionBerserker)
	{
		_uint uiAnimationObjIdx = static_cast<CPrionBerserker*>(pObj)->Get_AnimationObjectIdx();
		vector<Engine::VECTOR_SKINNING_MATRIX> vecSkinningMatrix = *(static_cast<CPrionBerserker*>(m_vecAnimationPrionBerserker[uiAnimationObjIdx])->Get_MeshComponent()->Get_AniCtrl()->Get_VecSkinningMatrix());

		pObj->Set_MoveStop(m_vecAnimationPrionBerserker[uiAnimationObjIdx]->Get_MoveStop());
		static_cast<CPrionBerserker*>(pObj)->Get_MeshComponent()->Set_VecSkinningMatrix(vecSkinningMatrix);
	}
}

void CCinemaMgr::Spawn_Vergos()
{
	m_pObjectMgr->Get_GameObject(L"Layer_GameObject", L"CINEMAVERGOS", 0)->Set_State(CinemaVergos::SPAWN);
}

void CCinemaMgr::Scream_PrionBerserkerBoss()
{
	m_pObjectMgr->Get_GameObject(L"Layer_GameObject", L"PRIONBERSERKERBOSS", 0)->Set_State(PrionBerserkerBoss::ANGRY);
}

void CCinemaMgr::Scream_PrionBerserkers()
{
	m_fPrionBerserkerAnimationSpeed = 0.7f;

	for (int z = 0; z < LINEUP_Z; ++z)
	{
		for (int x = 0; x < LINEUP_X; ++x)
		{
			arrPrionBerserker[z][x]->Set_State(PrionBerserker::ANGRY);
		}
	}
}

void CCinemaMgr::Command_PrionBerserkerBoss()
{
	m_pObjectMgr->Get_GameObject(L"Layer_GameObject", L"PRIONBERSERKERBOSS", 0)->Set_State(PrionBerserkerBoss::COMMAND);
}

void CCinemaMgr::Rush_Prion()
{
	m_fPrionBerserkerAnimationSpeed = 0.3f;
	//m_pObjectMgr->Get_GameObject(L"Layer_GameObject", L"PRIONBERSERKERBOSS", 0)->Set_State(PrionBerserkerBoss::RUN);
	//m_pObjectMgr->Get_GameObject(L"Layer_GameObject", L"PRIONBERSERKERBOSS", 0)->Set_MoveStop(false);

	for (int z = 0; z < LINEUP_Z; ++z)
	{
		for (int x = 0; x < LINEUP_X; ++x)
		{
			arrPrionBerserker[z][x]->Set_State(PrionBerserker::RUN);
			arrPrionBerserker[z][x]->Set_MoveStop(false);
		}
	}
}

void CCinemaMgr::Rush_Lakan()
{
	m_fLakanAnimationSpeed = 0.75f;

	for (int z = 0; z < LINEUP_Z; ++z)
	{
		for (int x = 0; x < LINEUP_X; ++x)
		{	
			arrLakan[z][x]->Set_State(Lakan::ACTIVE_MOVE_START);		
		}
	}
}

void CCinemaMgr::Reset_LakanPosition()
{
	for (auto& pLakan : m_vecLakan)
	{
		static_cast<CLakan*>(pLakan)->Reset_Position();
		static_cast<CLakan*>(pLakan)->Set_State(Lakan::A_WAIT);
		static_cast<CLakan*>(pLakan)->Set_MoveStop(true);
	}

	for (auto& pObj : m_vecAnimationLakan)
	{
		random_device					rd;
		default_random_engine			dre{ rd() };
		uniform_int_distribution<_int>	uid{ 0, 6400 };

		static_cast<CLakan*>(pObj)->Get_MeshComponent()->Set_AnimationTime(_float(uid(dre)));
	}
}

void CCinemaMgr::Reset_Vergos()
{
	static_cast<CCinemaVergos*>(m_pVergos)->Set_MonsterState(CinemaVergos::A_WAIT);
	static_cast<CCinemaVergos*>(m_pVergos)->Get_MeshComponent()->Set_AnimationTime(0.0f);
	static_cast<CCinemaVergos*>(m_pVergos)->Set_CurAnimationFrame(0);
	static_cast<CCinemaVergos*>(m_pVergos)->Set_IsUpdate(false);
}

void CCinemaMgr::Reset_PrionBerserkerPosition()
{
	for (auto& pPrionBerserker : m_vecPrionBerserker)
	{
		static_cast<CPrionBerserker*>(pPrionBerserker)->Reset_Position();
		static_cast<CPrionBerserker*>(pPrionBerserker)->Set_State(PrionBerserker::A_WAIT);
		static_cast<CPrionBerserker*>(pPrionBerserker)->Set_MoveStop(true);
	}

	for (auto& pObj : m_vecAnimationPrionBerserker)
	{
		random_device					rd;
		default_random_engine			dre{ rd() };
		uniform_int_distribution<_int>	uid{ 0, 9600 };

		static_cast<CPrionBerserker*>(pObj)->Get_MeshComponent()->Set_AnimationTime(_float(uid(dre)));
	}

	static_cast<CPrionBerserkerBoss*>(m_pPrionBerserkerBoss)->Reset_Position();
	static_cast<CPrionBerserkerBoss*>(m_pPrionBerserkerBoss)->Set_State(PrionBerserkerBoss::A_WAIT);
	static_cast<CPrionBerserkerBoss*>(m_pPrionBerserkerBoss)->Set_MoveStop(true);
	static_cast<CPrionBerserkerBoss*>(m_pPrionBerserkerBoss)->Set_IsPrionBerserkerScreaming(false);
}

void CCinemaMgr::Set_IsMoveStopPrionBerserker(const _bool& bIsMoveStop)
{
	for (auto& pPrionBerserker : m_vecAnimationPrionBerserker)
	{
		static_cast<CPrionBerserker*>(pPrionBerserker)->Set_MoveStop(bIsMoveStop);
	}

	static_cast<CPrionBerserkerBoss*>(m_pPrionBerserkerBoss)->Set_IsMoveStob(bIsMoveStop);
}

void CCinemaMgr::Free()
{
	m_vecAnimationLakan.clear();
	m_vecAnimationLakan.shrink_to_fit();
	m_vecAnimationPrionBerserker.clear();
	m_vecAnimationPrionBerserker.shrink_to_fit();
	m_vecLakan.clear();
	m_vecLakan.shrink_to_fit();
	m_vecPrionBerserker.clear();
	m_vecPrionBerserker.shrink_to_fit();
}
