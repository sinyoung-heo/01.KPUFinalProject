#pragma once
class CSector
{
public:
	CSector();
	~CSector();
	
public:
	void Set_MinMax(const int& x, const int& z);
	void Set_SectorNumber(const int& num);

	const unordered_set<int>& Get_ObjList() { return m_usetObj; }
public:
	void Add_Client(const int& new_id);
	void Leave_Client(const int& id);

public:
	bool is_InSector(const int& x, const int& z);

public:
	void Release();

private:
	SECINFO m_tInfo;
	unordered_set<int>	m_usetObj;
public:
	mutex m_sector_lock;
};

