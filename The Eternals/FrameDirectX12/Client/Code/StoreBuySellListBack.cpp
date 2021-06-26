#include "stdafx.h"
#include "StoreBuySellListBack.h"
#include "Font.h"
#include "DirectInput.h"

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

	// m_bIsActive = false;
	m_bIsActive = true;

	m_pFont->Set_Color(D2D1::ColorF::Cornsilk);

	if (L"UIStoreBuyListBack" == wstrObjectTag)
		m_pFont->Set_Text(L"구매 목록");
	else
		m_pFont->Set_Text(L"판매 목록");

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
		if (Engine::KEY_PRESSING(DIK_Z))
		{
			++fOffsetX;
			cout << fOffsetX << ", " << fOffsetY << endl;
		}
		if (Engine::KEY_PRESSING(DIK_X))
		{
			--fOffsetX;
			cout << fOffsetX << ", " << fOffsetY << endl;
		}
		if (Engine::KEY_PRESSING(DIK_C))
		{
			++fOffsetY;
			cout << fOffsetX << ", " << fOffsetY << endl;
		}
		if (Engine::KEY_PRESSING(DIK_V))
		{
			--fOffsetY;
			cout << fOffsetX << ", " << fOffsetY << endl;
		}

		_vec3 vPos = _vec3(m_pTransColor->m_matWorld._41, m_pTransColor->m_matWorld._42, m_pTransColor->m_matWorld._43).Convert_DescartesTo2DWindow(WINCX, WINCY);
		vPos.x += -52.0f;
		vPos.y += -115.0f;

		m_pFont->Set_Pos(_vec2(vPos.x, vPos.y));
		m_pFont->Update_GameObject(fTimeDelta);
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
}

