#include "stdafx.h"
#include "StoreMgr.h"
#include "Font.h"
#include "InventoryEquipmentMgr.h"

IMPLEMENT_SINGLETON(CStoreMgr)

CStoreMgr::CStoreMgr()
{
	m_vecBuySlotList.reserve(16);
	m_vecBuySlotList.reserve(16);
}

void CStoreMgr::Set_StoreState(const STORE_STATE& eState)
{
	m_eCurStoreState = eState;

	if (m_eCurStoreState != m_ePreStoreState)
	{
		m_ePreStoreState = m_eCurStoreState;

		switch (m_eCurStoreState)
		{
		case STORE_WEAPON:
		{
			m_mapStoreTab[L"UIStoreItemTab1"]->Set_IsActive(true);
			m_mapStoreTab[L"UIStoreItemTab2"]->Set_IsActive(true);
			m_mapStoreTab[L"UIStoreItemTab3"]->Set_IsActive(true);
			m_mapStoreTab[L"UIStoreItemTab1"]->Get_Font()->Set_Text(L"검");
			m_mapStoreTab[L"UIStoreItemTab2"]->Get_Font()->Set_Text(L"활");
			m_mapStoreTab[L"UIStoreItemTab3"]->Get_Font()->Set_Text(L"로드");
			m_mapStoreTab[L"UIStoreItemTab1"]->Set_FontTextPosOffset(_vec2(-12.0f, -16.0f));
			m_mapStoreTab[L"UIStoreItemTab2"]->Set_FontTextPosOffset(_vec2(-12.0f, -16.0f));
			m_mapStoreTab[L"UIStoreItemTab3"]->Set_FontTextPosOffset(_vec2(-22.0f, -16.0f));
			m_mapStoreTab[L"UIStoreItemTab1"]->Set_IsSelected(true);
			m_mapStoreTab[L"UIStoreItemTab2"]->Set_IsSelected(false);
			m_mapStoreTab[L"UIStoreItemTab3"]->Set_IsSelected(false);
			m_mapStoreTab[L"UIStoreItemTab1"]->Set_ItemType(ItemType_WeaponTwoHand);
			m_mapStoreTab[L"UIStoreItemTab2"]->Set_ItemType(ItemType_WeaponBow);
			m_mapStoreTab[L"UIStoreItemTab3"]->Set_ItemType(ItemType_WeaponRod);
		}
			break;
		case STORE_ARMOR:
		{
			m_mapStoreTab[L"UIStoreItemTab1"]->Set_IsActive(true);
			m_mapStoreTab[L"UIStoreItemTab2"]->Set_IsActive(true);
			m_mapStoreTab[L"UIStoreItemTab3"]->Set_IsActive(true);
			m_mapStoreTab[L"UIStoreItemTab1"]->Get_Font()->Set_Text(L"투구");
			m_mapStoreTab[L"UIStoreItemTab2"]->Get_Font()->Set_Text(L"갑옷");
			m_mapStoreTab[L"UIStoreItemTab3"]->Get_Font()->Set_Text(L"신발");
			m_mapStoreTab[L"UIStoreItemTab1"]->Set_FontTextPosOffset(_vec2(-22.0f, -16.0f));
			m_mapStoreTab[L"UIStoreItemTab2"]->Set_FontTextPosOffset(_vec2(-22.0f, -16.0f));
			m_mapStoreTab[L"UIStoreItemTab3"]->Set_FontTextPosOffset(_vec2(-22.0f, -16.0f));
			m_mapStoreTab[L"UIStoreItemTab1"]->Set_IsSelected(true);
			m_mapStoreTab[L"UIStoreItemTab2"]->Set_IsSelected(false);
			m_mapStoreTab[L"UIStoreItemTab3"]->Set_IsSelected(false);
			m_mapStoreTab[L"UIStoreItemTab1"]->Set_ItemType(ItemType_Helmet);
			m_mapStoreTab[L"UIStoreItemTab2"]->Set_ItemType(ItemType_Armor);
			m_mapStoreTab[L"UIStoreItemTab3"]->Set_ItemType(ItemType_Shoes);
		}
			break;
		case STORE_POTION:
		{
			m_mapStoreTab[L"UIStoreItemTab1"]->Set_IsActive(true);
			m_mapStoreTab[L"UIStoreItemTab2"]->Set_IsActive(false);
			m_mapStoreTab[L"UIStoreItemTab3"]->Set_IsActive(false);

			m_mapStoreTab[L"UIStoreItemTab1"]->Get_Font()->Set_Text(L"물약");
			m_mapStoreTab[L"UIStoreItemTab1"]->Set_FontTextPosOffset(_vec2(-22.0f, -16.0f));
			m_mapStoreTab[L"UIStoreItemTab1"]->Set_IsSelected(true);
			m_mapStoreTab[L"UIStoreItemTab1"]->Set_ItemType(ItemType_Potion);
		}
			break;
		case STORE_STATE_END:
		{
			m_mapStoreTab[L"UIStoreItemTab1"]->Set_IsActive(false);
			m_mapStoreTab[L"UIStoreItemTab2"]->Set_IsActive(false);
			m_mapStoreTab[L"UIStoreItemTab3"]->Set_IsActive(false);
		}
			break;
		default:
			break;
		}
	}
}

