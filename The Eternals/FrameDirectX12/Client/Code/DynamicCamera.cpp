#include "stdafx.h"
#include "DynamicCamera.h"

#include "GraphicDevice.h"
#include "DirectInput.h"
#include "ObjectMgr.h"
#include "Font.h"
#include "ColliderSphere.h"


CDynamicCamera::CDynamicCamera(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList)
	: Engine::CCamera(pGraphicDevice, pCommandList)
{
}

HRESULT CDynamicCamera::Ready_GameObject(const Engine::CAMERA_DESC& tCameraInfo,
										 const Engine::PROJ_DESC& tProjInfo,
										 const Engine::ORTHO_DESC& tOrthoInfo)
{
	Engine::FAILED_CHECK_RETURN(Engine::CGameObject::Ready_GameObject(true), E_FAIL);
	Engine::FAILED_CHECK_RETURN(Engine::CCamera::Ready_GameObject(tCameraInfo, tProjInfo, tOrthoInfo), E_FAIL);
	
	m_pTransCom->m_vAngle.x = (CAM_ANGLE_OFFSETMIN_X + CAM_ANGLE_OFFSETMAX_X) / 2.0f;
	m_pTransCom->m_vAngle.y = 0.0f;

	// Font 생성.
	m_pFont = static_cast<Engine::CFont*>(Engine::CObjectMgr::Get_Instance()->Clone_GameObjectPrototype(L"Font_NetmarbleLight"));
	Engine::NULL_CHECK_RETURN(m_pFont, E_FAIL);
	Engine::FAILED_CHECK_RETURN(m_pFont->Ready_GameObject(L"", _vec2(1150.f, 0.f), D2D1::ColorF::Cyan), E_FAIL);

	return S_OK;
}

HRESULT CDynamicCamera::LateInit_GameObject()
{
	return S_OK;
}

_int CDynamicCamera::Update_GameObject(const _float & fTimeDelta)
{
	Engine::FAILED_CHECK_RETURN(Engine::CGameObject::LateInit_GameObject(), E_FAIL);

	if (!g_bIsOnDebugCaemra)
	{
		SetUp_DynamicCameraFromTarget(fTimeDelta);

		Engine::CCamera::Update_GameObject(fTimeDelta);
		Engine::CGraphicDevice::Get_Instance()->Set_Transform(Engine::MATRIXID::VIEW, &m_tCameraInfo.matView);
	}

	return NO_EVENT;
}

_int CDynamicCamera::LateUpdate_GameObject(const _float & fTimeDelta)
{
	SetUp_CameraFont(fTimeDelta);

	return NO_EVENT;
}

void CDynamicCamera::Render_GameObject(const _float & fTimeDelta)
{
}

void CDynamicCamera::SetUp_DynamicCameraFromTarget(const _float& fTimeDelta)
{
	if (nullptr != m_pTarget)
	{
		/*__________________________________________________________________________________________________________
		[ Key Input ]
		____________________________________________________________________________________________________________*/
		// MouseCursor 활성화 시, 카메라 움직임 X.
		if (!CMouseCursorMgr::Get_Instance()->Get_IsActiveMouse())
			Key_Input(fTimeDelta);

		/*__________________________________________________________________________________________________________
		[ Target의 Position과 Distance를 기준으로 Camera의 Eye값 계산. ]
		____________________________________________________________________________________________________________*/
		m_pTransCom->m_vScale = _vec3(0.0f, 0.0f, m_fDistFromTarget);
		m_pTransCom->m_vPos   = m_pTarget->Get_Transform()->m_vPos;
		Engine::CGameObject::Update_GameObject(fTimeDelta);

		m_tCameraInfo.vEye.TransformCoord(_vec3(0.0f, 0.0f, -1.0f), m_pTransCom->m_matWorld);
		if (m_tCameraInfo.vEye.y < 1.0f)
			m_tCameraInfo.vEye.y = 1.0f;

		if (nullptr == m_pCameraAtSkinningMatrix)
		{
			SetUp_CameraAtHeightByTargetDist();
			m_tCameraInfo.vAt   = m_pTransCom->m_vPos;
			m_tCameraInfo.vAt.y = (m_pTarget->Get_BoundingBox()->Get_MaxConerPosY() + m_fAtHeightOffset);
		}
		else
		{
			_matrix matWorld = m_pTransCom->m_matWorld;
			_matrix matBoneFinalTransform = ((m_pCameraAtSkinningMatrix->matBoneScale
											* m_pCameraAtSkinningMatrix->matBoneRotation
											* m_pCameraAtSkinningMatrix->matBoneTrans)
											* m_pCameraAtSkinningMatrix->matParentTransform)
											* m_pCameraAtSkinningMatrix->matRootTransform;

			matWorld = (matBoneFinalTransform) * (m_pTarget->Get_Transform()->m_matWorld);
			m_tCameraInfo.vAt = _vec3(m_pTransCom->m_vPos.x, matWorld._42, m_pTransCom->m_vPos.z);
		}
	}
	else
	{
		m_pTarget = m_pObjectMgr->Get_GameObject(L"Layer_GameObject", L"ThisPlayer");
		if (nullptr != m_pTarget)
			m_pTarget->Get_BoundingBox()->Set_SetUpCameraAt(true);
	}
}

