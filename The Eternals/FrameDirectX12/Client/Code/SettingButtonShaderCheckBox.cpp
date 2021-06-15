#include "stdafx.h"
#include "SettingButtonShaderCheckBox.h"
#include "DirectInput.h"
#include "ShaderMgr.h"
#include "Font.h"
#include "Renderer.h"


CSettingButtonShaderCheckBox::CSettingButtonShaderCheckBox(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList)
	: CGameUIChild(pGraphicDevice, pCommandList)
	, m_pShaderMgr(Engine::CShaderMgr::Get_Instance())
{
}

HRESULT CSettingButtonShaderCheckBox::Ready_GameObject(wstring wstrRootObjectTag, 
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
															   true, L"Font_BinggraeMelona24"), E_FAIL);

	SetUp_ShaderTag();

	m_bIsActive = true;

	return S_OK;
}

HRESULT CSettingButtonShaderCheckBox::LateInit_GameObject()
{
	Engine::FAILED_CHECK_RETURN(CGameUIChild::LateInit_GameObject(), E_FAIL);

	return S_OK;
}

_int CSettingButtonShaderCheckBox::Update_GameObject(const _float& fTimeDelta)
{
	Engine::FAILED_CHECK_RETURN(Engine::CGameObject::LateInit_GameObject(), E_FAIL);
	
	if (m_bIsDead)
		return DEAD_OBJ;
	if (!m_bIsActive)
		return NO_EVENT;

	CGameUIChild::Update_GameObject(fTimeDelta);

	return NO_EVENT;
}

_int CSettingButtonShaderCheckBox::LateUpdate_GameObject(const _float& fTimeDelta)
{
	if (!m_bIsActive)
		return NO_EVENT;

	CGameUIChild::LateUpdate_GameObject(fTimeDelta);

	// Check Mouse Collision.
	if (CMouseCursorMgr::Get_Instance()->Get_IsActiveMouse() && 
		CMouseCursorMgr::Get_Instance()->Check_CursorInRect(m_tRect) &&
		Engine::MOUSE_KEYDOWN(Engine::MOUSEBUTTON::DIM_LB))
	{
		_bool bIsOnShader = m_pShaderMgr->Find_IsOnShader(m_wstrShaderTag);
		bIsOnShader = !bIsOnShader;
		m_pShaderMgr->Set_IsOnShader(m_wstrShaderTag, bIsOnShader);

		if (L"Shadow" == m_wstrShaderTag)
			m_pRenderer->Set_IsRenderShadow(bIsOnShader);
	}

	SetUp_CheckBox();
	SetUp_FontPosition(fTimeDelta);

	return NO_EVENT;
}

void CSettingButtonShaderCheckBox::Render_GameObject(const _float& fTimeDelta)
{
	CGameUIChild::Render_GameObject(fTimeDelta);
}

void CSettingButtonShaderCheckBox::SetUp_CheckBox()
{
	if (m_pShaderMgr->Find_IsOnShader(m_wstrShaderTag))
		m_uiTexIdx = 2;
	else
		m_uiTexIdx = 1;
}

void CSettingButtonShaderCheckBox::SetUp_ShaderTag()
{
	if (L"SystemSettingCheckBox_Specular" == m_wstrObjectTag)
		m_wstrShaderTag = L"Specular";

	else if (L"SystemSettingCheckBox_Shade" == m_wstrObjectTag)
		m_wstrShaderTag = L"Shade";

	else if (L"SystemSettingCheckBox_SSAO" == m_wstrObjectTag)
		m_wstrShaderTag = L"SSAO";

	else if (L"SystemSettingCheckBox_HDRTone" == m_wstrObjectTag)
		m_wstrShaderTag = L"HDR Tone";

	else if (L"SystemSettingCheckBox_DOF" == m_wstrObjectTag)
		m_wstrShaderTag = L"DOF";

	else if (L"SystemSettingCheckBox_Shadow" == m_wstrObjectTag)
		m_wstrShaderTag = L"Shadow";

	m_pFont->Set_Color(D2D1::ColorF::Goldenrod);
	m_pFont->Set_Text(m_wstrShaderTag);

	if (L"SystemSettingCheckBox_Shade" == m_wstrObjectTag)
	{
		m_wstrShaderTag = L"Shade";
		m_pFont->Set_Text(L"[ Shader Setting ]\n\nShade");
	}

}

void CSettingButtonShaderCheckBox::SetUp_FontPosition(const _float& fTimeDelta)
{
	if (nullptr != m_pFont)
	{
		_vec3 vPos = _vec3(m_pTransColor->m_matWorld._41, m_pTransColor->m_matWorld._42, m_pTransColor->m_matWorld._43).Convert_DescartesTo2DWindow(WINCX, WINCY);
		if (L"Shade" == m_wstrShaderTag)
		{
			vPos.x -= 140.0f;
			vPos.y -= 75.0f;
		}
		else
		{
			vPos.x -= 140.0f;
			vPos.y -= 15.0f;
		}
		m_pFont->Set_Pos(_vec2(vPos.x, vPos.y));
		m_pFont->Update_GameObject(fTimeDelta);
	}
}

Engine::CGameObject* CSettingButtonShaderCheckBox::Create(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList,
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
	CSettingButtonShaderCheckBox* pInstance = new CSettingButtonShaderCheckBox(pGraphicDevice, pCommandList);

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

void CSettingButtonShaderCheckBox::Free()
{
	CGameUIChild::Free();
}