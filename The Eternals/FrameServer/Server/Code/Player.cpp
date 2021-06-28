#include "stdafx.h"
#include "Player.h"

CPlayer::CPlayer()
    :m_iLevel(0), m_iHp(0), m_iMaxHp(0),
    m_iMp(0), m_iMaxMp(0), m_iExp(0), m_iMaxExp(0),
    m_iMaxAtt(0), m_fSpd(0.f), m_bIsAttackStance(false),
    m_bIsPartyState(false), m_iPartyNumber(-1), m_iMoney(0)
{
    m_tEquipment.weapon =   -1;
    m_tEquipment.helmet =   -1;
    m_tEquipment.armor  =   -1;
    m_tEquipment.shoes  =   -1;
}

CPlayer::~CPlayer()
{
}

bool CPlayer::Is_Full_Inventory(const int& count)
{
    size_t size = 0;

    for (int i = 0; i < ITEM::ITEM_ETC; ++i)
    {
        for (auto& pair : m_mapInventory[i])
            size += pair.second.iCount;
    }

    // 잡화 아이템 개수는 제외
    if (size + 2 + count < MAX_ITEMSIZE)
        return false;
    else
        return true;
}

bool CPlayer::Is_inInventory(const int& itemNumber, ITEM eItemType, const int& count)
{
    if (m_mapInventory[eItemType][itemNumber].iCount > 0
        && m_mapInventory[eItemType][itemNumber].iCount >= count)
        return true;

    return false;
}

bool CPlayer::Add_Item(const int& itemNumber, ITEM eItemType)
{
    if (!Is_Full_Inventory())
    {
        m_mapInventory[eItemType][itemNumber].iCount++;

        if (m_mapInventory[eItemType][itemNumber].iCount <= 1)
            CDBMgr::GetInstance()->insert_Inventory(m_sNum, itemNumber, m_mapInventory[eItemType][itemNumber].iCount);
        else
            CDBMgr::GetInstance()->update_Inventory(m_sNum, itemNumber, m_mapInventory[eItemType][itemNumber].iCount);

        return true;
    }
    return false;
}

bool CPlayer::Delete_Item(const int& itemNumber, ITEM eItemType)
{
    if (m_mapInventory[eItemType][itemNumber].iCount > 0)
    {
        m_mapInventory[eItemType][itemNumber].iCount--;

        if (m_mapInventory[eItemType][itemNumber].iCount <= 0)
            CDBMgr::GetInstance()->delete_Inventory(m_sNum, itemNumber);
        else
            CDBMgr::GetInstance()->update_Inventory(m_sNum, itemNumber, m_mapInventory[eItemType][itemNumber].iCount);

        return true;
    }
    return false;
}

bool CPlayer::Buy_Item(const int& itemNumber, ITEM eItemType, const int& count)
{
    if (m_mapInventory[eItemType][itemNumber].iCount <= 0)
        CDBMgr::GetInstance()->insert_Inventory(m_sNum, itemNumber, count);
    else
        CDBMgr::GetInstance()->update_Inventory(m_sNum, itemNumber, count);

    m_mapInventory[eItemType][itemNumber].iCount += count;
    
    return true;
}

bool CPlayer::Sell_Item(const int& itemNumber, ITEM eItemType, const int& count)
{
    if (count == 0) return false;

    if (m_mapInventory[eItemType][itemNumber].iCount >= count)
    {
        m_mapInventory[eItemType][itemNumber].iCount -= count;

        if (m_mapInventory[eItemType][itemNumber].iCount <= 0)
            CDBMgr::GetInstance()->delete_Inventory(m_sNum, itemNumber);
        else
            CDBMgr::GetInstance()->update_Inventory(m_sNum, itemNumber, m_mapInventory[eItemType][itemNumber].iCount);

        return true;
    }
    return false;
}

void CPlayer::Load_InventoryFromDB(const GAMEITEM& item, const int& itemNumber, const int& count)
{
    m_mapInventory[static_cast<ITEM>(item.cItemType)][itemNumber] = item;
}

const int& CPlayer::Get_ItemCount(const int& itemNumber, ITEM eItemType)
{
    return  m_mapInventory[eItemType][itemNumber].iCount;
}

void CPlayer::Release_Inventory()
{
    for (int i = 0; i < ITEM::ITEM_END; ++i)
        m_mapInventory[i].clear();
}

void CPlayer::Load_EquipmentFromDB(const int& weapon, const int& helmet, const int& armor, const int& shoes)
{
    m_tEquipment.weapon = static_cast<char>(weapon);
    m_tEquipment.helmet = static_cast<char>(helmet);
    m_tEquipment.armor = static_cast<char>(armor);
    m_tEquipment.shoes = static_cast<char>(shoes);

    // 렌더링 무기
    m_chWeaponType = static_cast<char>(weapon);
}

const char& CPlayer::Get_Equipment(const char& itemType)
{
    if (itemType == EQUIP_WEAPON)
    {
        return m_tEquipment.weapon;
    }
    else if (itemType == EQUIP_ARMOR)
    {
        return m_tEquipment.armor;
    }
    else if (itemType == EQUIP_HELMET)
    {
        return m_tEquipment.helmet;
    }
    else if (itemType == EQUIP_SHOES)
    {
        return m_tEquipment.shoes;
    }

    return -1;
}

void CPlayer::Equip_Item(const char& itemName, const char& eItemType)
{
    if (eItemType == EQUIP_WEAPON)
    {
        m_tEquipment.weapon = itemName;
    }
    else if (eItemType == EQUIP_ARMOR)
    {
        m_tEquipment.armor = itemName;
    }
    else if (eItemType == EQUIP_HELMET)
    {
        m_tEquipment.helmet = itemName;
    }
    else if (eItemType == EQUIP_SHOES)
    {
        m_tEquipment.shoes = itemName;
    }

    CDBMgr::GetInstance()->update_Equipment(m_sNum);
}

void CPlayer::Unequip_Item(const char& itemName, const char& eItemType)
{
    if (eItemType == EQUIP_WEAPON)
    {
        m_tEquipment.weapon = -1;
    }
    else if (eItemType == EQUIP_ARMOR)
    {
        m_tEquipment.armor = -1;
    }
    else if (eItemType == EQUIP_HELMET)
    {
        m_tEquipment.helmet = -1;
    }
    else if (eItemType == EQUIP_SHOES)
    {
        m_tEquipment.shoes = -1;
    }

    CDBMgr::GetInstance()->update_Equipment(m_sNum);
}

void CPlayer::Release_Equipment()
{
    m_tEquipment.weapon = -1;
    m_tEquipment.armor  = -1;
    m_tEquipment.helmet = -1;
    m_tEquipment.shoes  = -1;
}

void CPlayer::send_load_InventoryAndEquipment()
{
    for (int i = 0; i < ITEM::ITEM_END; ++i)
    {
        for (auto& item : m_mapInventory[i])
        {
            send_update_inventory(m_sNum,
                                  item.second.cItemType,
                                  item.second.cItemName,
                                  item.second.iCount,
                                  true);
        }
    }

    char itemType[EQUIP_END] = { EQUIP_WEAPON, EQUIP_ARMOR, EQUIP_HELMET, EQUIP_SHOES };
    char itemName[EQUIP_END] = { m_tEquipment.weapon, m_tEquipment.armor, m_tEquipment.helmet, m_tEquipment.shoes };
    send_load_equipment(m_sNum, itemType, itemName);
}

DWORD CPlayer::Release()
{
    return 0;
}