void CStoreMgr::Set_StoreItemType(const char& chItemType)
{
	m_chCurItemType = chItemType;

	if (m_chCurItemType != m_chPreItemType)
	{
		switch (m_chCurItemType)
		{
		case ItemType_WeaponTwoHand:
		{
			m_vecStoreItemSlot[ITEM_GRADE::ITEM_D]->Set_IsActive(true);
			m_vecStoreItemSlot[ITEM_GRADE::ITEM_C]->Set_IsActive(true);
			m_vecStoreItemSlot[ITEM_GRADE::ITEM_B]->Set_IsActive(true);
			m_vecStoreItemSlot[ITEM_GRADE::ITEM_A]->Set_IsActive(true);
			m_vecStoreItemSlot[ITEM_GRADE::ITEM_S]->Set_IsActive(true);
			m_vecStoreItemSlot[ITEM_GRADE::ITEM_D]->Set_CurItemInfo(ItemType_WeaponTwoHand, Twohand19_A_SM);
			m_vecStoreItemSlot[ITEM_GRADE::ITEM_C]->Set_CurItemInfo(ItemType_WeaponTwoHand, TwoHand27_SM);
			m_vecStoreItemSlot[ITEM_GRADE::ITEM_B]->Set_CurItemInfo(ItemType_WeaponTwoHand, TwoHand29_SM);
			m_vecStoreItemSlot[ITEM_GRADE::ITEM_A]->Set_CurItemInfo(ItemType_WeaponTwoHand, TwoHand31_SM);
			m_vecStoreItemSlot[ITEM_GRADE::ITEM_S]->Set_CurItemInfo(ItemType_WeaponTwoHand, TwoHand33_B_SM);
		}
			break;
		case ItemType_WeaponBow:
		{
			m_vecStoreItemSlot[ITEM_GRADE::ITEM_D]->Set_IsActive(true);
			m_vecStoreItemSlot[ITEM_GRADE::ITEM_C]->Set_IsActive(true);
			m_vecStoreItemSlot[ITEM_GRADE::ITEM_B]->Set_IsActive(true);
			m_vecStoreItemSlot[ITEM_GRADE::ITEM_A]->Set_IsActive(true);
			m_vecStoreItemSlot[ITEM_GRADE::ITEM_S]->Set_IsActive(true);
			m_vecStoreItemSlot[ITEM_GRADE::ITEM_D]->Set_CurItemInfo(ItemType_WeaponBow, Bow18_A_SM);
			m_vecStoreItemSlot[ITEM_GRADE::ITEM_C]->Set_CurItemInfo(ItemType_WeaponBow, Bow27_SM);
			m_vecStoreItemSlot[ITEM_GRADE::ITEM_B]->Set_CurItemInfo(ItemType_WeaponBow, Bow23_SM);
			m_vecStoreItemSlot[ITEM_GRADE::ITEM_A]->Set_CurItemInfo(ItemType_WeaponBow, Bow31_SM);
			m_vecStoreItemSlot[ITEM_GRADE::ITEM_S]->Set_CurItemInfo(ItemType_WeaponBow, Event_Season_Bow_01_SM);
		}
			break;
		case ItemType_WeaponRod:
		{
			m_vecStoreItemSlot[ITEM_GRADE::ITEM_D]->Set_IsActive(true);
			m_vecStoreItemSlot[ITEM_GRADE::ITEM_C]->Set_IsActive(true);
			m_vecStoreItemSlot[ITEM_GRADE::ITEM_B]->Set_IsActive(true);
			m_vecStoreItemSlot[ITEM_GRADE::ITEM_A]->Set_IsActive(true);
			m_vecStoreItemSlot[ITEM_GRADE::ITEM_S]->Set_IsActive(true);
			m_vecStoreItemSlot[ITEM_GRADE::ITEM_D]->Set_CurItemInfo(ItemType_WeaponRod, Event_Wit_Rod_01);
			m_vecStoreItemSlot[ITEM_GRADE::ITEM_C]->Set_CurItemInfo(ItemType_WeaponRod, Rod19_A);
			m_vecStoreItemSlot[ITEM_GRADE::ITEM_B]->Set_CurItemInfo(ItemType_WeaponRod, Rod28_B);
			m_vecStoreItemSlot[ITEM_GRADE::ITEM_A]->Set_CurItemInfo(ItemType_WeaponRod, Rod31);
			m_vecStoreItemSlot[ITEM_GRADE::ITEM_S]->Set_CurItemInfo(ItemType_WeaponRod, Rod33_B);
		}
			break;
		case ItemType_Helmet:
		{
			m_vecStoreItemSlot[ITEM_GRADE::ITEM_D]->Set_IsActive(true);
			m_vecStoreItemSlot[ITEM_GRADE::ITEM_C]->Set_IsActive(true);
			m_vecStoreItemSlot[ITEM_GRADE::ITEM_B]->Set_IsActive(true);
			m_vecStoreItemSlot[ITEM_GRADE::ITEM_A]->Set_IsActive(true);
			m_vecStoreItemSlot[ITEM_GRADE::ITEM_S]->Set_IsActive(true);
			m_vecStoreItemSlot[ITEM_GRADE::ITEM_D]->Set_CurItemInfo(ItemType_Helmet, Helmet_D);
			m_vecStoreItemSlot[ITEM_GRADE::ITEM_C]->Set_CurItemInfo(ItemType_Helmet, Helmet_C);
			m_vecStoreItemSlot[ITEM_GRADE::ITEM_B]->Set_CurItemInfo(ItemType_Helmet, Helmet_B);
			m_vecStoreItemSlot[ITEM_GRADE::ITEM_A]->Set_CurItemInfo(ItemType_Helmet, Helmet_A);
			m_vecStoreItemSlot[ITEM_GRADE::ITEM_S]->Set_CurItemInfo(ItemType_Helmet, Helmet_S);
		}
			break;
		case ItemType_Armor:
		{
			m_vecStoreItemSlot[ITEM_GRADE::ITEM_D]->Set_IsActive(true);
			m_vecStoreItemSlot[ITEM_GRADE::ITEM_C]->Set_IsActive(true);
			m_vecStoreItemSlot[ITEM_GRADE::ITEM_B]->Set_IsActive(true);
			m_vecStoreItemSlot[ITEM_GRADE::ITEM_A]->Set_IsActive(true);
			m_vecStoreItemSlot[ITEM_GRADE::ITEM_S]->Set_IsActive(true);
			m_vecStoreItemSlot[ITEM_GRADE::ITEM_D]->Set_CurItemInfo(ItemType_Armor, Armor_D);
			m_vecStoreItemSlot[ITEM_GRADE::ITEM_C]->Set_CurItemInfo(ItemType_Armor, Armor_C);
			m_vecStoreItemSlot[ITEM_GRADE::ITEM_B]->Set_CurItemInfo(ItemType_Armor, Armor_B);
			m_vecStoreItemSlot[ITEM_GRADE::ITEM_A]->Set_CurItemInfo(ItemType_Armor, Armor_A);
			m_vecStoreItemSlot[ITEM_GRADE::ITEM_S]->Set_CurItemInfo(ItemType_Armor, Armor_S);
		}
			break;
		case ItemType_Shoes:
		{
			m_vecStoreItemSlot[ITEM_GRADE::ITEM_D]->Set_IsActive(true);
			m_vecStoreItemSlot[ITEM_GRADE::ITEM_C]->Set_IsActive(true);
			m_vecStoreItemSlot[ITEM_GRADE::ITEM_B]->Set_IsActive(true);
			m_vecStoreItemSlot[ITEM_GRADE::ITEM_A]->Set_IsActive(true);
			m_vecStoreItemSlot[ITEM_GRADE::ITEM_S]->Set_IsActive(true);
			m_vecStoreItemSlot[ITEM_GRADE::ITEM_D]->Set_CurItemInfo(ItemType_Shoes, Shoes_D);
			m_vecStoreItemSlot[ITEM_GRADE::ITEM_C]->Set_CurItemInfo(ItemType_Shoes, Shoes_C);
			m_vecStoreItemSlot[ITEM_GRADE::ITEM_B]->Set_CurItemInfo(ItemType_Shoes, Shoes_B);
			m_vecStoreItemSlot[ITEM_GRADE::ITEM_A]->Set_CurItemInfo(ItemType_Shoes, Shoes_A);
			m_vecStoreItemSlot[ITEM_GRADE::ITEM_S]->Set_CurItemInfo(ItemType_Shoes, Shoes_S);
		}
			break;
		case ItemType_Potion:
		{
			m_vecStoreItemSlot[ITEM_GRADE::ITEM_D]->Set_IsActive(true);
			m_vecStoreItemSlot[ITEM_GRADE::ITEM_C]->Set_IsActive(true);
			m_vecStoreItemSlot[ITEM_GRADE::ITEM_B]->Set_IsActive(false);
			m_vecStoreItemSlot[ITEM_GRADE::ITEM_A]->Set_IsActive(false);
			m_vecStoreItemSlot[ITEM_GRADE::ITEM_S]->Set_IsActive(false);
			m_vecStoreItemSlot[ITEM_GRADE::ITEM_D]->Set_CurItemInfo(ItemType_Potion, Potion_HP);
			m_vecStoreItemSlot[ITEM_GRADE::ITEM_C]->Set_CurItemInfo(ItemType_Potion, Potion_MP);
			m_vecStoreItemSlot[ITEM_GRADE::ITEM_B]->Set_CurItemInfo(NO_ITEM, NO_ITEM);
			m_vecStoreItemSlot[ITEM_GRADE::ITEM_A]->Set_CurItemInfo(NO_ITEM, NO_ITEM);
			m_vecStoreItemSlot[ITEM_GRADE::ITEM_S]->Set_CurItemInfo(NO_ITEM, NO_ITEM);
		}
			break;
		default:
			break;
		}
	}

	m_chPreItemType = m_chCurItemType;
}

