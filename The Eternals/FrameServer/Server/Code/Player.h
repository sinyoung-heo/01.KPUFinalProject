#pragma once
#include "Obj.h"
class CPlayer :
    public CObj
{
public:
    CPlayer() {}
    virtual ~CPlayer() {}

private:
	/*=============½Ã½ºÅÛ ÄÁÅÙÃ÷==============*/
	SOCKET	m_sock; // player
	OVER_EX	m_recv_over;  // player 
	unsigned char* m_packet_start; // player
	unsigned char* m_recv_start; // player

	/*=============°ÔÀÓ ÄÁÅÙÃ÷===============*/
	int	level; // player
	int Hp, maxHp; // player, monster
	int Exp, maxExp; // player
	int att; // player monster
	float spd; // player monster

	unordered_set<int> view_list; // player
	mutex v_lock; // player 
};

