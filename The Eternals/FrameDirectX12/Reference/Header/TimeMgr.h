#pragma once
#include "Timer.h"

BEGIN(Engine)

/*__________________________________________________________________________________________________________
[ 여러 개의 Timer 클래스들을 map 컨테이너로 관리할 TimerMgr ]
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