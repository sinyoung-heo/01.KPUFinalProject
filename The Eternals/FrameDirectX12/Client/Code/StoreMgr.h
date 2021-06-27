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
	map<wstring, CStoreTab*>&	Get_StoreTabList()	{ return m_mapStoreTab; }
	const STORE_STATE&			Get_StoreState()	{ return m_eCurStoreState; }
	void Set_StoreState(const STORE_STATE& eState);
	void Set_StoreItemType(const char& chItemType);

	void		Add_StoreTab(wstring wstrTag, CStoreTab* pTab);
	void		Add_StoreItemSlot(CStoreItemSlot* pSlot);
	CStoreTab*	Find_StoreTab(wstring wstrTag);
private:
	STORE_STATE m_eCurStoreState = STORE_STATE::STORE_STATE_END;
	STORE_STATE m_ePreStoreState = STORE_STATE::STORE_STATE_END;

	// Tab && StoreItemSlot
	map<wstring, CStoreTab*>	m_mapStoreTab;
	vector<CStoreItemSlot*>		m_vecStoreItemSlot;
	char m_chCurItemType = NO_ITEM;
	char m_chPreItemType = NO_ITEM;

private:
	virtual void Free();
};

