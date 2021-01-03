#pragma once
#include "Engine_Include.h"
#include "Base.h"

BEGIN(Engine)

/*__________________________________________________________________________________________________________
[ 1개의 시간 값을 관리하는 Timer 클래스 ]
____________________________________________________________________________________________________________*/

class ENGINE_DLL CTimer final : public CBase
{
private:
	explicit CTimer();
	virtual ~CTimer() = default;

public:
	_float	Get_TimeDelta(void) const { return m_fTimeDelta; }
	void	SetUp_TimeDelta(void);
	HRESULT	Ready_Timer(void);

private:
	LARGE_INTEGER	m_FrameTime		{ 0 };
	LARGE_INTEGER	m_FixTime		{ 0 };
	LARGE_INTEGER	m_LastTime		{ 0 };
	LARGE_INTEGER	m_CpuTick		{ 0 };

	_float			m_fTimeDelta	= 0.0f;

public:
	static CTimer*	Create(void);
	virtual void	Free(void);
};

END