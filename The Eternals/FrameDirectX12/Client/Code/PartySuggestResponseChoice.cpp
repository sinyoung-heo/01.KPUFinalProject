#include "stdafx.h"
#include "PartySuggestResponseChoice.h"
#include "DirectInput.h"
#include "ObjectMgr.h"
#include "MainMenuLogout.h"
#include "Font.h"
#include "PartySuggestResponseCanvas.h"

CPartySuggestResponseChoice::CPartySuggestResponseChoice(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList)
	: CGameUIChild(pGraphicDevice, pCommandList)
{
}

HRESULT CPartySuggestResponseChoice::Ready_GameObject(wstring wstrRootObjectTag, 
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

	if (L"SystemButtonYesPartySuggestResponse" == wstrObjectTag)
		m_pFont->Set_Text(L"YES");
	else
		m_pFont->Set_Text(L"NO");


	return S_OK;
}

HRESULT CPartySuggestResponseChoice::LateInit_GameObject()
{
	Engine::FAILED_CHECK_RETURN(CGameUIChild::LateInit_GameObject(), E_FAIL);

	return S_OK;
}

_int CPartySuggestResponseChoice::Update_GameObject(const _float& fTimeDelta)
{
	Engine::FAILED_CHECK_RETURN(Engine::CGameObject::LateInit_GameObject(), E_FAIL);
	
	if (m_bIsDead)
		return DEAD_OBJ;
	if (!m_bIsActive)
		return NO_EVENT;

	CGameUIChild::Update_GameObject(fTimeDelta);

	return NO_EVENT;
}

_int CPartySuggestResponseChoice::LateUpdate_GameObject(const _float& fTimeDelta)
{
	if (!m_bIsActive)
		return NO_EVENT;

	CGameUIChild::LateUpdate_GameObject(fTimeDelta);

	if (nullptr != m_pCanvas)
		m_ePartyRequestState = m_pCanvas->Get_PartyRequestState();

	if (CMouseCursorMgr::Get_Instance()->Check_CursorInRect(m_tRect) &&
		Engine::MOUSE_KEYUP(Engine::MOUSEBUTTON::DIM_LB) && g_bIsActive &&
		m_bIsKeyPressing)
	{
		Engine::CGameObject* pThisPlayer = m_pObjectMgr->Get_GameObject(L"Layer_GameObject", L"ThisPlayer");
		_int iSuggesterNumber = pThisPlayer->Get_PartySuggestSNum();
		cout << iSuggesterNumber << endl;

		if (-1 != iSuggesterNumber)
		{
			if (L"SystemButtonYesPartySuggestResponse" == m_wstrObjectTag)
			{
				// 파티초대 - 수락
				if (PARTY_REQUEST_STATE::REQUEST_PARTY_INVITE == m_ePartyRequestState &&
					pThisPlayer->Get_RequestParty())
				{
					CPacketMgr::Get_Instance()->send_respond_party(true, iSuggesterNumber);
					pThisPlayer->Set_RequestParty(false);
					pThisPlayer->Set_PartySuggestSNum(-1);
				}
				// 파티가입 - 수락
				else if (PARTY_REQUEST_STATE::REQUEST_PARTY_JOIN == m_ePartyRequestState &&
						 pThisPlayer->Get_PartyJoinRequest())
				{
					CPacketMgr::Get_Instance()->send_decide_party(true, iSuggesterNumber);
					pThisPlayer->Set_JoinRequest(false);
					pThisPlayer->Set_PartySuggestSNum(-1);
				}
			}
			else
			{
				// 파티초대 - 거절
				if (PARTY_REQUEST_STATE::REQUEST_PARTY_INVITE == m_ePartyRequestState &&
					pThisPlayer->Get_RequestParty())
				{
					CPacketMgr::Get_Instance()->send_respond_party(false, iSuggesterNumber);
					pThisPlayer->Set_RequestParty(false);
					pThisPlayer->Set_PartySuggestSNum(-1);
				}
				// 파티가입 - 거절
				else if (PARTY_REQUEST_STATE::REQUEST_PARTY_JOIN == m_ePartyRequestState &&
						 pThisPlayer->Get_PartyJoinRequest())
				{
					CPacketMgr::Get_Instance()->send_decide_party(false, iSuggesterNumber);
					pThisPlayer->Set_JoinRequest(false);
					pThisPlayer->Set_PartySuggestSNum(-1);
				}
			}
		}


		if (nullptr != m_pCanvas)
		{
			m_pCanvas->Set_IsActive(false);
			m_pCanvas->Set_IsChildActive(false);
		}
	}

	m_bIsKeyPressing = false;

	// Check Mouse Collision.
	if (CMouseCursorMgr::Get_Instance()->Get_IsActiveMouse() &&
		CMouseCursorMgr::Get_Instance()->Check_CursorInRect(m_tRect))
	{
		if (Engine::MOUSE_PRESSING(Engine::MOUSEBUTTON::DIM_LB))
		{
			m_uiTexIdx       = 1;
			m_bIsKeyPressing = true;
		}
		else
		{
			m_uiTexIdx       = 0;
			m_bIsKeyPressing = false;
		}
	}
	else
	{
		m_uiTexIdx = 0;
	}

	SetUp_FontPosition(fTimeDelta);

	return NO_EVENT;
}

void CPartySuggestResponseChoice::Render_GameObject(const _float& fTimeDelta)
{
	CGameUIChild::Render_GameObject(fTimeDelta);
}

void CPartySuggestResponseChoice::SetUp_FontPosition(const _float& fTimeDelta)
{
	if (nullptr != m_pFont && !g_bIsStageChange)
	{
		_vec3 vPos = _vec3(m_pTransColor->m_matWorld._41, m_pTransColor->m_matWorld._42, m_pTransColor->m_matWorld._43).Convert_DescartesTo2DWindow(WINCX, WINCY);
		if (L"SystemButtonYesPartySuggestResponse" == m_wstrObjectTag)
			vPos.x -= 22.0f;
		else
			vPos.x -= 15.0f;

		vPos.y -= 15.0f;

		m_pFont->Set_Pos(_vec2(vPos.x, vPos.y));
		m_pFont->Update_GameObject(fTimeDelta);
	}
}

Engine::CGameObject* CPartySuggestResponseChoice::Create(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList,
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
	CPartySuggestResponseChoice* pInstance = new CPartySuggestResponseChoice(pGraphicDevice, pCommandList);

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

void CPartySuggestResponseChoice::Free()
{
	CGameUIChild::Free();
}
