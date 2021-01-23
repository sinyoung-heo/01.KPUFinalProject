#include "stdafx.h"
#include "StaticMeshObject.h"

#include "GraphicDevice.h"
#include "DirectInput.h"
#include "ObjectMgr.h"
#include "LightMgr.h"
#include "DynamicCamera.h"
#include "RenderTarget.h"


CStaticMeshObject::CStaticMeshObject(ID3D12Device * pGraphicDevice, ID3D12GraphicsCommandList * pCommandList)
	: Engine::CGameObject(pGraphicDevice, pCommandList)
{
}

CStaticMeshObject::CStaticMeshObject(const CStaticMeshObject & rhs)
	: Engine::CGameObject(rhs)
	, m_wstrMeshTag(rhs.m_wstrMeshTag)
	, m_pDynamicCamera(rhs.m_pDynamicCamera)
{
}


HRESULT CStaticMeshObject::Ready_GameObject(wstring wstrMeshTag, 
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

	// BoundingBox.
	Engine::CGameObject::SetUp_BoundingBox(&(m_pTransCom->m_matWorld),
										   m_pTransCom->m_vScale,
										   m_pMeshCom->Get_CenterPos(),
										   m_pMeshCom->Get_MinVector(),
										   m_pMeshCom->Get_MaxVector());
	// BoundingSphere.
	Engine::CGameObject::SetUp_BoundingSphere(&(m_pTransCom->m_matWorld),
											  m_pTransCom->m_vScale,
											  vBoundingSphereScale,
											  vBoundingSpherePos);

	// 그림자를 그리지 않으면 0번 패스.
	m_bIsRenderShadow = bIsRenderShadow; 
	if (!m_bIsRenderShadow)
		m_pShaderCom->Set_PipelineStatePass(0);
	else
		m_pShaderCom->Set_PipelineStatePass(1);


	return S_OK;
}

HRESULT CStaticMeshObject::LateInit_GameObject()
{
	/*__________________________________________________________________________________________________________
	[ Get GameObject - DynamicCamera ]
	____________________________________________________________________________________________________________*/
	m_pDynamicCamera = static_cast<CDynamicCamera*>(m_pObjectMgr->Get_GameObject(L"Layer_Camera", L"DynamicCamera"));
	Engine::NULL_CHECK_RETURN(m_pDynamicCamera, E_FAIL);
	m_pDynamicCamera->AddRef();

	// SetUp Shader ConstantBuffer
	m_pShaderCom->SetUp_ShaderConstantBuffer((_uint)(m_pMeshCom->Get_DiffTexture().size()));
	m_pShadowCom->SetUp_ShaderConstantBuffer((_uint)(m_pMeshCom->Get_DiffTexture().size()));

	return S_OK;
}

_int CStaticMeshObject::Update_GameObject(const _float & fTimeDelta)
{
	Engine::FAILED_CHECK_RETURN(Engine::CGameObject::LateInit_GameObject(), E_FAIL);

	if (m_bIsDead)
		return DEAD_OBJ;

	/*____________________________________________________________________
	TransCom - Update WorldMatrix.
	______________________________________________________________________*/
	Engine::CGameObject::Update_GameObject(fTimeDelta);

	return NO_EVENT;
}

_int CStaticMeshObject::LateUpdate_GameObject(const _float & fTimeDelta)
{
	Engine::NULL_CHECK_RETURN(m_pRenderer, -1);

	/*__________________________________________________________________________________________________________
	[ Renderer - Add Render Group ]
	____________________________________________________________________________________________________________*/
	Engine::FAILED_CHECK_RETURN(m_pRenderer->Add_Renderer(Engine::CRenderer::RENDER_NONALPHA, this), -1);

	return NO_EVENT;
}

void CStaticMeshObject::Render_GameObject(const _float & fTimeDelta)
{
	Set_ConstantTable();
	m_pMeshCom->Render_StaticMesh(m_pShaderCom);
}

void CStaticMeshObject::Render_ShadowDepth(const _float & fTimeDelta)
{
	Set_ConstantTableShadowDepth();
	m_pMeshCom->Render_StaticMeshShadowDepth(m_pShadowCom);
}

void CStaticMeshObject::Render_GameObject(const _float& fTimeDelta, 
										  ID3D12GraphicsCommandList * pCommandList,
										  const _int& iContextIdx)
{
	Set_ConstantTable();
	m_pMeshCom->Render_StaticMesh(pCommandList, iContextIdx, m_pShaderCom);
}

