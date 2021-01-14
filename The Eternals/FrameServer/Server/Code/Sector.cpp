#include "stdafx.h"
#include "Sector.h"

CSector::CSector()
{
	ZeroMemory(&m_tInfo, 0);
}

CSector::~CSector()
{
	Release();
}

void CSector::Set_MinMax(const int& x, const int& z)
{
	m_tInfo.m_iMinX = (int)(x - SECTOR_SIZE * 0.5f);
	m_tInfo.m_iMinY = (int)(z - SECTOR_SIZE * 0.5f);
	m_tInfo.m_iMaxX = (int)(x + SECTOR_SIZE * 0.5f);
	m_tInfo.m_iMaxY = (int)(z + SECTOR_SIZE * 0.5f);
}											 

void CSector::Set_SectorNumber(const int& num)
{
	m_tInfo.m_iSectorNum = num;
}

void CSector::Add_Client(const int& new_id)
{
	m_usetObj.emplace(new_id);
}

void CSector::Leave_Client(const int& id)
{
	if (0 != m_usetObj.count(id))
		m_usetObj.erase(id);
}

bool CSector::is_InSector(const int& x, const int& z)
{
	if (m_tInfo.m_iMinX < x && x <= m_tInfo.m_iMaxX
		&& m_tInfo.m_iMinY < z && z <= m_tInfo.m_iMaxY)
		return true;

	return false;
}

void CSector::Release()
{
	m_usetObj.clear();
}