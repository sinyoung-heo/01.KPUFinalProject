#include "stdafx.h"
#include "MainMenuSettingCanvas.h"

CMainMenuSettingCanvas::CMainMenuSettingCanvas(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList)
	: CGameUIRoot(pGraphicDevice, pCommandList)
{
}

HRESULT CMainMenuSettingCanvas::Ready_GameObject(wstring wstrObjectTag, 
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
															  iUIDepth), E_FAIL);

	m_bIsActive = false;

	return S_OK;
}

HRESULT CMainMenuSettingCanvas::LateInit_GameObject()
{
	Engine::FAILED_CHECK_RETURN(CGameUIRoot::LateInit_GameObject(), E_FAIL);

	return S_OK;
}

_int CMainMenuSettingCanvas::Update_GameObject(const _float& fTimeDelta)
{
	Engine::FAILED_CHECK_RETURN(Engine::CGameObject::LateInit_GameObject(), E_FAIL);

	if (m_bIsDead)
		return DEAD_OBJ;

	CGameUIRoot::Update_GameObject(fTimeDelta);

	return NO_EVENT;
}

_int CMainMenuSettingCanvas::LateUpdate_GameObject(const _float& fTimeDelta)
{
	CGameUIRoot::LateUpdate_GameObject(fTimeDelta);

	return NO_EVENT;
}

void CMainMenuSettingCanvas::Render_GameObject(const _float& fTimeDelta)
{
	CGameUIRoot::Render_GameObject(fTimeDelta);
}

Engine::CGameObject* CMainMenuSettingCanvas::Create(ID3D12Device* pGraphicDevice, 
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
	CMainMenuSettingCanvas* pInstance = new CMainMenuSettingCanvas(pGraphicDevice, pCommandList);

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

void CMainMenuSettingCanvas::Free()
{
	CGameUIRoot::Free();
}
