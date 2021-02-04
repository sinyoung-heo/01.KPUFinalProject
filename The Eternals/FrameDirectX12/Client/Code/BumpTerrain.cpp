#include "stdafx.h"
#include "BumpTerrain.h"

#include "GraphicDevice.h"
#include "Renderer.h"
#include "RenderTarget.h"

CBumpTerrain::CBumpTerrain(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList)
	: Engine::CGameObject(pGraphicDevice, pCommandList)
{
}

CBumpTerrain::CBumpTerrain(const CBumpTerrain& rhs)
	: Engine::CGameObject(rhs)
{
}

HRESULT CBumpTerrain::Ready_GameObject(wstring wstrTextureTag)
{
	Engine::FAILED_CHECK_RETURN(Engine::CGameObject::Ready_GameObject(true), E_FAIL);
	Engine::FAILED_CHECK_RETURN(Add_Component(wstrTextureTag), E_FAIL);

	return S_OK;
}

HRESULT CBumpTerrain::LateInit_GameObject()
{
	// SetUp Shader ConstantBuffer
	// m_pShaderCom->SetUp_ShaderConstantBuffer();
	// m_pShadowCom->SetUp_ShaderConstantBuffer(1);

	return S_OK;
}

_int CBumpTerrain::Update_GameObject(const _float& fTimeDelta)
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

_int CBumpTerrain::LateUpdate_GameObject(const _float& fTimeDelta)
{
	Engine::NULL_CHECK_RETURN(m_pRenderer, -1);

	/*__________________________________________________________________________________________________________
	[ Renderer - Add Render Group ]
	____________________________________________________________________________________________________________*/
	Engine::FAILED_CHECK_RETURN(m_pRenderer->Add_Renderer(Engine::CRenderer::RENDER_NONALPHA, this), -1);

	return NO_EVENT;
}

void CBumpTerrain::Render_GameObject(const _float& fTimeDelta)
{
	//Set_ConstantTable();

	//m_pShaderCom->Begin_Shader(m_pTextureCom->Get_TexDescriptorHeap(), m_pRenderer->Get_TargetShadowDepth()->Get_TargetTextureDescriptorHeap());
	//m_pBufferCom->Begin_Buffer();

	//m_pBufferCom->Render_Buffer();
}

void CBumpTerrain::Render_ShadowDepth(const _float& fTimeDelta)
{
	//Set_ConstantTableShadowDepth();

	//m_pShadowCom->Begin_Shader();
	//m_pBufferCom->Begin_Buffer();

	//m_pBufferCom->Render_Buffer();
}

void CBumpTerrain::Render_GameObject(const _float& fTimeDelta, 
									 ID3D12GraphicsCommandList* pCommandList, 
									 const _int& iContextIdx)
{
	// Set_ConstantTable();

	// m_pShaderCom->Begin_Shader(pCommandList, iContextIdx, m_pTextureCom->Get_TexDescriptorHeap(), m_pRenderer->Get_TargetShadowDepth()->Get_TargetTextureDescriptorHeap());
	// m_pBufferCom->Begin_Buffer(pCommandList);

	// m_pBufferCom->Render_Buffer(pCommandList);
}

void CBumpTerrain::Render_ShadowDepth(const _float& fTimeDelta, 
									  ID3D12GraphicsCommandList* pCommandList,
									  const _int& iContextIdx)
{
	//Set_ConstantTableShadowDepth();

	//m_pShadowCom->Begin_Shader(pCommandList, iContextIdx);
	//m_pBufferCom->Begin_Buffer();

	//m_pBufferCom->Render_Buffer();
}

HRESULT CBumpTerrain::Add_Component(wstring wstrTextureTag)
{
	Engine::NULL_CHECK_RETURN(m_pComponentMgr, E_FAIL);

	//// Buffer
	//m_pBufferCom = static_cast<Engine::CTerrainTex*>(m_pComponentMgr->Clone_Component(L"TerrainTex", Engine::COMPONENTID::ID_STATIC));
	//Engine::NULL_CHECK_RETURN(m_pBufferCom, E_FAIL);
	//m_pBufferCom->AddRef();
	//m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Buffer", m_pBufferCom);

	//// Texture
	//m_pTextureCom = static_cast<Engine::CTexture*>(m_pComponentMgr->Clone_Component(wstrTextureTag, Engine::COMPONENTID::ID_STATIC));
	//Engine::NULL_CHECK_RETURN(m_pTextureCom, E_FAIL);
	//m_pTextureCom->AddRef();
	//m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Texture", m_pTextureCom);

	//// Shader
	//m_pShaderCom = static_cast<Engine::CShaderBumpTerrain*>(m_pComponentMgr->Clone_Component(L"ShaderBumpTerrain", Engine::COMPONENTID::ID_STATIC));
	//Engine::NULL_CHECK_RETURN(m_pShaderCom, E_FAIL);
	//m_pShaderCom->AddRef();
	//Engine::FAILED_CHECK_RETURN(m_pShaderCom->Set_PipelineStatePass(0), E_FAIL);
	//m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Shader", m_pShaderCom);

	//// Shadow
	//m_pShadowCom = static_cast<Engine::CShaderShadow*>(m_pComponentMgr->Clone_Component(L"ShaderShadow", Engine::COMPONENTID::ID_STATIC));
	//Engine::NULL_CHECK_RETURN(m_pShadowCom, E_FAIL);
	//m_pShadowCom->AddRef();
	//Engine::FAILED_CHECK_RETURN(m_pShadowCom->Set_PipelineStatePass(1), E_FAIL);
	//m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Shadow", m_pShadowCom);

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

	// m_pShaderCom->Get_UploadBuffer_ShaderMesh()->CopyData(0, tCB_ShaderMesh);
}

void CBumpTerrain::Set_ConstantTableShadowDepth()
{
	/*__________________________________________________________________________________________________________
	[ Set ConstantBuffer Data ]
	____________________________________________________________________________________________________________*/
	Engine::SHADOW_DESC tShadowDesc = CShadowLightMgr::Get_Instance()->Get_ShadowDesc();

	Engine::CB_SHADER_SHADOW tCB_ShaderShadow;
	ZeroMemory(&tCB_ShaderShadow, sizeof(Engine::CB_SHADER_SHADOW));
	tCB_ShaderShadow.matWorld	= Engine::CShader::Compute_MatrixTranspose(m_pTransCom->m_matWorld);
	tCB_ShaderShadow.matView	= Engine::CShader::Compute_MatrixTranspose(tShadowDesc.matLightView);
	tCB_ShaderShadow.matProj	= Engine::CShader::Compute_MatrixTranspose(tShadowDesc.matLightProj);
	tCB_ShaderShadow.fProjFar	= tShadowDesc.fLightPorjFar;

	// m_pShadowCom->Get_UploadBuffer_ShaderShadow()->CopyData(0, tCB_ShaderShadow);
}

Engine::CGameObject* CBumpTerrain::Create(ID3D12Device* pGraphicDevice, 
										  ID3D12GraphicsCommandList* pCommandList, 
										  wstring wstrTextureTag)
{
	CBumpTerrain* pInstance = new CBumpTerrain(pGraphicDevice, pCommandList);

	if (FAILED(pInstance->Ready_GameObject(wstrTextureTag)))
		Engine::Safe_Release(pInstance);

	return pInstance;
}

void CBumpTerrain::Free()
{
	Engine::CGameObject::Free();

	//Engine::Safe_Release(m_pBufferCom);
	//Engine::Safe_Release(m_pTextureCom);
	//Engine::Safe_Release(m_pShaderCom);
	//Engine::Safe_Release(m_pShadowCom);
}
