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

private:
	virtual void Free();
};

