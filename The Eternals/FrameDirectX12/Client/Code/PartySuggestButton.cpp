#include "stdafx.h"
#include "PartySuggestButton.h"
#include "DirectInput.h"
#include "Font.h"
#include "PartySystemMgr.h"
#include "PartySuggestCanvas.h"
#include "ObjectMgr.h"


CPartySuggestButton::CPartySuggestButton(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList)
	: CGameUIChild(pGraphicDevice, pCommandList)
	, m_pPartySystemMgr(CPartySystemMgr::Get_Instance())
{
}

HRESULT CPartySuggestButton::Ready_GameObject(wstring wstrRootObjectTag, 
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

	if (L"PartySuggestInvite" == wstrObjectTag)
		m_pFont->Set_Text(L"파티초대");
	else
		m_pFont->Set_Text(L"파티가입");


	return S_OK;
}

HRESULT CPartySuggestButton::LateInit_GameObject()
{
	Engine::FAILED_CHECK_RETURN(CGameUIChild::LateInit_GameObject(), E_FAIL);

	return S_OK;
}

_int CPartySuggestButton::Update_GameObject(const _float& fTimeDelta)
{
	Engine::FAILED_CHECK_RETURN(Engine::CGameObject::LateInit_GameObject(), E_FAIL);
	
	if (m_bIsDead)
		return DEAD_OBJ;
	if (!m_bIsActive)
		return NO_EVENT;

	CGameUIChild::Update_GameObject(fTimeDelta);

	return NO_EVENT;
}

_int CPartySuggestButton::LateUpdate_GameObject(const _float& fTimeDelta)
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

		Engine::CGameObject* pThisPlayer = m_pObjectMgr->Get_GameObject(L"Layer_GameObject", L"ThisPlayer");
		Engine::CGameObject* pSelectPlayer = m_pPartySystemMgr->Get_SelectPlayer();
		if (nullptr != pSelectPlayer)
		{
			// 파티초대
			if (L"PartySuggestInvite" == m_wstrObjectTag)
			{
				if (pSelectPlayer->Get_PartyState() == false)
				{
					cout << pSelectPlayer->Get_ServerNumber() << "님에게 파티 신청 제안" << endl;
					CPacketMgr::Get_Instance()->send_suggest_party(pSelectPlayer->Get_ServerNumber());
				}
				else
					cout << "현재 유저는 다른 파티에 가입되어 있습니다." << endl;
			}

			// 파티가입
			else
			{
				// 현재 파티에 소속되어 있지 않아야 되며, 상대방은 파티에 소속되어 있을 경우만 가능
				if (pSelectPlayer->Get_PartyState() == true && pThisPlayer->Get_PartyState() == false)
				{
					cout << pSelectPlayer->Get_ServerNumber() << "님의 파티에 가입 신청" << endl;
					CPacketMgr::Get_Instance()->send_join_party(pSelectPlayer->Get_ServerNumber());
				}
				else
					cout << "파티 참여 신청을 할 수 없습니다." << endl;
			}
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

void CPartySuggestButton::Render_GameObject(const _float& fTimeDelta)
{
	CGameUIChild::Render_GameObject(fTimeDelta);
}

void CPartySuggestButton::SetUp_FontPosition(const _float& fTimeDelta)
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

Engine::CGameObject* CPartySuggestButton::Create(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList,
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
	CPartySuggestButton* pInstance = new CPartySuggestButton(pGraphicDevice, pCommandList);

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

void CPartySuggestButton::Free()
{
	CGameUIChild::Free();
	m_pCanvas = nullptr;
}
