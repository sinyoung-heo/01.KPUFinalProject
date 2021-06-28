#pragma once
#include "Obj.h"
class CPlayer :
    public CObj
{
public:
	CPlayer();
	virtual ~CPlayer();

public:
	/* Inventory */
	void						Load_InventoryFromDB(const GAMEITEM& item, const int& itemNumber, const int& count);
	const int&					Get_ItemCount(const int& itemNumber, ITEM eItemType);
	const map<int, GAMEITEM>&	Get_Inventory(ITEM eType) { return m_mapInventory[eType]; }
	bool						Is_Full_Inventory(const int& count = 0);
	bool						Is_inInventory(const int& itemNumber, ITEM eItemType, const int& count = 0);
	bool						Add_Item(const int& itemNumber, ITEM eItemType);
	bool						Delete_Item(const int& itemNumber, ITEM eItemType);
	bool						Buy_Item(const int& itemNumber, ITEM eItemType, const int& count = 0);
	bool						Sell_Item(const int& itemNumber, ITEM eItemType, const int& count = 0);
	void						Release_Inventory();

	/* Equipment */
	void						Load_EquipmentFromDB(const int& weapon, const int& helmet, const int& armor, const int& shoes);
	const char&					Get_Equipment(const char& itemType);
	const EQUIPMENT&			Get_All_Equipment() { return m_tEquipment; }
	void						Equip_Item(const char& itemName, const char& eItemType);
	void						Unequip_Item(const char& itemName, const char& eItemType);
	void						Release_Equipment();

	/* packet */
	void						send_load_InventoryAndEquipment();

	virtual DWORD				Release();

public:
	/*=============시스템 컨텐츠==============*/
	SOCKET			m_sock; 
	OVER_EX			m_recv_over;   
	unsigned char*	m_packet_start; 
	unsigned char*	m_recv_start; 

public:
	/*=============게임 컨텐츠===============*/
	bool	m_bIsAttackStance	= false;
	bool	m_bIsPartyState		= false;
	char	m_chWeaponType      = 0;
	int		m_iPartyNumber		= -1;
	int		m_iAniIdx			= 0;
	int		m_iLevel			= 0;
	int		m_iHp				= 0; 
	int		m_iMaxHp			= 0;
	int		m_iMp				= 0;
	int		m_iMaxMp			= 0;
	int		m_iExp				= 0;
	int		m_iMaxExp			= 0;
	int		m_iMinAtt			= 0;
	int		m_iMaxAtt			= 0;
	int		m_iMoney			= 0;
	float	m_fSpd				= 0;
	
	map<int, GAMEITEM>		m_mapInventory[ITEM::ITEM_END]; // 고유번호 + 아이템 세부정보
	EQUIPMENT				m_tEquipment;
	unordered_set<int>		view_list; 
	mutex					v_lock; 
};

