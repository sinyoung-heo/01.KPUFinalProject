#pragma once
#include "Include.h"
#include "Base.h"
#include "QuickSlot.h"

class CQuickSlotMgr : public Engine::CBase
{
	DECLARE_SINGLETON(CQuickSlotMgr)

private:
	explicit CQuickSlotMgr();
	virtual ~CQuickSlotMgr() = default;
public:
	vector<CQuickSlot*>& Get_QuickSlotList() { return m_vecQuickSlot; }
	void Add_QuickSlot(CQuickSlot* pSlot);
private:
	vector<CQuickSlot*> m_vecQuickSlot;

private:
	virtual void Free();
};

