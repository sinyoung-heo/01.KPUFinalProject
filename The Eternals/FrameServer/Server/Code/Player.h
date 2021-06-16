#pragma once
#include "Obj.h"
class CPlayer :
    public CObj
{
public:
	CPlayer();
	virtual ~CPlayer();

public:
	virtual DWORD Release();

public:
	/*=============Ω√Ω∫≈€ ƒ¡≈Ÿ√˜==============*/
	SOCKET	m_sock; 
	OVER_EX	m_recv_over;   
	unsigned char* m_packet_start; 
	unsigned char* m_recv_start; 

public:
	/*=============∞‘¿” ƒ¡≈Ÿ√˜===============*/
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
	float	m_fSpd				= 0;
	
	unordered_set<int> view_list; 
	mutex v_lock; 
};

