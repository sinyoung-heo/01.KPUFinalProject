#pragma once
class CSector
{
public:
	CSector();
	~CSector();
	
public:
	const unordered_set<int>& Get_ObjList() { return m_usetObj; }

public:
	void Add_Client(const int& new_id);
	void Leave_Client(const int& id);

public:
	void Release();

private:
	SECINFO m_tInfo;
	unordered_set<int>	m_usetObj;
public:
	mutex m_sector_lock;
};

