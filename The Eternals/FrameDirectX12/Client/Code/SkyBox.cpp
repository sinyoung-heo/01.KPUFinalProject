#include "stdafx.h"
#include "SkyBox.h"
#include "ObjectMgr.h"
#include "GraphicDevice.h"
#include "DirectInput.h"


CSkyBox::CSkyBox(ID3D12Device * pGraphicDevice, ID3D12GraphicsCommandList * pCommandList)
	: Engine::CGameObject(pGraphicDevice, pCommandList)
{
}

HRESULT CSkyBox::Ready_GameObject(wstring wstrTextureTag, 
								  const _vec3 & vScale,
								  const _vec3 & vAngle,
								  const _vec3 & vPos,
								  const _int& iIdx)
{
	Engine::FAILED_CHECK_RETURN(Engine::CGameObject::Ready_GameObject(true, false), E_FAIL);
	Engine::FAILED_CHECK_RETURN(Add_Component(wstrTextureTag), E_FAIL);

	m_pTransCom->m_vScale	= vScale;
	m_pTransCom->m_vAngle	= vAngle;
	m_pTransCom->m_vPos		= vPos;

	m_uiTexIdx	= iIdx;
	Engine::FAILED_CHECK_RETURN(m_pShaderCom->Set_PipelineStatePass(1), E_FAIL);

	return S_OK;
}

HRESULT CSkyBox::LateInit_GameObject()
{
	// SetUp Shader ConstantBuffer
	m_pShaderCom->SetUp_ShaderConstantBuffer();

	return S_OK;
}

_int CSkyBox::Update_GameObject(const _float & fTimeDelta)
{
	Engine::FAILED_CHECK_RETURN(Engine::CGameObject::LateInit_GameObject(), E_FAIL);

	if (m_bIsDead)
		return DEAD_OBJ;

	if (nullptr != m_pTarget)
	{
		m_pTransCom->m_vPos = m_pTarget->Get_Transform()->m_vPos;
	}
	else
		m_pTarget = m_pObjectMgr->Get_GameObject(L"Layer_GameObject", L"ThisPlayer");

	if (g_bIsCinemaStart)
		m_pTransCom->m_vPos = _vec3(380.0f, 0.0f, 480.0f);

	/*__________________________________________________________________________________________________________
	[ TransCom - Update WorldMatrix ]
	____________________________________________________________________________________________________________*/
	Engine::CGameObject::Update_GameObject(fTimeDelta);

	return NO_EVENT;
}

_int CSkyBox::LateUpdate_GameObject(const _float & fTimeDelta)
{
	Engine::NULL_CHECK_RETURN(m_pRenderer, -1);

	/*__________________________________________________________________________________________________________
	[ Renderer - Add Render Group ]
	____________________________________________________________________________________________________________*/
	Engine::FAILED_CHECK_RETURN(m_pRenderer->Add_Renderer(Engine::CRenderer::RENDER_PRIORITY, this), -1);

	Set_ConstantTable();

	return NO_EVENT;
}

void CSkyBox::Render_GameObject(const _float & fTimeDelta)
{
	m_pShaderCom->Begin_Shader(m_pTextureCom->Get_TexDescriptorHeap(), 0, m_uiTexIdx, Engine::MATRIXID::PROJECTION);
	m_pBufferCom->Begin_Buffer();
	m_pBufferCom->Render_Buffer();
}

void CSkyBox::Render_GameObject(const _float& fTimeDelta, 
								ID3D12GraphicsCommandList* pCommandList, 
								const _int& iContextIdx)
{
	m_pBufferCom->Render_Buffer(pCommandList, 
								iContextIdx,
								m_pShaderCom, 
								m_pTextureCom->Get_TexDescriptorHeap(),
								m_uiTexIdx);
}

HRESULT CSkyBox::Add_Component(wstring wstrTextureTag)
{
	Engine::NULL_CHECK_RETURN(m_pComponentMgr, E_FAIL);

	// Buffer
	m_pBufferCom = static_cast<Engine::CCubeTex*>(m_pComponentMgr->Clone_Component(L"CubeTex", Engine::COMPONENTID::ID_STATIC));
	Engine::NULL_CHECK_RETURN(m_pBufferCom, E_FAIL);
	m_pBufferCom->AddRef();
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Buffer", m_pBufferCom);

	// Texture
	m_pTextureCom = static_cast<Engine::CTexture*>(m_pComponentMgr->Clone_Component(wstrTextureTag, Engine::COMPONENTID::ID_STATIC));
	Engine::NULL_CHECK_RETURN(m_pTextureCom, E_FAIL);
	m_pTextureCom->AddRef();
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Texture", m_pTextureCom);

	// Shader
	m_pShaderCom = static_cast<Engine::CShaderSkyBox*>(m_pComponentMgr->Clone_Component(L"ShaderSkyBox", Engine::COMPONENTID::ID_STATIC));
	Engine::NULL_CHECK_RETURN(m_pShaderCom, E_FAIL);
	m_pShaderCom->AddRef();
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Shader", m_pShaderCom);

	return S_OK;
}

void CSkyBox::Set_ConstantTable()
{
	/*__________________________________________________________________________________________________________
	[ Set ConstantBuffer Data ]
	____________________________________________________________________________________________________________*/
	Engine::CB_SHADER_SKYBOX tCB_ShaderSkyBox;
	ZeroMemory(&tCB_ShaderSkyBox, sizeof(Engine::CB_SHADER_SKYBOX));
	tCB_ShaderSkyBox.matWorld = Engine::CShader::Compute_MatrixTranspose(m_pTransCom->m_matWorld);

	m_pShaderCom->Get_UploadBuffer_ShaderSkyBox()->CopyData(0, tCB_ShaderSkyBox);
}

Engine::CGameObject* CSkyBox::Create(ID3D12Device * pGraphicDevice,
									 ID3D12GraphicsCommandList * pCommandList,
									 wstring wstrTextureTag,
									 const _vec3 & vScale,
									 const _vec3 & vAngle,
									 const _vec3 & vPos,
									 const _int& iIdx)
{
	CSkyBox* pInstance = new CSkyBox(pGraphicDevice, pCommandList);

	if (FAILED(pInstance->Ready_GameObject(wstrTextureTag, vScale, vAngle, vPos, iIdx)))
		Engine::Safe_Release(pInstance);

	return pInstance;
}

void CSkyBox::Free()
{
	Engine::CGameObject::Free();

	Engine::Safe_Release(m_pBufferCom);
	Engine::Safe_Release(m_pShaderCom);
	Engine::Safe_Release(m_pTextureCom);
}