void CDynamicCamera::SetUp_CameraFont(const _float& fTimeDelta)
{
	if (!g_bIsOnDebugCaemra)
	{
		/*__________________________________________________________________________________________________________
		[ Font Update ]
		____________________________________________________________________________________________________________*/
		if (Engine::CRenderer::Get_Instance()->Get_RenderOnOff(L"DebugFont"))
		{
			m_wstrText = wstring(L"[ Camera Info ] \n") +
						 wstring(L"Eye (%d, %d, %d) \n") +
						 wstring(L"At (%d, %d, %d)\n") +
						 wstring(L"Angle (%d, %d, %d) \n") +
						 wstring(L"MoveSpeed (%d, %d, %d) \n") +
						 wstring(L"TargetDist : %d \n");

			wsprintf(m_szText, m_wstrText.c_str(),
					 (_int)m_tCameraInfo.vEye.x, (_int)m_tCameraInfo.vEye.y, (_int)m_tCameraInfo.vEye.z,
					 (_int)m_tCameraInfo.vAt.x, (_int)m_tCameraInfo.vAt.y, (_int)m_tCameraInfo.vAt.z,
					 (_int)m_pTransCom->m_vAngle.x, (_int)m_pTransCom->m_vAngle.y, (_int)m_pTransCom->m_vAngle.z,
					 (_int)(m_fCameraMoveResponsiveness.x * 100.0f), (_int)(m_fCameraMoveResponsiveness.y * 100.0f), (_int)(m_fCameraMoveResponsiveness.z),
					 (_int)m_fDistFromTarget);

			m_pFont->Update_GameObject(fTimeDelta);
			m_pFont->Set_Text(wstring(m_szText));
		}
	}
}

void CDynamicCamera::Key_Input(const _float & fTimeDelta)
{
	if (!g_bIsActive) return;

	_matrix matWorld = INIT_MATRIX;
	matWorld = MATRIX_INVERSE(m_tCameraInfo.matView);

	/*__________________________________________________________________________________________________________
	[ 마우스 좌, 우 이동 ]
	____________________________________________________________________________________________________________*/
	_long dwMouseMoveX = 0;
	if (dwMouseMoveX = Engine::GetDIMouseMove(Engine::MOUSEMOVESTATE::DIMS_X))
	{
		m_pTransCom->m_vAngle.y += static_cast<_float>(dwMouseMoveX) * (1.0f + m_fCameraMoveResponsiveness.x) * fTimeDelta;
	}

	/*__________________________________________________________________________________________________________
	[ 마우스 상, 하 이동 ]
	___________________________________________________________________________________________________________*/
	_long dwMouseMoveY = 0;
	if (dwMouseMoveY = Engine::GetDIMouseMove(Engine::MOUSEMOVESTATE::DIMS_Y))
	{
		m_pTransCom->m_vAngle.x += static_cast<_float>(dwMouseMoveY) * (1.0f + m_fCameraMoveResponsiveness.y) * fTimeDelta;

		if (m_pTransCom->m_vAngle.x < CAM_ANGLE_OFFSETMIN_X)
			m_fTarget_DistFromTarget += (static_cast<_float>(dwMouseMoveY) / m_fCameraMoveResponsiveness.z) * fTimeDelta;
	}

	/*__________________________________________________________________________________________________________
	[ 마우수 휠 버튼 처리. ]
	___________________________________________________________________________________________________________*/
	_long dwMouseMoveZ = 0;
	if (dwMouseMoveZ = Engine::GetDIMouseMove(Engine::MOUSEMOVESTATE::DIMS_Z))
	{
		m_fTarget_DistFromTarget += (static_cast<_float>(dwMouseMoveZ) / m_fCameraMoveResponsiveness.z) * fTimeDelta;
	}

	SetUp_TargetFromDist(fTimeDelta);
	SetUp_CameraAngle();
}

