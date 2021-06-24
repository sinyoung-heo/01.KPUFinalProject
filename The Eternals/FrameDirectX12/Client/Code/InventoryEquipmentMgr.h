#pragma once
#include "Include.h"
#include "Base.h"
#include "InventoryCanvas.h"
#include "InventoryItemSlot.h"
#include "InventoryButtonClose.h"
#include "EquipmentCanvas.h"
#include "EquipmentItemSlot.h"
#include "EquipmentButtonClose.h"

class CInventoryEquipmentMgr : public Engine::CBase
{
	DECLARE_SINGLETON(CInventoryEquipmentMgr)

private:
	explicit CInventoryEquipmentMgr();
	virtual ~CInventoryEquipmentMgr() = default;
public:
	CInventoryCanvas* Get_InventoryCanvasClass() { return m_pInventoryCanvas; }
	CEquipmentCanvas* Get_EquipmentCanvasClass() { return m_pEquipmentCanvas; }

	void Set_InventoryCanvasClass(CInventoryCanvas* pUI) { m_pInventoryCanvas = pUI; };
	void Set_EquipmentCanvasClass(CEquipmentCanvas* pUI) { m_pEquipmentCanvas = pUI; };

private:
	CInventoryCanvas* m_pInventoryCanvas = nullptr;
	CEquipmentCanvas* m_pEquipmentCanvas = nullptr;

private:
	virtual void Free();
};

