#include "stdafx.h"
#include "CollisionTick.h"
#include "InstancePoolMgr.h"

CCollisionTick::CCollisionTick()
	: Engine::CGameObject()
	, m_pInstancePoolMgr(CInstancePoolMgr::Get_Instance())
{
}

CCollisionTick::CCollisionTick(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList)
	: Engine::CGameObject(pGraphicDevice, pCommandList)
	, m_pInstancePoolMgr(CInstancePoolMgr::Get_Instance())
{
}

HRESULT CCollisionTick::Ready_GameObject(wstring wstrCollisionTag, 
										 const _vec3& vScale, 
										 const _vec3& vPos,
										 const _uint& uiDamage,
										 const _float& fLifeTime)
{
	Engine::FAILED_CHECK_RETURN(Engine::CGameObject::Ready_GameObject(true, false, false, true), E_FAIL);
	Engine::FAILED_CHECK_RETURN(Add_Component(), E_FAIL);
	m_pTransCom->m_vScale = vScale;
	m_pTransCom->m_vAngle = _vec3(0.0f);
	m_pTransCom->m_vPos	  = vPos;
	m_uiDamage            = uiDamage;
	m_fLifeTime           = fLifeTime;
	m_wstrCollisionTag    = wstrCollisionTag;

	// BoundingSphere
	Engine::CGameObject::SetUp_BoundingSphere(&(m_pTransCom->m_matWorld), m_pTransCom->m_vScale, _vec3(1.0f), _vec3(0.0f));

	return S_OK;
}

HRESULT CCollisionTick::LateInit_GameObject()
{
	return S_OK;
}

_int CCollisionTick::Update_GameObject(const _float& fTimeDelta)
{
	Engine::FAILED_CHECK_RETURN(Engine::CGameObject::LateInit_GameObject(), E_FAIL);

	if (m_bIsDead)
		return DEAD_OBJ;

	if (m_bIsReturn)
	{
		m_pInstancePoolMgr->Return_CollisionTickInstance(m_uiInstanceIdx);
		return RETURN_OBJ;
	}

	m_fTimeDelta += fTimeDelta;
	if (m_fTimeDelta >= m_fLifeTime)
		Set_DeadGameObject();

	m_pBoundingSphereCom->Set_Color(_rgba(0.0f, 1.0f, 0.0f, 1.0f));

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

_int CCollisionTick::LateUpdate_GameObject(const _float& fTimeDelta)
{
	Engine::NULL_CHECK_RETURN(m_pRenderer, -1);
	Process_Collision();

	return NO_EVENT;
}

void CCollisionTick::Process_Collision()
{

}

Engine::CGameObject* CCollisionTick::Create(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList, 
											wstring wstrCollisionTag, 
											const _vec3& vScale, 
											const _vec3& vPos, 
											const _uint& uiDamage, 
											const _float& fLifeTime)
{
	CCollisionTick* pInstance = new CCollisionTick(pGraphicDevice, pCommandList);

	if (FAILED(pInstance->Ready_GameObject(wstrCollisionTag, vScale, vPos, uiDamage, fLifeTime)))
		Engine::Safe_Release(pInstance);

	return pInstance;
}

CCollisionTick** CCollisionTick::Create_Instance(ID3D12Device* pGraphicDevice,
													  ID3D12GraphicsCommandList* pCommandList, 
													  const _uint& uiInstanceCnt)
{
	CCollisionTick** ppInstance = new CCollisionTick*[uiInstanceCnt];

	for (_uint i = 0; i < uiInstanceCnt; ++i)
	{
		ppInstance[i]->m_uiInstanceIdx  = i;
		ppInstance[i]->m_pGraphicDevice = pGraphicDevice;
		ppInstance[i]->m_pCommandList   = pCommandList;
		ppInstance[i]->Ready_GameObject(L"", _vec3(0.0f), _vec3(0.0f), 0, 0.0f);
	}

	return ppInstance;
}

void CCollisionTick::Free()
{
	Engine::CGameObject::Free();
}
