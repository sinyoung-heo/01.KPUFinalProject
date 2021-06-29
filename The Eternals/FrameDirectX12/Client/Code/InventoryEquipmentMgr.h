#pragma once
#include "Include.h"
#include "Base.h"
#include "InventoryCanvas.h"
#include "InventoryItemSlot.h"
#include "InventoryButtonClose.h"
#include "InventorySwapSlot.h"
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
	vector<CInventoryItemSlot*>&		Get_InventorySlotList()			{ return m_vecInventorySlot; }
	CInventoryCanvas*					Get_InventoryCanvasClass()		{ return m_pInventoryCanvas; }
	CInventorySwapSlot*					Get_InventorySwapSlotClass()	{ return m_pInventorySwapSlot; }
	CEquipmentCanvas*					Get_EquipmentCanvasClass()		{ return m_pEquipmentCanvas; }
	const _uint&						Get_MaxInventorySize()			{ return m_uiMaxSlotSize; }
	const _uint&						Get_CurInventorySize()			{ return m_uiCurSlotSize; }
	const _bool&						Get_IsInventoryItemSwapState()	{ return m_bIsInventoryItemSwapState; }
	const char&							Get_ThisPlayerJob()				{ return m_chJob; }
	map<wstring, CEquipmentItemSlot*>&	Get_EquipmentSlotMap()			{ return m_mapEquipmentSlot; }
	CInventoryItemSlot*					Get_HpPotionSlot()				{ return m_pHpPotionSlot; }
	CInventoryItemSlot*					Get_MpPotionSlot()				{ return m_pMpPotionSlot; }

	void Set_InventoryCanvasClass(CInventoryCanvas* pUI)		{ m_pInventoryCanvas = pUI; }
	void Set_InventorySwapSlotClass(CInventorySwapSlot* pUI)	{ m_pInventorySwapSlot = pUI; }
	void Set_IsInventoryItemSwapState(const _bool& bIsSwap)		{ m_bIsInventoryItemSwapState = bIsSwap; }
	void Set_CurInventorySize(const _uint& uiValue)				{ m_uiCurSlotSize = uiValue; }
	void Set_EquipmentCanvasClass(CEquipmentCanvas* pUI)		{ m_pEquipmentCanvas = pUI; }
	void Set_ThisPlayerJob(const char& chJob)					{ m_chJob = chJob; }
	void Set_HpPotionSlot(CInventoryItemSlot* pSlot) { m_pHpPotionSlot = pSlot; }
	void Set_MpPotionSlot(CInventoryItemSlot* pSlot) { m_pMpPotionSlot = pSlot; }

	void Add_InventorySlot(CInventoryItemSlot* pSlot);
	void Push_ItemInventory(const char& chItemType, const char& chItemName, const _int& iCnt = 1);
	void Pop_ItemInventory(const char& chItemType, const char& chItemName, const _int& uiCnt = -1);
	void Pop_ItemInventory(const _uint& uiIdx);
	void Pop_ItemInventory();

	void Add_EquipmentSlot(wstring wstrTag, CEquipmentItemSlot* pSlot);
private:
	vector<CInventoryItemSlot*> m_vecInventorySlot;
	CInventoryCanvas*			m_pInventoryCanvas          = nullptr;
	_uint						m_uiMaxSlotSize             = 80;
	_uint						m_uiCurSlotSize             = 0;
	CInventorySwapSlot*			m_pInventorySwapSlot        = nullptr;
	_bool						m_bIsInventoryItemSwapState = false;

	CInventoryItemSlot* m_pHpPotionSlot = nullptr;
	CInventoryItemSlot* m_pMpPotionSlot = nullptr;

	CEquipmentCanvas* m_pEquipmentCanvas = nullptr;
	map<wstring, CEquipmentItemSlot*> m_mapEquipmentSlot;
	char m_chJob = PC_GLADIATOR;
private:
	virtual void Free();
};

