#include "stdafx.h"
#include "QuestMessageButtonClose.h"
#include "DirectInput.h"
#include "QuestMgr.h"

CQuestMessageButtonClose::CQuestMessageButtonClose(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList)
	: CGameUIChild(pGraphicDevice, pCommandList)
{
}

HRESULT CQuestMessageButtonClose::Ready_GameObject(wstring wstrRootObjectTag, 
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

HRESULT CQuestMessageButtonClose::LateInit_GameObject()
{
	Engine::FAILED_CHECK_RETURN(CGameUIChild::LateInit_GameObject(), E_FAIL);

	return S_OK;
}

_int CQuestMessageButtonClose::Update_GameObject(const _float& fTimeDelta)
{
	Engine::FAILED_CHECK_RETURN(Engine::CGameObject::LateInit_GameObject(), E_FAIL);
	
	if (m_bIsDead)
		return DEAD_OBJ;
	if (!m_bIsActive)
		return NO_EVENT;

	CGameUIChild::Update_GameObject(fTimeDelta);

	return NO_EVENT;
}

_int CQuestMessageButtonClose::LateUpdate_GameObject(const _float& fTimeDelta)
{
	if (!m_bIsActive)
		return NO_EVENT;

	CGameUIChild::LateUpdate_GameObject(fTimeDelta);

	if (CMouseCursorMgr::Get_Instance()->Check_CursorInRect(m_tRect) &&
		Engine::MOUSE_KEYUP(Engine::MOUSEBUTTON::DIM_LB) && 
		m_bIsKeyPressing)
	{
		//g_bIsOpenShop = false;
		CMouseCursorMgr::Get_Instance()->Set_IsActiveMouse(false);
		CQuestMgr::Get_Instance()->Get_QuestMessageCanvas()->Set_IsActive(false);
		CQuestMgr::Get_Instance()->Get_QuestMessageCanvas()->Set_IsChildActive(false);
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

void CQuestMessageButtonClose::Render_GameObject(const _float& fTimeDelta)
{
	CGameUIChild::Render_GameObject(fTimeDelta);
}

HRESULT CQuestMessageButtonClose::SetUp_MainMenuState(wstring wstrTag, const UI_CHILD_STATE& tState)
{
	auto iter_find = m_mapMainMenuState.find(wstrTag);
	if (iter_find == m_mapMainMenuState.end())
		return E_FAIL;

	iter_find->second = tState;

	return S_OK;
}

Engine::CGameObject* CQuestMessageButtonClose::Create(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList,
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
	CQuestMessageButtonClose* pInstance = new CQuestMessageButtonClose(pGraphicDevice, pCommandList);

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

void CQuestMessageButtonClose::Free()
{
	CGameUIChild::Free();
	m_mapMainMenuState.clear();
}
