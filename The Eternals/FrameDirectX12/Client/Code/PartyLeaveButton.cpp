#include "stdafx.h"
#include "PartyLeaveButton.h"
#include "DirectInput.h"
#include "Font.h"
#include "GameUIRoot.h"
#include "PartySystemMgr.h"

CPartyLeaveButton::CPartyLeaveButton(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList)
	: CGameUIChild(pGraphicDevice, pCommandList)
{
}

HRESULT CPartyLeaveButton::Ready_GameObject(wstring wstrRootObjectTag, 
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

	m_bIsActive = false;

	m_pFont->Set_Color(D2D1::ColorF::Cornsilk);
	m_pFont->Set_Text(L"ÆÄÆ¼Å»Åð");

	return S_OK;
}

HRESULT CPartyLeaveButton::LateInit_GameObject()
{
	Engine::FAILED_CHECK_RETURN(CGameUIChild::LateInit_GameObject(), E_FAIL);

	return S_OK;
}

_int CPartyLeaveButton::Update_GameObject(const _float& fTimeDelta)
{
	Engine::FAILED_CHECK_RETURN(Engine::CGameObject::LateInit_GameObject(), E_FAIL);
	
	if (m_bIsDead)
		return DEAD_OBJ;
	if (!m_bIsActive)
		return NO_EVENT;

	CGameUIChild::Update_GameObject(fTimeDelta);

	return NO_EVENT;
}

_int CPartyLeaveButton::LateUpdate_GameObject(const _float& fTimeDelta)
{
	if (!m_bIsActive)
		return NO_EVENT;

	CGameUIChild::LateUpdate_GameObject(fTimeDelta);

	if (CMouseCursorMgr::Get_Instance()->Check_CursorInRect(m_tRect) &&
		Engine::MOUSE_KEYUP(Engine::MOUSEBUTTON::DIM_LB) && 
		m_bIsKeyPressing)
	{
		m_pCanvas->Set_IsActive(false);
		m_pCanvas->Set_IsChildActive(false);

		// ÆÄÆ¼Å»Åð
		Engine::CGameObject* pThisPlayer = m_pObjectMgr->Get_GameObject(L"Layer_GameObject", L"ThisPlayer");

		if (pThisPlayer->Get_PartyState())
		{
			cout << pThisPlayer->Get_ServerNumber() << "ÆÄÆ¼Å»Åð" << endl;
			CPacketMgr::Get_Instance()->send_leave_party(pThisPlayer->Get_ServerNumber());
			pThisPlayer->Set_PartyState(false);
			pThisPlayer->Clear_PartyMember();
			pThisPlayer->Set_RequestParty(false);
			pThisPlayer->JoinRequest_Party(false);
			pThisPlayer->Set_PartySuggestSNum(-1);
			CPartySystemMgr::Get_Instance()->SetUp_ThisPlayerPartyList();
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

void CPartyLeaveButton::Render_GameObject(const _float& fTimeDelta)
{
	CGameUIChild::Render_GameObject(fTimeDelta);
}

void CPartyLeaveButton::SetUp_FontPosition(const _float& fTimeDelta)
{
	if (nullptr != m_pFont)
	{
		_vec3 vPos = _vec3(m_pTransColor->m_matWorld._41, m_pTransColor->m_matWorld._42, m_pTransColor->m_matWorld._43).Convert_DescartesTo2DWindow(WINCX, WINCY);
		vPos.x -= 28.0f;
		vPos.y -= 10.0f;

		m_pFont->Set_Pos(_vec2(vPos.x, vPos.y));
		m_pFont->Update_GameObject(fTimeDelta);
	}
}

Engine::CGameObject* CPartyLeaveButton::Create(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList,
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
	CPartyLeaveButton* pInstance = new CPartyLeaveButton(pGraphicDevice, pCommandList);

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

void CPartyLeaveButton::Free()
{
	CGameUIChild::Free();
	m_pCanvas = nullptr;
}
