#pragma once
#include <Windows.h>

class CTimer
{
public:
	explicit CTimer();
	~CTimer();

public:
	float	Get_TimeDelta() { return m_fTimeDelta; }

public:
	HRESULT Ready_Timer();
	void	Update_Timer();

public:
	static	CTimer* Create();
	void	Free();

private:
	LARGE_INTEGER m_FrameTime;
	LARGE_INTEGER m_FixTime;
	LARGE_INTEGER m_LastTime;
	LARGE_INTEGER m_CpuTick;

	float m_fTimeDelta = 0.f;
};

