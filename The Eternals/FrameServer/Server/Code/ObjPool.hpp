#include "stdafx.h"
#include "ObjPool.h"

template<typename T>
CObjPool<T>::CObjPool(int size)
{
	m_iMaxSize = size;

	for (int i = 0; i < m_iMaxSize; ++i)
	{
		T* pObj = new T();
		m_stackObj.push(pObj);
	}
}

template<typename T>
CObjPool<T>::~CObjPool()
{
	o_lock ol(m_mutex);
	while (!m_stackObj.empty())
	{
		T* pObj = m_stackObj.top();
		m_stackObj.pop();
		delete pObj;
	}

	m_iMaxSize = 0;
}

template<typename T>
T* CObjPool<T>::use_Object()
{
	o_lock ol(m_mutex);

	// ���� Object�� ���� ��� Ȯ��
	if (m_stackObj.empty())
		Expand_Pool();

	T* pObj = m_stackObj.top();
	m_stackObj.pop();
	
	return pObj;
}

template<typename T>
inline void CObjPool<T>::return_Obj(T* obj)
{
	o_lock ol(m_mutex);
	m_stackObj.push(obj);
}

template<typename T>
void CObjPool<T>::Expand_Pool()
{
	o_lock ol(m_mutex);

	// max size��ŭ ���ο� ������ Ȯ���Ѵ�.
	for (int i = 0; i < m_iMaxSize; ++i)
	{
		T* pObj = new T();
		m_stackObj.push(pObj);
	}

	m_iMaxSize += m_iMaxSize;
}
