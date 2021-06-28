#pragma once
#include "Include.h"
#include "Base.h"
#include "InGameStoreCanvas.h"
#include "StoreButtonClose.h"
#include "StoreTab.h"
#include "StoreItemSlot.h"
#include "StoreBuySellList.h"
#include "StoreBuyListSlot.h"
#include "StoreSellListSlot.h"
#include "StoreButtonCalculate.h"
#include "StoreBuySellListBack.h"
#include "StoreItemSlotBack.h"

class CStoreMgr : public Engine::CBase
{
	DECLARE_SINGLETON(CStoreMgr)

private:
	explicit CStoreMgr();
	virtual ~CStoreMgr() = default;
public:
	map<wstring, CStoreTab*>&		Get_StoreTabList()		{ return m_mapStoreTab; }
	const STORE_STATE&				Get_StoreState()		{ return m_eCurStoreState; }
	vector<CStoreBuyListSlot*>&		Get_StoreBuySlotList()	{ return m_vecBuySlotList; }
	vector<CStoreSellListSlot*>&	Get_StoreSellSlotList()	{ return m_vecSellSlotList; }
	const _uint&					Get_MaxBuySlotSize()	{ return m_uiMaxBuySlotSize; }
	const _uint&					Get_MaxSellSlotSize()	{ return m_uiMaxSellSlotSize; }
	const _uint&					Get_CurBuySlotSize()	{ return m_uiCurBuySlotSize; }
	const _uint&					Get_CurSellSlotSize()	{ return m_uiCurSellSlotSize; }
	void Set_StoreState(const STORE_STATE& eState);
	void Set_StoreItemType(const char& chItemType);
	void Set_CurBuySlotSize(const _uint& uiSize)	{ m_uiCurBuySlotSize = uiSize; }
	void Set_CurSellSlotSize(const _uint& uiSize)	{ m_uiCurSellSlotSize = uiSize; }

	void		Add_StoreTab(wstring wstrTag, CStoreTab* pTab);
	void		Add_StoreItemSlot(CStoreItemSlot* pSlot);
	void		Add_StoreBuyItemSlot(CStoreBuyListSlot* pSlot);
	void		Add_StoreSellItemSlot(CStoreSellListSlot* pSlot);
	CStoreTab*	Find_StoreTab(wstring wstrTag);

	// ItemBuySlot
	void Reset_StoreItemBuySlotList();
	void Push_StoreItemBuySlot(const char& chItemType, const char& chItemName, const _uint& uiCnt);
	void Min_StoreBuySlotSize() { if (m_uiCurBuySlotSize > 0) --m_uiCurBuySlotSize; }
	// ItemSellSlot
	void Reset_StoreItemSellSlotList();
	void Push_StoreItemSellSlot(const char& chItemType, const char& chItemName, const _uint& uiInventoryIdx);
	void Min_StoreSellSlotSize() { if (m_uiCurSellSlotSize > 0) --m_uiCurSellSlotSize; }

private:
	STORE_STATE m_eCurStoreState = STORE_STATE::STORE_STATE_END;
	STORE_STATE m_ePreStoreState = STORE_STATE::STORE_STATE_END;

	// Tab && StoreItemSlot
	map<wstring, CStoreTab*>	m_mapStoreTab;
	vector<CStoreItemSlot*>		m_vecStoreItemSlot;
	char m_chCurItemType = NO_ITEM;
	char m_chPreItemType = NO_ITEM;

	// 구매 & 판매 목록 ItemSlot.
	vector<CStoreBuyListSlot*>	m_vecBuySlotList;
	_uint						m_uiMaxBuySlotSize = 12;
	_uint						m_uiCurBuySlotSize = 0;
	_int						m_iBuyItemSumGold  = 0;

	vector<CStoreSellListSlot*> m_vecSellSlotList;
	_uint						m_uiMaxSellSlotSize = 12;
	_uint						m_uiCurSellSlotSize = 0;
	_int						m_iSellItemSumGold  = 0;
private:
	virtual void Free();
};

