#include "stdafx.h"
#include "PartyInfoListCanvas.h"


CPartyInfoListCanvas::CPartyInfoListCanvas(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList)
	: CGameUIRoot(pGraphicDevice, pCommandList)
{
}

HRESULT CPartyInfoListCanvas::Ready_GameObject(wstring wstrObjectTag, 
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

	m_bIsActive = true;

	return S_OK;
}

HRESULT CPartyInfoListCanvas::LateInit_GameObject()
{
	Engine::FAILED_CHECK_RETURN(CGameUIRoot::LateInit_GameObject(), E_FAIL);

	return S_OK;
}

_int CPartyInfoListCanvas::Update_GameObject(const _float& fTimeDelta)
{
	Engine::FAILED_CHECK_RETURN(Engine::CGameObject::LateInit_GameObject(), E_FAIL);

	if (m_bIsDead)
		return DEAD_OBJ;
	if (!m_bIsActive)
		return NO_EVENT;

	CGameUIRoot::Update_GameObject(fTimeDelta);

	return NO_EVENT;
}

_int CPartyInfoListCanvas::LateUpdate_GameObject(const _float& fTimeDelta)
{
	if (!m_bIsActive)
		return NO_EVENT;

	CGameUIRoot::LateUpdate_GameObject(fTimeDelta);

	return NO_EVENT;
}

void CPartyInfoListCanvas::Render_GameObject(const _float& fTimeDelta)
{
	CGameUIRoot::Render_GameObject(fTimeDelta);
}

Engine::CGameObject* CPartyInfoListCanvas::Create(ID3D12Device* pGraphicDevice, 
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
	CPartyInfoListCanvas* pInstance = new CPartyInfoListCanvas(pGraphicDevice, pCommandList);

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

void CPartyInfoListCanvas::Free()
{
	CGameUIRoot::Free();

	m_pUIClassInfo = nullptr;
	m_pUIHpGauge   = nullptr;
	m_pUIMpGauge   = nullptr;
}
