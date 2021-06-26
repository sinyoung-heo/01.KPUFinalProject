#include "stdafx.h"
#include "InGameStoreCanvas.h"

CInGameStoreCanvas::CInGameStoreCanvas(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList)
	: CGameUIRoot(pGraphicDevice, pCommandList)
{
}

HRESULT CInGameStoreCanvas::Ready_GameObject(wstring wstrObjectTag, 
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

	// m_bIsActive = false;
	m_bIsActive = true;

	return S_OK;
}

HRESULT CInGameStoreCanvas::LateInit_GameObject()
{
	Engine::FAILED_CHECK_RETURN(CGameUIRoot::LateInit_GameObject(), E_FAIL);

	return S_OK;
}

_int CInGameStoreCanvas::Update_GameObject(const _float& fTimeDelta)
{
	Engine::FAILED_CHECK_RETURN(Engine::CGameObject::LateInit_GameObject(), E_FAIL);

	if (m_bIsDead)
		return DEAD_OBJ;
	if (!m_bIsActive)
		return NO_EVENT;

	CGameUIRoot::Update_GameObject(fTimeDelta);

	return NO_EVENT;
}

_int CInGameStoreCanvas::LateUpdate_GameObject(const _float& fTimeDelta)
{
	if (!m_bIsActive)
		return NO_EVENT;

	CGameUIRoot::LateUpdate_GameObject(fTimeDelta);

	return NO_EVENT;
}

void CInGameStoreCanvas::Render_GameObject(const _float& fTimeDelta)
{
	CGameUIRoot::Render_GameObject(fTimeDelta);
}

Engine::CGameObject* CInGameStoreCanvas::Create(ID3D12Device* pGraphicDevice, 
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
	CInGameStoreCanvas* pInstance = new CInGameStoreCanvas(pGraphicDevice, pCommandList);

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

void CInGameStoreCanvas::Free()
{
	CGameUIRoot::Free();
}
