#include "stdafx.h"
#include "CoolTime.h"
#include "DescriptorHeapMgr.h"

CCoolTime::CCoolTime(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList)
	: CGameUIChild(pGraphicDevice, pCommandList)
{
}

HRESULT CCoolTime::Ready_GameObject(wstring wstrRootObjectTag, 
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
	Engine::FAILED_CHECK_RETURN(CGameUIChild::Ready_GameObject(wstrRootObjectTag, 
															   wstrObjectTag,
															   wstrDataFilePath,
															   vPos,
															   vScale,
															   bIsSpriteAnimation,
															   fFrameSpeed,
															   vRectOffset,
															   vRectScale,
															   iUIDepth), E_FAIL);

	m_bIsActive = true;

	m_pShaderCom->Set_PipelineStatePass(12);

	return S_OK;
}

HRESULT CCoolTime::LateInit_GameObject()
{
	Engine::FAILED_CHECK_RETURN(CGameUIChild::LateInit_GameObject(), E_FAIL);

	return S_OK;
}

_int CCoolTime::Update_GameObject(const _float& fTimeDelta)
{
	Engine::FAILED_CHECK_RETURN(Engine::CGameObject::LateInit_GameObject(), E_FAIL);
	
	if (g_bIsCinemaStart)
		return NO_EVENT;
	if (m_bIsDead)
		return DEAD_OBJ;
	if (!m_bIsActive)
		return NO_EVENT;

	CGameUIChild::Update_GameObject(fTimeDelta);

	return NO_EVENT;
}

_int CCoolTime::LateUpdate_GameObject(const _float& fTimeDelta)
{
	if (g_bIsCinemaStart)
		return NO_EVENT;
	if (!m_bIsActive)
		return NO_EVENT;

	CGameUIChild::LateUpdate_GameObject(fTimeDelta);

	return NO_EVENT;
}

void CCoolTime::Render_GameObject(const _float& fTimeDelta)
{
	/*__________________________________________________________________________________________________________
	[ Set ConstantBuffer Data ]
	____________________________________________________________________________________________________________*/
	Engine::CB_SHADER_TEXTURE tCB_ShaderTexture;
	ZeroMemory(&tCB_ShaderTexture, sizeof(Engine::CB_SHADER_TEXTURE));
	tCB_ShaderTexture.matWorld	= Engine::CShader::Compute_MatrixTranspose(m_pTransCom->m_matWorld);
	tCB_ShaderTexture.fFrameCnt	= m_tFrame.fFrameCnt;
	tCB_ShaderTexture.fSceneCnt	= m_tFrame.fSceneCnt;
	tCB_ShaderTexture.fCurFrame = m_tFrame.fCurFrame;
	tCB_ShaderTexture.fCurScene = m_tFrame.fCurScene;
	tCB_ShaderTexture.fAlpha    = 0.75f;
	m_pShaderCom->Get_UploadBuffer_ShaderTexture()->CopyData(0, tCB_ShaderTexture);

	m_pShaderCom->Begin_Shader(Engine::CDescriptorHeapMgr::Get_Instance()->Find_DescriptorHeap(L"BackBuffer"),
							   0,
							   1,
							   Engine::MATRIXID::ORTHO);
	m_pBufferCom->Begin_Buffer();
	m_pBufferCom->Render_Buffer();
}

Engine::CGameObject* CCoolTime::Create(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList,
												   wstring wstrRootObjectTag,
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
	CCoolTime* pInstance = new CCoolTime(pGraphicDevice, pCommandList);

	if (FAILED(pInstance->Ready_GameObject(wstrRootObjectTag,
										   wstrObjectTag,
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

void CCoolTime::Free()
{
	CGameUIChild::Free();
}

