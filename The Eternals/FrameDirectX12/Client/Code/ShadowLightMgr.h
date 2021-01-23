#pragma once
#include "Include.h"
#include "Base.h"

class CShadowLightMgr : public Engine::CBase
{
	DECLARE_SINGLETON(CShadowLightMgr)

private:
	explicit CShadowLightMgr();
	virtual ~CShadowLightMgr() = default;

public:
	Engine::SHADOW_DESC& Get_ShadowDesc() { return m_tShadowDesc; }
	void Set_LightAt(const _vec3& vAt)			{ m_vLightAt = vAt; }
	void Set_LightHeight(const _float& fHeight) { m_fHeight = fHeight; }
	void Set_LightFovY(const _float& fFovY)		{ m_fFovY = fFovY; }
	void Set_LightFar(const _float& fFar)		{ m_fFar = fFar; }

	_int Update_ShadowLight();

private:
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

