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
										 _vec3(-12.0f, -12.0f, 0.0f),
										 _vec3(16.0f, 16.0f, 1.0f),
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

_bool CMouseCursorMgr::Check_CursorInRect(RECT& rcSrc)
{
	POINT ptMouse = Get_CursorPoint();

	if (Check_IntersectRect(rcSrc, static_cast<CGameUIRoot*>(m_pMouseCursor)->Get_Rect()))
		return true;

	return false;
}

_bool CMouseCursorMgr::Check_IntersectRect(RECT& rcSrc, RECT& rcDst)
{
	_bool bIsHorizon  = false;	//수평충돌
	_bool bIsVertical = false;	//수직충돌

	if (rcSrc.left  < rcDst.right &&	//수평충돌
		rcSrc.right > rcDst.left)
	{
		bIsHorizon = true;
	}
	else
		return false;

	if (rcSrc.bottom > rcDst.top &&		// 수직 충돌
		rcSrc.top < rcDst.bottom)
	{
		bIsVertical = true;
	}


	if (bIsHorizon && bIsVertical)
		return true;

	return false;
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