void CDynamicCamera::SetUp_TargetFromDist(const _float& fTimeDelta)
{
	if (m_fTarget_DistFromTarget > MAX_TARGETDIST)
		m_fTarget_DistFromTarget = MAX_TARGETDIST;
	else if (m_fTarget_DistFromTarget < MIN_TARGETDIST)
		m_fTarget_DistFromTarget = MIN_TARGETDIST;

	if (m_fDistFromTarget != m_fTarget_DistFromTarget)
	{
		if (m_fDistFromTarget < m_fTarget_DistFromTarget)
		{
			m_fDistFromTarget += m_fCameraMoveResponsiveness.z * fTimeDelta;
			if (m_fDistFromTarget > m_fTarget_DistFromTarget)
				m_fDistFromTarget = m_fTarget_DistFromTarget;
		}
		else if (m_fDistFromTarget > m_fTarget_DistFromTarget)
		{
			m_fDistFromTarget -= m_fCameraMoveResponsiveness.z * fTimeDelta;
			if (m_fDistFromTarget < m_fTarget_DistFromTarget)
				m_fDistFromTarget = m_fTarget_DistFromTarget;
		}
	}
}

void CDynamicCamera::SetUp_CameraAngle()
{
	if (m_pTransCom->m_vAngle.y < 0.f)
		m_pTransCom->m_vAngle.y = 359.9f;

	if (m_pTransCom->m_vAngle.y >= 360.f)
		m_pTransCom->m_vAngle.y = (_float)(static_cast<_int>(m_pTransCom->m_vAngle.y) % 360);

	if (m_pTransCom->m_vAngle.x > CAM_ANGLE_OFFSETMAX_X)
		m_pTransCom->m_vAngle.x = CAM_ANGLE_OFFSETMAX_X;
	else if (m_pTransCom->m_vAngle.x < CAM_ANGLE_OFFSETMIN_X)
		m_pTransCom->m_vAngle.x = CAM_ANGLE_OFFSETMIN_X;
}

void CDynamicCamera::SetUp_CameraAtHeightByTargetDist()
{
	m_fAtHeightOffset = (m_fDistFromTarget / (MIN_TARGETDIST + MAX_TARGETDIST)) * 1.5f;
}

Engine::CGameObject* CDynamicCamera::Create(ID3D12Device * pGraphicDevice,
											ID3D12GraphicsCommandList * pCommandList,
											const Engine::CAMERA_DESC & tCameraInfo,
											const Engine::PROJ_DESC & tProjInfo, 
											const Engine::ORTHO_DESC & tOrthoInfo)
{
	CDynamicCamera* pInstance = new CDynamicCamera(pGraphicDevice, pCommandList);

	if (FAILED(pInstance->Ready_GameObject(tCameraInfo, tProjInfo, tOrthoInfo)))
		Engine::Safe_Release(pInstance);

	return pInstance;
}

void CDynamicCamera::Free()
{
	Engine::Safe_Release(m_pFont);
	
	CCamera::Free();
}
