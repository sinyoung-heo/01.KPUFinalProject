#pragma once
class CItemMgr
{
	DECLARE_SINGLETON(CItemMgr)

private:
	explicit CItemMgr(void);
	virtual ~CItemMgr(void);

public:
	HRESULT		Ready_Item();
	const int&	Find_ItemNumber(const char& type, const char& name);
	
public:
	void Release();

private:
	map<int, GAMEITEM> m_mapGameItem;
};

