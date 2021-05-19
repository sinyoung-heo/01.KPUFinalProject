#include "stdafx.h"
#include "SectorMgr.h"

IMPLEMENT_SINGLETON(CSectorMgr)

CSectorMgr::CSectorMgr(void)
{
}

CSectorMgr::~CSectorMgr(void)
{
}

const SECTOR CSectorMgr::Get_SectorList()
{
	return m_arrSectorList;
}

void CSectorMgr::Enter_ClientInSector(const int& objNum, const int& col, const int& row)
{
	m_arrSectorList[col][row].m_sector_lock.lock();
	m_arrSectorList[col][row].Add_Client(objNum);
	m_arrSectorList[col][row].m_sector_lock.unlock();
}

void CSectorMgr::Leave_ClientInSector(const int& objNum, const int& col, const int& row)
{
	m_arrSectorList[col][row].m_sector_lock.lock();
	m_arrSectorList[col][row].Leave_Client(objNum);
	m_arrSectorList[col][row].m_sector_lock.unlock();
}

void CSectorMgr::Compare_exchange_Sector(const int& objNum, const int& oriZ, const int& oriX, const int& curZ, const int& curX)
{
	int old_sectorZ = oriZ / SECTOR_SIZE;
	int old_sectorX = oriX / SECTOR_SIZE;

	int new_sectorZ = curZ / SECTOR_SIZE;
	int new_sectorX = curX / SECTOR_SIZE;

	if (new_sectorX < 0 || new_sectorZ < 0)
		return;

	if (old_sectorZ != new_sectorZ || old_sectorX != new_sectorX)
	{
		Leave_ClientInSector(objNum, old_sectorZ, old_sectorX);
		Enter_ClientInSector(objNum, new_sectorZ, new_sectorX);
	}
}

void CSectorMgr::Get_NearSectorIndex(unordered_set<pair<int, int>>* pSet, int posX, int posZ)
{
	// Front
	if (Check_NearSector(posZ, posX, posZ + VIEW_LIMIT, posX))
		pSet->emplace((posZ + VIEW_LIMIT) / SECTOR_SIZE, posX / SECTOR_SIZE);
	// Back
	if (Check_NearSector(posZ, posX, posZ - VIEW_LIMIT, posX))
		pSet->emplace((posZ - VIEW_LIMIT) / SECTOR_SIZE, posX / SECTOR_SIZE);
	// Right
	if (Check_NearSector(posZ, posX, posZ, posX + VIEW_LIMIT))
		pSet->emplace(posZ / SECTOR_SIZE, (posX + VIEW_LIMIT) / SECTOR_SIZE);
	// Left
	if (Check_NearSector(posZ, posX, posZ, posX - VIEW_LIMIT))
		pSet->emplace(posZ / SECTOR_SIZE, (posX - VIEW_LIMIT) / SECTOR_SIZE);

	// Front Right
	if (Check_NearSector(posZ, posX, posZ + VIEW_LIMIT, posX + VIEW_LIMIT))
		pSet->emplace((posZ + VIEW_LIMIT) / SECTOR_SIZE, (posX + VIEW_LIMIT) / SECTOR_SIZE);
	// Front Left
	if (Check_NearSector(posZ, posX, posZ + VIEW_LIMIT, posX - VIEW_LIMIT))
		pSet->emplace((posZ + VIEW_LIMIT) / SECTOR_SIZE, (posX - VIEW_LIMIT) / SECTOR_SIZE);
	// Back Right
	if (Check_NearSector(posZ, posX, posZ - VIEW_LIMIT, posX + VIEW_LIMIT))
		pSet->emplace((posZ - VIEW_LIMIT) / SECTOR_SIZE, (posX + VIEW_LIMIT) / SECTOR_SIZE);
	// Back Left
	if (Check_NearSector(posZ, posX, posZ - VIEW_LIMIT, posX - VIEW_LIMIT))
		pSet->emplace((posZ - VIEW_LIMIT) / SECTOR_SIZE, (posX - VIEW_LIMIT) / SECTOR_SIZE);

	// 원래 위치
	pSet->emplace(posZ / SECTOR_SIZE, posX / SECTOR_SIZE);
}

bool CSectorMgr::Check_NearSector(const int& oriZ, const int& oriX, const int& curZ, const int& curX)
{
	int old_sectorZ = oriZ / SECTOR_SIZE;
	int old_sectorX = oriX / SECTOR_SIZE;

	int new_sectorZ = curZ / SECTOR_SIZE;
	int new_sectorX = curX / SECTOR_SIZE;

	if (new_sectorX < 0 || new_sectorZ < 0)
		return false;

	if (old_sectorZ != new_sectorZ || old_sectorX != new_sectorX)
	{
		return true;
	}
	return false;
}

void CSectorMgr::Release()
{
	for (int i = 0; i < sector_col; ++i)
	{
		for (int j = 0; j < sector_row; ++j)
		{
			m_arrSectorList[i][j].Release();
		}
	}
}

