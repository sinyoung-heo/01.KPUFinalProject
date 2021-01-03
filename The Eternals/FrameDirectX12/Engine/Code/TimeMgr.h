#pragma once
#include "Timer.h"

BEGIN(Engine)

/*__________________________________________________________________________________________________________
[ ���� ���� Timer Ŭ�������� map �����̳ʷ� ������ TimerMgr ]
____________________________________________________________________________________________________________*/

class ENGINE_DLL CTimerMgr final : public CBase
{
	DECLARE_SINGLETON(CTimerMgr)

private:
	explicit CTimerMgr(void);
	virtual ~CTimerMgr(void) = default;
	
public:
	_float	Get_TimeDelta(wstring wstrTimerTag);
	void	SetUp_TimeDelta(wstring wstrTimerTag);
	HRESULT	Ready_Timer(wstring wstrTimerTag);

private:
	CTimer*	Find_Timer(wstring wstrTimerTag);

private:
	map<wstring, CTimer*>	m_mapTimer;

private:
	virtual void Free(void);
};


END