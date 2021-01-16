#include "stdafx.h"
#include "MouseMgr.h"
#include "GraphicDevice.h"
#include "ToolTerrain.h"
#include "ToolStaticMesh.h"
#include "LightMgr.h"
#include "Light.h"

IMPLEMENT_SINGLETON(CMouseMgr)

CMouseMgr::CMouseMgr()
{

}

_vec3 CMouseMgr::Picking_OnTerrain(CToolTerrain* pTerrain)
{
	POINT ptMouse{};

	GetCursorPos(&ptMouse);
	ScreenToClient(g_hWnd, &ptMouse);

	D3D12_VIEWPORT ViewPort = Engine::CGraphicDevice::Get_Instance()->Get_Viewport();

	// Window좌표 -> 투영 좌표계로 변환.
	_vec3 vMousePos = _vec3(0.f);
	vMousePos.x = ptMouse.x / (ViewPort.Width * 0.5f) - 1.f;
	vMousePos.y = ptMouse.y / -(ViewPort.Height * 0.5f) + 1.f;
	vMousePos.z = 0.f;

	// 투영 좌표계 -> 카메라 좌표계로 변환.
	_matrix matProj = *(Engine::CGraphicDevice::Get_Instance()->Get_Transform(Engine::MATRIXID::PROJECTION));
	matProj = XMMatrixInverse(nullptr, matProj);
	vMousePos.TransformCoord(vMousePos, matProj);

	// 카메라 좌표계 -> 월드 좌표계로 변환.
	_vec3 vRayDir, vRayPos;
	vRayPos = _vec3(0.f, 0.f, 0.f);
	vRayDir = vMousePos - vRayPos;

	_matrix matView = *(Engine::CGraphicDevice::Get_Instance()->Get_Transform(Engine::MATRIXID::VIEW));
	matView = XMMatrixInverse(nullptr, matView);
	vRayDir.TransformNormal(vRayDir, matView);
	vRayPos.TransformCoord(vRayPos, matView);
	
	// 월드 좌표계 -> 로컬 좌표계로 변환.
	_matrix matWorld = INIT_MATRIX;
	matWorld = XMMatrixInverse(nullptr, matWorld);
	vRayDir.TransformNormal(vRayDir, matWorld);
	vRayPos.TransformCoord(vRayPos, matWorld);


	// Picking.
	vector<Engine::VTXTEX> vecVertexBuffer = pTerrain->m_pBufferCom->Get_VertexBuffer();
	_ulong   dwVtxCntX = pTerrain->m_pBufferCom->Get_VtxCntX();
	_ulong   dwVtxCntZ = pTerrain->m_pBufferCom->Get_VtxCntZ();

	_ulong   dwVtxIdx[3];
	_float   fU, fV, fDist;

	for (_ulong i = 0; i < dwVtxCntZ - 1; ++i)
	{
		for (_ulong j = 0; j < dwVtxCntX - 1; ++j)
		{
			_ulong   dwIndex = i * dwVtxCntX + j;

			dwVtxIdx[0] = dwIndex + dwVtxCntX;
			dwVtxIdx[1] = dwIndex + dwVtxCntX + 1;
			dwVtxIdx[2] = dwIndex + 1;

			if (IntersectTriangle(vecVertexBuffer[dwVtxIdx[1]].vPos,
								  vecVertexBuffer[dwVtxIdx[2]].vPos,
								  vecVertexBuffer[dwVtxIdx[0]].vPos,
								  vRayPos, vRayDir,
								  &fU, &fV, &fDist))
			{
				return vRayPos + (vRayDir * fDist);
			}

			dwVtxIdx[0] = dwIndex + dwVtxCntX;
			dwVtxIdx[1] = dwIndex + 1;
			dwVtxIdx[2] = dwIndex;
			if (IntersectTriangle(vecVertexBuffer[dwVtxIdx[2]].vPos,
								  vecVertexBuffer[dwVtxIdx[0]].vPos,
								  vecVertexBuffer[dwVtxIdx[1]].vPos,
								  vRayPos, vRayDir,
								  &fU, &fV, &fDist))
			{
				return vRayPos + (vRayDir * fDist);
			}
		}
	}

	return _vec3(0.f, 0.f, 0.f);
}

_bool CMouseMgr::IntersectTriangle(_vec3& v0,
								   _vec3& v1, 
								   _vec3& v2, 
								   _vec3& orig, 
								   _vec3& dir, 
								   _float* u, _float* v, _float* t)
{
	// Find vectors for two edges sharing vert0
	_vec3 edge1 = v1 - v0;
	_vec3 edge2 = v2 - v0;

	// Begin calculating determinant - also used to calculate U parameter
	_vec3 pvec = dir.Cross_InputV2(edge2);

	// If determinant is near zero, ray lies in plane of triangle
	_float det = edge1.Dot(pvec);

	_vec3 tvec;
	if (det > 0)
		tvec = orig - v0;
	else
	{
		tvec = v0 - orig;
		det = -det;
	}

	if (det < 0.0001f)
		return false;
	

	// Calculate U parameter and test bounds
	*u = tvec.Dot(pvec);

	if (*u < 0.0f || *u > det)
		return false;

	// Prepare to test V parameter
	_vec3 qvec = tvec.Cross_InputV2(edge1);

	// Calculate V parameter and test bounds
	*v = dir.Dot(qvec);
	if (*v < 0.0f || *u + *v > det)
		return false;

	// Calculate t, scale parameters, ray intersects triangle
	*t = edge2.Dot(qvec);

	_float fInvDet = 1.0f / det;
	*t *= fInvDet;
	*u *= fInvDet;
	*v *= fInvDet;

	return true;
}

