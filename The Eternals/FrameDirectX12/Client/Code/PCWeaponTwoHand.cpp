#include "stdafx.h"
#include "PCWeaponTwoHand.h"
#include "ObjectMgr.h"
#include "ColliderBox.h"
#include "CollisionMgr.h"

CPCWeaponTwoHand::CPCWeaponTwoHand(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList)
	: CPCWeapon(pGraphicDevice, pCommandList)
{
}

HRESULT CPCWeaponTwoHand::Ready_GameObject(wstring wstrMeshTag, 
										   const _vec3& vScale, 
										   const _vec3& vAngle, 
										   const _vec3& vPos, 
										   Engine::HIERARCHY_DESC* pHierarchyDesc,
										   _matrix* pParentMatrix,
										   const _rgba& vEmissiveColor)
{
	Engine::FAILED_CHECK_RETURN(CPCWeapon::Ready_GameObject(wstrMeshTag,
															vScale, 
															vAngle, 
															vPos, 
															pHierarchyDesc, 
															pParentMatrix,
															vEmissiveColor), E_FAIL);

	Engine::CGameObject::SetUp_BoundingBox(&(m_pTransCom->m_matWorld),
										   m_pTransCom->m_vScale,
										   m_pMeshCom->Get_CenterPos(),
										   m_pMeshCom->Get_MinVector(),
										   m_pMeshCom->Get_MaxVector(),
										   1.0f,
										   _vec3(0.0f, 25.0f, 0.0f));

	//m_wstrCollisionTag = L"PCWeapon";

	return S_OK;
}

HRESULT CPCWeaponTwoHand::LateInit_GameObject()
{
	Engine::FAILED_CHECK_RETURN(CPCWeapon::LateInit_GameObject(), E_FAIL);

	// Create Trail
	m_pTrail = CEffectTrail::Create(m_pGraphicDevice, m_pCommandList, L"EffectTrailTexture", 0);		
	Engine::FAILED_CHECK_RETURN(m_pObjectMgr->Add_GameObject(L"Layer_GameObject", L"TestTrail", m_pTrail), E_FAIL);

	return S_OK;
}

_int CPCWeaponTwoHand::Update_GameObject(const _float& fTimeDelta)
{
	Engine::FAILED_CHECK_RETURN(Engine::CGameObject::LateInit_GameObject(), E_FAIL);

	if (m_bIsDead)
		return DEAD_OBJ;

	if (fTimeDelta > TIME_OFFSET)
		return NO_EVENT;

	//if (nullptr != m_pBoundingSphereCom)
	//	m_pBoundingSphereCom->Set_Color(_rgba(0.0f, 1.0f, 0.0f, 1.0f));
	//if (nullptr != m_pBoundingBoxCom)
	//	m_pBoundingBoxCom->Set_Color(_rgba(0.0f, 1.0f, 0.0f, 1.0f));

	SetUp_Dissolve(fTimeDelta);

	/*__________________________________________________________________________________________________________
	[ Renderer - Add Render Group ]
	____________________________________________________________________________________________________________*/
	Engine::FAILED_CHECK_RETURN(m_pRenderer->Add_Renderer(Engine::CRenderer::RENDER_NONALPHA, this), -1);

	//if (m_bIsCollision)
	//{
	//	/*__________________________________________________________________________________________________________
	//	[ Collision - Add Collision List ]
	//	____________________________________________________________________________________________________________*/
	//	m_pCollisonMgr->Add_CollisionCheckList(this);
	//}

	/*____________________________________________________________________
	TransCom - Update WorldMatrix.
	______________________________________________________________________*/
	Engine::CGameObject::Update_GameObject(fTimeDelta);

	// Upate BoneMatrix
	m_matBoneFinalTransform = (m_pHierarchyDesc->matScale * 
							   m_pHierarchyDesc->matRotate * 
							   m_pHierarchyDesc->matTrans) * 
							   m_pHierarchyDesc->matGlobalTransform;
	_matrix matSkinngingTransform = m_matBoneFinalTransform * (*m_pParentMatrix);
	m_pTransCom->m_matWorld *= matSkinngingTransform;

	// Update Trail
	if (nullptr != m_pTrail)
	{
		m_pBoundingBoxCom->Update_Component(fTimeDelta);

		_vec3 vMin = _vec3(m_pBoundingBoxCom->Get_BottomPlaneCenter());
		_vec3 vMax = _vec3(m_pBoundingBoxCom->Get_TopPlaneCenter());

		m_pTrail->SetUp_TrailByCatmullRom(&vMin, &vMax);
		m_pTrail->Get_Transform()->m_vPos = _vec3(0.f, 0.f, 0.0f);
	}

	return NO_EVENT;
}

_int CPCWeaponTwoHand::LateUpdate_GameObject(const _float& fTimeDelta)
{
	Process_Collision();

	return CPCWeapon::LateUpdate_GameObject(fTimeDelta);;
}

void CPCWeaponTwoHand::Process_Collision()
{
	//if (!m_bIsCollision)
	//	return;

	//for (auto& pDst : m_lstCollisionDst)
	//{
	//	if (L"Monster" == pDst->Get_CollisionTag())
	//	{
	//		if (nullptr != m_pBoundingSphereCom)
	//			m_pBoundingSphereCom->Set_Color(_rgba(1.0f, 0.0f, 0.0f, 1.0f));
	//		if (nullptr != m_pBoundingBoxCom)
	//			m_pBoundingBoxCom->Set_Color(_rgba(1.0f, 0.0f, 0.0f, 1.0f));
	//	}
	//}
}

void CPCWeaponTwoHand::Render_GameObject(const _float& fTimeDelta, 
										 ID3D12GraphicsCommandList* pCommandList, 
										 const _int& iContextIdx)
{
	CPCWeapon::Render_GameObject(fTimeDelta, pCommandList, iContextIdx);
}

void CPCWeaponTwoHand::Render_ShadowDepth(const _float& fTimeDelta, 
										  ID3D12GraphicsCommandList* pCommandList, 
										  const _int& iContextIdx)
{
	CPCWeapon::Render_ShadowDepth(fTimeDelta, pCommandList, iContextIdx);

}

CPCWeaponTwoHand* CPCWeaponTwoHand::Create(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList,
										   wstring wstrMeshTag, 
										   const _vec3& vScale, 
										   const _vec3& vAngle, 
										   const _vec3& vPos, 
										   Engine::HIERARCHY_DESC* pHierarchyDesc,
										   _matrix* pParentMatrix,
										   const _rgba& vEmissiveColor)
{
	CPCWeaponTwoHand* pInstance = new CPCWeaponTwoHand(pGraphicDevice, pCommandList);

	if (FAILED(pInstance->Ready_GameObject(wstrMeshTag,
										   vScale,
										   vAngle,
										   vPos,
										   pHierarchyDesc,
										   pParentMatrix,
										   vEmissiveColor)))
		Engine::Safe_Release(pInstance);

	return pInstance;
}

void CPCWeaponTwoHand::Free()
{
	m_pTrail->Set_DeadGameObject();
	CPCWeapon::Free();
}
