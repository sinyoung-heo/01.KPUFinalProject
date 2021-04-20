#pragma once
class CObj
{
public:
	CObj();
	virtual ~CObj();

public:
	mutex& Get_ClientLock()					{ return c_lock; }
	const bool& Get_IsConnected()			{ return m_bIsConnect; }

public:
	void Set_IsConnected(const bool& set)	{ m_bIsConnect = set; }
	void Set_IsDead(const bool& set)		{ m_bIsDead = set; }
	void Set_ServerNumber(const int& num)	{ m_sNum = num; }

public:
	virtual DWORD Release();

public:
	/*=============게임 컨텐츠===============*/
	bool m_bIsConnect					= false;
	bool m_bIsDead						= false;
	char m_ID[MAX_ID_LEN]; 
	char m_type;										// 직업
	char m_naviType[MIDDLE_STR_LEN];					// 삭제 예정
	char m_chStageId					= 0;
	int  m_sNum							= -1;

	int	 move_time						= 0;
	
	_vec3 m_vAngle						= _vec3(0.f);
	_vec3 m_vPos						= _vec3(0.f);
	_vec3 m_vTempPos					= _vec3(0.f);
	_vec3 m_vDir						= _vec3(0.f);
	atomic<STATUS> m_status				= STATUS::ST_END;

protected:
	mutex c_lock; 
};

