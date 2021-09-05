#pragma once
#include "Include.h"
#include "Base.h"
#include "QuickSlot.h"
#include "QuickSlotSwapSlot.h"

class CQuickSlotMgr : public Engine::CBase
{
	DECLARE_SINGLETON(CQuickSlotMgr)

private:
	explicit CQuickSlotMgr();
	virtual ~CQuickSlotMgr() = default;
public:
	vector<CQuickSlot*>&	Get_QuickSlotList()				{ return m_vecQuickSlot; }
	CQuickSlotSwapSlot*		Get_QuickSlotSwapSlot()			{ return m_pQuickSlotSwapSlot; }
	const _bool&			Get_IsQuickSlotSwapState()		{ return m_bIsQuickSlotSwapState; }
	void Set_QuickSlotSwapSlot(CQuickSlotSwapSlot* pSlot)	{ m_pQuickSlotSwapSlot = pSlot; }
	void Set_IsQuickSlotSwapState(const _bool& bIsSwap)		{ m_bIsQuickSlotSwapState = bIsSwap; }
	void Add_QuickSlot(CQuickSlot* pSlot);

private:
	vector<CQuickSlot*> m_vecQuickSlot;

	CQuickSlotSwapSlot* m_pQuickSlotSwapSlot = nullptr;
	_bool				m_bIsQuickSlotSwapState = false;

private:
	virtual void Free();
};

