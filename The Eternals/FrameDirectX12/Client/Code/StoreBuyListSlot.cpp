#include "stdafx.h"
#include "StoreBuyListSlot.h"

CStoreBuyListSlot::CStoreBuyListSlot(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList)
	: CGameUIChild(pGraphicDevice, pCommandList)
{
}

HRESULT CStoreBuyListSlot::Ready_GameObject(wstring wstrRootObjectTag, 
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

	// m_bIsActive = false;
	m_bIsActive = true;

	return S_OK;
}

HRESULT CStoreBuyListSlot::LateInit_GameObject()
{
	Engine::FAILED_CHECK_RETURN(CGameUIChild::LateInit_GameObject(), E_FAIL);

	return S_OK;
}

_int CStoreBuyListSlot::Update_GameObject(const _float& fTimeDelta)
{
	Engine::FAILED_CHECK_RETURN(Engine::CGameObject::LateInit_GameObject(), E_FAIL);
	
	if (m_bIsDead)
		return DEAD_OBJ;
	if (!m_bIsActive)
		return NO_EVENT;

	CGameUIChild::Update_GameObject(fTimeDelta);

	return NO_EVENT;
}

_int CStoreBuyListSlot::LateUpdate_GameObject(const _float& fTimeDelta)
{
	if (!m_bIsActive)
		return NO_EVENT;

	CGameUIChild::LateUpdate_GameObject(fTimeDelta);

	return NO_EVENT;
}

void CStoreBuyListSlot::Render_GameObject(const _float& fTimeDelta)
{
	CGameUIChild::Render_GameObject(fTimeDelta);
}

Engine::CGameObject* CStoreBuyListSlot::Create(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList,
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
	CStoreBuyListSlot* pInstance = new CStoreBuyListSlot(pGraphicDevice, pCommandList);

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

void CStoreBuyListSlot::Free()
{
	CGameUIChild::Free();
}

