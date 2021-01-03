#pragma once
#include "Engine_Include.h"
#include "Base.h"

BEGIN(Engine)

class CColliderBox;

const _uint POINT_COUNT = 8;
const _uint PLANE_COUNT = 6;

class ENGINE_DLL CFrustumMgr final : public CBase
{
	DECLARE_SINGLETON(CFrustumMgr)

private:
	explicit CFrustumMgr();
	virtual ~CFrustumMgr() = default;

public:
	const _vec3&			Get_FrustumMinPoint()	{ return m_vMinPoint; }
	const _vec3&			Get_FrustumMaxPoint()	{ return m_vMaxPoint; }
	const _vec3&			Get_FrustumCenterPos()	{ return m_vCenter; }
	const _vec3&			Get_FrustumNearMinPos()	{ return m_vNearMin; }
	const _vec3&			Get_FrustumNearMaxPos()	{ return m_vNearMax; }
	const _vec3&			Get_FrustumFarMinPos()	{ return m_vFarMin; }
	const _vec3&			Get_FrustumFarMaxPos()	{ return m_vFarMax; }
	const _float&			Get_FrustumRadius()		{ return m_fFrustumRadius; }
	const _float&			Get_FrustumLength()		{ return m_fFrustumLength; }
	//const _matrix&			Get_LightViewMatrix()	{ return m_matLightView; }
	//const _matrix&			Get_LightProjNatrix()	{ return m_matLightProj; }
public:
	void	SetUp_Frustum(const _matrix& matView, const _matrix& matProj);
	//void	Update_LightViewProjMatrix(const _matrix& matView, const _matrix& matProj);
private:
	/*__________________________________________________________________________________________________________
	[ Value ]
	____________________________________________________________________________________________________________*/
	_vec3   m_vOriPoint[POINT_COUNT];
	_vec3   m_vPoint[POINT_COUNT];
	_vec4	m_vPlane[PLANE_COUNT];

	_vec3	m_vCenter			= INIT_VEC3(0.0f);
	_vec3	m_vNearMin			= INIT_VEC3(0.0f);
	_vec3	m_vNearMax			= INIT_VEC3(0.0f);
	_vec3	m_vFarMin			= INIT_VEC3(0.0f);
	_vec3	m_vFarMax			= INIT_VEC3(0.0f);
	_vec3	m_vMinPoint			= INIT_VEC3(0.0f);
	_vec3	m_vMaxPoint			= INIT_VEC3(0.0f);
	_float	m_fFrustumRadius	= 0.0f;
	_float	m_fFrustumLength	= 0.0f;

	//_matrix m_matLightView		= INIT_MATRIX;
	//_matrix m_matLightProj		= INIT_MATRIX;
	//_vec3	m_vLightPos			= INIT_VEC3(0.0f);

private:
	virtual void Free();
};

END