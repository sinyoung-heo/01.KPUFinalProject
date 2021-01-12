#pragma once
#include "Sector.h"

constexpr int sector_col = WORLD_HEIGHT / SECTOR_SIZE;
constexpr int sector_row = WORLD_WIDTH / SECTOR_SIZE;
typedef CSector(*SECTOR)[sector_row];

class CSectorMgr
{
	DECLARE_SINGLETON(CSectorMgr)

private:
	explicit CSectorMgr(void);
	virtual ~CSectorMgr(void);

public:
	const SECTOR Get_Sector();

public:
	void Enter_ClientInSector(int objNum, int col, int row);
	void Leave_ClientInSector(int objNum, int col, int row);
	void Compare_exchange_Sector(int objNum, int oriY, int oriX, int curY, int curX);
	
public:
	void Get_NearSectorIndex(unordered_set<pair<int, int>>* pSet, int posX, int posZ);

public:
	void Release();

private:
	CSector m_arrSectorList[sector_col][sector_row];
};