void CStaticMeshObject::Render_ShadowDepth(const _float& fTimeDelta, 
										   ID3D12GraphicsCommandList * pCommandList,
										   const _int& iContextIdx)
{
	Set_ConstantTableShadowDepth();
	m_pMeshCom->Render_StaticMeshShadowDepth(pCommandList, iContextIdx, m_pShadowCom);
}

HRESULT CStaticMeshObject::Add_Component(wstring wstrMeshTag)
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

	// Shadow
	m_pShadowCom = static_cast<Engine::CShaderShadow*>(m_pComponentMgr->Clone_Component(L"ShaderShadow", Engine::COMPONENTID::ID_STATIC));
	Engine::NULL_CHECK_RETURN(m_pShadowCom, E_FAIL);
	m_pShadowCom->AddRef();
	Engine::FAILED_CHECK_RETURN(m_pShadowCom->Set_PipelineStatePass(0), E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Shadow", m_pShadowCom);

	return S_OK;
}

void CStaticMeshObject::Set_ConstantTable()
{
	/*__________________________________________________________________________________________________________
	[ Set ConstantBuffer Data ]
	____________________________________________________________________________________________________________*/
	Engine::SHADOW_DESC tShadowInfo = CShadowLightMgr::Get_Instance()->Get_ShadowDesc();
	
	Engine::CB_SHADER_MESH tCB_ShaderMesh;
	ZeroMemory(&tCB_ShaderMesh, sizeof(Engine::CB_SHADER_MESH));
	tCB_ShaderMesh.matWorld			= Engine::CShader::Compute_MatrixTranspose(m_pTransCom->m_matWorld);
	tCB_ShaderMesh.matLightView		= Engine::CShader::Compute_MatrixTranspose(tShadowInfo.matLightView);
	tCB_ShaderMesh.matLightProj		= Engine::CShader::Compute_MatrixTranspose(tShadowInfo.matLightProj);
	tCB_ShaderMesh.vLightPos		= tShadowInfo.vLightPosition;
	tCB_ShaderMesh.fLightPorjFar	= tShadowInfo.fLightPorjFar;

	m_pShaderCom->Get_UploadBuffer_ShaderMesh()->CopyData(0, tCB_ShaderMesh);
}

void CStaticMeshObject::Set_ConstantTableShadowDepth()
{
	/*__________________________________________________________________________________________________________
	[ Set ConstantBuffer Data ]
	____________________________________________________________________________________________________________*/
	Engine::SHADOW_DESC tShadowInfo = CShadowLightMgr::Get_Instance()->Get_ShadowDesc();

	Engine::CB_SHADOWDEPTH_DESC	tCB_ShadowDepthDesc;
	ZeroMemory(&tCB_ShadowDepthDesc, sizeof(Engine::CB_SHADOWDEPTH_DESC));
	tCB_ShadowDepthDesc.matWVP			= Engine::CShader::Compute_MatrixTranspose(m_pTransCom->m_matWorld * tShadowInfo.matLightView * tShadowInfo.matLightProj);
	tCB_ShadowDepthDesc.matWorld		= Engine::CShader::Compute_MatrixTranspose(m_pTransCom->m_matWorld);
	tCB_ShadowDepthDesc.matLightView	= Engine::CShader::Compute_MatrixTranspose(tShadowInfo.matLightView);
	tCB_ShadowDepthDesc.matLightProj	= Engine::CShader::Compute_MatrixTranspose(tShadowInfo.matLightProj);
	tCB_ShadowDepthDesc.vLightPosition	= tShadowInfo.vLightPosition;
	tCB_ShadowDepthDesc.fLightPorjFar	= tShadowInfo.fLightPorjFar;

	m_pShadowCom->Get_UploadBuffer_ShadowDepthDesc()->CopyData(0, tCB_ShadowDepthDesc);
}

CStaticMeshObject * CStaticMeshObject::Create(ID3D12Device * pGraphicDevice, ID3D12GraphicsCommandList * pCommandList, 
											  wstring wstrMeshTag, 
											  const _vec3 & vScale, 
											  const _vec3 & vAngle, 
											  const _vec3 & vPos,
											  const _bool& bIsRenderShadow,
											  const _bool& bIsCollision,
											  const _vec3& vBoundingSphereScale,
											  const _vec3& vBoundingSpherePos)
{
	CStaticMeshObject* pInstance = new CStaticMeshObject(pGraphicDevice, pCommandList);

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

void CStaticMeshObject::Free()
{
	Engine::CGameObject::Free();

	Engine::Safe_Release(m_pDynamicCamera);

	Engine::Safe_Release(m_pMeshCom);
	Engine::Safe_Release(m_pShaderCom);
	Engine::Safe_Release(m_pShadowCom);

}
