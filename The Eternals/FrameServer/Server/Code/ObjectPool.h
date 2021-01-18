#pragma once
#include "Obj.h"

typedef lock_guard<recursive_mutex> o_lock;
class CObjectPool
{
public:
	CObjectPool() = default;
	explicit CObjectPool(int size);
	~CObjectPool();

public:
	template<typename T>
	HRESULT Create_ObjectPool(int size)
	{
		m_iMaxSize = size;

		for (int i = m_iMaxSize-1; i >= 0; --i)
		{
			T* pObj = new T();
			pObj->Set_ServerNumber(i);
			m_stackObj.push(pObj);
		}

		return S_OK;
	}
public:
	// Object 사용
	CObj* use_Object();
	// Object 반환
	void return_Obj(CObj* obj);

private:
	// Object Pool 확장
	void Expand_Pool();
	void Release();

private:
	recursive_mutex m_mutex;
	stack<CObj*> m_stackObj;
	int m_iMaxSize;
};

