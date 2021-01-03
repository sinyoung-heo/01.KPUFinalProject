#include "stdafx.h"
#include "TerrainMeshObject.h"


#include "GraphicDevice.h"
#include "DirectInput.h"
#include "ObjectMgr.h"
#include "LightMgr.h"
#include "DynamicCamera.h"
#include "RenderTarget.h"


CTerrainMeshObject::CTerrainMeshObject(ID3D12Device * pGraphicDevice, ID3D12GraphicsCommandList * pCommandList)
	: Engine::CGameObject(pGraphicDevice, pCommandList)
{
}

CTerrainMeshObject::CTerrainMeshObject(const CTerrainMeshObject & rhs)
	: Engine::CGameObject(rhs)
	, m_wstrMeshTag(rhs.m_wstrMeshTag)
	, m_pDynamicCamera(rhs.m_pDynamicCamera)
{
}

HRESULT CTerrainMeshObject::Ready_GameObject(wstring wstrMeshTag,
											 const _vec3 & vScale,
											 const _vec3 & vAngle, 
											 const _vec3 & vPos)
{
	m_wstrMeshTag = wstrMeshTag;

	Engine::FAILED_CHECK_RETURN(Engine::CGameObject::Ready_GameObject(true, true), E_FAIL);
	Engine::FAILED_CHECK_RETURN(Add_Component(m_wstrMeshTag), E_FAIL);

	m_pTransCom->m_vScale	= vScale;
	m_pTransCom->m_vAngle	= vAngle;
	m_pTransCom->m_vPos		= vPos;

	return S_OK;
}

HRESULT CTerrainMeshObject::LateInit_GameObject()
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

_int CTerrainMeshObject::Update_GameObject(const _float & fTimeDelta)
{
	Engine::FAILED_CHECK_RETURN(Engine::CGameObject::LateInit_GameObject(), E_FAIL);

	if (m_bIsDead)
		return DEAD_OBJ;

	/*__________________________________________________________________________________________________________
	[ Renderer - Add Render Group ]
	____________________________________________________________________________________________________________*/
	Engine::FAILED_CHECK_RETURN(m_pRenderer->Add_Renderer(Engine::CRenderer::RENDER_NONALPHA, this), -1);

	/*____________________________________________________________________
	TransCom - Update WorldMatrix.
	______________________________________________________________________*/
	Engine::CGameObject::Update_GameObject(fTimeDelta);

	return NO_EVENT;
}

_int CTerrainMeshObject::LateUpdate_GameObject(const _float & fTimeDelta)
{
	Engine::NULL_CHECK_RETURN(m_pRenderer, -1);
	
	Set_ConstantTable();
	Set_ConstantTableShadowDepth();

	return NO_EVENT;
}

void CTerrainMeshObject::Render_GameObject(const _float & fTimeDelta)
{
	m_pMeshCom->Render_StaticMesh(m_pShaderCom);
}

void CTerrainMeshObject::Render_ShadowDepth(const _float & fTimeDelta)
{
	m_pMeshCom->Render_StaticMeshShadowDepth(m_pShadowCom);
}

void CTerrainMeshObject::Render_GameObject(const _float& fTimeDelta, 
										   ID3D12GraphicsCommandList * pCommandList,
										   const _int& iContextIdx)
{
	m_pMeshCom->Render_StaticMesh(pCommandList, iContextIdx, m_pShaderCom);
}

void CTerrainMeshObject::Render_ShadowDepth(const _float& fTimeDelta, 
											ID3D12GraphicsCommandList * pCommandList,
											const _int& iContextIdx)
{
	m_pMeshCom->Render_StaticMeshShadowDepth(pCommandList, iContextIdx, m_pShadowCom);
}

