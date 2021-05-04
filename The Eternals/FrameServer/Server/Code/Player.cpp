#include "stdafx.h"
#include "Player.h"

CPlayer::CPlayer()
    :m_iLevel(0), m_iHp(0), m_iMaxHp(0),
    m_iMp(0), m_iMaxMp(0), m_iExp(0), m_iMaxExp(0),
    m_iMaxAtt(0), m_fSpd(0.f)
{
}

CPlayer::~CPlayer()
{
}

DWORD CPlayer::Release()
{
    return 0;
}
