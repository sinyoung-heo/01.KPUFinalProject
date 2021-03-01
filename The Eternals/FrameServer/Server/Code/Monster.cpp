#include "stdafx.h"
#include "Monster.h"

CMonster::CMonster()
	:Hp(0), maxHp(0), Exp(0), att(0)
{
}

CMonster::~CMonster()
{
}

DWORD CMonster::Release()
{
	return 0;
}
