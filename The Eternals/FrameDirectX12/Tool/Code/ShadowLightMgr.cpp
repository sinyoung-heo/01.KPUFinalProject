#include "stdafx.h"
#include "ShadowLightMgr.h"
#include "GraphicDevice.h"
#include "LightMgr.h"

IMPLEMENT_SINGLETON(CShadowLightMgr)

CShadowLightMgr::CShadowLightMgr()
{

}

_int CShadowLightMgr::Update_ShadowLight()
{
	_vec3 vLightDir = _vec3(Engine::CLightMgr::Get_Instance()->Get_LightInfo(Engine::LIGHTTYPE::D3DLIGHT_DIRECTIONAL, 0).Direction);
	vLightDir *= -1.0f;

	// LightView
	_vec3 vEye	= m_vLightEye + (vLightDir) *m_fHeight;
	_vec3 vAt	= m_vLightAt;

	m_tShadowInfo.vLightPosition	= _vec4(m_vLightEye, 1.0f);
	m_tShadowInfo.matLightView		= XMMatrixLookAtLH(vEye.Get_XMVECTOR(),
													   vAt.Get_XMVECTOR(),
													   _vec3(0.0f, 1.0f, 0.0f).Get_XMVECTOR());
	// LightProj
	m_tShadowInfo.fLightPorjFar		= m_fFar;
	m_tShadowInfo.matLightProj		= XMMatrixPerspectiveFovLH(XMConvertToRadians(m_fFovY),
															   1.0f,
															   m_fNear, 
															   m_fFar);

	return NO_EVENT;
}

void CShadowLightMgr::Free()
{
}
