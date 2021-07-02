#include "stdafx.h"
#include "StoreTab.h"
#include "DirectInput.h"
#include "Font.h"
#include "StoreMgr.h"

CStoreTab::CStoreTab(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList)
	: CGameUIChild(pGraphicDevice, pCommandList)
{
}

HRESULT CStoreTab::Ready_GameObject(wstring wstrRootObjectTag, 
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

	m_bIsActive = false;

	m_pFont->Set_Color(D2D1::ColorF::Cornsilk);
	m_pFont->Set_Text(L"");
	m_vFontOffset = _vec2(-22.0f, -16.0f);

	return S_OK;
}

HRESULT CStoreTab::LateInit_GameObject()
{
	Engine::FAILED_CHECK_RETURN(CGameUIChild::LateInit_GameObject(), E_FAIL);

	return S_OK;
}

_int CStoreTab::Update_GameObject(const _float& fTimeDelta)
{
	Engine::FAILED_CHECK_RETURN(Engine::CGameObject::LateInit_GameObject(), E_FAIL);
	
	if (m_bIsDead)
		return DEAD_OBJ;
	if (!m_bIsActive)
		return NO_EVENT;

	CGameUIChild::Update_GameObject(fTimeDelta);

	return NO_EVENT;
}

_int CStoreTab::LateUpdate_GameObject(const _float& fTimeDelta)
{
	if (!m_bIsActive)
		return NO_EVENT;

	CGameUIChild::LateUpdate_GameObject(fTimeDelta);

	if (CMouseCursorMgr::Get_Instance()->Check_CursorInRect(m_tRect) &&
		Engine::MOUSE_KEYUP(Engine::MOUSEBUTTON::DIM_LB) && 
		m_bIsKeyPressing &&
		!m_bIsSelected)
	{
		m_bIsSelected = true;
		m_uiTexIdx = 1;

		CStoreMgr::Get_Instance()->Set_StoreItemType(m_chItemType);
		map<wstring, CStoreTab*> mapStoreTab = CStoreMgr::Get_Instance()->Get_StoreTabList();
		for (auto& pair : mapStoreTab)
		{
			if (m_wstrObjectTag == pair.first)
				pair.second->Set_IsSelected(true);
			else
				pair.second->Set_IsSelected(false);
		}
	}

	m_bIsKeyPressing = false;

	// Check Mouse Collision.
	if (CMouseCursorMgr::Get_Instance()->Get_IsActiveMouse() &&
		CMouseCursorMgr::Get_Instance()->Check_CursorInRect(m_tRect))
	{
		if (Engine::MOUSE_PRESSING(Engine::MOUSEBUTTON::DIM_LB) && !m_bIsSelected)
		{
			m_uiTexIdx       = 1;
			m_bIsKeyPressing = true;
		}
		else
		{
			m_bIsKeyPressing = false;
		}
	}
	else
	{
		if (!m_bIsSelected)
			m_uiTexIdx = 0;
		else
			m_uiTexIdx = 1;
	}

	SetUp_FontPosition(fTimeDelta);

	return NO_EVENT;
}

void CStoreTab::Render_GameObject(const _float& fTimeDelta)
{
	CGameUIChild::Render_GameObject(fTimeDelta);
}

void CStoreTab::SetUp_FontPosition(const _float& fTimeDelta)
{
	if (nullptr != m_pFont && !g_bIsStageChange)
	{
		_vec3 vPos = _vec3(m_pTransColor->m_matWorld._41, m_pTransColor->m_matWorld._42, m_pTransColor->m_matWorld._43).Convert_DescartesTo2DWindow(WINCX, WINCY);
		vPos.x += m_vFontOffset.x;
		vPos.y += m_vFontOffset.y;

		m_pFont->Set_Pos(_vec2(vPos.x, vPos.y));
		m_pFont->Update_GameObject(fTimeDelta);
	}
}

Engine::CGameObject* CStoreTab::Create(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList,
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
	CStoreTab* pInstance = new CStoreTab(pGraphicDevice, pCommandList);

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

void CStoreTab::Free()
{
	CGameUIChild::Free();
}

