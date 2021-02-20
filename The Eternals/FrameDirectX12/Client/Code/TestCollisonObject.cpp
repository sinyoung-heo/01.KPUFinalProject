#include "stdafx.h"
#include "TestCollisonObject.h"
#include <random>

CTestCollisonObject::CTestCollisonObject(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList)
	: Engine::CGameObject(pGraphicDevice, pCommandList)
	, m_pShaderColorInstancing(Engine::CShaderColorInstancing::Get_Instance())
{
}

CTestCollisonObject::CTestCollisonObject(const CTestCollisonObject& rhs)
	: Engine::CGameObject(rhs)
{
}

HRESULT CTestCollisonObject::Ready_GameObject(const _vec3& vScale, const _vec3& vAngle, const _vec3& vPos)
{
	Engine::FAILED_CHECK_RETURN(Engine::CGameObject::Ready_GameObject(true, true, true, true), E_FAIL);
	Engine::FAILED_CHECK_RETURN(Add_Component(), E_FAIL);
	m_pTransCom->m_vScale	= vScale;
	m_pTransCom->m_vAngle	= vAngle;
	m_pTransCom->m_vPos		= vPos;
	m_wstrCollisionTag      = L"TestCollision";

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

	uniform_int_distribution<_int>	uid_dir		{ DIR_X, DIR_Z };
	uniform_int_distribution<_int>	uid_move	{ MOVE_MINUS, MOVE_PLUS };
	uniform_int_distribution<_int>	uid_speed	{ 10, 50 };
	// random_device			rn;
	default_random_engine	dre{ /*rn()*/ };

	m_eDir = DIRECTION(uid_dir(dre));
	if (MOVE_MINUS == MOVE(uid_move(dre)))
		m_fMove = -1.0f;
	else
		m_fMove = 1.0f;

	m_pInfoCom->m_fSpeed = _float(uid_speed(dre));

	return S_OK;
}

HRESULT CTestCollisonObject::LateInit_GameObject()
{
	return S_OK;
}

_int CTestCollisonObject::Update_GameObject(const _float& fTimeDelta)
{
	Engine::FAILED_CHECK_RETURN(Engine::CGameObject::LateInit_GameObject(), E_FAIL);

	if (m_bIsDead)
		return DEAD_OBJ;

	m_pBoundingSphereCom->Set_Color(_rgba(0.0f, 1.0f, 0.0f, 1.0f));
	m_pBoundingBoxCom->Set_Color(_rgba(0.0f, 1.0f, 0.0f, 1.0f));


	/*__________________________________________________________________________________________________________
	[ Move Collision Object ]
	____________________________________________________________________________________________________________*/
	Move_Direction(fTimeDelta);

	/*__________________________________________________________________________________________________________
	[ TransCom - Update WorldMatrix ]
	____________________________________________________________________________________________________________*/
	Engine::CGameObject::Update_GameObject(fTimeDelta);

	m_pCollisonMgr->Add_CollisionCheckList(m_wstrCollisionTag, this);

	return NO_EVENT;
}

_int CTestCollisonObject::LateUpdate_GameObject(const _float& fTimeDelta)
{
	Engine::NULL_CHECK_RETURN(m_pRenderer, -1);
	Process_Collision();

	return NO_EVENT;
}

void CTestCollisonObject::Render_GameObject(const _float& fTimeDelta)
{
}

void CTestCollisonObject::Process_Collision()
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

HRESULT CTestCollisonObject::Add_Component()
{
	return S_OK;
}

void CTestCollisonObject::Move_Direction(const _float& fTimeDelta)
{
	if (DIR_X == m_eDir)
	{
		m_pTransCom->m_vPos.x += m_pInfoCom->m_fSpeed * m_fMove * fTimeDelta;

		if (m_pTransCom->m_vPos.x > OFFSET_MAX)
		{
			m_fMove *= -1.0f;
			m_pTransCom->m_vPos.x = OFFSET_MAX;
		}
		else if (m_pTransCom->m_vPos.x < OFFSET_MIN)
		{
			m_fMove *= -1.0f;
			m_pTransCom->m_vPos.x = OFFSET_MIN;
		}
	}
	else if (DIR_Y == m_eDir)
	{
		m_pTransCom->m_vPos.y += m_pInfoCom->m_fSpeed * m_fMove * fTimeDelta;

		if (m_pTransCom->m_vPos.y > OFFSET_MAX)
		{
			m_fMove *= -1.0f;
			m_pTransCom->m_vPos.y = OFFSET_MAX;
		}
		else if (m_pTransCom->m_vPos.y < OFFSET_MIN)
		{
			m_fMove *= -1.0f;
			m_pTransCom->m_vPos.y = OFFSET_MIN;
		}
	}
	else if (DIR_Z == m_eDir)
	{
		m_pTransCom->m_vPos.z += m_pInfoCom->m_fSpeed * m_fMove * fTimeDelta;

		if (m_pTransCom->m_vPos.z > OFFSET_MAX)
		{
			m_fMove *= -1.0f;
			m_pTransCom->m_vPos.z = OFFSET_MAX;
		}
		else if (m_pTransCom->m_vPos.z < OFFSET_MIN)
		{
			m_fMove *= -1.0f;
			m_pTransCom->m_vPos.z = OFFSET_MIN;
		}
	}
}

Engine::CGameObject* CTestCollisonObject::Create(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList, 
												 const _vec3& vScale, 
												 const _vec3& vAngle, 
												 const _vec3& vPos)
{
	CTestCollisonObject* pInstance = new CTestCollisonObject(pGraphicDevice, pCommandList);

	if (FAILED(pInstance->Ready_GameObject(vScale, vAngle, vPos)))
		Engine::Safe_Release(pInstance);

	return pInstance;
}

void CTestCollisonObject::Free()
{
	Engine::CGameObject::Free();
}
