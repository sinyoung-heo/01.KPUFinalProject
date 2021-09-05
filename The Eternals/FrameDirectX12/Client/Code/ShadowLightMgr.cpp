#include "stdafx.h"
#include "ShadowLightMgr.h"
#include "ObjectMgr.h"
#include "LightMgr.h"
#include "GameObject.h"
#include "DynamicCamera.h"

IMPLEMENT_SINGLETON(CShadowLightMgr)

CShadowLightMgr::CShadowLightMgr()
{

}

_int CShadowLightMgr::Update_ShadowLight()
{
	if (g_bIsCinemaStart)
		m_eShadowType = SHADOW_TYPE_ALL;
	else
		m_eShadowType = SHADOW_TYPE_PLAYER;

	_vec3 vLightDir = _vec3(Engine::CLightMgr::Get_Instance()->Get_LightInfo(Engine::LIGHTTYPE::D3DLIGHT_DIRECTIONAL, 0).Direction);
	vLightDir *= -1.0f;

	if (SHADOW_TYPE_ALL == m_eShadowType)
	{
		m_pThisPlayer = Engine::CObjectMgr::Get_Instance()->Get_GameObject(L"Layer_GameObject", L"ThisPlayer");
		if (nullptr != m_pThisPlayer)
		{
			char chStateID = m_pThisPlayer->Get_CurrentStageID();
			switch (chStateID)
			{
			case STAGE_VELIKA:
			{
				m_vLightEye        = _vec3(0.0f, 0.0f, 0.0f) + _vec3(STAGE_VELIKA_OFFSET_X, 0.0f, STAGE_VELIKA_OFFSET_Z);
				m_vAllSceneLightAt = _vec3(128.0f) + _vec3(STAGE_VELIKA_OFFSET_X, 0.0f, STAGE_VELIKA_OFFSET_Z);
				m_vLightEye = (vLightDir)*m_fAllSceneLightHeight;
			}
				break;
			case STAGE_BEACH:
			{
				m_vLightEye        = _vec3(0.0f, 0.0f, 0.0f) + _vec3(STAGE_BEACH_OFFSET_X, 0.0f, STAGE_BEACH_OFFSET_Z);
				m_vAllSceneLightAt = _vec3(128.0f) + _vec3(STAGE_BEACH_OFFSET_X, 0.0f, STAGE_BEACH_OFFSET_Z);
				m_vLightEye = (vLightDir)*m_fAllSceneLightHeight;
			}
				break;
			case STAGE_WINTER:
			{
				m_vLightEye = _vec3(226.f, 1200.f, 236.f);
				m_vAllSceneLightAt = _vec3(360.0f, 0.0f, 390.0f);
			}
				break;
			default:
				break;
			}
		}

		// LightView
		m_tShadowDesc.vLightPosition = _vec4(m_vLightEye, 1.0f);

		m_tShadowDesc.matLightView	 = XMMatrixLookAtLH(m_vLightEye.Get_XMVECTOR(), 
														m_vAllSceneLightAt.Get_XMVECTOR(), 
														_vec3(0.0f, 1.0f, 0.0f).Get_XMVECTOR());

		// LightProj
		m_tShadowDesc.fLightPorjFar	= m_fAllSceneLightFar;
		m_tShadowDesc.matLightProj	= XMMatrixPerspectiveFovLH(XMConvertToRadians(m_fAllSceneLightFovY), 
																				  1.0f,
																				  m_fNear, 
																				  m_fAllSceneLightFar);
	}
	else if (SHADOW_TYPE_PLAYER == m_eShadowType)
	{
		if (nullptr != m_pThisPlayer)
		{
			_vec3 vPlayerPos = m_pThisPlayer->Get_Transform()->m_vPos;

			vLightDir.x *= 0.8f;
			//vLightDir.y *= 2.0f;
			vLightDir.z *= 0.8f;

			// LightView
			m_vLightEye = vPlayerPos + (vLightDir) * m_fHeight;

			m_tShadowDesc.vLightPosition = _vec4(m_vLightEye, 1.0f);
			m_tShadowDesc.matLightView	 = XMMatrixLookAtLH(m_vLightEye.Get_XMVECTOR(), 
															vPlayerPos.Get_XMVECTOR(), 
															_vec3(0.0f, 1.0f, 0.0f).Get_XMVECTOR());

			// LightProj
			m_tShadowDesc.fLightPorjFar	= m_fFar;
			m_tShadowDesc.matLightProj	= XMMatrixPerspectiveFovLH(XMConvertToRadians(m_fFovY),
																					  1.0f,
																					  m_fNear, 
																					  m_fFar);
		}
		else
		{
			m_pThisPlayer = Engine::CObjectMgr::Get_Instance()->Get_GameObject(L"Layer_GameObject", L"ThisPlayer");
		}
	}

	// MiniMap View
	if (nullptr != m_pThisPlayer)
	{
		_vec3 vEye = m_pThisPlayer->Get_Transform()->m_vPos;
		_vec3 vAt  = m_pThisPlayer->Get_Transform()->m_vPos;

		vEye.z -= MINIMAP_OFFSET;
		vEye.y = m_fMiniMapHeight;

		m_matMiniMapView = XMMatrixLookAtLH(vEye.Get_XMVECTOR(),
											vAt.Get_XMVECTOR(), 
											_vec3(0.0f, 1.0f, 0.0f).Get_XMVECTOR());

		m_matMiniMapProj = XMMatrixPerspectiveFovLH(XMConvertToRadians(MINIMAP_FOV_Y),
																	   1.0f,
																	   1.0f, 
																	   MINIMAP_FAR);
	}
	else
	{
		m_pThisPlayer = Engine::CObjectMgr::Get_Instance()->Get_GameObject(L"Layer_GameObject", L"ThisPlayer");
	}

	return NO_EVENT;
}

void CShadowLightMgr::Free()
{
}
