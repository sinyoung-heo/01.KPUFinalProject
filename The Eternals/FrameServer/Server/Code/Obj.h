#pragma once
class CObj
{
public:
	CObj();
	virtual ~CObj();

public:
	mutex& Get_ClientLock() { return c_lock; }
	const bool& Get_IsConnected() { return m_bIsConnect; }

public:
	void Set_IsConnected(const bool& set) { m_bIsConnect = set; }
	void Set_IsDead(const bool& set) { m_bIsDead = set; }
	void Set_ServerNumber(const int& num) { m_sNum = num; }

public:
	virtual DWORD Release();

protected:
	/*=============Ω√Ω∫≈€ ƒ¡≈Ÿ√˜==============*/
	//SOCKET	m_sock; // player
	//OVER_EX	m_recv_over;  // player 
	//unsigned char* m_packet_start; // player
	//unsigned char* m_recv_start; // player

	/*=============∞‘¿” ƒ¡≈Ÿ√˜===============*/
public:
	int move_time;
	bool m_bIsConnect; // all
	bool m_bIsDead; //all
	char m_ID[MAX_ID_LEN]; // all
	char m_type; // all
	int  m_sNum;
	//int	level; // player
	//int Hp, maxHp; // player, monster
	//int Exp, maxExp; // player
	//int att; // player monster
	//float spd; // player monster
	_vec3 m_vAngle;
	_vec3 m_vPos; // all
	_vec3 m_vDir; // all
	atomic<STATUS> m_status; // all

protected:
	mutex c_lock;  // all

	//unordered_set<int> view_list; // player
	//mutex v_lock; // player 
	
};

