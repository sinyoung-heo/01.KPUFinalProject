#include "stdafx.h"
#include "DynamicCamera.h"
#include "GraphicDevice.h"
#include "DirectInput.h"
#include "ObjectMgr.h"
#include "Font.h"
#include <random>


CDynamicCamera::CDynamicCamera(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList)
	: Engine::CCamera(pGraphicDevice, pCommandList)
{
}

void CDynamicCamera::Set_CameraShakingDesc(const CAMERA_SHAKING_DESC& tDesc)
{
	m_tCameraShakingDesc = tDesc;
	m_tCameraShakingDesc.bIsStartCameraShaking = true;
	m_tCameraShakingDesc.bIsCameraShaking      = true;
	m_tCameraShakingDesc.bIsReSetting          = true;
	m_tCameraShakingDesc.fShakingTime          = 0.0f;

	random_device					rd;
	default_random_engine			dre{ rd() };
	uniform_int_distribution<_int>	uid_x{ (_int)(m_tCameraShakingDesc.vMin.x), (_int)(m_tCameraShakingDesc.vMax.x) };
	uniform_int_distribution<_int>	uid_y{ (_int)(m_tCameraShakingDesc.vMin.y), (_int)(m_tCameraShakingDesc.vMax.y) };
	
	_float fInputX = (_float)(uid_x(dre));
	_float fInputY = (_float)(uid_y(dre));

	if (fInputX < 0)
	{
		if (abs(fInputX) < abs(m_tCameraShakingDesc.vMin.x) * 0.7f)
			fInputX = m_tCameraShakingDesc.vMin.x * 0.7f;
	}
	else if (fInputX > 0)
	{
		if (fInputX < m_tCameraShakingDesc.vMax.x * 0.7f)
			fInputX = m_tCameraShakingDesc.vMax.x * 0.7f;
	}

	if (fInputY < 0)
	{
		if (abs(fInputY) < abs(m_tCameraShakingDesc.vMin.y) * 0.7f)
			fInputY = m_tCameraShakingDesc.vMin.y * 0.7f;
	}
	else if (fInputY > 0)
	{
		if (fInputY < m_tCameraShakingDesc.vMax.y * 0.7f)
			fInputY = m_tCameraShakingDesc.vMax.y * 0.7f;
	}

	if (fInputX == 0 && m_tCameraShakingDesc.vMin.x != 0.f && m_tCameraShakingDesc.vMax.x != 0.0f)
	{
		_int temp = rand() % 2;
		if (temp % 2 == 0)
			fInputX = m_tCameraShakingDesc.vMax.x / 2.0f;
		else
			fInputX = -(m_tCameraShakingDesc.vMax.x) / 2.0f;

	}
	if (fInputY == 0 && m_tCameraShakingDesc.vMin.y != 0.f && m_tCameraShakingDesc.vMax.y != 0.0f)
	{
		_int temp = rand() % 2;
		if (temp % 2 == 0)
			fInputY = m_tCameraShakingDesc.vMax.y / 2.0f;
		else
			fInputY = -(m_tCameraShakingDesc.vMax.y) / 2.0f;
	}

	m_tCameraShakingDesc.tOffsetInterpolationDesc.v1                     = _vec2(0.0f);
	m_tCameraShakingDesc.tOffsetInterpolationDesc.v2                     = _vec2(fInputX * 0.01f, fInputY * 0.01f);
	m_tCameraShakingDesc.tOffsetInterpolationDesc.linear_ratio           = 0.0f;
	m_tCameraShakingDesc.tOffsetInterpolationDesc.is_start_interpolation = true;

	m_tCameraShakingDesc.vPreOffsetRange = _vec2(fInputX, fInputY);
}

void CDynamicCamera::Set_CameraZoomDesc(const CAMERA_ZOOM_DESC& tDesc)
{
	m_tCameraZoomDesc = tDesc;
	m_tCameraZoomDesc.bIsStartZoomInOut = true;
	m_tCameraZoomDesc.bIsCameraZoom     = true;

	m_tCameraZoomDesc.tFovYInterpolationDesc.linear_ratio           = 0.0f;
	m_tCameraZoomDesc.tFovYInterpolationDesc.is_start_interpolation = true;
	m_tCameraZoomDesc.tFovYInterpolationDesc.v1 = m_tProjInfo.fFovY;

	if (CAMERA_ZOOM::ZOOM_IN == m_tCameraZoomDesc.eZoomState)
		m_tCameraZoomDesc.tFovYInterpolationDesc.v2 = m_tProjInfo.fFovY - m_tProjInfo.fFovY * m_tCameraZoomDesc.fPower;
	else
		m_tCameraZoomDesc.tFovYInterpolationDesc.v2 = m_tProjInfo.fFovY + m_tProjInfo.fFovY * m_tCameraZoomDesc.fPower;

}

