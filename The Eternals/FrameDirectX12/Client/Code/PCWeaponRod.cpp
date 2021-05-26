#include "stdafx.h"
#include "PCWeaponRod.h"
#include "ObjectMgr.h"
#include "ColliderBox.h"
#include "CollisionMgr.h"
#include "InstancePoolMgr.h"

CPCWeaponRod::CPCWeaponRod(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList)
	: CPCWeapon(pGraphicDevice, pCommandList)
{
}

HRESULT CPCWeaponRod::Ready_GameObject(wstring wstrMeshTag, 
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
										   _vec3(0.0f, 0.0f, 0.0f));

	if (m_wstrMeshTag == L"Event_Wit_Rod_01")
		m_chCurWeaponType = Event_Wit_Rod_01;

	else if (m_wstrMeshTag == L"Rod19_A")
		m_chCurWeaponType = Rod19_A;

	else if (m_wstrMeshTag == L"Rod28_B")
		m_chCurWeaponType = Rod28_B;

	else if (m_wstrMeshTag == L"Rod31")
		m_chCurWeaponType = Rod31;

	else if (m_wstrMeshTag == L"Rod33_B")
		m_chCurWeaponType = Rod33_B;

	return S_OK;
}

HRESULT CPCWeaponRod::LateInit_GameObject()
{
	Engine::FAILED_CHECK_RETURN(CPCWeapon::LateInit_GameObject(), E_FAIL);

	return S_OK;
}

_int CPCWeaponRod::Update_GameObject(const _float& fTimeDelta)
{
	Engine::FAILED_CHECK_RETURN(Engine::CGameObject::LateInit_GameObject(), E_FAIL);

	if (m_bIsDead)
		return DEAD_OBJ;

	if (m_bIsReturn)
	{
		Return_Instance(CInstancePoolMgr::Get_Instance()->Get_PCWeaponRod(m_chCurWeaponType), m_uiInstanceIdx);

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
		m_pBoundingBoxCom->Update_Component(fTimeDelta);
	}

	return NO_EVENT;
}

_int CPCWeaponRod::LateUpdate_GameObject(const _float& fTimeDelta)
{
	return CPCWeapon::LateUpdate_GameObject(fTimeDelta);
}

void CPCWeaponRod::Render_GameObject(const _float& fTimeDelta, 
									 ID3D12GraphicsCommandList* pCommandList, 
									 const _int& iContextIdx)
{
	CPCWeapon::Render_GameObject(fTimeDelta, pCommandList, iContextIdx);
}

void CPCWeaponRod::Render_ShadowDepth(const _float& fTimeDelta, 
									  ID3D12GraphicsCommandList* pCommandList, 
									  const _int& iContextIdx)
{
	CPCWeapon::Render_ShadowDepth(fTimeDelta, pCommandList, iContextIdx);
}

CPCWeaponRod* CPCWeaponRod::Create(ID3D12Device* pGraphicDevice, 
								   ID3D12GraphicsCommandList* pCommandList, 
								   wstring wstrMeshTag,
								   const _vec3& vScale, 
								   const _vec3& vAngle, 
								   const _vec3& vPos,
								   Engine::HIERARCHY_DESC* pHierarchyDesc,
								   _matrix* pParentMatrix, 
								   const _rgba& vEmissiveColor)
{
	CPCWeaponRod* pInstance = new CPCWeaponRod(pGraphicDevice, pCommandList);

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

CPCWeaponRod** CPCWeaponRod::Create_InstancePool(ID3D12Device* pGraphicDevice,
												 ID3D12GraphicsCommandList* pCommandList, 
												 wstring wstrMeshTag, 
												 const _uint& uiInstanceCnt)
{
	CPCWeaponRod** ppInstance = new (CPCWeaponRod * [uiInstanceCnt]);


	for (_uint i = 0; i < uiInstanceCnt; ++i)
	{
		ppInstance[i] = new CPCWeaponRod(pGraphicDevice, pCommandList);
		ppInstance[i]->m_uiInstanceIdx = i;
		ppInstance[i]->Ready_GameObject(wstrMeshTag,				// MeshTag
										_vec3(1.35f),				// Scale
										 _vec3(0.0f, 0.0f, 180.0f),	// Angle
										_vec3(0.0f, 0.0f, 0.0f),	// Pos
										nullptr,					// HierarchyDesc
										nullptr,					// ParentMatrix
										_rgba(0.64f, 0.96f, 0.97f, 1.0f));
	}

	return ppInstance;
}

void CPCWeaponRod::Free()
{
	CPCWeapon::Free();
}
