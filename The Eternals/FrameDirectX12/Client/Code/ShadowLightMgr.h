#pragma once
#include "Include.h"
#include "Base.h"

namespace Engine
{
	class CGameObject;
}

class CShadowLightMgr : public Engine::CBase
{
	DECLARE_SINGLETON(CShadowLightMgr)

private:
	explicit CShadowLightMgr();
	virtual ~CShadowLightMgr() = default;

public:
	_matrix&			Get_MiniMapView() { return m_matMiniMapView; }
	_matrix&			Get_MiniMapProj() { return m_matMiniMapProj; }
	Engine::SHADOW_DESC&	Get_ShadowDesc()	{ return m_tShadowDesc; }
	void					Set_ShadowType(const SHADOW_TYPE& eType) { m_eShadowType = SHADOW_TYPE_PLAYER; }

	void Set_LightAt(const _vec3& vAt)			{ m_vAllSceneLightAt = vAt; }
	void Set_LightHeight(const _float& fHeight) { m_fAllSceneLightHeight = fHeight; }
	void Set_LightFovY(const _float& fFovY)		{ m_fAllSceneLightFovY = fFovY; }
	void Set_LightFar(const _float& fFar)		{ m_fAllSceneLightFar = fFar; }

	_int Update_ShadowLight();

private:
	Engine::SHADOW_DESC		m_tShadowDesc{ };
	SHADOW_TYPE				m_eShadowType = SHADOW_TYPE_ALL;
	Engine::CGameObject*	m_pThisPlayer = nullptr;

	_vec3	m_vLightEye = _vec3(0.0f, 0.0f, 0.0f);
	_float	m_fNear		= 1.0f;

	_vec3	m_vAllSceneLightAt     = _vec3(128.0f);
	_float	m_fAllSceneLightHeight = 1000.0f;
	_float	m_fAllSceneLightFovY   = 60.0f;
	_float	m_fAllSceneLightFar    = 1500.0f;

	_vec3	m_vLightAt	=_vec3(0.0f);
	_float	m_fHeight	= 300.0f;
	_float	m_fFovY		= 15.0f;
	_float	m_fFar		= 1000.0f;

	_float	m_fMiniMapHeight = 12.5f;
	_matrix	m_matMiniMapView = INIT_MATRIX;
	_matrix	m_matMiniMapProj = INIT_MATRIX;

private:
	virtual void Free();
};

