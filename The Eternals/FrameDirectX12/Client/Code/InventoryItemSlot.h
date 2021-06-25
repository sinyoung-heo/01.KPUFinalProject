#pragma once
#include "GameUIChild.h"

class CInventoryEquipmentMgr;

class CInventoryItemSlot : public CGameUIChild
{
private:
	explicit CInventoryItemSlot(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList);
	virtual ~CInventoryItemSlot() = default;

public:
	const ITEM_INFO&	Get_CurItemInfo()	{ return m_tCurItemInfo; }
	const _uint&		Get_CurItemCnt()	{ return m_uiCnt; }
	const _uint&		Get_ItemSlotIdx()	{ return m_uiIdx; }
	const _bool&		Get_IsOnEquipment() { return m_bIsOnEquipment; }
	wstring				Get_EquipmentTag()	{ return m_wstrEquipSlotTag; }
	void Set_CurItemInfo(const char& chItemType, const char& chItemName, const _int& iCnt = 0);
	void Set_CurItemCnt(const _int& iCnt)		{ m_uiCnt = iCnt; }
	void Set_ItemSlotIdx(const _uint& uiIdx)	{ m_uiIdx = uiIdx; }
	void Set_IsOnEquipment(const _bool& bIsOn)	{ m_bIsOnEquipment = bIsOn; }
	void Set_EquipSlotTag(wstring wstrTag)		{ m_wstrEquipSlotTag = wstrTag; }
	void Add_PotionCnt(const _int& iValue)		{ m_uiCnt += iValue; }

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
	void SetUp_FontPotionCnt(const _float& fTimeDelta);
private:
	/*__________________________________________________________________________________________________________
	[ Mgr ]
	____________________________________________________________________________________________________________*/
	CInventoryEquipmentMgr* m_pInvenEquipMgr = nullptr;

	/*__________________________________________________________________________________________________________
	[ Value ]
	____________________________________________________________________________________________________________*/
	_bool m_bIsKeyPressingLB = false;
	_bool m_bIsKeyPressingRB = false;

	wstring			m_wstrEquipSlotTag = L"";
	_bool			m_bIsOnEquipment = false;	// 아이템 장착	- 노란색 테두리.
	CGameUIChild*	m_pSlotFrame     = nullptr;	// 마우스 이벤트 - 붉은색 테두리.
	_uint			m_uiSlotFrameIdx = 2;
	_uint			m_bIsOnMouse     = false;

	ITEM_INFO	m_tNoItemInfo;
	ITEM_INFO	m_tCurItemInfo;
	char		m_chPreItemType = NO_ITEM;
	char		m_chPreItemName = NO_ITEM;
	char		m_chJob = -1;

	// 포션 개수.
	_uint m_uiCnt = 0;

	// SlotIdx
	_uint m_uiIdx = 0;
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

