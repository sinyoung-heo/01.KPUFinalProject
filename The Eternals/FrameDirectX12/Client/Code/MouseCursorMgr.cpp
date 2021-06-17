#include "stdafx.h"
#include "MouseCursorMgr.h"
#include "GraphicDevice.h"
#include "DirectInput.h"
#include "GameUIRoot.h"
#include "GameObject.h"

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
	// DebugCaemra ON�̸� Actavie false.
	if (g_bIsOnDebugCaemra)
		m_bIsActiveMouse = false;

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
			//if (DIERR_INPUTLOST   != Engine::CDirectInput::Get_Instance()->Get_DeviceMouseState() &&
			//	DIERR_NOTACQUIRED != Engine::CDirectInput::Get_Instance()->Get_DeviceMouseState() &&
			//	!g_bIsOnDebugCaemra)
			//{
			//	SetCursorPos(WINCX / 2, WINCY / 2);
			//}

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

_bool CMouseCursorMgr::Check_PickingBoundingBox(Engine::CGameObject** ppPickingObject, Engine::OBJLIST* pOBJLIST)
{
	POINT ptMouse = Get_CursorPoint();

	D3D12_VIEWPORT ViewPort = Engine::CGraphicDevice::Get_Instance()->Get_Viewport();

	// Window��ǥ -> ���� ��ǥ��� ��ȯ.
	_vec3 vMousePos = _vec3(0.f);
	vMousePos.x		= ptMouse.x / (ViewPort.Width * 0.5f) - 1.f;
	vMousePos.y		= ptMouse.y / -(ViewPort.Height * 0.5f) + 1.f;
	vMousePos.z		= 0.f;

	// ���� ��ǥ�� -> ī�޶� ��ǥ��� ��ȯ.
	_matrix matProj = *(Engine::CGraphicDevice::Get_Instance()->Get_Transform(Engine::MATRIXID::PROJECTION));
	matProj = XMMatrixInverse(nullptr, matProj);
	vMousePos.TransformCoord(vMousePos, matProj);

	// ī�޶� ��ǥ�� -> ���� ��ǥ��� ��ȯ.
	_vec3 vRayDir, vRayPos;
	vRayPos = _vec3(0.f, 0.f, 0.f);
	vRayDir = vMousePos - vRayPos;

	_matrix matView = *(Engine::CGraphicDevice::Get_Instance()->Get_Transform(Engine::MATRIXID::VIEW));
	matView = XMMatrixInverse(nullptr, matView);
	vRayDir.TransformNormal(vRayDir, matView);
	vRayPos.TransformCoord(vRayPos, matView);


	_float fDist = 0.0f;
	auto iter_begin = pOBJLIST->begin();
	auto iter_end	= pOBJLIST->end();
	for (; iter_begin != iter_end; ++iter_begin)
	{
		vRayDir.Normalize();

		// �浹�ߴٸ�, BoundingBox�� ������ Red�� ����.
		if ((*iter_begin)->Get_BoundingBox()->Get_BoundingInfo().Intersects(vRayPos.Get_XMVECTOR(),
																			vRayDir.Get_XMVECTOR(), 
																			fDist))
		{
			*ppPickingObject = (*iter_begin);
			return true;
		}
	}

	return false;
}

_bool CMouseCursorMgr::Check_PickingBoundingBox(Engine::CGameObject* pPickingObject)
{
	POINT ptMouse = Get_CursorPoint();

	D3D12_VIEWPORT ViewPort = Engine::CGraphicDevice::Get_Instance()->Get_Viewport();

	// Window��ǥ -> ���� ��ǥ��� ��ȯ.
	_vec3 vMousePos = _vec3(0.f);
	vMousePos.x		= ptMouse.x / (ViewPort.Width * 0.5f) - 1.f;
	vMousePos.y		= ptMouse.y / -(ViewPort.Height * 0.5f) + 1.f;
	vMousePos.z		= 0.f;

	// ���� ��ǥ�� -> ī�޶� ��ǥ��� ��ȯ.
	_matrix matProj = *(Engine::CGraphicDevice::Get_Instance()->Get_Transform(Engine::MATRIXID::PROJECTION));
	matProj = XMMatrixInverse(nullptr, matProj);
	vMousePos.TransformCoord(vMousePos, matProj);

	// ī�޶� ��ǥ�� -> ���� ��ǥ��� ��ȯ.
	_vec3 vRayDir, vRayPos;
	vRayPos = _vec3(0.f, 0.f, 0.f);
	vRayDir = vMousePos - vRayPos;

	_matrix matView = *(Engine::CGraphicDevice::Get_Instance()->Get_Transform(Engine::MATRIXID::VIEW));
	matView = XMMatrixInverse(nullptr, matView);
	vRayDir.TransformNormal(vRayDir, matView);
	vRayPos.TransformCoord(vRayPos, matView);


	_float fDist = 0.0f;
	vRayDir.Normalize();

	// �浹�ߴٸ�, BoundingBox�� ������ Red�� ����.
	if (pPickingObject->Get_BoundingBox()->Get_BoundingInfo().Intersects(vRayPos.Get_XMVECTOR(),
																		vRayDir.Get_XMVECTOR(), 
																		fDist))
	{
		return true;
	}

	return false;
}

_bool CMouseCursorMgr::Check_IntersectRect(RECT& rcSrc, RECT& rcDst)
{
	_bool bIsHorizon  = false;	//�����浹
	_bool bIsVertical = false;	//�����浹

	if (rcSrc.left  < rcDst.right &&	//�����浹
		rcSrc.right > rcDst.left)
	{
		bIsHorizon = true;
	}
	else
		return false;

	if (rcSrc.bottom > rcDst.top &&		// ���� �浹
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