HRESULT CTerrainMeshObject::Add_Component(wstring wstrMeshTag)
{
	Engine::NULL_CHECK_RETURN(m_pComponentMgr, E_FAIL);

	// Mesh
	m_pMeshCom = static_cast<Engine::CMesh*>(m_pComponentMgr->Clone_Component(wstrMeshTag.c_str(), Engine::COMPONENTID::ID_STATIC));
	Engine::NULL_CHECK_RETURN(m_pMeshCom, E_FAIL);
	m_pMeshCom->AddRef();
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Mesh", m_pMeshCom);

	// Shader
	m_pShaderCom = static_cast<Engine::CShaderMesh*>(m_pComponentMgr->Clone_Component(L"Prototype_ShaderMesh", Engine::COMPONENTID::ID_STATIC));
	Engine::NULL_CHECK_RETURN(m_pShaderCom, E_FAIL);
	m_pShaderCom->AddRef();
	Engine::FAILED_CHECK_RETURN(m_pShaderCom->Set_PipelineStatePass(1), E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Shader", m_pShaderCom);

	// Shadow
	m_pShadowCom = static_cast<Engine::CShaderShadow*>(m_pComponentMgr->Clone_Component(L"Prototype_ShaderShadow", Engine::COMPONENTID::ID_STATIC));
	Engine::NULL_CHECK_RETURN(m_pShadowCom, E_FAIL);
	m_pShadowCom->AddRef();
	Engine::FAILED_CHECK_RETURN(m_pShadowCom->Set_PipelineStatePass(0), E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Shadow", m_pShadowCom);

	return S_OK;
}

void CTerrainMeshObject::Set_ConstantTable()
{
	_matrix* pmatView = Engine::CGraphicDevice::Get_Instance()->Get_Transform(Engine::VIEW);
	_matrix* pmatProj = Engine::CGraphicDevice::Get_Instance()->Get_Transform(Engine::PROJECTION);

	if (nullptr == pmatView || nullptr == pmatProj)
		return;

	/*__________________________________________________________________________________________________________
	[ Matrix Info ]
	____________________________________________________________________________________________________________*/
	Engine::CB_MATRIX_DESC	tCB_MatrixDesc;
	ZeroMemory(&tCB_MatrixDesc, sizeof(Engine::CB_MATRIX_DESC));
	XMStoreFloat4x4(&tCB_MatrixDesc.matWVP, XMMatrixTranspose(m_pTransCom->m_matWorld * (*pmatView) * (*pmatProj)));
	XMStoreFloat4x4(&tCB_MatrixDesc.matWorld, XMMatrixTranspose(m_pTransCom->m_matWorld));
	XMStoreFloat4x4(&tCB_MatrixDesc.matView, XMMatrixTranspose(*pmatView));
	XMStoreFloat4x4(&tCB_MatrixDesc.matProj, XMMatrixTranspose(*pmatProj));

	m_pShaderCom->Get_UploadBuffer_MatrixDesc()->CopyData(0, tCB_MatrixDesc);
}

void CTerrainMeshObject::Set_ConstantTableShadowDepth()
{
	if (nullptr == m_pDynamicCamera)
		return;

	_vec3 vLightDir			= _vec3(Engine::CLightMgr::Get_Instance()->Get_LightInfo(0).Direction);
	_vec3 vDynamicCamEye	= m_pDynamicCamera->Get_CameraInfo().vEye;

	Engine::CGameObject::SetUp_ShadowDepth(vDynamicCamEye, vDynamicCamEye, vLightDir);

	/*__________________________________________________________________________________________________________
	[ ShadowDepth ]
	____________________________________________________________________________________________________________*/
	Engine::CB_SHADOWDEPTH_DESC	tCB_ShadowDepthDesc;
	ZeroMemory(&tCB_ShadowDepthDesc, sizeof(Engine::CB_SHADOWDEPTH_DESC));
	XMStoreFloat4x4(&tCB_ShadowDepthDesc.matWVP, XMMatrixTranspose(m_pTransCom->m_matWorld * m_tShadowInfo.matLightView * m_tShadowInfo.matLightProj));
	XMStoreFloat4x4(&tCB_ShadowDepthDesc.matWorld, XMMatrixTranspose(m_pTransCom->m_matWorld));
	XMStoreFloat4x4(&tCB_ShadowDepthDesc.matLightView, XMMatrixTranspose(m_tShadowInfo.matLightView));
	XMStoreFloat4x4(&tCB_ShadowDepthDesc.matLightProj, XMMatrixTranspose(m_tShadowInfo.matLightProj));
	tCB_ShadowDepthDesc.vLightPosition = m_tShadowInfo.vLightPosition;
	tCB_ShadowDepthDesc.fLightPorjFar  = m_tShadowInfo.fLightPorjFar;

	m_pShadowCom->Get_UploadBuffer_ShadowDepthDesc()->CopyData(0, tCB_ShadowDepthDesc);

	/*__________________________________________________________________________________________________________
	[ ShadowInfo ]
	____________________________________________________________________________________________________________*/
	Engine::CB_SHADOW_DESC	tCB_ShadowDesc;
	ZeroMemory(&tCB_ShadowDesc, sizeof(Engine::CB_SHADOW_DESC));
	XMStoreFloat4x4(&tCB_ShadowDesc.matLightView, XMMatrixTranspose(m_tShadowInfo.matLightView));
	XMStoreFloat4x4(&tCB_ShadowDesc.matLightProj, XMMatrixTranspose(m_tShadowInfo.matLightProj));
	tCB_ShadowDesc.vLightPosition	= m_tShadowInfo.vLightPosition;
	tCB_ShadowDesc.fLightPorjFar	= m_tShadowInfo.fLightPorjFar;

	m_pShaderCom->Get_UploadBuffer_ShadowDesc()->CopyData(0, tCB_ShadowDesc);

}


CTerrainMeshObject * CTerrainMeshObject::Create(ID3D12Device * pGraphicDevice, 
												ID3D12GraphicsCommandList * pCommandList, 
												wstring wstrMeshTag, 
												const _vec3 & vScale,
												const _vec3 & vAngle,
												const _vec3 & vPos)
{
	CTerrainMeshObject* pInstance = new CTerrainMeshObject(pGraphicDevice, pCommandList);

	if (FAILED(pInstance->Ready_GameObject(wstrMeshTag, vScale, vAngle, vPos)))
		Engine::Safe_Release(pInstance);

	return pInstance;
}

void CTerrainMeshObject::Free()
{
	Engine::CGameObject::Free();

	Engine::Safe_Release(m_pDynamicCamera);

	Engine::Safe_Release(m_pMeshCom);
	Engine::Safe_Release(m_pShaderCom);
	Engine::Safe_Release(m_pShadowCom);

}