void CStoreMgr::Add_StoreTab(wstring wstrTag, CStoreTab* pTab)
{
	auto iter_find = m_mapStoreTab.find(wstrTag);
	if (m_mapStoreTab.end() == iter_find && nullptr != pTab)
	{
		m_mapStoreTab.emplace(wstrTag, pTab);
	}
}

void CStoreMgr::Add_StoreItemSlot(CStoreItemSlot* pSlot)
{
	if (nullptr != pSlot)
		m_vecStoreItemSlot.emplace_back(pSlot);
}

void CStoreMgr::Add_StoreBuyItemSlot(CStoreBuyListSlot* pSlot)
{
	if (nullptr != pSlot)
		m_vecBuySlotList.emplace_back(pSlot);
}

void CStoreMgr::Add_StoreSellItemSlot(CStoreSellListSlot* pSlot)
{
	if (nullptr != pSlot)
		m_vecSellSlotList.emplace_back(pSlot);
}

CStoreTab* CStoreMgr::Find_StoreTab(wstring wstrTag)
{
	auto iter_find = m_mapStoreTab.find(wstrTag);

	if (m_mapStoreTab.end() == iter_find)
		return nullptr;

	return iter_find->second;
}

void CStoreMgr::Reset_StoreItemBuySlotList()
{
	for (auto& pBuyItemSlot : m_vecBuySlotList)
	{
		pBuyItemSlot->Set_CurItemInfo(NO_ITEM, NO_ITEM);
		pBuyItemSlot->Set_CurItemCnt(0);
	}

	m_uiCurBuySlotSize = 0;
	m_iBuyItemSumGold  = 0;
}

