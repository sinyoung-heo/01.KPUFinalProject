#include "stdafx.h"
#include "MiniMapCanvas.h"
#include "Renderer.h"
#include "RenderTarget.h"
#include "DirectInput.h"

CMiniMapCanvas::CMiniMapCanvas(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList)
	: CGameUIRoot(pGraphicDevice, pCommandList)
{
}

HRESULT CMiniMapCanvas::Ready_GameObject(wstring wstrObjectTag, 
										 wstring wstrDataFilePath,
										 const _vec3& vPos,
										 const _vec3& vScale, 
										 const _bool& bIsSpriteAnimation, 
										 const _float& fFrameSpeed,
										 const _vec3& vRectOffset,
										 const _vec3& vRectScale,
										 const _long& iUIDepth)
{
	Engine::FAILED_CHECK_RETURN(CGameUIRoot::Ready_GameObject(wstrObjectTag,
															  wstrDataFilePath,
															  vPos,
															  vScale,
															  bIsSpriteAnimation,
															  fFrameSpeed,
															  vRectOffset,
															  vRectScale,
															  iUIDepth), E_FAIL);

	m_bIsActive = true;

	// Shader
	m_pShaderTargetCom = static_cast<Engine::CShaderTexture*>(m_pComponentMgr->Clone_Component(L"ShaderTexture", Engine::COMPONENTID::ID_STATIC));
	Engine::NULL_CHECK_RETURN(m_pShaderTargetCom, E_FAIL);
	m_pShaderTargetCom->AddRef();
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_ShaderTarget", m_pShaderTargetCom);
	m_pShaderTargetCom->Set_PipelineStatePass(5);

	return S_OK;
}

HRESULT CMiniMapCanvas::LateInit_GameObject()
{
	Engine::FAILED_CHECK_RETURN(CGameUIRoot::LateInit_GameObject(), E_FAIL);
	m_pShaderTargetCom->SetUp_ShaderConstantBuffer();

	return S_OK;
}

_int CMiniMapCanvas::Update_GameObject(const _float& fTimeDelta)
{
	Engine::FAILED_CHECK_RETURN(Engine::CGameObject::LateInit_GameObject(), E_FAIL);

	if (m_bIsDead)
		return DEAD_OBJ;
	if (!m_bIsActive)
		return NO_EVENT;

	CGameUIRoot::Update_GameObject(fTimeDelta);

	return NO_EVENT;
}

_int CMiniMapCanvas::LateUpdate_GameObject(const _float& fTimeDelta)
{
	if (!m_bIsActive)
		return NO_EVENT;

	CGameUIRoot::LateUpdate_GameObject(fTimeDelta);

	return NO_EVENT;
}

void CMiniMapCanvas::Render_GameObject(const _float& fTimeDelta)
{
	CGameUIRoot::Render_GameObject(fTimeDelta);
	Render_MiniMapTarget(fTimeDelta);
}

void CMiniMapCanvas::Render_MiniMapTarget(const _float& fTimeDelta)
{
	Engine::CRenderTarget* pTargetMiniMap = m_pRenderer->Get_TargetMiniMap();
	if (nullptr == pTargetMiniMap)
		return;

	_matrix matWorld = m_pTransCom->m_matWorld;
	matWorld._11 *= 0.5f;
	matWorld._22 *= 0.5f;

	/*__________________________________________________________________________________________________________
	[ Set ConstantBuffer Data ]
	____________________________________________________________________________________________________________*/
	Engine::CB_SHADER_TEXTURE tCB_ShaderTexture;
	ZeroMemory(&tCB_ShaderTexture, sizeof(Engine::CB_SHADER_TEXTURE));
	tCB_ShaderTexture.matWorld	= Engine::CShader::Compute_MatrixTranspose(matWorld);
	tCB_ShaderTexture.fFrameCnt	= m_tFrame.fFrameCnt;
	tCB_ShaderTexture.fSceneCnt	= m_tFrame.fSceneCnt;
	tCB_ShaderTexture.fCurFrame = m_tFrame.fCurFrame;
	tCB_ShaderTexture.fCurScene = m_tFrame.fCurScene;
	m_pShaderTargetCom->Get_UploadBuffer_ShaderTexture()->CopyData(0, tCB_ShaderTexture);

	m_pShaderTargetCom->Begin_Shader(pTargetMiniMap->Get_TargetTextureDescriptorHeap(),
									 0,
									 0, 
									 Engine::MATRIXID::ORTHO);
	m_pBufferCom->Begin_Buffer();
	m_pBufferCom->Render_Buffer();
}

Engine::CGameObject* CMiniMapCanvas::Create(ID3D12Device* pGraphicDevice, 
											ID3D12GraphicsCommandList* pCommandList,
											wstring wstrObjectTag, 
											wstring wstrDataFilePath,
											const _vec3& vPos, 
											const _vec3& vScale,
											const _bool& bIsSpriteAnimation,
											const _float& fFrameSpeed,
											const _vec3& vRectOffset,
											const _vec3& vRectScale, 
											const _long& iUIDepth)
{
	CMiniMapCanvas* pInstance = new CMiniMapCanvas(pGraphicDevice, pCommandList);

	if (FAILED(pInstance->Ready_GameObject(wstrObjectTag,
										   wstrDataFilePath,
										   vPos,
										   vScale,
										   bIsSpriteAnimation,
										   fFrameSpeed,
										   vRectOffset,
										   vRectScale,
										   iUIDepth)))
		Engine::Safe_Release(pInstance);

	return pInstance;
}

void CMiniMapCanvas::Free()
{
	CGameUIRoot::Free();
	Engine::Safe_Release(m_pShaderTargetCom);
}
