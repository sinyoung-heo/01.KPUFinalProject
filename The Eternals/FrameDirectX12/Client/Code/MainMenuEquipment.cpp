#include "stdafx.h"
#include "MainMenuEquipment.h"

CMainMenuEquipment::CMainMenuEquipment(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList)
	: CGameUIRoot(pGraphicDevice, pCommandList)
{
}

HRESULT CMainMenuEquipment::Ready_GameObject(wstring wstrObjectTag, 
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


	return S_OK;
}

HRESULT CMainMenuEquipment::LateInit_GameObject()
{
	Engine::FAILED_CHECK_RETURN(CGameUIRoot::LateInit_GameObject(), E_FAIL);

	return S_OK;
}

_int CMainMenuEquipment::Update_GameObject(const _float& fTimeDelta)
{
	Engine::FAILED_CHECK_RETURN(Engine::CGameObject::LateInit_GameObject(), E_FAIL);

	CGameUIRoot::Update_GameObject(fTimeDelta);

	return NO_EVENT;
}

_int CMainMenuEquipment::LateUpdate_GameObject(const _float& fTimeDelta)
{
	CGameUIRoot::LateUpdate_GameObject(fTimeDelta);

	return NO_EVENT;
}

void CMainMenuEquipment::Render_GameObject(const _float& fTimeDelta)
{
	CGameUIRoot::Render_GameObject(fTimeDelta);
}

Engine::CGameObject* CMainMenuEquipment::Create(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList, 
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
	CMainMenuEquipment* pInstance = new CMainMenuEquipment(pGraphicDevice, pCommandList);

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

void CMainMenuEquipment::Free()
{
	CGameUIRoot::Free();
}
