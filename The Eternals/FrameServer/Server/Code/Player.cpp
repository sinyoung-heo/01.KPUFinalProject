#include "stdafx.h"
#include "Player.h"

CPlayer::CPlayer()
    :m_iLevel(0), m_iHp(0), m_iMaxHp(0),
    m_iMp(0), m_iMaxMp(0), m_iExp(0), m_iMaxExp(0),
    m_iMaxAtt(0), m_fSpd(0.f), m_bIsAttackStance(false)
{
}

CPlayer::~CPlayer()
{
}

bool CPlayer::Is_Full_Inventory()
{
    int size = 0;

    for (int i = 0; i < 2; ++i)
        size += m_mapInventory[i].size();

    // 잡화 아이템 개수는 제외
    if (size + 2 <= MAX_ITEMSIZE)
        return false;
    else
        return true;
}

bool CPlayer::Add_Item(const char itemName, ITEM eItemType)
{
    if (!Is_Full_Inventory())
    {
        m_mapInventory[eItemType][itemName]++;
        return true;
    }
    return false;
}

bool CPlayer::Delete_Item(const char itemName, ITEM eItemType)
{
    if (m_mapInventory[eItemType][itemName] > 0)
    {
        m_mapInventory[eItemType][itemName]--;
        return true;
    }
    return false;
}

const int& CPlayer::Get_ItemCount(const char itemName, ITEM eItemType)
{
    return  m_mapInventory[eItemType][itemName];
}

void CPlayer::Release_Inventory()
{
    for (int i = 0; i < ITEM::ITEM_END; ++i)
        m_mapInventory[i].clear();
}

DWORD CPlayer::Release()
{
    return 0;
}
