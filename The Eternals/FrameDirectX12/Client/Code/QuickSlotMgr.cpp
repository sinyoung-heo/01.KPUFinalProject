#include "stdafx.h"
#include "QuickSlotMgr.h"

IMPLEMENT_SINGLETON(CQuickSlotMgr)

CQuickSlotMgr::CQuickSlotMgr()
{
	m_vecQuickSlot.reserve(10);
}

_uint uiQuickSlotIdx = 0;
void CQuickSlotMgr::Add_QuickSlot(CQuickSlot* pSlot)
{
	if (nullptr != pSlot)
	{
		m_vecQuickSlot.emplace_back(pSlot);
		pSlot->Set_QuickSlotIdx(uiQuickSlotIdx);
		pSlot->Set_DIKKey(uiQuickSlotIdx + 2);
		++uiQuickSlotIdx;
	}
}

void CQuickSlotMgr::Free()
{
	m_pQuickSlotSwapSlot = nullptr;
	m_vecQuickSlot.clear();
	m_vecQuickSlot.shrink_to_fit();
}
