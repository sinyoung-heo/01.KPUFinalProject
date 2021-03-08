#include "stdafx.h"
#include "MouseCursorMgr.h"
#include "GraphicDevice.h"
#include "ObjectMgr.h"
#include "DirectInput.h"
#include "GameUIRoot.h"

IMPLEMENT_SINGLETON(CMouseCursorMgr)

CMouseCursorMgr::CMouseCursorMgr()
{

}

HRESULT CMouseCursorMgr::Ready_MouseCursorMgr()
{
	m_pMouseCursor = CGameUIRoot::Create(Engine::CGraphicDevice::Get_Instance()->Get_GraphicDevice(),
										 Engine::CGraphicDevice::Get_Instance()->Get_CommandList(Engine::CMD_MAIN),
										 L"Cursor",
										 L"../../Bin/ToolData/TexUVCursor.texuv",
										 _vec3(0.0f, 0.0f, 0.0f),
										 _vec3(32.0f, 32.0f, 1.0f),
										 false,
										 0.0f,
										 _vec3(0.0f),
										 _vec3(32.0f, 32.0f, 1.0f),
										 0);
	Engine::CObjectMgr::Get_Instance()->Add_GameObject(L"Layer_UI", L"MouseCursor", m_pMouseCursor);

	return S_OK;
}

void CMouseCursorMgr::Update_MouseCursorMgr(const _float& fTimeDelta)
{
	if (nullptr != m_pMouseCursor)
	{
		if (m_bIsActiveMouse)
		{
			m_bIsResetMouse = false;
			static_cast<CGameUIRoot*>(m_pMouseCursor)->Set_IsRender(true);

			POINT ptMouse{};
			GetCursorPos(&ptMouse);
			ScreenToClient(g_hWnd, &ptMouse);
			m_pMouseCursor->Get_Transform()->m_vPos = _vec3((_float)ptMouse.x, (_float)ptMouse.y, 0.0f);
		}
		else
		{
			SetCursorPos(WINCX / 2, WINCY / 2);

			if (!m_bIsResetMouse)
			{
				m_bIsResetMouse = true;

				static_cast<CGameUIRoot*>(m_pMouseCursor)->Set_IsRender(false);
				POINT ptMouse{};
				GetCursorPos(&ptMouse);
				ScreenToClient(g_hWnd, &ptMouse);
				m_pMouseCursor->Get_Transform()->m_vPos = _vec3((_float)ptMouse.x, (_float)ptMouse.y, 0.0f);
			}
		}

	}
}

POINT CMouseCursorMgr::Get_CursorPoint()
{
	POINT ptMouse{};
	GetCursorPos(&ptMouse);
	ScreenToClient(g_hWnd, &ptMouse);

	return ptMouse;
}

void CMouseCursorMgr::Free()
{
}
