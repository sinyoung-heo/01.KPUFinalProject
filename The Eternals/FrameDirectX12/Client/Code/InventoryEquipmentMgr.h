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

	CInventoryCanvas*	Get_InventoryCanvasClass()	{ return m_pInventoryCanvas; }
	CEquipmentCanvas*	Get_EquipmentCanvasClass()	{ return m_pEquipmentCanvas; }
	const _uint&		Get_MaxInventorySize()		{ return m_uiMaxSlotSize; }
	const _uint&		Get_CurInventorySize()		{ return m_uiCurSlotSize; }
	void Set_InventoryCanvasClass(CInventoryCanvas* pUI) { m_pInventoryCanvas = pUI; };
	void Set_EquipmentCanvasClass(CEquipmentCanvas* pUI) { m_pEquipmentCanvas = pUI; };
	void Set_CurInventorySize(const _uint& uiValue) { m_uiCurSlotSize = uiValue; }

	void Add_InventorySlot(CInventoryItemSlot* pSlot);
	void Push_ItemInventory(const char& chItemType, const char& chItemName, const _int& iCnt = 1);
	void Pop_ItemInventory(const char& chItemType, const char& chItemName, const _int& uiCnt = -1);
	void Pop_ItemInventory(const _uint& uiIdx);
	void Pop_ItemInventory();
private:
	CInventoryCanvas*			m_pInventoryCanvas = nullptr;
	vector<CInventoryItemSlot*> m_vecInventorySlot;
	_uint						m_uiMaxSlotSize = 80;
	_uint						m_uiCurSlotSize = 0;

	CEquipmentCanvas* m_pEquipmentCanvas = nullptr;


private:
	virtual void Free();
};

