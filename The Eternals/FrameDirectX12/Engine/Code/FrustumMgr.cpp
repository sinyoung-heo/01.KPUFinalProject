#include "FrustumMgr.h"

#include "GraphicDevice.h"
#include "LightMgr.h"
#include "ComponentMgr.h"

USING(Engine)
IMPLEMENT_SINGLETON(CFrustumMgr)

CFrustumMgr::CFrustumMgr()
{
	/*__________________________________________________________________________________________________________
	Near    Far
	0----1  4----5
	|    |  |    |
	|    |  |    |
	3----2  7----6
	____________________________________________________________________________________________________________*/
	m_vPoint[0] = m_vOriPoint[0] = _vec3(-1.f, 1.f, 0.f);
	m_vPoint[1] = m_vOriPoint[1] = _vec3(1.f, 1.f, 0.f);
	m_vPoint[2] = m_vOriPoint[2] = _vec3(1.f, -1.f, 0.f);
	m_vPoint[3] = m_vOriPoint[3] = _vec3(-1.f, -1.f, 0.f);

	m_vPoint[4] = m_vOriPoint[4] = _vec3(-1.f, 1.f, 1.f);
	m_vPoint[5] = m_vOriPoint[5] = _vec3(1.f, 1.f, 1.f);
	m_vPoint[6] = m_vOriPoint[6] = _vec3(1.f, -1.f, 1.f);
	m_vPoint[7] = m_vOriPoint[7] = _vec3(-1.f, -1.f, 1.f);
}


void CFrustumMgr::SetUp_Frustum(const _matrix & matView, const _matrix & matProj)
{
	/*__________________________________________________________________________________________________________
	[ Plane ]
	____________________________________________________________________________________________________________*/
	_matrix matViewProj = matView * matProj;

	//가까운면
	m_vPlane[0].x = matViewProj._14 + matViewProj._13;
	m_vPlane[0].y = matViewProj._24 + matViewProj._23;
	m_vPlane[0].z = matViewProj._34 + matViewProj._33;
	m_vPlane[0].w = matViewProj._44 + matViewProj._43;
	m_vPlane[0].Normalize();

	//먼 평면
	m_vPlane[1].x = matViewProj._14 - matViewProj._13;
	m_vPlane[1].y = matViewProj._24 - matViewProj._23;
	m_vPlane[1].z = matViewProj._34 - matViewProj._33;
	m_vPlane[1].w = matViewProj._44 - matViewProj._43;
	m_vPlane[1].Normalize();

	//왼쪽 평면
	m_vPlane[2].x = matViewProj._14 + matViewProj._11;
	m_vPlane[2].y = matViewProj._24 + matViewProj._21;
	m_vPlane[2].z = matViewProj._34 + matViewProj._31;
	m_vPlane[2].w = matViewProj._44 + matViewProj._41;
	m_vPlane[2].Normalize();

	//오른쪽 평면
	m_vPlane[3].x = matViewProj._14 - matViewProj._11;
	m_vPlane[3].y = matViewProj._24 - matViewProj._21;
	m_vPlane[3].z = matViewProj._34 - matViewProj._31;
	m_vPlane[3].w = matViewProj._44 - matViewProj._41;
	m_vPlane[3].Normalize();

	//윗 평면
	m_vPlane[4].x = matViewProj._14 - matViewProj._12;
	m_vPlane[4].y = matViewProj._24 - matViewProj._22;
	m_vPlane[4].z = matViewProj._34 - matViewProj._32;
	m_vPlane[4].w = matViewProj._44 - matViewProj._42;
	m_vPlane[4].Normalize();

	//아랫 평면
	m_vPlane[5].x = matViewProj._14 + matViewProj._12;
	m_vPlane[5].y = matViewProj._24 + matViewProj._22;
	m_vPlane[5].z = matViewProj._34 + matViewProj._32;
	m_vPlane[5].w = matViewProj._44 + matViewProj._42;
	m_vPlane[5].Normalize();

	/*__________________________________________________________________________________________________________
	[ Point ]
	____________________________________________________________________________________________________________*/
	_matrix matViewInv		= XMMatrixInverse(nullptr, matView);
	_matrix matViewProjInv	= XMMatrixInverse(nullptr, matView * matProj);

	ZeroMemory(m_vPoint, sizeof(_vec3) * POINT_COUNT);
	for (int i = 0; i < POINT_COUNT; ++i)
	{
		m_vPoint[i].TransformCoord(m_vOriPoint[i], matViewProjInv);
	}

	_vec3 vCamDir = INIT_VEC3(0.0f);
	_vec3 vCamPos = INIT_VEC3(0.0f);
	memcpy(&vCamDir, &matViewInv._31, sizeof(_vec3));
	memcpy(&vCamPos, &matViewInv._41, sizeof(_vec3));

	// Center
	_vec3 vSum = _vec3{ 0.f,0.f,0.f };
	for (_int i = 0; i < POINT_COUNT; ++i)
	{
		vSum += m_vPoint[i];
	}

	m_vCenter.x = vSum.x / 8.f;
	m_vCenter.y = vSum.y / 8.f;
	m_vCenter.z = vSum.z / 8.f;

	// Min, Max Point
	m_vNearMin	= m_vPoint[0];
	m_vNearMax	= m_vPoint[0];
	m_vFarMin	= m_vPoint[0];
	m_vFarMax	= m_vPoint[0];

	for (_int i = 0; i < POINT_COUNT; ++i)
	{
		if (i >= 0 && i < POINT_COUNT / 2)
		{
			if (m_vNearMin.x > m_vPoint[i].x)
				m_vNearMin.x = m_vPoint[i].x;

			if (m_vNearMin.y > m_vPoint[i].y)
				m_vNearMin.y = m_vPoint[i].y;

			if (m_vNearMin.z > m_vPoint[i].z)
				m_vNearMin.z = m_vPoint[i].z;

			if (m_vNearMax.x < m_vPoint[i].x)
				m_vNearMax.x = m_vPoint[i].x;

			if (m_vNearMax.y < m_vPoint[i].y)
				m_vNearMax.y = m_vPoint[i].y;

			if (m_vNearMax.z < m_vPoint[i].z)
				m_vNearMax.z = m_vPoint[i].z;
		}
		else
		{
			if (m_vFarMin.x > m_vPoint[i].x)
				m_vFarMin.x = m_vPoint[i].x;

			if (m_vFarMin.y > m_vPoint[i].y)
				m_vFarMin.y = m_vPoint[i].y;

			if (m_vFarMin.z > m_vPoint[i].z)
				m_vFarMin.z = m_vPoint[i].z;

			if (m_vFarMax.x < m_vPoint[i].x)
				m_vFarMax.x = m_vPoint[i].x;

			if (m_vFarMax.y < m_vPoint[i].y)
				m_vFarMax.y = m_vPoint[i].y;

			if (m_vFarMax.z < m_vPoint[i].z)
				m_vFarMax.z = m_vPoint[i].z;
		}

		if (m_vMaxPoint.x < m_vPoint[i].x)
			m_vMaxPoint.x = m_vPoint[i].x;

		if (m_vMaxPoint.y < m_vPoint[i].y)
			m_vMaxPoint.y = m_vPoint[i].y;

		if (m_vMaxPoint.z < m_vPoint[i].z)
			m_vMaxPoint.z = m_vPoint[i].z;

		if (m_vMinPoint.x > m_vPoint[i].x)
			m_vMinPoint.x = m_vPoint[i].x;

		if (m_vMinPoint.y > m_vPoint[i].y)
			m_vMinPoint.y = m_vPoint[i].y;

		if (m_vMinPoint.z > m_vPoint[i].z)
			m_vMinPoint.z = m_vPoint[i].z;
	}

	m_fFrustumRadius = _vec3(m_vMaxPoint - m_vMinPoint).Get_Length();
	m_fFrustumLength = _vec3(m_vFarMax - m_vNearMin).Get_Length();

}

