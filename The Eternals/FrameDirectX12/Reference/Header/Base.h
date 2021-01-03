#pragma once

#include "Engine_Include.h"


// ��� Ŭ������ �θ�
// ��� ��ü�� ���� ������ üũ�Ѵ�. (��ü�� �ּҸ� ��� �������� ������ ������ �ִ����� üũ)
namespace Engine
{
	class ENGINE_DLL CBase abstract
	{
	protected:
		explicit CBase(void);
		virtual ~CBase(void);
	public:
		unsigned long AddRef(void);
		unsigned long Release(void);
	protected:
		unsigned long		m_dwRefCnt = 0; // ��������
	protected:
		virtual void Free(void) = 0;
	};

}