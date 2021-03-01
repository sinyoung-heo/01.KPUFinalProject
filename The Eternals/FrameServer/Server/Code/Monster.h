#pragma once
#include "Obj.h"
class CMonster :
    public CObj
{
public:
	CMonster();
	virtual ~CMonster();

public:
	virtual DWORD Release();

public:
	int Hp		= 0;
	int maxHp	= 0;
	int Exp		= 0;
	int att		= 0;
	char m_monNum;
};

