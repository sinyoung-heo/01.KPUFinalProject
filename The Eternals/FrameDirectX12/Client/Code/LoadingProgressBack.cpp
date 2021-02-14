#include "stdafx.h"
#include "LoadingProgressBack.h"

CLoadingProgressBack::CLoadingProgressBack(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList)
	: Engine::CGameObject(pGraphicDevice, pCommandList)
{
}

HRESULT CLoadingProgressBack::Ready_GameObject(wstring wstrTextureTag)
{
	Engine::FAILED_CHECK_RETURN(Engine::CGameObject::Ready_GameObject(true, false), E_FAIL);
	Engine::FAILED_CHECK_RETURN(Add_Component(wstrTextureTag), E_FAIL);

	m_matView				= INIT_MATRIX;
	m_matProj				= XMMatrixOrthographicLH(WINCX, WINCY, 0.0f, 1.0f);
	m_pTransCom->m_vPos		= _vec3((_float)WINCX / 2.0f, (_float)WINCY - 50.0f, 1.0f);
	m_pTransCom->m_vScale	= _vec3(1200.0f, 100.0f, 1.0f);

	m_uiTexIdx	= 2;
	m_UIDepth	= 900;

	return S_OK;
}

HRESULT CLoadingProgressBack::LateInit_GameObject()
{
	// SetUp Shader ConstantBuffer
	m_pShaderCom->SetUp_ShaderConstantBuffer();

	return S_OK;
}

_int CLoadingProgressBack::Update_GameObject(const _float& fTimeDelta)
{
	Engine::FAILED_CHECK_RETURN(Engine::CGameObject::LateInit_GameObject(), E_FAIL);

	if (m_bIsDead)
		return DEAD_OBJ;

	/*__________________________________________________________________________________________________________
	[ Renderer - Add Render Group ]
	____________________________________________________________________________________________________________*/
	Engine::FAILED_CHECK_RETURN(m_pRenderer->Add_Renderer(Engine::CRenderer::RENDER_UI, this), -1);

	/*__________________________________________________________________________________________________________
	[ TransCom - Update WorldMatrix ]
	____________________________________________________________________________________________________________*/
	m_vConvert = m_pTransCom->m_vPos.Convert_2DWindowToDescartes(WINCX, WINCY);

	_matrix matScale = XMMatrixScaling(m_pTransCom->m_vScale.x, m_pTransCom->m_vScale.y, m_pTransCom->m_vScale.z);
	_matrix matTrans = XMMatrixTranslation(m_vConvert.x, m_vConvert.y, m_vConvert.z);

	m_pTransCom->m_matWorld = matScale *  matTrans;

	return NO_EVENT;
}

_int CLoadingProgressBack::LateUpdate_GameObject(const _float& fTimeDelta)
{
	Engine::NULL_CHECK_RETURN(m_pRenderer, -1);

	return NO_EVENT;
}

void CLoadingProgressBack::Render_GameObject(const _float& fTimeDelta)
{
	Set_ConstantTable();

	m_pShaderCom->Begin_Shader(m_pTextureCom->Get_TexDescriptorHeap(), 0, m_uiTexIdx, Engine::MATRIXID::ORTHO);
	m_pBufferCom->Begin_Buffer();

	m_pBufferCom->Render_Buffer();
}

HRESULT CLoadingProgressBack::Add_Component(wstring wstrTextureTag)
{
	Engine::NULL_CHECK_RETURN(m_pComponentMgr, E_FAIL);

	// Buffer
	m_pBufferCom = static_cast<Engine::CRcTex*>(m_pComponentMgr->Clone_Component(L"RcTex", Engine::COMPONENTID::ID_STATIC));
	Engine::NULL_CHECK_RETURN(m_pBufferCom, E_FAIL);
	m_pBufferCom->AddRef();
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Buffer", m_pBufferCom);

	// Texture
	m_pTextureCom = static_cast<Engine::CTexture*>(m_pComponentMgr->Clone_Component(wstrTextureTag, Engine::COMPONENTID::ID_STATIC));
	Engine::NULL_CHECK_RETURN(m_pTextureCom, E_FAIL);
	m_pTextureCom->AddRef();
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Texture", m_pTextureCom);

	// Shader
	m_pShaderCom = static_cast<Engine::CShaderTexture*>(m_pComponentMgr->Clone_Component(L"ShaderTexture", Engine::COMPONENTID::ID_STATIC));
	Engine::NULL_CHECK_RETURN(m_pShaderCom, E_FAIL);
	m_pShaderCom->AddRef();
	m_pShaderCom->Set_PipelineStatePass(3);	// Z Write (X)
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Shader", m_pShaderCom);

	return S_OK;
}

void CLoadingProgressBack::Set_ConstantTable()
{
	/*__________________________________________________________________________________________________________
	[ Set ConstantBuffer Data ]
	____________________________________________________________________________________________________________*/
	Engine::CB_CAMERA_MATRIX tCB_CameraMatrix;
	ZeroMemory(&tCB_CameraMatrix, sizeof(Engine::CB_CAMERA_MATRIX));
	tCB_CameraMatrix.matView	= Engine::CShader::Compute_MatrixTranspose(m_matView);
	tCB_CameraMatrix.matProj	= Engine::CShader::Compute_MatrixTranspose(m_matProj);

	Engine::CB_SHADER_TEXTURE tCB_ShaderTexture;
	ZeroMemory(&tCB_ShaderTexture, sizeof(Engine::CB_SHADER_TEXTURE));
	tCB_ShaderTexture.matWorld = Engine::CShader::Compute_MatrixTranspose(m_pTransCom->m_matWorld);

	m_pShaderCom->Get_UploadBuffer_CameraOrthoMatrix()->CopyData(0, tCB_CameraMatrix);
	m_pShaderCom->Get_UploadBuffer_ShaderTexture()->CopyData(0, tCB_ShaderTexture);
}

Engine::CGameObject* CLoadingProgressBack::Create(ID3D12Device* pGraphicDevice,
												  ID3D12GraphicsCommandList* pCommandList, 
												  wstring wstrTextureTag)
{
	CLoadingProgressBack* pInstance = new CLoadingProgressBack(pGraphicDevice, pCommandList);

	if (FAILED(pInstance->Ready_GameObject(wstrTextureTag)))
		Engine::Safe_Release(pInstance);

	return pInstance;
}

void CLoadingProgressBack::Free()
{
	Engine::CGameObject::Free();

	Engine::Safe_Release(m_pBufferCom);
	Engine::Safe_Release(m_pShaderCom);
	Engine::Safe_Release(m_pTextureCom);
}
