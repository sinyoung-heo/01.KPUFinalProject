#include "stdafx.h"
#include "PartySuggestResponseClose.h"
#include "DirectInput.h"
#include "ObjectMgr.h"
#include "PartySuggestResponseCanvas.h"

CPartySuggestResponseClose::CPartySuggestResponseClose(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList)
	: CGameUIChild(pGraphicDevice, pCommandList)
{
}

HRESULT CPartySuggestResponseClose::Ready_GameObject(wstring wstrRootObjectTag, 
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
															   iUIDepth), E_FAIL);

	m_mapMainMenuState[L"Normal"]                = UI_CHILD_STATE();
	m_mapMainMenuState[L"Normal"].tFrame         = m_tFrame;
	m_mapMainMenuState[L"Normal"].vPos           = m_pTransCom->m_vPos;
	m_mapMainMenuState[L"Normal"].vScale         = m_pTransCom->m_vScale;
	m_mapMainMenuState[L"Normal"].vRectPosOffset = vRectOffset;
	m_mapMainMenuState[L"Normal"].vRectScale     = vRectScale;
	m_mapMainMenuState[L"MouseOn"]      = UI_CHILD_STATE();
	m_mapMainMenuState[L"MouseClicked"] = UI_CHILD_STATE();

	m_bIsActive = false;

	return S_OK;
}

HRESULT CPartySuggestResponseClose::LateInit_GameObject()
{
	Engine::FAILED_CHECK_RETURN(CGameUIChild::LateInit_GameObject(), E_FAIL);

	return S_OK;
}

_int CPartySuggestResponseClose::Update_GameObject(const _float& fTimeDelta)
{
	Engine::FAILED_CHECK_RETURN(Engine::CGameObject::LateInit_GameObject(), E_FAIL);
	
	if (m_bIsDead)
		return DEAD_OBJ;
	if (!m_bIsActive)
		return NO_EVENT;

	CGameUIChild::Update_GameObject(fTimeDelta);

	return NO_EVENT;
}

_int CPartySuggestResponseClose::LateUpdate_GameObject(const _float& fTimeDelta)
{
	if (!m_bIsActive)
		return NO_EVENT;

	CGameUIChild::LateUpdate_GameObject(fTimeDelta);

	if (nullptr != m_pCanvas)
		m_ePartyRequestState = m_pCanvas->Get_PartyRequestState();

	if (CMouseCursorMgr::Get_Instance()->Check_CursorInRect(m_tRect) &&
		Engine::MOUSE_KEYUP(Engine::MOUSEBUTTON::DIM_LB) && 
		m_bIsKeyPressing)
	{
		Engine::CGameObject* pThisPlayer = m_pObjectMgr->Get_GameObject(L"Layer_GameObject", L"ThisPlayer");
		_int iSuggesterNumber = pThisPlayer->Get_PartySuggestSNum();
		cout << iSuggesterNumber << endl;

		// 파티초대 - 거절
		if (PARTY_REQUEST_STATE::REQUEST_PARTY_INVITE == m_ePartyRequestState &&
			pThisPlayer->Get_RequestParty())
		{
			CPacketMgr::Get_Instance()->send_respond_party(false, iSuggesterNumber);
			pThisPlayer->Set_RequestParty(false);
			pThisPlayer->Set_PartySuggestSNum(-1);
			cout << "파티초대 - 거절" << endl;
		}
		// 파티가입 - 거절
		else if (PARTY_REQUEST_STATE::REQUEST_PARTY_JOIN == m_ePartyRequestState &&
				 pThisPlayer->Get_PartyJoinRequest())
		{
			CPacketMgr::Get_Instance()->send_decide_party(false, iSuggesterNumber);
			pThisPlayer->Set_JoinRequest(false);
			pThisPlayer->Set_PartySuggestSNum(-1);
			cout << "파티가입 - 거절" << endl;
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
			m_tFrame                = m_mapMainMenuState[L"MouseClicked"].tFrame;
			m_pTransCom->m_vPos     = m_mapMainMenuState[L"MouseClicked"].vPos;
			m_pTransCom->m_vScale   = m_mapMainMenuState[L"MouseClicked"].vScale;
			m_vRectOffset           = m_mapMainMenuState[L"MouseClicked"].vRectPosOffset;
			m_pTransColor->m_vScale = m_mapMainMenuState[L"MouseClicked"].vRectScale;
			m_bIsKeyPressing = true;
		}
		else
		{
			m_tFrame                = m_mapMainMenuState[L"MouseOn"].tFrame;
			m_pTransCom->m_vPos     = m_mapMainMenuState[L"MouseOn"].vPos;
			m_pTransCom->m_vScale   = m_mapMainMenuState[L"MouseOn"].vScale;
			m_vRectOffset           = m_mapMainMenuState[L"MouseOn"].vRectPosOffset;
			m_pTransColor->m_vScale = m_mapMainMenuState[L"MouseOn"].vRectScale;
			m_bIsKeyPressing = false;
		}
	}
	else
	{
		m_tFrame                = m_mapMainMenuState[L"Normal"].tFrame;
		m_pTransCom->m_vPos     = m_mapMainMenuState[L"Normal"].vPos;
		m_pTransCom->m_vScale   = m_mapMainMenuState[L"Normal"].vScale;
		m_vRectOffset           = m_mapMainMenuState[L"Normal"].vRectPosOffset;
		m_pTransColor->m_vScale = m_mapMainMenuState[L"Normal"].vRectScale;
		
	}

	return NO_EVENT;
}

void CPartySuggestResponseClose::Render_GameObject(const _float& fTimeDelta)
{
	CGameUIChild::Render_GameObject(fTimeDelta);
}

HRESULT CPartySuggestResponseClose::SetUp_MainMenuState(wstring wstrTag, const UI_CHILD_STATE& tState)
{
	auto iter_find = m_mapMainMenuState.find(wstrTag);
	if (iter_find == m_mapMainMenuState.end())
		return E_FAIL;

	iter_find->second = tState;

	return S_OK;
}

Engine::CGameObject* CPartySuggestResponseClose::Create(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList,
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
	CPartySuggestResponseClose* pInstance = new CPartySuggestResponseClose(pGraphicDevice, pCommandList);

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

void CPartySuggestResponseClose::Free()
{
	CGameUIChild::Free();
	m_mapMainMenuState.clear();
}
