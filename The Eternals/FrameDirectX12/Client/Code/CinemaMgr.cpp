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

				if (z == 0)
					m_vecAnimationLakan.emplace_back(pGameObj);
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
						pGameObj->Get_Transform()->m_vPos = _vec3(345.f + (x * 10), 0.f, 425.f + (z * 6));
					else
						pGameObj->Get_Transform()->m_vPos = _vec3(345.f + (x * 10), 0.f, 425.f + (z * 5));
				}
				else if (z == 1)
				{
					if (x % 2 == 0)
						pGameObj->Get_Transform()->m_vPos = _vec3(320.f + (x * 15), 0.f, 427.f + (z * 8));
					else
						pGameObj->Get_Transform()->m_vPos = _vec3(320.f + (x * 15), 0.f, 427.f + (z * 6));
				}
				else if (z == 2)
				{
					if (x % 2 == 0)
						pGameObj->Get_Transform()->m_vPos = _vec3(335.f + (x * 12), 0.f, 425.f + (z * 9));
					else
						pGameObj->Get_Transform()->m_vPos = _vec3(335.f + (x * 12), 0.f, 425.f + (z * 8));
				}
				else if (z == 3)
				{
					if (x % 2 == 0)
						pGameObj->Get_Transform()->m_vPos = _vec3(347.f + (x * 9), 0.f, 425.f + (z * 9));
					else
						pGameObj->Get_Transform()->m_vPos = _vec3(347.f + (x * 9), 0.f, 425.f + (z * 8));
				}
				else if (z == 4)
				{
					if (x % 2 == 0)
						pGameObj->Get_Transform()->m_vPos = _vec3(325.f + (x * 14), 0.f, 425.f + (z * 9));
					else
						pGameObj->Get_Transform()->m_vPos = _vec3(325.f + (x * 14), 0.f, 425.f + (z * 8));
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
		pGameObj->Get_Transform()->m_vPos = _vec3(387.f, 0.f, 415.f);
		pGameObj->Set_State(0);
		m_pObjectMgr->Add_GameObject(L"Layer_GameObject", L"PRIONBERSERKERBOSS", pGameObj);
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

		static_cast<CLakan*>(pObj)->Get_MeshComponent()->Set_VecSkinningMatrix(vecSkinningMatrix);
	}

	for (auto& pObj : m_vecAnimationPrionBerserker)
		static_cast<CPrionBerserker*>(pObj)->Get_MeshComponent()->Play_Animation(fTimeDelta * TPS * m_fPrionBerserkerAnimationSpeed);

	for (auto& pObj : m_vecPrionBerserker)
	{
		_uint uiAnimationObjIdx = static_cast<CPrionBerserker*>(pObj)->Get_AnimationObjectIdx();
		vector<Engine::VECTOR_SKINNING_MATRIX> vecSkinningMatrix = *(static_cast<CPrionBerserker*>(m_vecAnimationPrionBerserker[uiAnimationObjIdx])->Get_MeshComponent()->Get_AniCtrl()->Get_VecSkinningMatrix());

		static_cast<CLakan*>(pObj)->Get_MeshComponent()->Set_VecSkinningMatrix(vecSkinningMatrix);
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
	m_pObjectMgr->Get_GameObject(L"Layer_GameObject", L"PRIONBERSERKERBOSS", 0)->Set_State(PrionBerserkerBoss::RUN);
	m_pObjectMgr->Get_GameObject(L"Layer_GameObject", L"PRIONBERSERKERBOSS", 0)->Set_MoveStop(false);

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
	for (int z = 0; z < LINEUP_Z; ++z)
	{
		for (int x = 0; x < LINEUP_X; ++x)
		{	
			arrLakan[z][x]->Set_State(Lakan::ACTIVE_MOVE_START);		
		}
	}
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
