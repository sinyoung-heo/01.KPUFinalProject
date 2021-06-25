#pragma once
#include "GameUIChild.h"

class CGameUIChild;
class CInventoryItemSlot;

class CEquipmentItemSlot : public CGameUIChild
{
private:
	explicit CEquipmentItemSlot(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList);
	virtual ~CEquipmentItemSlot() = default;

public:
	const ITEM_INFO&	Get_CurItemInfo()			{ return m_tCurItemInfo; }
	const _uint&		Get_InventorySlotIdx()		{ return m_uiInventoryIdx; }
	const _bool&		Get_IsOnEquipment()			{ return m_bIsOnEquipment; }
	CInventoryItemSlot* Get_InventoryItemSlot()		{ return m_pInventoryItemSlot; }
	void Set_CurItemInfo(const char& chItemType, const char& chItemName) { m_tCurItemInfo.chItemType = chItemType; m_tCurItemInfo.chItemName = chItemName; };
	void Set_InventorySlotIdx(const _uint& uiSlotIdx)		{ m_uiInventoryIdx = uiSlotIdx; }
	void Set_IsOnEquipment(const _bool& bIsOn)				{ m_bIsOnEquipment = bIsOn; }
	void Set_InventorySlotClass(CInventoryItemSlot* pUI)	{ m_pInventoryItemSlot = pUI; }

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
	void KeyInput_MouseButton(const _float& fTimeDelta);
	void SetUp_ItemIcon();
private:
	/*__________________________________________________________________________________________________________
	[ Value ]
	____________________________________________________________________________________________________________*/
	CInventoryItemSlot* m_pInventoryItemSlot = nullptr;

	_bool m_bIsKeyPressingRB = false;

	CGameUIChild*	m_pSlotFrame     = nullptr;	// 마우스 이벤트 - 붉은색 테두리.
	_uint			m_uiSlotFrameIdx = 1;
	_uint			m_bIsOnMouse     = false;

	ITEM_INFO	m_tNoItemInfo;
	ITEM_INFO	m_tCurItemInfo;
	char		m_chPreItemType = NO_ITEM;
	char		m_chPreItemName = NO_ITEM;

	_bool m_bIsOnEquipment = false;
	_uint m_uiInventoryIdx = NO_ITEM;

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