void CStoreMgr::Push_StoreItemBuySlot(const char& chItemType, const char& chItemName, const _uint& uiCnt, const _uint& uiPrice)
{
	if (m_uiCurBuySlotSize >= m_uiMaxBuySlotSize)
		return;

	if (ItemType_Potion == chItemType)
	{
		// 현재 인벤토리에 포션이 있는지 탐색.
		for (auto& pBuyItemSlot : m_vecBuySlotList)
		{
			if (chItemType == pBuyItemSlot->Get_CurItemInfo().chItemType &&
				chItemName == pBuyItemSlot->Get_CurItemInfo().chItemName)
			{
				pBuyItemSlot->Add_PotionCnt(uiCnt);
				m_iBuyItemSumGold += uiPrice;
				return;
			}
		}

		// 현재 인벤토리에 포션이 없다면 빈칸에 포션을 새로 생성.
		for (auto& pBuyItemSlot : m_vecBuySlotList)
		{
			if (NO_ITEM == pBuyItemSlot->Get_CurItemInfo().chItemType)
			{
				pBuyItemSlot->Set_CurItemInfo(chItemType, chItemName, uiCnt);
				++m_uiCurBuySlotSize;
				m_iBuyItemSumGold += uiPrice;
				break;
			}
		}
	}
	else
	{
		for (auto& pBuyItemSlot : m_vecBuySlotList)
		{
			if (NO_ITEM == pBuyItemSlot->Get_CurItemInfo().chItemType)
			{
				pBuyItemSlot->Set_CurItemInfo(chItemType, chItemName, uiCnt);
				++m_uiCurBuySlotSize;
				m_iBuyItemSumGold += uiPrice;
				break;
			}
		}
	}
}

