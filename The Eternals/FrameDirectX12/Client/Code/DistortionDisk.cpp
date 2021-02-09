#include "stdafx.h"
#include "DistortionDisk.h"

#include "GraphicDevice.h"
#include "DirectInput.h"
#include "ObjectMgr.h"
#include "LightMgr.h"
#include "DynamicCamera.h"
#include "RenderTarget.h"
#include "TimeMgr.h"

CDistortionDisk::CDistortionDisk(ID3D12Device * pGraphicDevice, ID3D12GraphicsCommandList * pCommandList)
	: Engine::CGameObject(pGraphicDevice, pCommandList)
{
}

CDistortionDisk::CDistortionDisk(const CDistortionDisk & rhs)
	: Engine::CGameObject(rhs)
	, m_wstrMeshTag(rhs.m_wstrMeshTag)
	, m_pDynamicCamera(rhs.m_pDynamicCamera)
{
}


HRESULT CDistortionDisk::Ready_GameObject(wstring wstrMeshTag, 
											const _vec3 & vScale, 
											const _vec3 & vAngle, 
											const _vec3 & vPos,
											const _bool& bIsRenderShadow,
											const _bool& bIsCollision,
											const _vec3& vBoundingSphereScale,
											const _vec3& vBoundingSpherePos)
{
	Engine::FAILED_CHECK_RETURN(Engine::CGameObject::Ready_GameObject(true, false, true, true), E_FAIL);
	Engine::FAILED_CHECK_RETURN(Add_Component(wstrMeshTag), E_FAIL);
	m_wstrMeshTag			= wstrMeshTag;
	m_pTransCom->m_vScale	= vScale;
	m_pTransCom->m_vAngle	= vAngle;
	m_pTransCom->m_vPos		= vPos;
	m_bIsCollision			= bIsCollision;

	m_pShaderCom->Set_PipelineStatePass(2);	
	return S_OK;
}

HRESULT CDistortionDisk::LateInit_GameObject()
{
	/*__________________________________________________________________________________________________________
	[ Get GameObject - DynamicCamera ]
	____________________________________________________________________________________________________________*/
	m_pDynamicCamera = static_cast<CDynamicCamera*>(m_pObjectMgr->Get_GameObject(L"Layer_Camera", L"DynamicCamera"));
	Engine::NULL_CHECK_RETURN(m_pDynamicCamera, E_FAIL);
	m_pDynamicCamera->AddRef();

	// SetUp Shader ConstantBuffer
	m_pShaderCom->SetUp_ShaderConstantBuffer((_uint)(m_pMeshCom->Get_DiffTexture().size()));
	
	return S_OK;
}

_int CDistortionDisk::Update_GameObject(const _float & fTimeDelta)
{
	Engine::FAILED_CHECK_RETURN(Engine::CGameObject::LateInit_GameObject(), E_FAIL);

	if (m_bIsDead)
		return DEAD_OBJ;

	m_pTransCom->m_vScale.x += fTimeDelta;
	m_pTransCom->m_vScale.z += fTimeDelta;
	/*____________________________________________________________________
	TransCom - Update WorldMatrix.
	______________________________________________________________________*/
	Engine::CGameObject::Update_GameObject(fTimeDelta);



	return NO_EVENT;
}

_int CDistortionDisk::LateUpdate_GameObject(const _float & fTimeDelta)
{
	Engine::NULL_CHECK_RETURN(m_pRenderer, -1);

	/*__________________________________________________________________________________________________________
	[ Renderer - Add Render Group ]
	____________________________________________________________________________________________________________*/
	Engine::FAILED_CHECK_RETURN(m_pRenderer->Add_Renderer(Engine::CRenderer::RENDER_DISTORTION, this), -1);

	return NO_EVENT;
}

void CDistortionDisk::Render_GameObject(const _float & fTimeDelta)
{
	Set_ConstantTable();
	m_pMeshCom->Render_StaticMesh(m_pShaderCom);
}

void CDistortionDisk::Render_ShadowDepth(const _float & fTimeDelta)
{
}

