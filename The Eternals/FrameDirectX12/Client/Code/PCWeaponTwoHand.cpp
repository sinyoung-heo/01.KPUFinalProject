#include "stdafx.h"
#include "PCWeaponTwoHand.h"
#include "ObjectMgr.h"
#include "ColliderBox.h"
#include "CollisionMgr.h"
#include "InstancePoolMgr.h"

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
										   1.5f,
										   _vec3(0.0f, 35.0f, 0.0f));

	if (m_wstrMeshTag == L"Twohand19_A_SM")
		m_chCurWeaponType = Twohand19_A_SM;

	else if (m_wstrMeshTag == L"TwoHand27_SM")
		m_chCurWeaponType = TwoHand27_SM;

	else if (m_wstrMeshTag == L"TwoHand29_SM")
		m_chCurWeaponType = TwoHand29_SM;

	else if (m_wstrMeshTag == L"TwoHand31_SM")
		m_chCurWeaponType = TwoHand31_SM;

	else if (m_wstrMeshTag == L"TwoHand33_B_SM")
		m_chCurWeaponType = TwoHand33_B_SM;

	return S_OK;
}

HRESULT CPCWeaponTwoHand::LateInit_GameObject()
{
	Engine::FAILED_CHECK_RETURN(CPCWeapon::LateInit_GameObject(), E_FAIL);

	// Create Trail
	m_pTrail = CEffectTrail::Create(m_pGraphicDevice, m_pCommandList, L"EffectTrailTexture", 11);		
	Engine::FAILED_CHECK_RETURN(m_pObjectMgr->Add_GameObject(L"Layer_GameObject", L"Trail", m_pTrail), E_FAIL);

	m_pDistortionTrail = CEffectTrail::Create(m_pGraphicDevice, m_pCommandList, L"EffectTrailTexture", 10, Engine::CRenderer::RENDER_DISTORTION);
	Engine::FAILED_CHECK_RETURN(m_pObjectMgr->Add_GameObject(L"Layer_GameObject", L"Trail", m_pDistortionTrail), E_FAIL);

	return S_OK;
}

_int CPCWeaponTwoHand::Update_GameObject(const _float& fTimeDelta)
{
	Engine::FAILED_CHECK_RETURN(Engine::CGameObject::LateInit_GameObject(), E_FAIL);

	if (m_bIsDead)
		return DEAD_OBJ;

	if (m_bIsReturn)
	{
		Return_Instance(CInstancePoolMgr::Get_Instance()->Get_PCWeaponTwoHand(m_chCurWeaponType), m_uiInstanceIdx);

		m_bIsStartInterpolation = false;
		m_fLinearRatio          = 1.0f;
		m_fMinDissolve          = -0.05f;
		m_fMaxDissolve          = 1.0f;
		m_fDissolve             = 1.0f;

		return RETURN_OBJ;
	}

	if (fTimeDelta > TIME_OFFSET)
		return NO_EVENT;

	SetUp_Dissolve(fTimeDelta);

	/*__________________________________________________________________________________________________________
	[ Renderer - Add Render Group ]
	____________________________________________________________________________________________________________*/
	Engine::FAILED_CHECK_RETURN(m_pRenderer->Add_Renderer(Engine::CRenderer::RENDER_NONALPHA, this), -1);


	/*____________________________________________________________________
	TransCom - Update WorldMatrix.
	______________________________________________________________________*/
	Engine::CGameObject::Update_GameObject(fTimeDelta);

	// Upate BoneMatrix
	if (nullptr != m_pHierarchyDesc && nullptr != m_pParentMatrix)
	{
		m_matBoneFinalTransform = (m_pHierarchyDesc->matScale * 
								   m_pHierarchyDesc->matRotate * 
								   m_pHierarchyDesc->matTrans) * 
								   m_pHierarchyDesc->matGlobalTransform;
		_matrix matSkinngingTransform = m_matBoneFinalTransform * (*m_pParentMatrix);
		m_pTransCom->m_matWorld *= matSkinngingTransform;
	}

	// Update Trail
	if (nullptr != m_pTrail && nullptr != m_pDistortionTrail)
	{
		m_pBoundingBoxCom->Update_Component(fTimeDelta);
		
		_vec3 vLook = _vec3((*m_pParentMatrix)._31, 0.0f, (*m_pParentMatrix)._33);
		vLook.Normalize();

		_vec3 vMin = _vec3(m_pBoundingBoxCom->Get_BottomPlaneCenter()) + vLook * 0.75f;
		_vec3 vMax = _vec3(m_pBoundingBoxCom->Get_TopPlaneCenter()) + vLook * 0.75f;

		m_pTrail->SetUp_TrailByCatmullRom(&vMin, &vMax);
		m_pTrail->Get_Transform()->m_vPos = _vec3(0.f, 0.f, 0.0f);

		vMin.y -= 0.1f;
		vMax.y -= 0.1f;
		m_pDistortionTrail->SetUp_TrailByCatmullRom(&vMin, &vMax);
		m_pDistortionTrail->Get_Transform()->m_vPos = _vec3(0.f, 0.f, 1.0f);
	}

	return NO_EVENT;
}

_int CPCWeaponTwoHand::LateUpdate_GameObject(const _float& fTimeDelta)
{
	return CPCWeapon::LateUpdate_GameObject(fTimeDelta);
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

CPCWeaponTwoHand** CPCWeaponTwoHand::Create_InstancePool(ID3D12Device* pGraphicDevice, 
														 ID3D12GraphicsCommandList* pCommandList, 
														 wstring wstrMeshTag, 
														 const _uint& uiInstanceCnt)
{
	CPCWeaponTwoHand** ppInstance = new (CPCWeaponTwoHand* [uiInstanceCnt]);


	for (_uint i = 0; i < uiInstanceCnt; ++i)
	{
		ppInstance[i] = new CPCWeaponTwoHand(pGraphicDevice, pCommandList);
		ppInstance[i]->m_uiInstanceIdx = i;
		ppInstance[i]->Ready_GameObject(wstrMeshTag,				// MeshTag
										_vec3(0.75f),				// Scale
										 _vec3(0.0f, 0.0f, 180.0f),	// Angle
										_vec3(0.0f, 0.0f, 0.0f),	// Pos
										nullptr,					// HierarchyDesc
										nullptr,					// ParentMatrix
										_rgba(0.64f, 0.96f, 0.97f, 1.0f));
	}

	return ppInstance;
}

void CPCWeaponTwoHand::Free()
{
	CPCWeapon::Free();

	if (nullptr != m_pTrail)
		m_pTrail->Set_DeadGameObject();

	if (nullptr != m_pDistortionTrail)
		m_pDistortionTrail->Set_DeadGameObject();
}
