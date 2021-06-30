#include "stdafx.h"
#include "InventoryCanvas.h"
#include "DirectInput.h"
#include "Font.h"

CInventoryCanvas::CInventoryCanvas(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList)
	: CGameUIRoot(pGraphicDevice, pCommandList)
{
}

HRESULT CInventoryCanvas::Ready_GameObject(wstring wstrObjectTag, 
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
															  iUIDepth,
															  true, L"Font_BinggraeMelona24"), E_FAIL);

	m_bIsActive = false;

	m_wstrFontText = L"%d  G";
	m_pFont->Set_Color(D2D1::ColorF::Goldenrod);
	m_pFont->Set_Text(L"");

	return S_OK;
}

HRESULT CInventoryCanvas::LateInit_GameObject()
{
	Engine::FAILED_CHECK_RETURN(CGameUIRoot::LateInit_GameObject(), E_FAIL);

	return S_OK;
}

_int CInventoryCanvas::Update_GameObject(const _float& fTimeDelta)
{
	Engine::FAILED_CHECK_RETURN(Engine::CGameObject::LateInit_GameObject(), E_FAIL);

	if (m_bIsDead)
		return DEAD_OBJ;
	if (!m_bIsActive)
		return NO_EVENT;

	KeyInput_MouseMove(fTimeDelta);

	CGameUIRoot::Update_GameObject(fTimeDelta);

	return NO_EVENT;
}

_int CInventoryCanvas::LateUpdate_GameObject(const _float& fTimeDelta)
{
	if (!m_bIsActive)
		return NO_EVENT;

	CGameUIRoot::LateUpdate_GameObject(fTimeDelta);

	if (nullptr != m_pFont)
	{
		_vec3 vPos = _vec3(m_pTransColor->m_matWorld._41, m_pTransColor->m_matWorld._42, m_pTransColor->m_matWorld._43).Convert_DescartesTo2DWindow(WINCX, WINCY);
		vPos.x += -185.0f;
		vPos.y += 615.0f;

		Engine::CGameObject* pThisPlayer = m_pObjectMgr->Get_GameObject(L"Layer_GameObject", L"ThisPlayer");
		if (nullptr != pThisPlayer)
		{
			_uint uiMoney = pThisPlayer->Get_Info()->m_iMoney;

			_tchar	szText[MIN_STR] = L"";
			wsprintf(szText, m_wstrFontText.c_str(), uiMoney);

			m_pFont->Set_Text(szText);
			m_pFont->Set_Pos(_vec2(vPos.x, vPos.y));
			m_pFont->Update_GameObject(fTimeDelta);
		}
	}

	return NO_EVENT;
}

void CInventoryCanvas::Render_GameObject(const _float& fTimeDelta)
{
	CGameUIRoot::Render_GameObject(fTimeDelta);
}

void CInventoryCanvas::KeyInput_MouseMove(const _float& fTimeDelta)
{
	// Check Mouse Collision.
	if (CMouseCursorMgr::Get_Instance()->Get_IsActiveMouse() &&
		CMouseCursorMgr::Get_Instance()->Check_CursorInRect(m_tRect) &&
		Engine::MOUSE_PRESSING(Engine::MOUSEBUTTON::DIM_LB)/* &&
		!g_bIsOpenShop*/)
	{
		/*__________________________________________________________________________________________________________
		[ 마우스 좌, 우 이동 ]
		____________________________________________________________________________________________________________*/
		_long dwMouseMoveX = 0;
		if (dwMouseMoveX = Engine::GetDIMouseMove(Engine::MOUSEMOVESTATE::DIMS_X))
		{
			m_pTransCom->m_vPos.x += static_cast<_float>(dwMouseMoveX);
		}

		/*__________________________________________________________________________________________________________
		[ 마우스 상, 하 이동 ]
		___________________________________________________________________________________________________________*/
		_long dwMouseMoveY = 0;
		if (dwMouseMoveY = Engine::GetDIMouseMove(Engine::MOUSEMOVESTATE::DIMS_Y))
		{
			m_pTransCom->m_vPos.y += static_cast<_float>(dwMouseMoveY);
		}
	}
}

Engine::CGameObject* CInventoryCanvas::Create(ID3D12Device* pGraphicDevice, 
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
	CInventoryCanvas* pInstance = new CInventoryCanvas(pGraphicDevice, pCommandList);

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

void CInventoryCanvas::Free()
{
	CGameUIRoot::Free();
}
