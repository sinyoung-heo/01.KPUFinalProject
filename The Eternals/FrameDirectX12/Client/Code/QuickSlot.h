#pragma once
#include "GameUIChild.h"

class CQuickSlotMgr;

class CQuickSlot : public CGameUIChild
{
private:
	explicit CQuickSlot(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList);
	virtual ~CQuickSlot() = default;
public:
	const _uint&		Get_CurItemCnt()	{ return m_uiCnt; }
	const _uint&		Get_ItemSlotIdx()	{ return m_uiIdx; }
	const _uint&		Get_DIKKey()		{ return m_uiDIK_Key; }
	const char&			Get_QuickSlotName() { return m_chCurSlotName; }
	void Set_CurQuickSlotName(const char& chSlot)	{ m_chCurSlotName = chSlot; }
	void Set_QuickSlotIconIdx(const _uint& uiIdx)	{ m_uiSlotIncoIdx = uiIdx; }
	void Set_CurItemCnt(const _int& iCnt)			{ m_uiCnt = iCnt; }
	void Set_QuickSlotIdx(const _uint& uiIdx)		{ m_uiIdx = uiIdx; }
	void Set_DIKKey(const _uint& uiKey)				{ m_uiDIK_Key = uiKey; }

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

	void SetUp_SlotIcon();
private:
	void KeyInput_MouseButton(const _float& fTimeDelta);
	void SetUp_FontPotionCnt(const _float& fTimeDelta);
	void SetUp_FontDIKKey(const _float& fTimeDelta);
private:
	/*__________________________________________________________________________________________________________
	[ Value ]
	____________________________________________________________________________________________________________*/
	CQuickSlotMgr* m_pQuickSlotMgr = nullptr;

	_bool m_bIsKeyPressingLB = false;
	_bool m_bIsKeyPressingRB = false;

	char m_chCurSlotName = EMPTY_SLOT;
	char m_chPreSlotName = EMPTY_SLOT;

	CGameUIChild*	m_pSlotIcon     = nullptr;
	_uint			m_uiSlotIncoIdx = 0;
	CGameUIChild*	m_pSlotFrame    = nullptr;	// 마우스 이벤트 - 붉은색 테두리.
	_bool			m_bIsOnMouse    = false;

	// 포션 개수.
	_uint m_uiCnt = 0;

	// SlotIdx
	_uint m_uiIdx     = 0;
	_uint m_uiDIK_Key = 0;

	Engine::CFont* m_pFontPotionCnt = nullptr;

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

