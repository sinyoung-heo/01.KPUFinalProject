#include "stdafx.h"
#include "Player.h"

CPlayer::CPlayer()
    :m_iLevel(0), m_iHp(0), m_iMaxHp(0),
    m_iMp(0), m_iMaxMp(0), m_iExp(0), m_iMaxExp(0),
    m_iMaxAtt(0), m_fSpd(0.f), m_bIsAttackStance(false),
    m_bIsPartyState(false), m_iPartyNumber(-1), m_iMoney(0)
{
}

CPlayer::~CPlayer()
{
}

bool CPlayer::Is_Full_Inventory()
{
    size_t size = 0;

    for (int i = 0; i < ITEM::ITEM_ETC; ++i)
    {
        for (auto& pair : m_mapInventory[i])
            size += pair.second;
    }


    // 잡화 아이템 개수는 제외
    if (size + 2 < MAX_ITEMSIZE)
        return false;
    else
        return true;
}

bool CPlayer::Is_inInventory(const char& itemName, ITEM eItemType)
{
    if (m_mapInventory[eItemType][itemName] > 0)
        return true;

    return false;
}

bool CPlayer::Add_Item(const char& itemName, ITEM eItemType)
{
    if (!Is_Full_Inventory())
    {
        m_mapInventory[eItemType][itemName]++;
        return true;
    }
    return false;
}

bool CPlayer::Delete_Item(const char& itemName, ITEM eItemType)
{
    if (m_mapInventory[eItemType][itemName] > 0)
    {
        m_mapInventory[eItemType][itemName]--;
        return true;
    }
    return false;
}

const int& CPlayer::Get_ItemCount(const char& itemName, ITEM eItemType)
{
    return  m_mapInventory[eItemType][itemName];
}

void CPlayer::Release_Inventory()
{
    for (int i = 0; i < ITEM::ITEM_END; ++i)
        m_mapInventory[i].clear();
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
}

DWORD CPlayer::Release()
{
    return 0;
}