void CDistortionDisk::Render_GameObject(const _float& fTimeDelta, 
										  ID3D12GraphicsCommandList * pCommandList,
										  const _int& iContextIdx)
{
	Set_ConstantTable();
	m_pMeshCom->Render_StaticMesh(pCommandList, iContextIdx, m_pShaderCom);
}

void CDistortionDisk::Render_ShadowDepth(const _float& fTimeDelta, 
										   ID3D12GraphicsCommandList * pCommandList,
										   const _int& iContextIdx)
{
}

HRESULT CDistortionDisk::Add_Component(wstring wstrMeshTag)
{
	Engine::NULL_CHECK_RETURN(m_pComponentMgr, E_FAIL);

	// Mesh
	m_pMeshCom = static_cast<Engine::CMesh*>(m_pComponentMgr->Clone_Component(wstrMeshTag.c_str(), Engine::COMPONENTID::ID_STATIC));
	Engine::NULL_CHECK_RETURN(m_pMeshCom, E_FAIL);
	m_pMeshCom->AddRef();
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Mesh", m_pMeshCom);

	// Shader
	m_pShaderCom = static_cast<Engine::CShaderMesh*>(m_pComponentMgr->Clone_Component(L"ShaderMesh", Engine::COMPONENTID::ID_STATIC));
	Engine::NULL_CHECK_RETURN(m_pShaderCom, E_FAIL);
	m_pShaderCom->AddRef();
	Engine::FAILED_CHECK_RETURN(m_pShaderCom->Set_PipelineStatePass(1), E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Shader", m_pShaderCom);

	return S_OK;
}

void CDistortionDisk::Set_ConstantTable()
{
	/*__________________________________________________________________________________________________________
	[ Set ConstantBuffer Data ]
	____________________________________________________________________________________________________________*/
	Engine::SHADOW_DESC tShadowDesc = CShadowLightMgr::Get_Instance()->Get_ShadowDesc();

	Engine::CB_SHADER_MESH tCB_ShaderMesh;
	ZeroMemory(&tCB_ShaderMesh, sizeof(Engine::CB_SHADER_MESH));
	tCB_ShaderMesh.matWorld = Engine::CShader::Compute_MatrixTranspose(m_pTransCom->m_matWorld);

	m_fDeltaTime += (Engine::CTimerMgr::Get_Instance()->Get_TimeDelta(L"Timer_TimeDelta")) * 0.05f;
	tCB_ShaderMesh.fDeltaTime = m_fDeltaTime;
	if (m_fDeltaTime > 1.f)
		m_fDeltaTime = 0.f;

	m_pShaderCom->Get_UploadBuffer_ShaderMesh()->CopyData(0, tCB_ShaderMesh);
}

void CDistortionDisk::Set_ConstantTableShadowDepth()
{
}

Engine::CGameObject* CDistortionDisk::Create(ID3D12Device * pGraphicDevice, ID3D12GraphicsCommandList * pCommandList,
											   wstring wstrMeshTag, 
											   const _vec3 & vScale, 
											   const _vec3 & vAngle, 
											   const _vec3 & vPos,
											   const _bool& bIsRenderShadow,
											   const _bool& bIsCollision,
											   const _vec3& vBoundingSphereScale,
											   const _vec3& vBoundingSpherePos)
{
	CDistortionDisk* pInstance = new CDistortionDisk(pGraphicDevice, pCommandList);

	if (FAILED(pInstance->Ready_GameObject(wstrMeshTag, 
										   vScale, 
										   vAngle,
										   vPos, 
										   bIsRenderShadow,
										   bIsCollision, 
										   vBoundingSphereScale,
										   vBoundingSpherePos)))
		Engine::Safe_Release(pInstance);

	return pInstance;
}

void CDistortionDisk::Free()
{
	Engine::CGameObject::Free();
	Engine::Safe_Release(m_pDynamicCamera);
	Engine::Safe_Release(m_pMeshCom);
	Engine::Safe_Release(m_pShaderCom);

}
