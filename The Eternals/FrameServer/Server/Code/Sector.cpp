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

void CSector::Add_Client(const int& new_id)
{
	m_usetObj.emplace(new_id);
}

void CSector::Leave_Client(const int& id)
{
	if (0 != m_usetObj.count(id))
		m_usetObj.erase(id);
}

void CSector::Release()
{
	m_usetObj.clear();
}