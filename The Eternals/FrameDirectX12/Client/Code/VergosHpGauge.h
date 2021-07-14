#pragma once
#include "GameUIChild.h"

class CVergosHpGauge : public CGameUIChild
{
private:
	explicit CVergosHpGauge(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList);
	virtual ~CVergosHpGauge() = default;

public:
	void Set_Percent(const _float& fPercent, const _int& iCurHp, const _int& iMaxHp) { m_fPercent = fPercent; m_iCurHp = iCurHp; m_iMaxHp = iMaxHp; }

	// CGameObject을(를) 통해 상속됨
	virtual HRESULT	Ready_GameObject(wstring wstrRootObjectTag,
									 wstring wstrObjectTag,							   
									 wstring wstrDataFilePath,
									 const _vec3& vPos,
									 const _vec3& vScale,
									 const _bool& bIsSpriteAnimation,
									 const _float& fFrameSpeed,
									 const _vec3& vRectOffset,
									 const _vec3& vRectScale,
									 const _long& iUIDepth = 1000);
	virtual HRESULT	LateInit_GameObject();
	virtual _int	Update_GameObject(const _float& fTimeDelta);
	virtual _int	LateUpdate_GameObject(const _float& fTimeDelta);
	virtual void	Render_GameObject(const _float& fTimeDelta);
private:
	void			Set_ConstantTable();

private:
	/*__________________________________________________________________________________________________________
	[ Value ]
	____________________________________________________________________________________________________________*/
	_float m_fPercent = 1.0f;
	_int	m_iCurHp  = 0;
	_int	m_iMaxHp  = 0;

	_vec2 vOffset = _vec2(0.0f);

public:
	static Engine::CGameObject* Create(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList,
									   wstring wstrRootObjectTag,
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

