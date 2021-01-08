#pragma once
class CObj
{
public:
	CObj() = default;
	virtual ~CObj() = default;

protected:
	/*=============╫ц╫╨еш даеыцВ==============*/
	int move_time;
	SOCKET	m_sock;
	OVER_EX	m_recv_over;
	unsigned char* m_packet_start;
	unsigned char* m_recv_start;

	/*=============╟тюс даеыцВ===============*/
	bool m_bIsConnect;
	bool m_bIsDead;
	char m_ID[MAX_ID_LEN];
	char m_type;
	int level;
	int m_posX, m_posY, m_posZ;
	int Hp, maxHp, Exp, maxExp;
};

