#pragma once
#include "GameUIRoot.h"

class CQuickSlotSwapSlot : public CGameUIRoot
{
private:
	explicit CQuickSlotSwapSlot(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList);
	virtual ~CQuickSlotSwapSlot() = default;
public:
	const char&		Get_QuickSlotName() { return m_chCurSlotName; }
	const _uint&	Get_QuickSlotIdx()	{ return m_uiIdx; }
	void Set_CurQuickSlotName(const char& chSlot)	{ m_chCurSlotName = chSlot; }
	void Set_QuickSlotIdx(const _uint& uiIdx)		{ m_uiIdx = uiIdx; }

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
	void SetUp_SlotIcon();
private:
	/*__________________________________________________________________________________________________________
	[ Value ]
	____________________________________________________________________________________________________________*/
	char m_chCurSlotName = EMPTY_SLOT;
	char m_chPreSlotName = EMPTY_SLOT;
	_uint m_uiIdx = 0;

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

