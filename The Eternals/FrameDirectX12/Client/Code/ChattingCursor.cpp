#include "stdafx.h"
#include "ChattingCursor.h"


CChattingCursor::CChattingCursor(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList)
	: CGameUIChild(pGraphicDevice, pCommandList)
{
}

HRESULT CChattingCursor::Ready_GameObject(wstring wstrRootObjectTag, 
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

	m_bIsActive = true;
	m_fOriginPosX = vPos.x;

	return S_OK;
}

HRESULT CChattingCursor::LateInit_GameObject()
{
	Engine::FAILED_CHECK_RETURN(CGameUIChild::LateInit_GameObject(), E_FAIL);

	return S_OK;
}

_int CChattingCursor::Update_GameObject(const _float& fTimeDelta)
{
	Engine::FAILED_CHECK_RETURN(Engine::CGameObject::LateInit_GameObject(), E_FAIL);
	
	if (g_bIsCinemaStart)
		return NO_EVENT;
	if (m_bIsDead)
		return DEAD_OBJ;
	if (!m_bIsActive)
		return NO_EVENT;

	if (g_bIsChattingInput)
	{

		if (m_bIsRenderOn)
		{
			m_fRenderTime += fTimeDelta;

			if (m_fRenderTime >= m_fUpdateRenderTime)
			{
				m_fRenderTime = 0.0f;
				m_bIsRenderOn = false;
			}
		}

		if (!m_bIsRenderOn)
		{
			m_fTime += fTimeDelta;
			if (m_fTime >= m_fUpdateTime)
			{
				m_fTime       = 0.0f;
				m_bIsRenderOn = true;
			}
		}

		if (m_bIsRenderOn)
			CGameUIChild::Update_GameObject(fTimeDelta);
	}

	return NO_EVENT;
}

_int CChattingCursor::LateUpdate_GameObject(const _float& fTimeDelta)
{
	if (g_bIsCinemaStart)
		return NO_EVENT;
	if (!m_bIsActive)
		return NO_EVENT;

	if (g_bIsChattingInput)
		CGameUIChild::LateUpdate_GameObject(fTimeDelta);

	return NO_EVENT;
}

void CChattingCursor::Render_GameObject(const _float& fTimeDelta)
{
	CGameUIChild::Render_GameObject(fTimeDelta);
}

void CChattingCursor::Move_CursorPos(const CHATTING_CURSOR_MOVE& eMove, const _float& fOffset)
{
	if (CHATTING_CURSOR_MOVE::MOVE_RIGHT == eMove)
		m_pTransCom->m_vPos.x += fOffset;

	else if (CHATTING_CURSOR_MOVE::MOVE_LEFT == eMove)
		m_pTransCom->m_vPos.x -= fOffset;

	else if (CHATTING_CURSOR_MOVE::MOVE_ORIGIN == eMove)
		m_pTransCom->m_vPos.x = m_fOriginPosX;
}

Engine::CGameObject* CChattingCursor::Create(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList,
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
	CChattingCursor* pInstance = new CChattingCursor(pGraphicDevice, pCommandList);

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

void CChattingCursor::Free()
{
	CGameUIChild::Free();
}
