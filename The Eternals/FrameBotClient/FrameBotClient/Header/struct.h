#pragma once

struct OverlappedEx 
{
	WSAOVERLAPPED over;
	WSABUF wsabuf;
	unsigned char IOCP_buf[MAX_BUFF_SIZE];
	OPTYPE event_type;
	int event_target;
};

struct CLIENT 
{
	/*=============∞‘¿” ƒ¡≈Ÿ√˜===============*/
	atomic_bool m_bIsConnect	= false;
	bool m_bIsDead				= false;

	char m_ID[MAX_ID_LEN];
	char m_type;
	char m_chStageId			= 0;
	int  m_sNum					= INVALID_ID;

	_vec3 m_vAngle				= _vec3(0.f);
	_vec3 m_vPos				= _vec3(0.f);
	_vec3 m_vTempPos			= _vec3(0.f);
	_vec3 m_vDir				= _vec3(0.f);
	atomic<STATUS> m_status		= STATUS::ST_END;

	bool	m_bIsAttackStance	= false;
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
	char	m_chWeaponType		= 0;
	
	/*=============Ω√Ω∫≈€ ƒ¡≈Ÿ√˜===============*/
	SOCKET client_socket;
	OverlappedEx recv_over;
	unsigned char packet_buf[MAX_PACKET_SIZE];
	int prev_packet_data;
	int curr_packet_size;
	high_resolution_clock::time_point last_move_time;
};