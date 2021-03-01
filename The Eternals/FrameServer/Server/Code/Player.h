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
	/*=============½Ã½ºÅÛ ÄÁÅÙÃ÷==============*/
	SOCKET	m_sock; 
	OVER_EX	m_recv_over;   
	unsigned char* m_packet_start; 
	unsigned char* m_recv_start; 

public:
	/*=============°ÔÀÓ ÄÁÅÙÃ÷===============*/
	int	level	= 0;
	int Hp		= 0; 
	int maxHp	= 0;
	int Exp		= 0;
	int maxExp	= 0;
	int att		= 0;
	float spd	= 0;
	
	unordered_set<int> view_list; 
	mutex v_lock; 
};

