#include "stdafx.h"
#include "TestCollisonObject.h"

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

	/*__________________________________________________________________________________________________________
	[ TransCom - Update WorldMatrix ]
	____________________________________________________________________________________________________________*/
	Engine::CGameObject::Update_GameObject(fTimeDelta);


	return NO_EVENT;
}

_int CTestCollisonObject::LateUpdate_GameObject(const _float& fTimeDelta)
{
	Engine::NULL_CHECK_RETURN(m_pRenderer, -1);

	return NO_EVENT;
}

void CTestCollisonObject::Render_GameObject(const _float& fTimeDelta)
{
}

HRESULT CTestCollisonObject::Add_Component()
{
	return S_OK;
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
