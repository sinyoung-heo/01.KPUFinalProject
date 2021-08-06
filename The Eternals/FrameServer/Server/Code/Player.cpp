#include "stdafx.h"
#include "Player.h"
#include "Ai.h"

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

void CPlayer::Heal_PartyMember(const int& ani)
{
    for (auto& member : *CObjMgr::GetInstance()->Get_PARTYLIST(m_iPartyNumber))
    {
        if (member >= AI_NUM_START)
        {
            CAi* pOther = nullptr;
            pOther = static_cast<CAi*>(CObjMgr::GetInstance()->Get_GameObject(L"AI", member));
            if (pOther == nullptr || !pOther->m_bIsConnect || !pOther->m_bIsPartyState) continue;

            if (member != m_sNum)
            {
                switch (ani)
                {
                case Priest::HEAL_START:
                {
                    pOther->m_iHp += (int)((float)pOther->m_iMaxHp * Priest::PLUS_HP / PERCENT);

                    if (pOther->m_iHp >= pOther->m_iMaxHp)
                        pOther->m_iHp = pOther->m_iMaxHp;
                }
                break;

                case Priest::MP_CHARGE_START:
                {
                    pOther->m_iMp += (int)((float)pOther->m_iMaxMp * Priest::PLUS_MP / PERCENT);

                    if (pOther->m_iMp >= pOther->m_iMaxMp)
                        pOther->m_iMp = pOther->m_iMaxMp;
                }
                break;
                }
            }
        }
        else
        {
            CPlayer* pOther = nullptr;
            pOther = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", member));
            if (pOther == nullptr || !pOther->m_bIsConnect || !pOther->m_bIsPartyState) continue;

            if (member != m_sNum)
            {
                switch (ani)
                {
                case Priest::HEAL_START:
                {
                    pOther->m_iHp += (int)((float)pOther->m_iMaxHp * Priest::PLUS_HP / PERCENT);

                    if (pOther->m_iHp >= pOther->m_iMaxHp)
                        pOther->m_iHp = pOther->m_iMaxHp;
                }
                break;

                case Priest::MP_CHARGE_START:
                {
                    pOther->m_iMp += (int)((float)pOther->m_iMaxMp * Priest::PLUS_MP / PERCENT);

                    if (pOther->m_iMp >= pOther->m_iMaxMp)
                        pOther->m_iMp = pOther->m_iMaxMp;
                }
                break;
                }
            }
            send_buff_stat(member, ani, pOther->m_iHp, pOther->m_iMaxHp, pOther->m_iMp, pOther->m_iMaxMp);
        }
    }
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
    {
        CDBMgr::GetInstance()->insert_Inventory(m_sNum, itemNumber, count);
        m_mapInventory[eItemType][itemNumber].iCount += count;
        return true;
    }
    else
    {
        m_mapInventory[eItemType][itemNumber].iCount += count;
        CDBMgr::GetInstance()->update_Inventory(m_sNum, itemNumber, m_mapInventory[eItemType][itemNumber].iCount);
        return true;
    }

    return false;
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

void CPlayer::logout_equipment()
{
    switch (this->m_type)
    {
    case PC_GLADIATOR:
    {
        process_logoutForEquipment(m_sNum, EQUIP_WEAPON, ItemType_WeaponTwoHand, m_tEquipment.weapon);
    }
    break;

    case PC_ARCHER:
    {
        process_logoutForEquipment(m_sNum, EQUIP_WEAPON, ItemType_WeaponBow, m_tEquipment.weapon);
    }
    break;

    case PC_PRIEST:
    {
        process_logoutForEquipment(m_sNum, EQUIP_WEAPON, ItemType_WeaponRod, m_tEquipment.weapon);
    }
    break;
    }

   process_logoutForEquipment(m_sNum, EQUIP_ARMOR, ItemType_Armor, m_tEquipment.armor);
   process_logoutForEquipment(m_sNum, EQUIP_HELMET, ItemType_Helmet, m_tEquipment.helmet);
   process_logoutForEquipment(m_sNum, EQUIP_SHOES, ItemType_Shoes, m_tEquipment.shoes);
}

void CPlayer::Release_Equipment()
{
    m_tEquipment.weapon = -1;
    m_tEquipment.armor  = -1;
    m_tEquipment.helmet = -1;
    m_tEquipment.shoes  = -1;
}


/*_____________________________________________________________________________________________________________________*/
/*                                                   Send Packet                                                       */
/*_____________________________________________________________________________________________________________________*/
void CPlayer::send_login_ok()
{
    sc_packet_login_ok p;

    p.size          = sizeof(p);
    p.type          = SC_PACKET_LOGIN_OK;
    p.id            = m_sNum;

    p.o_type        = m_type;
    p.weaponType    = m_chWeaponType;
    p.naviType      = m_chStageId;

    p.money         = m_iMoney;
    p.level         = m_iLevel;
    p.hp            = m_iHp;
    p.maxHp         = m_iMaxHp;
    p.mp            = m_iMp;
    p.maxMp         = m_iMaxMp;
    p.exp           = m_iExp;
    p.maxExp        = m_iMaxExp;
    p.min_att       = m_iMinAtt;
    p.max_att       = m_iMaxAtt;
    p.spd           = m_fSpd;

    p.posX          = m_vPos.x;
    p.posY          = m_vPos.y;
    p.posZ          = m_vPos.z;

    send_packet(m_sNum, &p);
}

void CPlayer::send_enter_packet(const int& to_client)
{
    sc_packet_enter p;

	p.size	            = sizeof(p);
	p.type	            = SC_PACKET_ENTER;
	p.id	            = m_sNum;

    c_lock.lock();
	strncpy_s(p.name, m_ID, strlen(m_ID));
    c_lock.unlock();

	p.o_type			= m_type;
	p.weaponType		= m_chWeaponType;
	p.stageID			= m_chStageId;
	p.is_stance_attack	= m_bIsAttackStance;
	p.is_party_state	= m_bIsPartyState;

	p.posX				= m_vPos.x;
	p.posY				= m_vPos.y;
	p.posZ				= m_vPos.z;

	p.dirX				= m_vDir.x;
	p.dirY				= m_vDir.y;
	p.dirZ				= m_vDir.z;

	send_packet(to_client, &p);
}

void CPlayer::send_move_packet(const int& to_client)
{
    sc_packet_move p;

    p.size      = sizeof(p);
    p.type      = SC_PACKET_MOVE;
    p.id        = m_sNum;

    p.spd       = m_fSpd;
    p.animIdx   = m_iAniIdx;
    p.move_time = move_time;

    p.posX      = m_vTempPos.x;
    p.posY      = m_vTempPos.y;
    p.posZ      = m_vTempPos.z;

    p.dirX      = m_vDir.x;
    p.dirY      = m_vDir.y;
    p.dirZ      = m_vDir.z;

    send_packet(to_client, &p);
}

void CPlayer::send_move_stop_packet(const int& to_client)
{
    sc_packet_move p;

    p.size      = sizeof(p);
    p.type      = SC_PACKET_MOVE_STOP;
    p.id        = m_sNum;

    p.spd       = m_fSpd;
    p.animIdx   = m_iAniIdx;
    p.move_time = move_time;

    p.posX      = m_vPos.x;
    p.posY      = m_vPos.y;
    p.posZ      = m_vPos.z;

    p.dirX      = m_vDir.x;
    p.dirY      = m_vDir.y;
    p.dirZ      = m_vDir.z;

    send_packet(to_client, &p);
}

void CPlayer::send_attack_packet(const int& to_client, const int& animIdx, const float& end_angleY)
{
    sc_packet_attack p;

    p.size          = sizeof(p);
    p.type          = SC_PACKET_ATTACK;
    p.id            = m_sNum;

    p.o_type        = m_type;

    p.posX          = m_vTempPos.x;
    p.posY          = m_vTempPos.y;
    p.posZ          = m_vTempPos.z;

    p.dirX          = m_vDir.x;
    p.dirY          = m_vDir.y;
    p.dirZ          = m_vDir.z;

    p.animIdx       = animIdx;
    p.end_angleY    = end_angleY;

    send_packet(to_client, &p);
}

void CPlayer::send_attack_stop_packet(const int& to_client, const int& animIdx)
{
    sc_packet_attack p;

    p.size      = sizeof(p);
    p.type      = SC_PACKET_ATTACK_STOP;
    p.id        = m_sNum;

    p.o_type    = m_type;
    p.animIdx   = animIdx;

    p.posX      = m_vPos.x;
    p.posY      = m_vPos.y;
    p.posZ      = m_vPos.z;

    p.dirX      = m_vDir.x;
    p.dirY      = m_vDir.y;
    p.dirZ      = m_vDir.z;

    send_packet(to_client, &p);
}

void CPlayer::send_player_stat(const int& to_client)
{
    sc_packet_stat_change p;

    p.size      = sizeof(p);
    p.type      = SC_PACKET_STAT_CHANGE;
    p.id        = m_sNum;

    p.hp        = m_iHp;
    p.maxHp     = m_iMaxHp;
    p.mp        = m_iMp;
    p.maxMp     = m_iMaxMp;
    p.exp       = m_iExp;
    p.maxExp    = m_iMaxExp;
    p.lev       = m_iLevel;
    p.maxAtt    = m_iMaxAtt;
    p.minAtt    = m_iMinAtt;

    send_packet(to_client, &p);
}

void CPlayer::send_player_stance_change(const int& to_client, const bool& st)
{
    sc_packet_stance_change p;

    p.size              = sizeof(p);
    p.type              = SC_PACKET_STANCE_CHANGE;
    p.id                = m_sNum;

    p.animIdx           = m_iAniIdx;
    p.o_type            = m_type;
    p.is_stance_attack  = st;

    send_packet(to_client, &p);
}

void CPlayer::send_player_stage_change(const int& to_client)
{
    sc_packet_stage_change p;

    p.size      = sizeof(p);
    p.type      = SC_PACKET_STAGE_CHANGE;
    p.id        = m_sNum;

    p.stage_id  = m_chStageId;
    p.posX      = m_vPos.x;
    p.posY      = m_vPos.y;
    p.posZ      = m_vPos.z;

    send_packet(to_client, &p);
}

void CPlayer::send_enter_party(const int& to_client)
{
    sc_packet_enter_party p;

    p.size      = sizeof(p);
    p.type      = SC_PACKET_ENTER_PARTY;
    p.id        = m_sNum;

    strncpy_s(p.name, m_ID, strlen(m_ID));
    p.o_type    = m_type;
    p.hp        = m_iHp;
    p.maxHp     = m_iMaxHp;
    p.mp        = m_iMp;
    p.maxMp     = m_iMaxMp;

    send_packet(to_client, &p);
}

void CPlayer::send_leave_party(const int& to_client)
{
    sc_packet_suggest_party p;

    p.size  = sizeof(p);
    p.type  = SC_PACKET_LEAVE_PARTY;
    p.id    = m_sNum;

    send_packet(to_client, &p);
}

void CPlayer::send_update_party(const int& to_client)
{
    sc_packet_update_party p;

    p.size  = sizeof(p);
    p.type  = SC_PACKET_UPDATE_PARTY;
    p.id    = m_sNum;

    p.hp    = m_iHp;
    p.maxHp = m_iMaxHp;
    p.mp    = m_iMp;
    p.maxMp = m_iMaxMp;

    send_packet(to_client, &p);
}

void CPlayer::send_chat(const int& to_client, const char* buffer, const int& len)
{
    sc_packet_chat p;

    p.size  = sizeof(p);
    p.type  = SC_PACKET_CHAT;
    p.id    = m_sNum;

    strncpy_s(p.name, m_ID, strlen(m_ID));
    strncpy_s(p.message, buffer, len);
    p.messageLen = len;

    send_packet(to_client, &p);
}

void CPlayer::send_update_inventory(const int& to_client, const char& chItemType, const char& chName, const int& count, const bool& isPushItem)
{
    sc_packet_update_inventory p;

    p.size = sizeof(p);
    p.type = SC_PACKET_UPDATE_INVENTORY;

    p.itemType      = chItemType;
    p.itemName      = chName;
    p.count         = count;
    p.is_pushItem   = isPushItem;

    send_packet(to_client, &p);
}

void CPlayer::send_update_equipment(const int& to_client, const char& chItemSlotType, const bool& isPushItem)
{
    sc_packet_update_equipment p;

    p.size = sizeof(p);
    p.type = SC_PACKET_UPDATE_EQUIPMENT;

    p.equipType     = chItemSlotType;
    p.itemName      = Get_Equipment(chItemSlotType);
    p.is_pushItem   = isPushItem;

    p.minAtt        = m_iMinAtt;
    p.maxAtt        = m_iMaxAtt;
    p.hp            = m_iHp;
    p.maxHp         = m_iMaxHp;
    p.mp            = m_iMp;
    p.maxMp         = m_iMaxMp;

    send_packet(to_client, &p);
}

void CPlayer::send_load_equipment(const int& to_client, const char* chItemType, const char* chName)
{
    sc_packet_load_equipment p;

    p.size = sizeof(p);
    p.type = SC_PACKET_LOAD_EQUIPMENT;

    for (int i = 0; i < EQUIP_END; ++i)
    {
        p.equipType[i] = chItemType[i];
        p.itemName[i] = chName[i];
    }

    send_packet(to_client, &p);
}

void CPlayer::send_user_money()
{
    sc_packet_leave p;

    p.size = sizeof(p);
    p.type = SC_PACKET_UPDATE_MONEY;

    // money
    p.id = m_iMoney;

    send_packet(m_sNum, &p);
}

void CPlayer::send_drink_potion(const int& to_client, const int& ability, const char& chItemType, const char& chName, const int& count, const bool& isPushItem)
{
    sc_packet_potion p;

    p.size          = sizeof(p);
    p.type          = SC_PACKET_DRINK_POTION;

    p.itemType      = chItemType;
    p.itemName      = chName;
    p.count         = count;
    p.is_pushItem   = isPushItem;
    p.ability       = ability;

    send_packet(to_client, &p);
}

void CPlayer::send_consume_point(const int& to_client)
{
    sc_packet_update_party p;

    p.size  = sizeof(p);
    p.type  = SC_PACKET_CONSUME_POINT;

    p.hp    = m_iHp;
    p.maxHp = m_iMaxHp;
    p.mp    = m_iMp;
    p.maxMp = m_iMaxMp;

    send_packet(to_client, &p);
}

void CPlayer::send_buff_stat(const int& to_client, const int& ani, const int& hp, const int& maxHp, const int& mp, const int& maxMp)
{
    sc_packet_buff p;

    p.size          = sizeof(p);
    p.type          = SC_PACKET_BUFF;

    p.animIdx       = ani;
    p.priest_id     = m_sNum;

    p.hp            = hp;
    p.maxHp         = maxHp;
    p.mp            = mp;
    p.maxMp         = maxMp;

    p.priest_hp     = m_iHp;
    p.priest_maxHp  = m_iMaxHp;
    p.priest_mp     = m_iMp;
    p.priest_maxMp  = m_iMaxMp;

    send_packet(to_client, &p);
}

void CPlayer::send_load_InventoryAndEquipment()
{
    /* 인벤토리 불러오기 */
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

    /* 장비창 불러오기 */
   char arrItemType[EQUIP_END] = { EQUIP_WEAPON, EQUIP_ARMOR, EQUIP_HELMET, EQUIP_SHOES };
   char arrItemName[EQUIP_END] = { m_tEquipment.weapon, m_tEquipment.armor, m_tEquipment.helmet, m_tEquipment.shoes };
   send_load_equipment(m_sNum, arrItemType, arrItemName);
    
    switch (this->m_type)
    {
    case PC_GLADIATOR:
    {
        process_load_equipment(m_sNum, EQUIP_WEAPON, ItemType_WeaponTwoHand, m_tEquipment.weapon);
    }
    break;

    case PC_ARCHER:
    {
        process_load_equipment(m_sNum, EQUIP_WEAPON, ItemType_WeaponBow, m_tEquipment.weapon);
    }
    break;

    case PC_PRIEST:
    {
        process_load_equipment(m_sNum, EQUIP_WEAPON, ItemType_WeaponRod, m_tEquipment.weapon);
    }
    break;
    }  

    process_load_equipment(m_sNum, EQUIP_ARMOR, ItemType_Armor, m_tEquipment.armor);
    process_load_equipment(m_sNum, EQUIP_HELMET, ItemType_Helmet, m_tEquipment.helmet);
    process_load_equipment(m_sNum, EQUIP_SHOES, ItemType_Shoes, m_tEquipment.shoes);

    send_update_equipment(m_sNum, EQUIP_END, true);
}

DWORD CPlayer::Release()
{
    return 0;
}
