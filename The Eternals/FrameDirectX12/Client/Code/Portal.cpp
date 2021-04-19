#include "stdafx.h"
#include "Portal.h"

CPortal::CPortal(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList)
	: Engine::CGameObject(pGraphicDevice, pCommandList)
{
}


HRESULT CPortal::Ready_GameObject(wstring wstrCollisionTag, const _vec3& vScale, const _vec3& vPos)
{
	Engine::FAILED_CHECK_RETURN(Engine::CGameObject::Ready_GameObject(true, false, false, true), E_FAIL);
	m_pTransCom->m_vScale = vScale;
	m_pTransCom->m_vAngle = _vec3(0.0f);
	m_pTransCom->m_vPos	  = vPos;
	m_wstrCollisionTag    = wstrCollisionTag;

	// BoundingSphere
	Engine::CGameObject::SetUp_BoundingSphere(&(m_pTransCom->m_matWorld), m_pTransCom->m_vScale, _vec3(1.0f), _vec3(0.0f));
	m_lstCollider.push_back(m_pBoundingSphereCom);

	return S_OK;
}

HRESULT CPortal::LateInit_GameObject()
{
	return S_OK;
}

_int CPortal::Update_GameObject(const _float& fTimeDelta)
{
	Engine::FAILED_CHECK_RETURN(Engine::CGameObject::LateInit_GameObject(), E_FAIL);

	if (m_bIsDead)
		return DEAD_OBJ;

	/*__________________________________________________________________________________________________________
	[ Collision - Add Collision List ]
	____________________________________________________________________________________________________________*/
	m_pCollisonMgr->Add_CollisionCheckList(this);

	/*__________________________________________________________________________________________________________
	[ TransCom - Update WorldMatrix ]
	____________________________________________________________________________________________________________*/
	Engine::CGameObject::Update_GameObject(fTimeDelta);

	return NO_EVENT;
}

_int CPortal::LateUpdate_GameObject(const _float& fTimeDelta)
{
	Engine::NULL_CHECK_RETURN(m_pRenderer, -1);

	m_pBoundingSphereCom->Set_Color(_rgba(0.0f, 0.0f, 1.0f, 1.0f));
	Process_Collision();

	return NO_EVENT;
}

void CPortal::Process_Collision()
{

}

Engine::CGameObject* CPortal::Create(ID3D12Device* pGraphicDevice, 
									 ID3D12GraphicsCommandList* pCommandList, 
									 wstring wstrCollisionTag, 
									 const _vec3& vScale, 
									 const _vec3& vPos)
{
	CPortal* pInstance = new CPortal(pGraphicDevice, pCommandList);

	if (FAILED(pInstance->Ready_GameObject(wstrCollisionTag, vScale, vPos)))
		Engine::Safe_Release(pInstance);

	return pInstance;
}

void CPortal::Free()
{
	Engine::CGameObject::Free();

}
