#include "stdafx.h"
#include "StoreBuySellListBack.h"
#include "Font.h"
#include "DirectInput.h"
#include "StoreMgr.h"

CStoreBuySellListBack::CStoreBuySellListBack(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList)
	: CGameUIChild(pGraphicDevice, pCommandList)
{
}

HRESULT CStoreBuySellListBack::Ready_GameObject(wstring wstrRootObjectTag, 
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

	m_pFontPrice = static_cast<Engine::CFont*>(m_pObjectMgr->Clone_GameObjectPrototype(L"Font_BinggraeMelona24"));
	Engine::NULL_CHECK_RETURN(m_pFontPrice, E_FAIL);
	Engine::FAILED_CHECK_RETURN(m_pFontPrice->Ready_GameObject(L"", _vec2(0.0f, 0.f), D2D1::ColorF::White), E_FAIL);

	m_bIsActive = false;

	m_pFont->Set_Color(D2D1::ColorF::Cornsilk);

	if (L"UIStoreBuyListBack" == wstrObjectTag)
	{
		m_pFont->Set_Text(L"구매 목록");

		m_pFontPrice->Set_Color(D2D1::ColorF::Crimson);
		m_wstrFontText = L"- %d";
	}
	else
	{
		m_pFont->Set_Text(L"판매 목록");

		m_pFontPrice->Set_Color(D2D1::ColorF::SteelBlue);
		m_wstrFontText = L"+ %d";
	}



	return S_OK;
}

HRESULT CStoreBuySellListBack::LateInit_GameObject()
{
	Engine::FAILED_CHECK_RETURN(CGameUIChild::LateInit_GameObject(), E_FAIL);

	return S_OK;
}

_int CStoreBuySellListBack::Update_GameObject(const _float& fTimeDelta)
{
	Engine::FAILED_CHECK_RETURN(Engine::CGameObject::LateInit_GameObject(), E_FAIL);
	
	if (m_bIsDead)
		return DEAD_OBJ;
	if (!m_bIsActive)
		return NO_EVENT;

	CGameUIChild::Update_GameObject(fTimeDelta);

	return NO_EVENT;
}

_int CStoreBuySellListBack::LateUpdate_GameObject(const _float& fTimeDelta)
{
	if (!m_bIsActive)
		return NO_EVENT;

	CGameUIChild::LateUpdate_GameObject(fTimeDelta);
	SetUp_FontPosition(fTimeDelta);

	return NO_EVENT;
}

void CStoreBuySellListBack::Render_GameObject(const _float& fTimeDelta)
{
	CGameUIChild::Render_GameObject(fTimeDelta);
}

void CStoreBuySellListBack::SetUp_FontPosition(const _float& fTimeDelta)
{
	if (nullptr != m_pFont)
	{
		_vec3 vPos = _vec3(m_pTransColor->m_matWorld._41, m_pTransColor->m_matWorld._42, m_pTransColor->m_matWorld._43).Convert_DescartesTo2DWindow(WINCX, WINCY);

		m_pFont->Set_Pos(_vec2(vPos.x - 52.0f, vPos.y - 115.0f));
		m_pFont->Update_GameObject(fTimeDelta);

		_tchar	szText[MIN_STR] = L"";
		if (L"UIStoreBuyListBack" == m_wstrObjectTag)
			wsprintf(szText, m_wstrFontText.c_str(), CStoreMgr::Get_Instance()->Get_BuyItemSumGold());
		else
			wsprintf(szText, m_wstrFontText.c_str(), CStoreMgr::Get_Instance()->Get_SellItemSumGold());

		m_pFontPrice->Set_Text(szText);
		m_pFontPrice->Set_Pos(_vec2(vPos.x + -100.0f, vPos.y + 77.0f));
		m_pFontPrice->Update_GameObject(fTimeDelta);
	}
}

Engine::CGameObject* CStoreBuySellListBack::Create(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList,
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
	CStoreBuySellListBack* pInstance = new CStoreBuySellListBack(pGraphicDevice, pCommandList);

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

void CStoreBuySellListBack::Free()
{
	CGameUIChild::Free();
	Engine::Safe_Release(m_pFontPrice);
}

