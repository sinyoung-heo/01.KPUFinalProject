#include "stdafx.h"
#include "ChattingInput.h"
#include "DirectInput.h"
#include "Font.h"
#include "ChattingMgr.h"

CChattingInput::CChattingInput(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList)
	: CGameUIChild(pGraphicDevice, pCommandList)
{
}

void CChattingInput::Set_FontText(wstring wstrChatting)
{
	m_pFont->Set_Text(wstrChatting);
}

const _tchar& CChattingInput::Get_ChattingTextBack()
{
	wstring wstrText = m_pFont->Get_FontText();

	if (!wstrText.empty())
		return wstrText.back();
	else
		return 0;
}

HRESULT CChattingInput::Ready_GameObject(wstring wstrRootObjectTag,
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
															   true, L"Font_BinggraeMelona16"), E_FAIL);

	m_bIsActive = true;

	m_pFont->Set_Color(D2D1::ColorF::Cornsilk);
	m_pFont->Set_Text(L"");

	return S_OK;
}

HRESULT CChattingInput::LateInit_GameObject()
{
	Engine::FAILED_CHECK_RETURN(CGameUIChild::LateInit_GameObject(), E_FAIL);

	return S_OK;
}

_int CChattingInput::Update_GameObject(const _float& fTimeDelta)
{
	Engine::FAILED_CHECK_RETURN(Engine::CGameObject::LateInit_GameObject(), E_FAIL);
	
	if (m_bIsDead)
		return DEAD_OBJ;
	if (!m_bIsActive)
		return NO_EVENT;

	KeyInput_Chatting(fTimeDelta);
	CGameUIChild::Update_GameObject(fTimeDelta);

	return NO_EVENT;
}

_int CChattingInput::LateUpdate_GameObject(const _float& fTimeDelta)
{
	if (!m_bIsActive)
		return NO_EVENT;

	CGameUIChild::LateUpdate_GameObject(fTimeDelta);

	if (nullptr != m_pFont)
	{
		_vec3 vPos = _vec3(m_pTransColor->m_matWorld._41, m_pTransColor->m_matWorld._42, m_pTransColor->m_matWorld._43).Convert_DescartesTo2DWindow(WINCX, WINCY);
		vPos.x += -180.0f;
		vPos.y += -12.0f;

		m_pFont->Set_Pos(_vec2(vPos.x, vPos.y));
		m_pFont->Update_GameObject(fTimeDelta);
	}

	return NO_EVENT;
}

void CChattingInput::Render_GameObject(const _float& fTimeDelta)
{
	CGameUIChild::Render_GameObject(fTimeDelta);
}

void CChattingInput::KeyInput_Chatting(const _float& fTimeDelta)
{
	if (!g_bIsActive) return;
	if (g_bIsOpenShop)
	{
		g_bIsChattingInput = false;
		return;
	}

	if (Engine::KEY_DOWN(DIK_ESCAPE))
		g_bIsChattingInput = false;

	if (Engine::KEY_DOWN(DIK_RETURN) && INPUT_CHATTING)
	{
		g_bIsChattingInput = !g_bIsChattingInput;

		// Send Chatting Packet.
		if (!g_bIsChattingInput)
		{
			CPacketMgr::Get_Instance()->send_chat(g_Text);
		}

		lstrcpy(g_Text, L"");
		m_pFont->Set_Text(L"");
		CChattingMgr::Get_Instance()->Move_CursorPos(CHATTING_CURSOR_MOVE::MOVE_ORIGIN);
	}
}

Engine::CGameObject* CChattingInput::Create(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList,
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
	CChattingInput* pInstance = new CChattingInput(pGraphicDevice, pCommandList);

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

void CChattingInput::Free()
{
	CGameUIChild::Free();
}
