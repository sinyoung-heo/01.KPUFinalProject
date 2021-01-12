#include "stdafx.h"
#include "SectorMgr.h"

IMPLEMENT_SINGLETON(CSectorMgr)

CSectorMgr::CSectorMgr(void)
{
}

CSectorMgr::~CSectorMgr(void)
{
}

const SECTOR CSectorMgr::Get_Sector()
{
	return m_arrSectorList;
}

void CSectorMgr::Enter_ClientInSector(int objNum, int col, int row)
{
	m_arrSectorList[col][row].m_sector_lock.lock();
	m_arrSectorList[col][row].Add_Client(objNum);
	m_arrSectorList[col][row].m_sector_lock.unlock();
}

void CSectorMgr::Leave_ClientInSector(int objNum, int col, int row)
{
	m_arrSectorList[col][row].m_sector_lock.lock();
	m_arrSectorList[col][row].Leave_Client(objNum);
	m_arrSectorList[col][row].m_sector_lock.unlock();
}

void CSectorMgr::Compare_exchange_Sector(int objNum, int oriY, int oriX, int curY, int curX)
{
	int old_sectorY = oriY / SECTOR_SIZE;
	int old_sectorX = oriX / SECTOR_SIZE;

	int new_sectorY = curY / SECTOR_SIZE;
	int new_sectorX = curX / SECTOR_SIZE;

	if (old_sectorY != new_sectorY || old_sectorX != new_sectorX)
	{
		Leave_ClientInSector(objNum, old_sectorY, old_sectorX);
		Enter_ClientInSector(objNum, new_sectorY, new_sectorX);
	}
}

void CSectorMgr::Get_NearSectorIndex(unordered_set<pair<int, int>>* pSet, int posX, int posZ)
{
	//상
	if (posZ >= VIEW_LIMIT)
		pSet->emplace((posZ - VIEW_LIMIT) / SECTOR_SIZE, posX / SECTOR_SIZE);
	//하
	if (posZ < WORLD_HEIGHT - VIEW_LIMIT)
		pSet->emplace((posZ + VIEW_LIMIT) / SECTOR_SIZE, posX / SECTOR_SIZE);
	//좌
	if (posX >= VIEW_LIMIT)
		pSet->emplace(posZ / SECTOR_SIZE, (posX - VIEW_LIMIT) / SECTOR_SIZE);
	//우
	if (posX < WORLD_WIDTH - VIEW_LIMIT)
		pSet->emplace(posZ / SECTOR_SIZE, (posX + VIEW_LIMIT) / SECTOR_SIZE);

	//원래위치
	pSet->emplace(posZ / SECTOR_SIZE, posX / SECTOR_SIZE);
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

