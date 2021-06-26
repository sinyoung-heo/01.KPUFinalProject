#include "stdafx.h"
#include "LoginIDButton.h"
#include "DescriptorHeapMgr.h"
#include "Font.h"
#include "DirectInput.h"

CLoginIDButton::CLoginIDButton(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList)
	: CGameUIChild(pGraphicDevice, pCommandList)
{
}

void CLoginIDButton::Set_TexDescriptorHeap(wstring wstrTag)
{
	m_pTexDescriptorHeap = Engine::CDescriptorHeapMgr::Get_Instance()->Find_DescriptorHeap(m_wstrTextureTag);
}

HRESULT CLoginIDButton::Ready_GameObject(wstring wstrRootObjectTag, 
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
															   iUIDepth,
															   true, L"Font_NetmarbleBold48"), E_FAIL);

	m_matView = INIT_MATRIX;
	m_matProj = XMMatrixOrthographicLH(WINCX, WINCY, 0.0f, 1.0f);

	// Font Text
	m_pFont->Set_Color(D2D1::ColorF::Cornsilk);
	m_pFont->Set_Text(L"GAME START");

	return S_OK;
}

HRESULT CLoginIDButton::LateInit_GameObject()
{
	Engine::FAILED_CHECK_RETURN(CGameUIChild::LateInit_GameObject(), E_FAIL);

	return S_OK;
}

_int CLoginIDButton::Update_GameObject(const _float& fTimeDelta)
{
	Engine::FAILED_CHECK_RETURN(Engine::CGameObject::LateInit_GameObject(), E_FAIL);
	
	if (m_bIsDead)
		return DEAD_OBJ;

	if (g_bIsLoadingFinish)
		CGameUIChild::Update_GameObject(fTimeDelta);

	return NO_EVENT;
}

_int CLoginIDButton::LateUpdate_GameObject(const _float& fTimeDelta)
{
	if (g_bIsLoadingFinish)
	{
		CGameUIChild::LateUpdate_GameObject(fTimeDelta);

		if (CMouseCursorMgr::Get_Instance()->Check_CursorInRect(m_tRect) &&
			Engine::MOUSE_KEYUP(Engine::MOUSEBUTTON::DIM_LB) &&
			m_bIsKeyPressing &&
			!g_bIsGameStart)
		{
			g_bIsGameStart = true;
		}

		m_bIsKeyPressing = false;

		// Check Mouse Collision.
		if (CMouseCursorMgr::Get_Instance()->Get_IsActiveMouse() &&
			CMouseCursorMgr::Get_Instance()->Check_CursorInRect(m_tRect))
		{
			if (Engine::MOUSE_PRESSING(Engine::MOUSEBUTTON::DIM_LB))
			{
				m_uiTexIdx = 1;
				m_bIsKeyPressing = true;
			}
			else
			{
				m_uiTexIdx = 0;
				m_bIsKeyPressing = false;
			}
		}
		else
		{
			m_uiTexIdx = 0;
		}

		if (nullptr != m_pFont && m_bIsActive && !g_bIsGameStart)
		{
			_vec3 vPos = _vec3(m_pTransColor->m_matWorld._41, m_pTransColor->m_matWorld._42, m_pTransColor->m_matWorld._43).Convert_DescartesTo2DWindow(WINCX, WINCY);
			vPos.x -= 120.0f;
			vPos.y -= 35.0f;
			m_pFont->Set_Pos(_vec2(vPos.x, vPos.y));
			m_pFont->Update_GameObject(fTimeDelta);
		}
	}

	return NO_EVENT;
}

void CLoginIDButton::Render_GameObject(const _float& fTimeDelta)
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

void CLoginIDButton::Set_ConstantTable()
{
	/*__________________________________________________________________________________________________________
	[ Set ConstantBuffer Data ]
	____________________________________________________________________________________________________________*/
	Engine::CB_CAMERA_MATRIX tCB_CameraMatrix;
	ZeroMemory(&tCB_CameraMatrix, sizeof(Engine::CB_CAMERA_MATRIX));
	tCB_CameraMatrix.matView = Engine::CShader::Compute_MatrixTranspose(m_matView);
	tCB_CameraMatrix.matProj = Engine::CShader::Compute_MatrixTranspose(m_matProj);

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
	m_pShaderCom->Get_UploadBuffer_CameraOrthoMatrix()->CopyData(0, tCB_CameraMatrix);
	m_pShaderCom->Get_UploadBuffer_ShaderTexture()->CopyData(0, tCB_ShaderTexture);


	Engine::CB_SHADER_COLOR tCB_ShaderColor;
	ZeroMemory(&tCB_ShaderColor, sizeof(Engine::CB_SHADER_COLOR));
	tCB_ShaderColor.matWorld	= Engine::CShader::Compute_MatrixTranspose(m_pTransColor->m_matWorld);
	tCB_ShaderColor.vColor		= _rgba(0.0f, 1.0f, 0.0f, 0.25f);

	m_pShaderColor->Get_UploadBuffer_CameraOrthoMatrix()->CopyData(0, tCB_CameraMatrix);
	m_pShaderColor->Get_UploadBuffer_ShaderColor()->CopyData(0, tCB_ShaderColor);
}

Engine::CGameObject* CLoginIDButton::Create(ID3D12Device* pGraphicDevice, 
										  ID3D12GraphicsCommandList* pCommandList,
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
	CLoginIDButton* pInstance = new CLoginIDButton(pGraphicDevice, pCommandList);

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

void CLoginIDButton::Free()
{
	CGameUIChild::Free();
}