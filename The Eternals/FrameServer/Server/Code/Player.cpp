#include "stdafx.h"
#include "Player.h"

CPlayer::CPlayer()
    :level(0),Hp(0),maxHp(0),Exp(0),maxExp(0),att(0),spd(0)
{
}

CPlayer::~CPlayer()
{
}

DWORD CPlayer::Release()
{
    return 0;
}