HRESULT CDynamicCamera::Ready_GameObject(const Engine::CAMERA_DESC& tCameraInfo,
										 const Engine::PROJ_DESC& tProjInfo,
										 const Engine::ORTHO_DESC& tOrthoInfo)
{
	Engine::FAILED_CHECK_RETURN(Engine::CGameObject::Ready_GameObject(true), E_FAIL);
	Engine::FAILED_CHECK_RETURN(Engine::CCamera::Ready_GameObject(tCameraInfo, tProjInfo, tOrthoInfo), E_FAIL);
	
	//m_pTransCom->m_vAngle.x = (CAM_ANGLE_OFFSETMIN_X + CAM_ANGLE_OFFSETMAX_X) / 2.0f;
	//m_pTransCom->m_vAngle.y = 0.0f;
	m_pTransCom->m_vAngle.x = 15.0f;
	m_pTransCom->m_vAngle.y = 330.0f;
	m_fDistFromTarget = 6.0f;

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

			// Camera Shaking
			SetUp_CameraShaking(fTimeDelta);
			m_tCameraInfo.vEye.x += m_tCameraShakingDesc.vEyeOffset.x;
			m_tCameraInfo.vEye.y += m_tCameraShakingDesc.vEyeOffset.y;

			// Camera Zoom
			SetUp_CameraZoom(fTimeDelta);
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

void CDynamicCamera::SetUp_CameraShaking(const _float& fTimeDelta)
{
	if (!m_tCameraShakingDesc.bIsStartCameraShaking)
		return;

	if (m_tCameraShakingDesc.bIsCameraShaking)
	{
		m_tCameraShakingDesc.fShakingTime += fTimeDelta;

		if (m_tCameraShakingDesc.fShakingTime >= m_tCameraShakingDesc.fUpdateShakingTime &&
			m_tCameraShakingDesc.tOffsetInterpolationDesc.linear_ratio >= 1.0f)
		{
			m_tCameraShakingDesc.bIsCameraShaking = false;
			m_tCameraShakingDesc.tOffsetInterpolationDesc.linear_ratio           = 0.0f;
			m_tCameraShakingDesc.tOffsetInterpolationDesc.is_start_interpolation = true;
			m_tCameraShakingDesc.tOffsetInterpolationDesc.v1                     = m_tCameraShakingDesc.vEyeOffset;
			m_tCameraShakingDesc.tOffsetInterpolationDesc.v2                     = _vec2(0.0f);
		}

		if (!m_tCameraShakingDesc.bIsReSetting && m_tCameraShakingDesc.tOffsetInterpolationDesc.linear_ratio >= 1.0f)
		{
			m_tCameraShakingDesc.bIsReSetting = true;

			random_device					rd;
			default_random_engine			dre{ rd() };
			uniform_int_distribution<_int>	uid_x{ (_int)(m_tCameraShakingDesc.vMin.x), (_int)(m_tCameraShakingDesc.vMax.x) };
			uniform_int_distribution<_int>	uid_y{ (_int)(m_tCameraShakingDesc.vMin.y), (_int)(m_tCameraShakingDesc.vMax.y) };

			_float fInputX = (_float)(uid_x(dre));
			_float fInputY = (_float)(uid_y(dre));

			if (fInputX < 0)
			{
				if (abs(fInputX) < abs(m_tCameraShakingDesc.vMin.x) * 0.7f)
					fInputX = m_tCameraShakingDesc.vMin.x * 0.7f;
			}
			else if (fInputX > 0)
			{
				if (fInputX < m_tCameraShakingDesc.vMax.x * 0.7f)
					fInputX = m_tCameraShakingDesc.vMax.x * 0.7f;
			}

			if (fInputY < 0)
			{
				if (abs(fInputY) < abs(m_tCameraShakingDesc.vMin.y) * 0.7f)
					fInputY = m_tCameraShakingDesc.vMin.y * 0.7f;
			}
			else if (fInputY > 0)
			{
				if (fInputY < m_tCameraShakingDesc.vMax.y * 0.7f)
					fInputY = m_tCameraShakingDesc.vMax.y * 0.7f;
			}

			if (fInputX == 0 && m_tCameraShakingDesc.vMin.x != 0.f && m_tCameraShakingDesc.vMax.x != 0.0f)
			{
				_int temp = rand() % 2;
				if (temp % 2 == 0)
					fInputX = m_tCameraShakingDesc.vMax.x / 2.0f;
				else
					fInputX = -(m_tCameraShakingDesc.vMax.x) / 2.0f;

			}
			if (fInputY == 0 && m_tCameraShakingDesc.vMin.y != 0.f && m_tCameraShakingDesc.vMax.y != 0.0f)
			{
				_int temp = rand() % 2;
				if (temp % 2 == 0)
					fInputY = m_tCameraShakingDesc.vMax.y / 2.0f;
				else
					fInputY = -(m_tCameraShakingDesc.vMax.y) / 2.0f;
			}

			if (0 < m_tCameraShakingDesc.vPreOffsetRange.x)
				fInputX = abs(fInputX);
			else
				fInputX = abs(fInputX) * -1.0f;

			if (0 < m_tCameraShakingDesc.vPreOffsetRange.y)
				fInputY = abs(fInputY);
			else
				fInputY = abs(fInputY) * -1.0f;

			m_tCameraShakingDesc.tOffsetInterpolationDesc.linear_ratio           = 0.0f;
			m_tCameraShakingDesc.tOffsetInterpolationDesc.is_start_interpolation = true;
			m_tCameraShakingDesc.tOffsetInterpolationDesc.v1                     = _vec2(0.0f);
			m_tCameraShakingDesc.tOffsetInterpolationDesc.v2                     = _vec2(fInputX * 0.01f, fInputY * 0.01f);

			m_tCameraShakingDesc.vPreOffsetRange = m_tCameraShakingDesc.tOffsetInterpolationDesc.v2;
		}

		if (m_tCameraShakingDesc.bIsReSetting && m_tCameraShakingDesc.tOffsetInterpolationDesc.linear_ratio >= 1.0f)
		{
			m_tCameraShakingDesc.bIsReSetting = false;

			m_tCameraShakingDesc.tOffsetInterpolationDesc.linear_ratio           = 0.0f;
			m_tCameraShakingDesc.tOffsetInterpolationDesc.is_start_interpolation = true;
			m_tCameraShakingDesc.tOffsetInterpolationDesc.v1                     = m_tCameraShakingDesc.vEyeOffset;
			m_tCameraShakingDesc.tOffsetInterpolationDesc.v2                     = _vec2(0.0f);
		}

		Engine::SetUp_LinearInterpolation(fTimeDelta, m_tCameraShakingDesc.vEyeOffset, m_tCameraShakingDesc.tOffsetInterpolationDesc);
	}
	else
	{
		if (m_tCameraShakingDesc.tOffsetInterpolationDesc.linear_ratio >= 1.0f)
		{
			m_tCameraShakingDesc.bIsStartCameraShaking = false;
			return;
		}

		Engine::SetUp_LinearInterpolation(fTimeDelta, m_tCameraShakingDesc.vEyeOffset, m_tCameraShakingDesc.tOffsetInterpolationDesc);
	}
}

void CDynamicCamera::SetUp_CameraZoom(const _float& fTimeDelta)
{
	if (!m_tCameraZoomDesc.bIsStartZoomInOut)
		return;

	if (m_tCameraZoomDesc.bIsCameraZoom)
	{
		if (m_tCameraZoomDesc.tFovYInterpolationDesc.linear_ratio >= 1.0f)
		{
			m_tCameraZoomDesc.bIsCameraZoom = false;

			m_tCameraZoomDesc.tFovYInterpolationDesc.linear_ratio           = 0.0f;
			m_tCameraZoomDesc.tFovYInterpolationDesc.is_start_interpolation = true;
			m_tCameraZoomDesc.tFovYInterpolationDesc.v1                     = m_tProjInfo.fFovY;
			m_tCameraZoomDesc.tFovYInterpolationDesc.v2                     = m_fOriginFovY;
		}

		if (m_tCameraZoomDesc.bIsCameraZoom)
			Engine::SetUp_LinearInterpolation(fTimeDelta, m_tProjInfo.fFovY, m_tCameraZoomDesc.tFovYInterpolationDesc);
	}
	else
	{
		if (m_tCameraZoomDesc.tFovYInterpolationDesc.linear_ratio >= 1.0f)
		{
			m_tCameraZoomDesc.bIsStartZoomInOut = false;
			return;
		}

		Engine::SetUp_LinearInterpolation(fTimeDelta, m_tProjInfo.fFovY, m_tCameraZoomDesc.tFovYInterpolationDesc);
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
	//if (m_pTransCom->m_vAngle.y < 0.f)
	//	m_pTransCom->m_vAngle.y = 359.9f;

	//if (m_pTransCom->m_vAngle.y >= 360.f)
	//	m_pTransCom->m_vAngle.y = (_float)(static_cast<_int>(m_pTransCom->m_vAngle.y) % 360);

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
