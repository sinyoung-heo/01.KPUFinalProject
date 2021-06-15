#include "stdafx.h"
#include "SettingButtonGameExit.h"

CSettingButtonGameExit::CSettingButtonGameExit(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList)
	: CGameUIChild(pGraphicDevice, pCommandList)
{
}

HRESULT CSettingButtonGameExit::Ready_GameObject(wstring wstrRootObjectTag, 
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

HRESULT CSettingButtonGameExit::LateInit_GameObject()
{
	Engine::FAILED_CHECK_RETURN(CGameUIChild::LateInit_GameObject(), E_FAIL);

	return S_OK;
}

_int CSettingButtonGameExit::Update_GameObject(const _float& fTimeDelta)
{
	Engine::FAILED_CHECK_RETURN(Engine::CGameObject::LateInit_GameObject(), E_FAIL);
	
	if (m_bIsDead)
		return DEAD_OBJ;
	
	CGameUIChild::Update_GameObject(fTimeDelta);

	return NO_EVENT;
}

_int CSettingButtonGameExit::LateUpdate_GameObject(const _float& fTimeDelta)
{
	CGameUIChild::LateUpdate_GameObject(fTimeDelta);

	return NO_EVENT;
}

void CSettingButtonGameExit::Render_GameObject(const _float& fTimeDelta)
{
	CGameUIChild::Render_GameObject(fTimeDelta);
}

Engine::CGameObject* CSettingButtonGameExit::Create(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList,
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
	CSettingButtonGameExit* pInstance = new CSettingButtonGameExit(pGraphicDevice, pCommandList);

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

void CSettingButtonGameExit::Free()
{
	CGameUIChild::Free();
}
