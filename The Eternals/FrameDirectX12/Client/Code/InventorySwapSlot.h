#pragma once
#include "GameUIRoot.h"

class CInventoryEquipmentMgr;

class CInventorySwapSlot : public CGameUIRoot
{
private:
	explicit CInventorySwapSlot(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList);
	virtual ~CInventorySwapSlot() = default;

public:
	const ITEM_DESC&	Get_CurItemInfo()	{ return m_tCurItemInfo; }
	const _uint&		Get_CurItemCnt()	{ return m_uiCnt; }
	const _uint&		Get_ItemSlotIdx()	{ return m_uiItemSlotIdx; }
	void Set_CurItemInfo(const char& chItemType, const char& chItemName, const _int& iCnt);
	void Set_CurItemCnt(const _int& iCnt)	{ m_uiCnt = iCnt; }
	void Set_ItemSlotIdx(const _uint& iIdx) { m_uiItemSlotIdx = iIdx; }

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
	void SetUp_ItemIcon();
private:
	/*__________________________________________________________________________________________________________
	[ Mgr ]
	____________________________________________________________________________________________________________*/
	CInventoryEquipmentMgr* m_pInvenEquipMgr = nullptr;

	/*__________________________________________________________________________________________________________
	[ Value ]
	____________________________________________________________________________________________________________*/
	ITEM_DESC	m_tCurItemInfo;
	char		m_chPreItemName = NO_ITEM;
	_uint		m_uiCnt = 0;

	_uint		m_uiItemSlotIdx = 0;
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

