#include "stdafx.h"
#include "ObjectPool.h"
#include "Player.h"

CObjectPool::CObjectPool(int size)
{
	m_iMaxSize = size;

	for (int i = 0; i < m_iMaxSize; ++i)
	{
		CObj* pObj = new CPlayer();
		m_stackObj.push(pObj);
	}
}

CObjectPool::~CObjectPool()
{
	Release();
}

CObj* CObjectPool::use_Object()
{
	o_lock ol(m_mutex);

	// ���� Object�� ���� ��� Ȯ��
	if (m_stackObj.empty())
		Expand_Pool();

	CObj* pObj = m_stackObj.top();
	m_stackObj.pop();

	return pObj;
}

void CObjectPool::return_Obj(CObj* obj)
{
	o_lock ol(m_mutex);
	m_stackObj.push(obj);
}

void CObjectPool::Expand_Pool()
{
	o_lock ol(m_mutex);

	// max size��ŭ ���ο� ������ Ȯ���Ѵ�.
	for (int i = 0; i < m_iMaxSize; ++i)
	{
		CObj* pObj = new CObj();
		m_stackObj.push(pObj);
	}

	m_iMaxSize += m_iMaxSize;
}

void CObjectPool::Release()
{
	o_lock ol(m_mutex);
	while (!m_stackObj.empty())
	{
		CObj* pObj = m_stackObj.top();
		m_stackObj.pop();
		delete pObj;
	}

	m_iMaxSize = 0;
}
