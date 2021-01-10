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
	// Object ���
	CObj* use_Object();
	// Object ��ȯ
	void return_Obj(CObj* obj);

	// Object Pool Ȯ��
	void Expand_Pool();
	void Release();

private:
	recursive_mutex m_mutex;
	stack<CObj*> m_stackObj;
	int m_iMaxSize;
};

