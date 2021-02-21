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
	const SECTOR Get_SectorList();

public:
	void Enter_ClientInSector(const int& objNum, const int& col, const int& row);
	void Leave_ClientInSector(const int& objNum, const int& col, const int& row);
	void Compare_exchange_Sector(const int& objNum, const int& oriZ, const int& oriX, const int& curZ, const int& curX);
	
public:
	void Get_NearSectorIndex(unordered_set<pair<int, int>>* pSet, int posX, int posZ);
private:
	bool Check_NearSector(const int& oriZ, const int& oriX, const int& curZ, const int& curX);

public:
	void Release();

private:
	CSector m_arrSectorList[sector_col][sector_row];
};

