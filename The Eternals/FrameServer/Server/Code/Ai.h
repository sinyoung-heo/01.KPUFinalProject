#pragma once
#include "Obj.h"
class CAi :
    public CObj
{
public:
	CAi();
	virtual ~CAi();

public:
	void Ready_AI(const char& chJob, const char& chWeaponType, const char& chStageID, const _vec3& vPos);
	void Release_AI();

	int Update_AI(const float& fTimeDelta);

public:
	void active_AI();

private:
	void process_disconnect_ai();
	void process_leave_party_ai();

public:
	void send_AI_enter_packet(const int& to_client);
	void send_leave_party_ai(const int& to_client);

public:
	virtual DWORD				Release();

public:
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
	int		m_iMinAtt			= 0;
	int		m_iMaxAtt			= 0;
	float	m_fSpd				= 0;
};

