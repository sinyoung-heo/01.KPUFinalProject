#pragma once
#include "Base.h"

class CShadowLightMgr : public Engine::CBase
{
	DECLARE_SINGLETON(CShadowLightMgr)

private:
	explicit CShadowLightMgr();
	virtual ~CShadowLightMgr() = default;

public:
	_int	Update_ShadowLight();

public:
	Engine::SHADOW_DESC	m_tShadowDesc{ };

	_vec3	m_vLightEye = _vec3(0.0f, 0.0f, 0.0f);
	_vec3	m_vLightAt	=_vec3(128.0f, 0.0f, 128.0f);
	_float	m_fHeight	= 500.0f;

	_float	m_fFovY		= 30.0f;
	_float	m_fNear		= 1.0f;
	_float	m_fFar		= 10'000.0f;

private:
	virtual void Free();
};

