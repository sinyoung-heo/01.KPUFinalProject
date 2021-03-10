#include "stdafx.h"
#include "BumpTerrain.h"
#include "GraphicDevice.h"
#include "Renderer.h"
#include "RenderTarget.h"
#include "DescriptorHeapMgr.h"

CBumpTerrain::CBumpTerrain(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList)
	: Engine::CGameObject(pGraphicDevice, pCommandList)
{
}

HRESULT CBumpTerrain::Ready_GameObject(wstring wstrTextureTag)
{
	Engine::FAILED_CHECK_RETURN(Engine::CGameObject::Ready_GameObject(), E_FAIL);
	Engine::FAILED_CHECK_RETURN(Add_Component(wstrTextureTag), E_FAIL);

	Engine::FAILED_CHECK_RETURN(m_pShaderCom->Set_PipelineStatePass(0), E_FAIL);

	m_wstrTextureTag = wstrTextureTag;

	return S_OK;
}

HRESULT CBumpTerrain::LateInit_GameObject()
{
	vector<ComPtr<ID3D12Resource>> vecTexture = m_pTextureCom->Get_Texture();
	vecTexture.emplace_back(m_pRenderer->Get_TargetShadowDepth()->Get_TargetTexture().front());

	// SetUp Shader ConstantBuffer
	Engine::FAILED_CHECK_RETURN(m_pShaderCom->SetUp_ShaderConstantBuffer(), E_FAIL);

	// SetUp Shader DescriptorHeap
	Engine::FAILED_CHECK_RETURN(m_pShaderCom->SetUp_ShaderDescriptorHeap(vecTexture), E_FAIL);

	return S_OK;
}

_int CBumpTerrain::Update_GameObject(const _float& fTimeDelta)
{
	Engine::FAILED_CHECK_RETURN(Engine::CGameObject::LateInit_GameObject(), E_FAIL);

	/*__________________________________________________________________________________________________________
	[ Renderer - Add Render Group ]
	____________________________________________________________________________________________________________*/
	Engine::FAILED_CHECK_RETURN(m_pRenderer->Add_Renderer(Engine::CRenderer::RENDER_NONALPHA, this), -1);

	/*__________________________________________________________________________________________________________
	[ TransCom - Update WorldMatrix ]
	____________________________________________________________________________________________________________*/
	Engine::CGameObject::Update_GameObject(fTimeDelta);

	return NO_EVENT;
}

_int CBumpTerrain::LateUpdate_GameObject(const _float& fTimeDelta)
{
	Engine::NULL_CHECK_RETURN(m_pRenderer, -1);

	return NO_EVENT;
}

void CBumpTerrain::Render_GameObject(const _float& fTimeDelta)
{
	Set_ConstantTable();

}

void CBumpTerrain::Render_GameObject(const _float& fTimeDelta, 
									 ID3D12GraphicsCommandList* pCommandList, 
									 const _int& iContextIdx)
{
	Set_ConstantTable();
	m_pBufferCom->Render_BumpTerrain(pCommandList, 
									 iContextIdx, 
									 m_pShaderCom);
}

HRESULT CBumpTerrain::Add_Component(wstring wstrTextureTag)
{
	Engine::NULL_CHECK_RETURN(m_pComponentMgr, E_FAIL);

	// Buffer
	m_pBufferCom = static_cast<Engine::CBumpTerrainTex*>(m_pComponentMgr->Clone_Component(L"BumpTerrainTex", Engine::COMPONENTID::ID_STATIC));
	Engine::NULL_CHECK_RETURN(m_pBufferCom, E_FAIL);
	m_pBufferCom->AddRef();
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Buffer", m_pBufferCom);

	// Texture
	m_pTextureCom = static_cast<Engine::CTexture*>(m_pComponentMgr->Clone_Component(wstrTextureTag, Engine::COMPONENTID::ID_STATIC));
	Engine::NULL_CHECK_RETURN(m_pTextureCom, E_FAIL);
	m_pTextureCom->AddRef();
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Texture", m_pTextureCom);

	// Shader
	m_pShaderCom = static_cast<Engine::CShaderBumpTerrain*>(m_pComponentMgr->Clone_Component(L"ShaderBumpTerrain", Engine::COMPONENTID::ID_STATIC));
	Engine::NULL_CHECK_RETURN(m_pShaderCom, E_FAIL);
	m_pShaderCom->AddRef();
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Shader", m_pShaderCom);

	return S_OK;
}

void CBumpTerrain::Set_ConstantTable()
{
	/*__________________________________________________________________________________________________________
	[ Set ConstantBuffer Data ]
	____________________________________________________________________________________________________________*/
	Engine::SHADOW_DESC tShadowDesc = CShadowLightMgr::Get_Instance()->Get_ShadowDesc();
	
	Engine::CB_SHADER_MESH tCB_ShaderMesh;
	ZeroMemory(&tCB_ShaderMesh, sizeof(Engine::CB_SHADER_MESH));
	tCB_ShaderMesh.matWorld			= Engine::CShader::Compute_MatrixTranspose(m_pTransCom->m_matWorld);
	tCB_ShaderMesh.matLightView		= Engine::CShader::Compute_MatrixTranspose(tShadowDesc.matLightView);
	tCB_ShaderMesh.matLightProj		= Engine::CShader::Compute_MatrixTranspose(tShadowDesc.matLightProj);
	tCB_ShaderMesh.vLightPos		= tShadowDesc.vLightPosition;
	tCB_ShaderMesh.fLightPorjFar	= tShadowDesc.fLightPorjFar;

	m_pShaderCom->Get_UploadBuffer_ShaderMesh()->CopyData(0, tCB_ShaderMesh);
}

Engine::CGameObject* CBumpTerrain::Create(ID3D12Device* pGraphicDevice,
										  ID3D12GraphicsCommandList* pCommandList, 
										  wstring wstrTextureTag)
{
	CBumpTerrain* pInstance = new CBumpTerrain(pGraphicDevice, pCommandList);

	if FAILED(pInstance->Ready_GameObject(wstrTextureTag))
		Engine::Safe_Release(pInstance);

	return pInstance;
}

void CBumpTerrain::Free()
{
	Engine::CGameObject::Free();
	Engine::Safe_Release(m_pBufferCom);
	Engine::Safe_Release(m_pTextureCom);
	Engine::Safe_Release(m_pShaderCom);

}
