#include "stdafx.h"
#include "PartyUIClassInfo.h"


CPartyUIClassInfo::CPartyUIClassInfo(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList)
	: CGameUIChild(pGraphicDevice, pCommandList)
{
}

void CPartyUIClassInfo::Set_CurrentClassTag(const char& cJob)
{
	wstring wstrTag = L"";

	if (cJob == PC_GLADIATOR)
		wstrTag = L"Gladiator";
	else if (cJob == PC_ARCHER)
		wstrTag = L"Archer";
	else
		wstrTag = L"Priest";

	auto iter_find = m_mapClassState.find(wstrTag);

	if (iter_find != m_mapClassState.end())
	{
		m_tFrame                = iter_find->second.tFrame;
		m_pTransCom->m_vPos     = iter_find->second.vPos;
		m_pTransCom->m_vScale   = iter_find->second.vScale;
		m_vRectOffset           = iter_find->second.vRectPosOffset;
		m_pTransColor->m_vScale = iter_find->second.vRectScale;
	}
}

HRESULT CPartyUIClassInfo::Ready_GameObject(wstring wstrRootObjectTag, 
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

	m_mapClassState[L"Gladiator"]                = UI_CHILD_STATE();
	m_mapClassState[L"Gladiator"].tFrame         = m_tFrame;
	m_mapClassState[L"Gladiator"].vPos           = m_pTransCom->m_vPos;
	m_mapClassState[L"Gladiator"].vScale         = m_pTransCom->m_vScale;
	m_mapClassState[L"Gladiator"].vRectPosOffset = vRectOffset;
	m_mapClassState[L"Gladiator"].vRectScale     = vRectScale;

	m_mapClassState[L"Archer"] = UI_CHILD_STATE();
	m_mapClassState[L"Priest"] = UI_CHILD_STATE();

	m_bIsActive = false;

	return S_OK;
}

HRESULT CPartyUIClassInfo::LateInit_GameObject()
{
	Engine::FAILED_CHECK_RETURN(CGameUIChild::LateInit_GameObject(), E_FAIL);

	return S_OK;
}

_int CPartyUIClassInfo::Update_GameObject(const _float& fTimeDelta)
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

_int CPartyUIClassInfo::LateUpdate_GameObject(const _float& fTimeDelta)
{
	if (g_bIsCinemaStart)
		return NO_EVENT;
	if (!m_bIsActive)
		return NO_EVENT;

	CGameUIChild::LateUpdate_GameObject(fTimeDelta);

	return NO_EVENT;
}

void CPartyUIClassInfo::Render_GameObject(const _float& fTimeDelta)
{
	CGameUIChild::Render_GameObject(fTimeDelta);
}

HRESULT CPartyUIClassInfo::SetUp_ClassState(wstring wstrTag, const UI_CHILD_STATE& tState)
{
	auto iter_find = m_mapClassState.find(wstrTag);
	if (iter_find == m_mapClassState.end())
		return E_FAIL;

	iter_find->second = tState;

	return S_OK;
}

Engine::CGameObject* CPartyUIClassInfo::Create(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList,
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
	CPartyUIClassInfo* pInstance = new CPartyUIClassInfo(pGraphicDevice, pCommandList);

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

void CPartyUIClassInfo::Free()
{
	CGameUIChild::Free();
	m_mapClassState.clear();
}
