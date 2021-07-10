#include "stdafx.h"
#include "InventoryEquipmentMgr.h"

IMPLEMENT_SINGLETON(CInventoryEquipmentMgr)

CInventoryEquipmentMgr::CInventoryEquipmentMgr()
{
	m_vecInventorySlot.reserve(80);
}

_uint uiInventorySlotIdx = 0;
void CInventoryEquipmentMgr::Add_InventorySlot(CInventoryItemSlot* pSlot)
{
	if (nullptr != pSlot)
	{
		m_vecInventorySlot.emplace_back(pSlot);
		pSlot->Set_ItemSlotIdx(uiInventorySlotIdx);

		++uiInventorySlotIdx;
	}
}

void CInventoryEquipmentMgr::Add_EquipmentSlot(wstring wstrTag, CEquipmentItemSlot* pSlot)
{
	auto iter_find = m_mapEquipmentSlot.find(wstrTag);

	if (iter_find == m_mapEquipmentSlot.end())
		m_mapEquipmentSlot.emplace(wstrTag, pSlot);
}


void CInventoryEquipmentMgr::Push_ItemInventory(const char& chItemType, const char& chItemName, const _int& iCnt)
{
	if (m_uiCurSlotSize >= m_uiMaxSlotSize)
	{
		cout << "Inventory is Full ! " << endl;
		return;
	}

	if (ItemType_Potion == chItemType)
	{
		// 현재 인벤토리에 포션이 있는지 탐색.
		for (auto& pSlot : m_vecInventorySlot)
		{
			if (chItemType == pSlot->Get_CurItemInfo().chItemType &&
				chItemName == pSlot->Get_CurItemInfo().chItemName)
			{
				pSlot->Add_PotionCnt(iCnt);
				return;
			}
		}

		// 현재 인벤토리에 포션이 없다면 빈칸에 포션을 새로 생성.
		for (auto& pSlot : m_vecInventorySlot)
		{
			if (NO_ITEM == pSlot->Get_CurItemInfo().chItemType)
			{
				pSlot->Set_CurItemInfo(chItemType, chItemName, iCnt);
				++m_uiCurSlotSize;

				if (Potion_HP == chItemName)
					m_pHpPotionSlot = pSlot;
				else
					m_pMpPotionSlot = pSlot;

				break;
			}
		}
	}
	else
	{
		for (auto& pSlot : m_vecInventorySlot)
		{
			if (NO_ITEM == pSlot->Get_CurItemInfo().chItemType)
			{
				pSlot->Set_CurItemInfo(chItemType, chItemName, iCnt);
				++m_uiCurSlotSize;

				break;
			}
		}
	}
}

void CInventoryEquipmentMgr::Pop_ItemInventory(const char& chItemType, const char& chItemName, const _int& iCnt)
{
	if (m_uiCurSlotSize <= 0)
	{
		cout << "Inventory is Empty ! " << endl;
		return;
	}

	if (ItemType_Potion == chItemType)
	{
		// 현재 인벤토리에 포션이 있는지 탐색.
		for (auto& pSlot : m_vecInventorySlot)
		{
			if (chItemType == pSlot->Get_CurItemInfo().chItemType &&
				chItemName == pSlot->Get_CurItemInfo().chItemName)
			{
				pSlot->Add_PotionCnt(iCnt);

				if (pSlot->Get_CurItemCnt() <= 0)
				{
					pSlot->Set_CurItemCnt(0);
					pSlot->Set_CurItemInfo(NO_ITEM, NO_ITEM);
					--m_uiCurSlotSize;

					//if (Potion_HP == chItemName)
					//	m_pHpPotionSlot = nullptr;
					//else
					//	m_pMpPotionSlot = nullptr;
				}

				break;
			}
		}
	}
	else
	{
		for (auto& pSlot : m_vecInventorySlot)
		{
			if (chItemType == pSlot->Get_CurItemInfo().chItemType &&
				chItemName == pSlot->Get_CurItemInfo().chItemName)
			{
				ITEM_DESC tItemInfo = pSlot->Get_CurItemInfo();

				pSlot->Set_CurItemInfo(NO_ITEM, NO_ITEM, iCnt);
				--m_uiCurSlotSize;

				break;
			}
		}
	}

}

void CInventoryEquipmentMgr::SetUp_LoginEquipment(const char& chItemType, const char& chItemName)
{
	if (NO_ITEM == chItemType || NO_ITEM == chItemName)
		return;

	// 인벤토리에서 아이템 탐색.
	CInventoryItemSlot* pItemSlot = nullptr;
	for (auto& pSlot : m_vecInventorySlot)
	{
		if (chItemType == pSlot->Get_CurItemInfo().chItemType &&
			chItemName == pSlot->Get_CurItemInfo().chItemName)
		{
			pItemSlot = pSlot;
			break;
		}
	}

	if (nullptr == pItemSlot)
		return;

	Engine::CGameObject* pThisPlayer = Engine::CObjectMgr::Get_Instance()->Get_GameObject(L"Layer_GameObject", L"ThisPlayer");
	wstring wsrEquipSlotTag = L"";

	switch (chItemType)
	{
	case ItemType_WeaponTwoHand:
	case ItemType_WeaponBow:
	case ItemType_WeaponRod:
	{
		if (ItemType_WeaponTwoHand == chItemType && PC_GLADIATOR != pThisPlayer->Get_OType())
			return;
		else if (ItemType_WeaponBow == chItemType && PC_ARCHER != pThisPlayer->Get_OType())
			return;
		else if (ItemType_WeaponRod == chItemType && PC_PRIEST != pThisPlayer->Get_OType())
			return;

		wsrEquipSlotTag = L"EquipmentWeapon";

		if (nullptr == m_mapEquipmentSlot[wsrEquipSlotTag]->Get_InventoryItemSlot())
		{
			m_mapEquipmentSlot[wsrEquipSlotTag]->Set_InventorySlotClass(pItemSlot);
			pItemSlot->Set_IsOnEquipment(true);
			pThisPlayer->Set_WeaponType(chItemName);
		}

		return;
	}
		break;

	case ItemType_Helmet:
	{
		wsrEquipSlotTag = L"EquipmentHelmet";
	}
		break;

	case ItemType_Armor:
	{
		wsrEquipSlotTag = L"EquipmentArmor";
	}
		break;

	case ItemType_Shoes:
	{
		wsrEquipSlotTag = L"EquipmentShoes";
	}
		break;
	default:
	{
		return;
	}
		break;
	}

	if (L"" != wsrEquipSlotTag &&
		nullptr == m_mapEquipmentSlot[wsrEquipSlotTag]->Get_InventoryItemSlot())
	{
		m_mapEquipmentSlot[wsrEquipSlotTag]->Set_InventorySlotClass(pItemSlot);
		pItemSlot->Set_IsOnEquipment(true);
	}
}

void CInventoryEquipmentMgr::Free()
{
	m_pInventoryCanvas   = nullptr;
	m_pInventorySwapSlot = nullptr;
	m_pEquipmentCanvas   = nullptr;

	m_pHpPotionSlot = nullptr;
	m_pMpPotionSlot = nullptr;
}
