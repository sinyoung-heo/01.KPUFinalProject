#include "stdafx.h"
#include "CinemaMgr.h"
#include "InstancePoolMgr.h"
#include "ObjectMgr.h"
#include "GameObject.h"

#include "Lakan.h"
#include "PrionBerserker.h"
#include "PrionBerserkerBoss.h"

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
	for (int z = 0; z < 5; ++z)
	{
		for (int x = 0; x < 10; ++x)
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
			}
		}
	}

	// PrionBerserker
	for (int z = 0; z < 5; ++z)
	{
		for (int x = 0; x < 10; ++x)
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

    return S_OK;
}

void CCinemaMgr::Scream_PrionBerserkerBoss()
{
	m_pObjectMgr->Get_GameObject(L"Layer_GameObject", L"PRIONBERSERKERBOSS", 0)->Set_State(PrionBerserkerBoss::ANGRY);
}

void CCinemaMgr::Scream_PrionBerserkers()
{
	for (int z = 0; z < 5; ++z)
	{
		for (int x = 0; x < 10; ++x)
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

	for (int z = 0; z < 5; ++z)
	{
		for (int x = 0; x < 10; ++x)
		{
			arrPrionBerserker[z][x]->Set_State(PrionBerserker::RUN);
			arrPrionBerserker[z][x]->Set_MoveStop(false);
		}
	}
}

void CCinemaMgr::Rush_Lakan()
{
	for (int z = 0; z < 5; ++z)
	{
		for (int x = 0; x < 10; ++x)
		{	
			arrLakan[z][x]->Set_State(Lakan::ACTIVE_MOVE_START);		
		}
	}
}

void CCinemaMgr::Free()
{
}
