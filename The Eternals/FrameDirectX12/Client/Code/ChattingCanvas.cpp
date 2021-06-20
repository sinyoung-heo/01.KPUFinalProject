#include "stdafx.h"
#include "ChattingCanvas.h"
#include "DirectInput.h"
#include "Font.h"

CChattingCanvas::CChattingCanvas(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList)
	: CGameUIRoot(pGraphicDevice, pCommandList)
{
}

HRESULT CChattingCanvas::Ready_GameObject(wstring wstrObjectTag,
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

	m_wstrChattingList.resize(MAX_CHATTING_SIZE);
	for (auto& chatting : m_wstrChattingList)
	{
		chatting = L"";
		m_wstrChattingCombined += chatting + wstring(L"\n");
	}


	m_pFont->Set_Color(D2D1::ColorF::Cornsilk);
	m_pFont->Set_Text(m_wstrChattingCombined);

	return S_OK;
}

HRESULT CChattingCanvas::LateInit_GameObject()
{
	Engine::FAILED_CHECK_RETURN(CGameUIRoot::LateInit_GameObject(), E_FAIL);

	return S_OK;
}

_int CChattingCanvas::Update_GameObject(const _float& fTimeDelta)
{
	Engine::FAILED_CHECK_RETURN(Engine::CGameObject::LateInit_GameObject(), E_FAIL);

	if (m_bIsDead)
		return DEAD_OBJ;
	if (!m_bIsActive)
		return NO_EVENT;

	CGameUIRoot::Update_GameObject(fTimeDelta);

	return NO_EVENT;
}

_int CChattingCanvas::LateUpdate_GameObject(const _float& fTimeDelta)
{
	if (!m_bIsActive)
		return NO_EVENT;

	CGameUIRoot::LateUpdate_GameObject(fTimeDelta);

	if (nullptr != m_pFont)
	{
		_vec3 vPos = _vec3(m_pTransColor->m_matWorld._41, m_pTransColor->m_matWorld._42, m_pTransColor->m_matWorld._43).Convert_DescartesTo2DWindow(WINCX, WINCY);
		vPos.x += -180.0f;
		vPos.y += -80.0f;

		m_pFont->Set_Pos(_vec2(vPos.x, vPos.y));
		m_pFont->Update_GameObject(fTimeDelta);
	}

	return NO_EVENT;
}

void CChattingCanvas::Render_GameObject(const _float& fTimeDelta)
{
	CGameUIRoot::Render_GameObject(fTimeDelta);
}

void CChattingCanvas::Push_ChattingMessage(char* pUserID, wstring wstrMessage)
{
	_tchar szUserID[MIN_STR] = L"";

	/*____________________________________________________________________
	멀티바이트 형식을 유니코드 형식으로 바꿔주는 함수.
	______________________________________________________________________*/
	MultiByteToWideChar(CP_ACP,
						0,
						pUserID,		// 변환 할 문자열.
						(_int)strlen(pUserID),
						szUserID,			// 변환 값 저장 버퍼.
						MAX_STR);

	wstring wstrUserID = szUserID;

	// 채팅 메시지 한 칸씩 위로.
	m_wstrChattingCombined = L"";

	for (_int i = 0; i < m_wstrChattingList.size() - 1; ++i)
	{
		m_wstrChattingList[i] = m_wstrChattingList[i + 1];
		m_wstrChattingCombined += m_wstrChattingList[i] + wstring(L"\n");
	}

	m_wstrChattingList[m_wstrChattingList.size() - 1] = wstrUserID + L" : " + wstrMessage;
	m_wstrChattingCombined += m_wstrChattingList[m_wstrChattingList.size() - 1] + wstring(L"\n");

	m_pFont->Set_Text(m_wstrChattingCombined);
}

Engine::CGameObject* CChattingCanvas::Create(ID3D12Device* pGraphicDevice, 
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
	CChattingCanvas* pInstance = new CChattingCanvas(pGraphicDevice, pCommandList);

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

void CChattingCanvas::Free()
{
	CGameUIRoot::Free();
}
