#include "stdafx.h"
#include "TestColMonster.h"
#include "DirectInput.h"
#include "DynamicCamera.h"
#include "ObjectMgr.h"
#include <random>

CTestColMonster::CTestColMonster(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList)
	: Engine::CGameObject(pGraphicDevice, pCommandList)
	, m_pShaderColorInstancing(Engine::CShaderColorInstancing::Get_Instance())
{
}

CTestColMonster::CTestColMonster(const CTestColMonster& rhs)
	: Engine::CGameObject(rhs)
{
}

HRESULT CTestColMonster::Ready_GameObject(const _vec3& vScale, const _vec3& vAngle, const _vec3& vPos)
{
	Engine::FAILED_CHECK_RETURN(Engine::CGameObject::Ready_GameObject(true, true, true, true), E_FAIL);
	Engine::FAILED_CHECK_RETURN(Add_Component(), E_FAIL);
	m_pTransCom->m_vScale	= vScale;
	m_pTransCom->m_vAngle	= vAngle;
	m_pTransCom->m_vPos		= vPos;
	m_wstrCollisionTag      = L"TestCollision";

	m_pInfoCom->m_vecArivePos = m_pTransCom->m_vPos;

	m_bIsCollision = true;
	// BoundingBox.
	Engine::CGameObject::SetUp_BoundingBox(&(m_pTransCom->m_matWorld),
										   m_pTransCom->m_vScale,
										   _vec3(0.0f),
										   _vec3(0.5f, 0.5f, 0.5f),
										   _vec3(-0.5f, -0.5f, -0.5f));
	// BoundingSphere
	Engine::CGameObject::SetUp_BoundingSphere(&(m_pTransCom->m_matWorld),
											  m_pTransCom->m_vScale,
											  _vec3(1.0f),
											  _vec3(0.0f));

	m_pInfoCom->m_fSpeed = 5.f;
	m_bIsMoveStop = true;

	return S_OK;
}

HRESULT CTestColMonster::LateInit_GameObject()
{
	return S_OK;
}

_int CTestColMonster::Update_GameObject(const _float& fTimeDelta)
{
	Engine::FAILED_CHECK_RETURN(Engine::CGameObject::LateInit_GameObject(), E_FAIL);

	if (m_bIsDead)
		return DEAD_OBJ;

	m_pBoundingSphereCom->Set_Color(_rgba(0.0f, 1.0f, 0.0f, 1.0f));
	m_pBoundingBoxCom->Set_Color(_rgba(0.0f, 1.0f, 0.0f, 1.0f));

	/*__________________________________________________________________________________________________________
	[ TransCom - Update WorldMatrix ]
	____________________________________________________________________________________________________________*/
	Engine::CGameObject::Update_GameObject(fTimeDelta);

	/*__________________________________________________________________________________________________________
	[ Collision - Add Collision List ]
	____________________________________________________________________________________________________________*/
	m_pCollisonMgr->Add_CollisionCheckList(this);

	return NO_EVENT;
}

_int CTestColMonster::LateUpdate_GameObject(const _float& fTimeDelta)
{
	Engine::NULL_CHECK_RETURN(m_pRenderer, -1);

	Process_Collision();
	Active_Monster(fTimeDelta);

	return NO_EVENT;
}

void CTestColMonster::Render_GameObject(const _float& fTimeDelta)
{
}

void CTestColMonster::Process_Collision()
{
	for (auto& pDst : m_lstCollisionDst)
	{
		if (L"TestCollision" == pDst->Get_CollisionTag())
		{
			m_pBoundingSphereCom->Set_Color(_rgba(1.0f, 0.0f, 0.0f, 1.0f));
			m_pBoundingBoxCom->Set_Color(_rgba(1.0f, 0.0f, 0.0f, 1.0f));
			pDst->Get_BoundingSphere()->Set_Color(_rgba(1.0f, 0.0f, 0.0f, 1.0f));
			pDst->Get_BoundingBox()->Set_Color(_rgba(1.0f, 0.0f, 0.0f, 1.0f));
		}
	}
}

HRESULT CTestColMonster::Add_Component()
{
	return S_OK;
}

void CTestColMonster::Active_Monster(const _float& fTimeDelta)
{
	m_pTransCom->m_vDir = m_pTransCom->Get_LookVector();
	m_pTransCom->m_vDir.Normalize();

	/* Monster MOVE */
	if (!m_bIsMoveStop)
	{
		// NaviMesh ÀÌµ¿.		
		if (!CServerMath::Get_Instance()->Is_Arrive_Point(m_pTransCom->m_vPos, m_pInfoCom->m_vecArivePos))
		{
			m_pTransCom->m_vPos += m_pTransCom->m_vDir * m_pInfoCom->m_fSpeed * fTimeDelta;
		}
		else
		{
			m_bIsMoveStop = true;
		}
	}
}

Engine::CGameObject* CTestColMonster::Create(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList, 
												 const _vec3& vScale, 
												 const _vec3& vAngle, 
												 const _vec3& vPos)
{
	CTestColMonster* pInstance = new CTestColMonster(pGraphicDevice, pCommandList);

	if (FAILED(pInstance->Ready_GameObject(vScale, vAngle, vPos)))
		Engine::Safe_Release(pInstance);

	return pInstance;
}

void CTestColMonster::Free()
{
	Engine::CGameObject::Free();
}
