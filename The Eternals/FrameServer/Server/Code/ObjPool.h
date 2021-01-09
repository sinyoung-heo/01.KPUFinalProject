#pragma once

typedef lock_guard<recursive_mutex> o_lock;

template<typename T>
class CObjPool
{
public:
	CObjPool(int size);
	~CObjPool();

public:
	// Object 사용
	T* use_Object();
	// Object 반환
	void return_Obj(T* obj);
	// Object Pool 확장
	void Expand_Pool();

private:
	recursive_mutex m_mutex;
	stack<T*> m_stackObj;
	int m_iMaxSize;
};
#include "ObjPool.hpp"
