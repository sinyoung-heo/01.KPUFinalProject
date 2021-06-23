#include "stdafx.h"
#include "InventoryEquipmentMgr.h"

IMPLEMENT_SINGLETON(CInventoryEquipmentMgr)

CInventoryEquipmentMgr::CInventoryEquipmentMgr()
{
}

void CInventoryEquipmentMgr::Free()
{
	m_pInventoryCanvas = nullptr;
	m_pEquipmentCanvas = nullptr;
}
