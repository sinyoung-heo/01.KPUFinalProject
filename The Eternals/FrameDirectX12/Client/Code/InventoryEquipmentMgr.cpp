#include "stdafx.h"
#include "InventoryEquipmentMgr.h"

IMPLEMENT_SINGLETON(CInventoryEquipmentMgr)

CInventoryEquipmentMgr::CInventoryEquipmentMgr()
{
	m_vecInventorySlot.reserve(80);
}

void CInventoryEquipmentMgr::Add_InventorySlot(CInventoryItemSlot* pSlot)
{
	if (nullptr != pSlot)
		m_vecInventorySlot.emplace_back(pSlot);
}

void CInventoryEquipmentMgr::Push_ItemInventory(const char& chItemType, const char& chItemName, const _uint& uiCnt)
{
	if (m_uiCurSlotSize >= m_uiMaxSlotSize)
	{
		cout << "Inventory is Full ! " << endl;
		return;
	}

	for (auto& pSlot : m_vecInventorySlot)
	{
		if (NO_ITEM == pSlot->Get_CurItemInfo().chItemType)
		{
			pSlot->Set_CurItemInfo(chItemType, chItemName, uiCnt);
			++m_uiCurSlotSize;

			break;
		}
	}
}

void CInventoryEquipmentMgr::Pop_ItemInventory(const char& chItemType, const char& chItemName, const _uint& uiCnt)
{
	if (m_uiCurSlotSize <= 0)
	{
		cout << "Inventory is Empty ! " << endl;
		return;
	}

	for (auto& pSlot : m_vecInventorySlot)
	{
		if (chItemType == pSlot->Get_CurItemInfo().chItemType &&
			chItemName == pSlot->Get_CurItemInfo().chItemName)
		{
			ITEM_INFO tItemInfo = pSlot->Get_CurItemInfo();

			pSlot->Set_CurItemInfo(NO_ITEM, NO_ITEM, 0);
			--m_uiCurSlotSize;

			break;
		}
	}
}

void CInventoryEquipmentMgr::Pop_ItemInventory(const _uint& uiIdx)
{
	if (uiIdx < 0 || uiIdx >= m_uiMaxSlotSize)
	{
		cout << "Error Idx ! " << endl;
		return;
	}

	if (NO_ITEM != m_vecInventorySlot[uiIdx]->Get_CurItemInfo().chItemType)
	{
		ITEM_INFO tItemInfo = m_vecInventorySlot[uiIdx]->Get_CurItemInfo();
		
		m_vecInventorySlot[uiIdx]->Set_CurItemInfo(NO_ITEM, NO_ITEM, 0);
		--m_uiCurSlotSize;
	}
}

void CInventoryEquipmentMgr::Pop_ItemInventory()
{
	if (m_uiCurSlotSize <= 0)
	{
		cout << "Inventory is Empty ! " << endl;
		return;
	}

	for (auto& pSlot : m_vecInventorySlot)
	{
		if (NO_ITEM != pSlot->Get_CurItemInfo().chItemType)
		{
			ITEM_INFO tItemInfo = pSlot->Get_CurItemInfo();

			pSlot->Set_CurItemInfo(NO_ITEM, NO_ITEM, 0);
			--m_uiCurSlotSize;

			break;
		}
	}
}

void CInventoryEquipmentMgr::Free()
{
	m_pInventoryCanvas = nullptr;
	m_pEquipmentCanvas = nullptr;
}
