#include "stdafx.h"
#include "Obj.h"

CObj::CObj()
	:move_time(0), m_bIsConnect(false), m_bIsDead(false),
	m_vPos(0.f), m_vTempPos(0.f), m_vDir(0.f), m_vAngle(0.f),
	m_status(STATUS::ST_END), m_sNum(-1), m_naviMeshType(0)
{
	
}

CObj::~CObj()
{
}

DWORD CObj::Release()
{
	return 0;
}
