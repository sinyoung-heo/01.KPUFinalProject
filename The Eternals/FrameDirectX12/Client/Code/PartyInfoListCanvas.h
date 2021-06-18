#pragma once
#include "GameUIRoot.h"
#include "PartyUIClassInfo.h"
#include "PartyUIHpGauge.h"
#include "PartyUIMpGauge.h"

class CPartyInfoListCanvas : public CGameUIRoot
{
private:
	explicit CPartyInfoListCanvas(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList);
	virtual ~CPartyInfoListCanvas() = default;
public:
	CPartyUIClassInfo*	Get_PartyUIClassInfoClass()	{ return m_pUIClassInfo; }
	CPartyUIHpGauge*	Get_PartyUIHpGaugeClass()	{ return m_pUIHpGauge; }
	CPartyUIMpGauge*	Get_PartyUIMpGaugeClass()	{ return m_pUIMpGauge; }
	void Set_PartyUIClassInfoClass(CPartyUIClassInfo* pClass)	{ m_pUIClassInfo = pClass; }
	void Set_PartyUIHpGaugeClass(CPartyUIHpGauge* pClass)		{ m_pUIHpGauge = pClass; }
	void Set_PartyUIMpGaugeClass(CPartyUIMpGauge* pClass)		{ m_pUIMpGauge = pClass; }

	// CGameObject을(를) 통해 상속됨
	virtual HRESULT	Ready_GameObject(wstring wstrObjectTag,
									 wstring wstrDataFilePath,
									 const _vec3& vPos, 
									 const _vec3& vScale,
									 const _bool& bIsSpriteAnimation,
									 const _float& fFrameSpeed,
									 const _vec3& vRectOffset,
									 const _vec3& vRectScale,
									 const _long& iUIDepth);
	virtual HRESULT	LateInit_GameObject();
	virtual _int	Update_GameObject(const _float& fTimeDelta);
	virtual _int	LateUpdate_GameObject(const _float& fTimeDelta);
	virtual void	Render_GameObject(const _float& fTimeDelta);
private:
	CPartyUIClassInfo*	m_pUIClassInfo = nullptr;
	CPartyUIHpGauge*	m_pUIHpGauge   = nullptr;
	CPartyUIMpGauge*	m_pUIMpGauge   = nullptr;

public:
	static Engine::CGameObject* Create(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList,
									   wstring wstrObjectTag,							   
									   wstring wstrDataFilePath,
									   const _vec3& vPos,
									   const _vec3& vScale,
									   const _bool& bIsSpriteAnimation,
									   const _float& fFrameSpeed,
									   const _vec3& vRectOffset,
									   const _vec3& vRectScale,
									   const _long& iUIDepth = 1000);
private:
	virtual void Free();
};

