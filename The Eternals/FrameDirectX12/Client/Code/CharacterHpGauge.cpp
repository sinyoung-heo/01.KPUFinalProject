#include "stdafx.h"
#include "CharacterHpGauge.h"

CCharacterHpGauge::CCharacterHpGauge(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList)
	: CGameUIChild(pGraphicDevice, pCommandList)
{
}

HRESULT CCharacterHpGauge::Ready_GameObject(wstring wstrRootObjectTag, 
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

	// Set Shader PipelineState
	Engine::FAILED_CHECK_RETURN(m_pShaderCom->Set_PipelineStatePass(6), E_FAIL);

	return S_OK;
}

HRESULT CCharacterHpGauge::LateInit_GameObject()
{
	Engine::FAILED_CHECK_RETURN(CGameUIChild::LateInit_GameObject(), E_FAIL);

	return S_OK;
}

_int CCharacterHpGauge::Update_GameObject(const _float& fTimeDelta)
{
	Engine::FAILED_CHECK_RETURN(Engine::CGameObject::LateInit_GameObject(), E_FAIL);
	CGameUIChild::Update_GameObject(fTimeDelta);

	return NO_EVENT;
}

_int CCharacterHpGauge::LateUpdate_GameObject(const _float& fTimeDelta)
{
	CGameUIChild::LateUpdate_GameObject(fTimeDelta);

	return NO_EVENT;
}

void CCharacterHpGauge::Render_GameObject(const _float& fTimeDelta)
{
	if (nullptr != m_pTexDescriptorHeap)
	{
		Set_ConstantTable();

		// TextureBuffer
		m_pShaderCom->Begin_Shader(m_pTexDescriptorHeap,
								   0,
								   m_uiTexIdx,
								   Engine::MATRIXID::ORTHO);
		m_pBufferCom->Begin_Buffer();
		m_pBufferCom->Render_Buffer();

		// ColorBuffer
		if (m_pRenderer->Get_RenderOnOff(L"Collider"))
		{
			m_pShaderColor->Begin_Shader(nullptr, 0, Engine::MATRIXID::ORTHO);
			m_pBufferColor->Begin_Buffer();
			m_pBufferColor->Render_Buffer();
		}
	}
}

void CCharacterHpGauge::Set_ConstantTable()
{
	/*__________________________________________________________________________________________________________
	[ Set ConstantBuffer Data ]
	____________________________________________________________________________________________________________*/
	Engine::CB_SHADER_TEXTURE tCB_ShaderTexture;
	ZeroMemory(&tCB_ShaderTexture, sizeof(Engine::CB_SHADER_TEXTURE));
	tCB_ShaderTexture.matWorld	= Engine::CShader::Compute_MatrixTranspose(m_pTransCom->m_matWorld);
	tCB_ShaderTexture.fFrameCnt	= m_tFrame.fFrameCnt;
	tCB_ShaderTexture.fSceneCnt	= m_tFrame.fSceneCnt;
	if (m_bIsSpriteAnimation)
	{
		tCB_ShaderTexture.fCurFrame = (_float)(_int)m_tFrame.fCurFrame;
		tCB_ShaderTexture.fCurScene	= (_float)(_int)m_tFrame.fCurScene;
	}
	else
	{
		tCB_ShaderTexture.fCurFrame = m_tFrame.fCurFrame;
		tCB_ShaderTexture.fCurScene = m_tFrame.fCurScene;
	}

	tCB_ShaderTexture.fGauge = m_fPercent;

	m_pShaderCom->Get_UploadBuffer_ShaderTexture()->CopyData(0, tCB_ShaderTexture);


	Engine::CB_SHADER_COLOR tCB_ShaderColor;
	ZeroMemory(&tCB_ShaderColor, sizeof(Engine::CB_SHADER_COLOR));
	tCB_ShaderColor.matWorld	= Engine::CShader::Compute_MatrixTranspose(m_pTransColor->m_matWorld);
	tCB_ShaderColor.vColor		= _rgba(0.0f, 1.0f, 0.0f, 0.25f);

	m_pShaderColor->Get_UploadBuffer_ShaderColor()->CopyData(0, tCB_ShaderColor);
}

Engine::CGameObject* CCharacterHpGauge::Create(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList,
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
	CCharacterHpGauge* pInstance = new CCharacterHpGauge(pGraphicDevice, pCommandList);

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

void CCharacterHpGauge::Free()
{
	CGameUIChild::Free();
}
