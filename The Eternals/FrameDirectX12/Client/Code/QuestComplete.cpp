#include "stdafx.h"
#include "QuestComplete.h"
#include "DirectInput.h"
#include "Font.h"
#include "QuestMgr.h"

CQuestComplete::CQuestComplete(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList)
	: CGameUIChild(pGraphicDevice, pCommandList)
{
}

HRESULT CQuestComplete::Ready_GameObject(wstring wstrRootObjectTag, 
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

	m_bIsActive = false;

	return S_OK;
}

HRESULT CQuestComplete::LateInit_GameObject()
{
	Engine::FAILED_CHECK_RETURN(CGameUIChild::LateInit_GameObject(), E_FAIL);

	return S_OK;
}

_int CQuestComplete::Update_GameObject(const _float& fTimeDelta)
{
	Engine::FAILED_CHECK_RETURN(Engine::CGameObject::LateInit_GameObject(), E_FAIL);
	
	if (g_bIsCinemaStart)
		return NO_EVENT;
	if (m_bIsDead)
		return DEAD_OBJ;
	if (!m_bIsActive)
		return NO_EVENT;

	CGameUIChild::Update_GameObject(fTimeDelta);

	return NO_EVENT;
}

_int CQuestComplete::LateUpdate_GameObject(const _float& fTimeDelta)
{
	if (g_bIsCinemaStart)
		return NO_EVENT;
	if (!m_bIsActive)
		return NO_EVENT;

	CGameUIChild::LateUpdate_GameObject(fTimeDelta);

	if (CMouseCursorMgr::Get_Instance()->Check_CursorInRect(m_tRect) &&
		Engine::MOUSE_KEYUP(Engine::MOUSEBUTTON::DIM_LB) && 
		m_bIsKeyPressing)
	{
		CQuestMgr::Get_Instance()->Get_QuestMessageCanvas()->Set_IsActive(true);
		CQuestMgr::Get_Instance()->Get_QuestMessageCanvas()->Set_IsChildActive(true);

		if (L"SubQuestComplete" == m_wstrObjectTag)
		{
			CQuestMgr::Get_Instance()->Set_ClearQuestType(QUEST_TYPE::QUEST_SUB);
			CQuestMgr::Get_Instance()->Get_QuestMessageCanvas()->Get_Font()->Set_Text(L"관문으로 이동하시겠습니까?");
		}
		else
		{
			CQuestMgr::Get_Instance()->Set_ClearQuestType(QUEST_TYPE::QUEST_MAIN);
			CQuestMgr::Get_Instance()->Get_QuestMessageCanvas()->Get_Font()->Set_Text(L"벨리카로 이동하시겠습니까?");
		}
	}

	m_bIsKeyPressing = false;

	// Check Mouse Collision.
	if (CMouseCursorMgr::Get_Instance()->Get_IsActiveMouse() &&
		CMouseCursorMgr::Get_Instance()->Check_CursorInRect(m_tRect))
	{
		if (Engine::MOUSE_PRESSING(Engine::MOUSEBUTTON::DIM_LB))
		{
			m_bIsKeyPressing = true;
		}
		else
		{
			m_bIsKeyPressing = false;
		}
	}


	return NO_EVENT;
}

void CQuestComplete::Render_GameObject(const _float& fTimeDelta)
{
	CGameUIChild::Render_GameObject(fTimeDelta);
}

Engine::CGameObject* CQuestComplete::Create(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList,
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
	CQuestComplete* pInstance = new CQuestComplete(pGraphicDevice, pCommandList);

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

void CQuestComplete::Free()
{
	CGameUIChild::Free();
}
