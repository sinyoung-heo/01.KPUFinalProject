#include "stdafx.h"
#include "ToolTerrain.h"
#include "ObjectMgr.h"
#include "GraphicDevice.h"


CToolTerrain::CToolTerrain(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList)
	: Engine::CGameObject(pGraphicDevice, pCommandList)
{
}

CToolTerrain::CToolTerrain(const CToolTerrain& rhs)
	: Engine::CGameObject(rhs)
{
}

HRESULT CToolTerrain::Ready_GameObject(wstring wstrTerrainTag)
{
	Engine::FAILED_CHECK_RETURN(Engine::CGameObject::Ready_GameObject(true, true), E_FAIL);
	Engine::FAILED_CHECK_RETURN(Add_Component(wstrTerrainTag), E_FAIL);

	m_pTransCom->m_vPos.y -= TERRAIN_OFFSET;

	return S_OK;
}

HRESULT CToolTerrain::LateInit_GameObject()
{
	// SetUp Shader ConstantBuffer
	m_pShaderCom->SetUp_ShaderConstantBuffer();

	return S_OK;
}

_int CToolTerrain::Update_GameObject(const _float& fTimeDelta)
{
	Engine::FAILED_CHECK_RETURN(Engine::CGameObject::LateInit_GameObject(), E_FAIL);

	if (m_bIsDead)
		return DEAD_OBJ;

	if (!m_bIsUpdate)
		return NO_EVENT;

	/*__________________________________________________________________________________________________________
	[ Renderer - Add Render Group ]
	____________________________________________________________________________________________________________*/
	if (m_bIsRender)
		Engine::FAILED_CHECK_RETURN(m_pRenderer->Add_Renderer(Engine::CRenderer::RENDER_PRIORITY, this), -1);

	/*__________________________________________________________________________________________________________
	[ TransCom - Update WorldMatrix ]
	____________________________________________________________________________________________________________*/
	Engine::CGameObject::Update_GameObject(fTimeDelta);

	return NO_EVENT;
}

_int CToolTerrain::LateUpdate_GameObject(const _float& fTimeDelta)
{
	Engine::NULL_CHECK_RETURN(m_pRenderer, -1);

	if (!m_bIsUpdate)
		return NO_EVENT;

	return NO_EVENT;
}

void CToolTerrain::Render_GameObject(const _float& fTimeDelta)
{
	Set_ConstantTable();

	m_pShaderCom->Begin_Shader(m_pTextureCom->Get_TexDescriptorHeap(), 0, m_uiTexIdx, Engine::MATRIXID::PROJECTION);
	m_pBufferCom->Begin_Buffer();

	m_pBufferCom->Render_Buffer();
}

HRESULT CToolTerrain::Add_Component(wstring wstrTerrainTag)
{
	Engine::NULL_CHECK_RETURN(m_pComponentMgr, E_FAIL);

	// Buffer
	m_pBufferCom = static_cast<Engine::CTerrainTex*>(m_pComponentMgr->Clone_Component(wstrTerrainTag, Engine::COMPONENTID::ID_STATIC));
	Engine::NULL_CHECK_RETURN(m_pBufferCom, E_FAIL);
	m_pBufferCom->AddRef();
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Buffer", m_pBufferCom);

	// Texture
	m_pTextureCom = static_cast<Engine::CTexture*>(m_pComponentMgr->Clone_Component(L"Terrain", Engine::COMPONENTID::ID_STATIC));
	Engine::NULL_CHECK_RETURN(m_pTextureCom, E_FAIL);
	m_pTextureCom->AddRef();
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Texture", m_pTextureCom);

	// Shader
	m_pShaderCom = static_cast<Engine::CShaderTexture*>(m_pComponentMgr->Clone_Component(L"ShaderTexture", Engine::COMPONENTID::ID_STATIC));
	Engine::NULL_CHECK_RETURN(m_pShaderCom, E_FAIL);
	m_pShaderCom->AddRef();
	Engine::FAILED_CHECK_RETURN(m_pShaderCom->Set_PipelineStatePass(m_uiShaderPass), E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Shader", m_pShaderCom);

	return S_OK;
}

void CToolTerrain::Set_ConstantTable()
{
	/*__________________________________________________________________________________________________________
	[ Set ConstantBuffer Data ]
	____________________________________________________________________________________________________________*/
	Engine::CB_SHADER_TEXTURE tCB_ShaderTexture;
	ZeroMemory(&tCB_ShaderTexture, sizeof(Engine::CB_SHADER_TEXTURE));
	tCB_ShaderTexture.matWorld		= Engine::CShader::Compute_MatrixTranspose(m_pTransCom->m_matWorld);

	m_pShaderCom->Get_UploadBuffer_ShaderTexture()->CopyData(0, tCB_ShaderTexture);
}

CToolTerrain* CToolTerrain::Create(ID3D12Device* pGraphicDevice, 
								   ID3D12GraphicsCommandList* pCommandList,
								   wstring wstrTerrainTag)
{
	CToolTerrain* pInstance = new CToolTerrain(pGraphicDevice, pCommandList);

	if (FAILED(pInstance->Ready_GameObject(wstrTerrainTag)))
		Engine::Safe_Release(pInstance);

	return pInstance;
}

void CToolTerrain::Free()
{
	Engine::CGameObject::Free();

	Engine::Safe_Release(m_pBufferCom);
	Engine::Safe_Release(m_pShaderCom);
	Engine::Safe_Release(m_pTextureCom);
}