_bool CMouseMgr::Picking_Object(Engine::CGameObject** ppPickingObject, Engine::OBJLIST* pOBJLIST)
{
	POINT ptMouse{};

	GetCursorPos(&ptMouse);
	ScreenToClient(g_hWnd, &ptMouse);

	D3D12_VIEWPORT ViewPort = Engine::CGraphicDevice::Get_Instance()->Get_Viewport();

	// Window좌표 -> 투영 좌표계로 변환.
	_vec3 vMousePos = _vec3(0.f);
	vMousePos.x		= ptMouse.x / (ViewPort.Width * 0.5f) - 1.f;
	vMousePos.y		= ptMouse.y / -(ViewPort.Height * 0.5f) + 1.f;
	vMousePos.z		= 0.f;

	// 투영 좌표계 -> 카메라 좌표계로 변환.
	_matrix matProj = *(Engine::CGraphicDevice::Get_Instance()->Get_Transform(Engine::MATRIXID::PROJECTION));
	matProj = XMMatrixInverse(nullptr, matProj);
	vMousePos.TransformCoord(vMousePos, matProj);

	// 카메라 좌표계 -> 월드 좌표계로 변환.
	_vec3 vRayDir, vRayPos;
	vRayPos = _vec3(0.f, 0.f, 0.f);
	vRayDir = vMousePos - vRayPos;

	_matrix matView = *(Engine::CGraphicDevice::Get_Instance()->Get_Transform(Engine::MATRIXID::VIEW));
	matView = XMMatrixInverse(nullptr, matView);
	vRayDir.TransformNormal(vRayDir, matView);
	vRayPos.TransformCoord(vRayPos, matView);

	
	// BoundingBox 색상 Green으로 변경.
	for (auto& pObject : *pOBJLIST)
		pObject->Set_BoundingBoxColor(_rgba(0.0f, 1.0f, 0.0f, 1.0f));

	_float fDist = 0.0f;
	auto iter_begin = pOBJLIST->begin();
	auto iter_end	= pOBJLIST->end();
	for (; iter_begin != iter_end; ++iter_begin)
	{
		if (static_cast<CToolStaticMesh*>(*iter_begin)->m_bIsMousePicking)
		{
			vRayDir.Normalize();

			// 충돌했다면, BoundingBox의 색상을 Red로 변경.
			if ((*iter_begin)->Get_BoundingBox()->Get_BoundingInfo().Intersects(vRayPos.Get_XMVECTOR(),
																				vRayDir.Get_XMVECTOR(), 
																				fDist))
			{
				(*iter_begin)->Set_BoundingBoxColor(_rgba(1.0f, 0.0f, 0.0f, 1.0f));
				*ppPickingObject = (*iter_begin);

				return true;
			}
		}

	}

	return false;
}

_bool CMouseMgr::Picking_Light(Engine::CLight** pPickingLight, vector<Engine::CLight*>& vecPointLight)
{
	POINT ptMouse{};

	GetCursorPos(&ptMouse);
	ScreenToClient(g_hWnd, &ptMouse);

	D3D12_VIEWPORT ViewPort = Engine::CGraphicDevice::Get_Instance()->Get_Viewport();

	// Window좌표 -> 투영 좌표계로 변환.
	_vec3 vMousePos = _vec3(0.f);
	vMousePos.x		= ptMouse.x / (ViewPort.Width * 0.5f) - 1.f;
	vMousePos.y		= ptMouse.y / -(ViewPort.Height * 0.5f) + 1.f;
	vMousePos.z		= 0.f;

	// 투영 좌표계 -> 카메라 좌표계로 변환.
	_matrix matProj = *(Engine::CGraphicDevice::Get_Instance()->Get_Transform(Engine::MATRIXID::PROJECTION));
	matProj = XMMatrixInverse(nullptr, matProj);
	vMousePos.TransformCoord(vMousePos, matProj);

	// 카메라 좌표계 -> 월드 좌표계로 변환.
	_vec3 vRayDir, vRayPos;
	vRayPos = _vec3(0.f, 0.f, 0.f);
	vRayDir = vMousePos - vRayPos;

	_matrix matView = *(Engine::CGraphicDevice::Get_Instance()->Get_Transform(Engine::MATRIXID::VIEW));
	matView = XMMatrixInverse(nullptr, matView);
	vRayDir.TransformNormal(vRayDir, matView);
	vRayPos.TransformCoord(vRayPos, matView);


	// PointLight Collder 색상 초기화.
	for (auto& pPointLight : vecPointLight)
		pPointLight->Set_ColliderColorDiffuse();


	_float fDist = 0.0f;
	auto iter_begin = vecPointLight.begin();
	auto iter_end	= vecPointLight.end();
	for (; iter_begin != iter_end; ++iter_begin)
	{
		vRayDir.Normalize();

		// 충돌했다면, BoundingBox의 색상을 Red로 변경.
		if ((*iter_begin)->Get_ColliderCom()->Get_BoundingInfo().Intersects(vRayPos.Get_XMVECTOR(),
																			vRayDir.Get_XMVECTOR(),
																			fDist))
		{
			(*iter_begin)->Set_ColliderColorSelected();
			*pPickingLight = (*iter_begin);

			return true;
		}
	}




	return false;
}

void CMouseMgr::Free()
{

}