//void CFrustumMgr::Update_LightViewProjMatrix(const _matrix& matView, const _matrix& matProj)
//{
//	//_vec3 vLightDir	= _vec3(Engine::CLightMgr::Get_Instance()->Get_LightInfo(0).Direction);
//
//	//// LightView
//	//_vec3 vEye		= m_vCenterPos + (vLightDir * -1.0f) * m_fFrustumLength;
//	//_vec3 vAt		= vEye - _vec3(Engine::CLightMgr::Get_Instance()->Get_LightInfo(0).Direction) * -1.0f;
//	//_vec3 vUp		= _vec3(0.0f, 1.0f, 0.0f);
//	//m_matLightView	= XMMatrixLookAtLH(vEye.Get_XMVECTOR(), vAt.Get_XMVECTOR(), vUp.Get_XMVECTOR());
//
//	//// LightProj
//	//_vec3 v1		= m_vNearMinPos - vEye;
//	//_vec3 v2		= m_vFarMaxPos - vEye;
//
//	//_float fFovY	= (_float)((_int)(v2.Get_Angle(v1)));
//	//_float fFar		= m_fFrustumLength * 2.5f;
//	//m_matLightProj	= XMMatrixPerspectiveFovLH(XMConvertToRadians(fFovY), (_float)WINCX / (_float)WINCY, 1.0f, fFar);
//
//}

void CFrustumMgr::Free()
{
#ifdef ENGINE_LOG
	COUT_STR("Destroy FrustumMgr");
#endif

}