void CStoreMgr::Reset_StoreItemSellSlotList(const _bool& bIsPushInventory)
{
	if (bIsPushInventory)
	{
		for (auto& pSellSlotList : m_vecSellSlotList)
		{
			if (NO_ITEM != pSellSlotList->Get_CurItemInfo().chItemType &&
				NO_ITEM != pSellSlotList->Get_CurItemInfo().chItemName)
			{
				CInventoryEquipmentMgr::Get_Instance()->Push_ItemInventory(pSellSlotList->Get_CurItemInfo().chItemType,
					pSellSlotList->Get_CurItemInfo().chItemName,
					pSellSlotList->Get_CurItemCnt());

				pSellSlotList->Set_CurItemInfo(NO_ITEM, NO_ITEM);
				pSellSlotList->Set_CurItemCnt(0);
				Min_StoreSellSlotSize();
			}
		}
	}

	for (auto& pSellItemSlot : m_vecSellSlotList)
	{
		pSellItemSlot->Set_CurItemInfo(NO_ITEM, NO_ITEM);
		pSellItemSlot->Set_CurItemCnt(0);
	}

	m_uiCurSellSlotSize = 0;
	m_iSellItemSumGold  = 0;
}

void CStoreMgr::Push_StoreItemSellSlot(const char& chItemType, const char& chItemName, const _uint& uiInventoryIdx, const _uint& uiPrice)
{
	if (m_uiCurSellSlotSize >= m_uiMaxSellSlotSize)
		return;

	if (ItemType_Potion == chItemType)
	{
		// 현재 인벤토리에 포션이 있는지 탐색.
		for (auto& pSellItemSlot : m_vecSellSlotList)
		{
			if (chItemType == pSellItemSlot->Get_CurItemInfo().chItemType &&
				chItemName == pSellItemSlot->Get_CurItemInfo().chItemName)
			{
				pSellItemSlot->Add_PotionCnt(1);
				m_iSellItemSumGold += uiPrice;
				return;
			}
		}

		// 현재 인벤토리에 포션이 없다면 빈칸에 포션을 새로 생성.
		for (auto& pSellItemSlot : m_vecSellSlotList)
		{
			if (NO_ITEM == pSellItemSlot->Get_CurItemInfo().chItemType)
			{
				pSellItemSlot->Set_CurItemInfo(chItemType, chItemName, 1);
				pSellItemSlot->Set_InventoryIdx(uiInventoryIdx);
				++m_uiCurSellSlotSize;
				m_iSellItemSumGold += uiPrice;
				break;
			}
		}
	}
	else
	{
		for (auto& pSellItemSlot : m_vecSellSlotList)
		{
			if (NO_ITEM == pSellItemSlot->Get_CurItemInfo().chItemType)
			{
				pSellItemSlot->Set_CurItemInfo(chItemType, chItemName, 1);
				pSellItemSlot->Set_InventoryIdx(uiInventoryIdx);
				++m_uiCurSellSlotSize;
				m_iSellItemSumGold += uiPrice;
				break;
			}
		}
	}
}

void CStoreMgr::Free()
{
	m_mapStoreTab.clear();
	m_vecStoreItemSlot.clear();
	m_vecStoreItemSlot.shrink_to_fit();
}