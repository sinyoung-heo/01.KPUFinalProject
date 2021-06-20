#include "stdafx.h"
#include "PartyInfoListCanvas.h"
#include "Font.h"
#include "DirectInput.h"


CPartyInfoListCanvas::CPartyInfoListCanvas(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList)
	: CGameUIRoot(pGraphicDevice, pCommandList)
{
}

void CPartyInfoListCanvas::Set_UserName(char* pUserName)
{
	_tchar* pOut = L"";
	/*____________________________________________________________________
	멀티바이트 형식을 유니코드 형식으로 바꿔주는 함수.
	______________________________________________________________________*/
	MultiByteToWideChar(CP_ACP,
						0,
						pUserName,		// 변환 할 문자열.
						(_int)strlen(pUserName),
						pOut,			// 변환 값 저장 버퍼.
						MAX_STR);

	m_pFont->Set_Text(pOut);
}

HRESULT CPartyInfoListCanvas::Ready_GameObject(wstring wstrObjectTag,
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
															  true, L"Font_BinggraeMelona11"), E_FAIL);

	m_bIsActive = true;

	m_pFont->Set_Color(D2D1::ColorF::Cornsilk);
	m_pFont->Set_Text(L"UserName");

	return S_OK;
}

HRESULT CPartyInfoListCanvas::LateInit_GameObject()
{
	Engine::FAILED_CHECK_RETURN(CGameUIRoot::LateInit_GameObject(), E_FAIL);

	return S_OK;
}

_int CPartyInfoListCanvas::Update_GameObject(const _float& fTimeDelta)
{
	Engine::FAILED_CHECK_RETURN(Engine::CGameObject::LateInit_GameObject(), E_FAIL);

	if (m_bIsDead)
		return DEAD_OBJ;
	if (!m_bIsActive)
		return NO_EVENT;

	CGameUIRoot::Update_GameObject(fTimeDelta);

	return NO_EVENT;
}

_int CPartyInfoListCanvas::LateUpdate_GameObject(const _float& fTimeDelta)
{
	if (!m_bIsActive)
		return NO_EVENT;

	CGameUIRoot::LateUpdate_GameObject(fTimeDelta);
	SetUp_FontPosition(fTimeDelta);

	return NO_EVENT;
}

void CPartyInfoListCanvas::Render_GameObject(const _float& fTimeDelta)
{
	CGameUIRoot::Render_GameObject(fTimeDelta);
}

void CPartyInfoListCanvas::SetUp_FontPosition(const _float& fTimeDelta)
{
	if (nullptr != m_pFont)
	{
		_vec3 vPos = _vec3(m_pTransColor->m_matWorld._41, m_pTransColor->m_matWorld._42, m_pTransColor->m_matWorld._43).Convert_DescartesTo2DWindow(WINCX, WINCY);
		vPos.x += -90.0f;
		vPos.y += -40.0f;

		m_pFont->Set_Pos(_vec2(vPos.x, vPos.y));
		m_pFont->Update_GameObject(fTimeDelta);
	}
}

Engine::CGameObject* CPartyInfoListCanvas::Create(ID3D12Device* pGraphicDevice, 
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
	CPartyInfoListCanvas* pInstance = new CPartyInfoListCanvas(pGraphicDevice, pCommandList);

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

void CPartyInfoListCanvas::Free()
{
	CGameUIRoot::Free();

	m_pUIClassInfo = nullptr;
	m_pUIHpGauge   = nullptr;
	m_pUIMpGauge   = nullptr;
}
