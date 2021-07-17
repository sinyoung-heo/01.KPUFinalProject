#include "stdafx.h"
#include "DynamicCamera.h"
#include "GraphicDevice.h"
#include "DirectInput.h"
#include "ObjectMgr.h"
#include "Font.h"
#include <random>
#include "CinemaMgr.h"
#include "Lakan.h"
#include "PrionBerserkerBoss.h"
#include "CinemaVergos.h"
#include "Vergos.h"
#include "FadeInOut.h"

CDynamicCamera::CDynamicCamera(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList)
	: Engine::CCamera(pGraphicDevice, pCommandList)
{
}

void CDynamicCamera::SetUp_ThirdPersonViewOriginData()
{
	m_tThirdPersonViewOriginDesc.bIsResetting = false;
	m_tThirdPersonViewOriginDesc.vOriginScale = m_pTransCom->m_vScale;
	m_tThirdPersonViewOriginDesc.vOriginAngle = m_pTransCom->m_vAngle;
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
		switch (m_eCameraState)
		{
		case CAMERA_STATE::THIRD_PERSON_VIEW:
			if (m_tThirdPersonViewOriginDesc.bIsResetting)
				SetUp_DynamicCameraFromTarget(fTimeDelta);
			else
			{
				Set_ResetFovY();
				m_tThirdPersonViewOriginDesc.bIsResetting = true;
				m_pTransCom->m_vScale = m_tThirdPersonViewOriginDesc.vOriginScale;
				m_pTransCom->m_vAngle = m_tThirdPersonViewOriginDesc.vOriginAngle;
			}
			break;
		case CAMERA_STATE::GLADIATOR_ULTIMATE:
			SetUp_DynamicCameraGladiatorUltimate(fTimeDelta);
			break;
		case CAMERA_STATE::ARCHER_ARROW_FALL:
			SetUp_DynamicCameraArcherArrowFall(fTimeDelta);
			break;
		case CAMERA_STATE::ARCHER_ULTIMATE:
			SetUp_DynamicCameraArcherUltimate(fTimeDelta);
			break;
		case CAMERA_STATE::PRIEST_BUFF:
			SetUp_DynamicCameraPriestBuffSkill(fTimeDelta);
			break;
		case CAMERA_STATE::CINEMATIC_LAKAN_ALL:
			SetUp_DynamicCameraCinematicLakanAll(fTimeDelta);
			break;
		case CAMERA_STATE::CINEMATIC_PRION_BERSERKER_ALL:
			SetUp_DynamicCameraCinematicPrionBerserkerAll(fTimeDelta);
			break;
		case CAMERA_STATE::CINEMATIC_LAKAN_CENTER:
			SetUp_DynamicCameraCinematicLakanCenter(fTimeDelta);
			break;
		case CAMERA_STATE::CINEMATIC_PRION_BERSERKER_BOSS:
			SetUp_DynamicCameraCinematicPrionBerserkerBoss(fTimeDelta);
			break;
		case CAMERA_STATE::CINEMATIC_VERGOS_SPAWN_FLYING:
			SetUp_DynamicCameraCinematicVergosFlying(fTimeDelta);
			break;
		case CAMERA_STATE::CINEMATIC_VERGOS_SPAWN_SCREAMING:
			SetUp_DynamicCameraCinematicVergosScreaming(fTimeDelta);
			break;
		case CAMERA_STATE::CINEMATIC_SCREAMING_PRIONBERSERKER:
			SetUp_DynamicCameraCinematicPrionBerserkerScreaming(fTimeDelta);
			break;
		case CAMERA_STATE::CINEMATIC_RUSH_PRIONBERSERKER:
			SetUp_DynamicCameraCinematicPrionBerserkerRush(fTimeDelta);
			break;
		case CAMERA_STATE::CINEMATIC_RUSH_LAKAN:
			SetUp_DynamicCameraCinematicLakanRush(fTimeDelta);
			break;
		case CAMERA_STATE::CINEMATIC_ENDING:
			SetUp_DynamicCameraCinematicEnding(fTimeDelta);
			break;
		case CAMERA_STATE::CINEMATIC_VERGOS_DEATH:
			SetUp_DynamicCameraCinematicVergosDeath(fTimeDelta);
			break;
		default:
			break;
		}

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
			_matrix matBoneFinalTransform = ((m_pCameraAtSkinningMatrix->matBoneScale
											* m_pCameraAtSkinningMatrix->matBoneRotation
											* m_pCameraAtSkinningMatrix->matBoneTrans)
											* m_pCameraAtSkinningMatrix->matParentTransform)
											* m_pCameraAtSkinningMatrix->matRootTransform;

			_matrix matWorld = (matBoneFinalTransform) * (m_pTarget->Get_Transform()->m_matWorld);
			m_tCameraInfo.vAt = _vec3(matWorld._41, matWorld._42, matWorld._43);
		}
	}
	else
	{
		m_pTarget = m_pObjectMgr->Get_GameObject(L"Layer_GameObject", L"ThisPlayer");
		if (nullptr != m_pTarget)
			m_pTarget->Get_BoundingBox()->Set_SetUpCameraAt(true);
	}
}

void CDynamicCamera::SetUp_DynamicCameraGladiatorUltimate(const _float& fTimeDelta)
{
	if (nullptr != m_pTarget && PC_GLADIATOR == m_pTarget->Get_OType())
	{
		m_pTransCom->m_vScale = _vec3(0.0f, 0.0f, 4.5f);
		m_pTransCom->m_vAngle = _vec3(3.0f, m_tThirdPersonViewOriginDesc.vOriginAngle.y + 135.0f, 0.0f);
		m_pTransCom->m_vPos   = m_pTarget->Get_Transform()->m_vPos;
		Engine::CGameObject::Update_GameObject(fTimeDelta);

		m_tCameraInfo.vEye.TransformCoord(_vec3(0.0f, 0.0f, -1.0f), m_pTransCom->m_matWorld);

		if (nullptr != m_pCameraAtSkinningMatrix)
		{
			_matrix matBoneFinalTransform = ((m_pCameraAtSkinningMatrix->matBoneScale
											* m_pCameraAtSkinningMatrix->matBoneRotation
											* m_pCameraAtSkinningMatrix->matBoneTrans)
											* m_pCameraAtSkinningMatrix->matParentTransform)
											* m_pCameraAtSkinningMatrix->matRootTransform;

			_matrix matWorld = (matBoneFinalTransform) * (m_pTarget->Get_Transform()->m_matWorld);
			m_tCameraInfo.vAt = _vec3(matWorld._41, matWorld._42, matWorld._43);
		}
	}
}

void CDynamicCamera::SetUp_DynamicCameraArcherArrowFall(const _float& fTimeDelta)
{
	if (nullptr != m_pTarget && PC_ARCHER == m_pTarget->Get_OType())
	{
		m_pTransCom->m_vScale = _vec3(0.0f, 0.0f, MIN_TARGETDIST);
		m_pTransCom->m_vAngle = _vec3(5.0f, m_tThirdPersonViewOriginDesc.vOriginAngle.y, 0.0f);
		m_pTransCom->m_vPos = m_pTarget->Get_Transform()->m_vPos;
		Engine::CGameObject::Update_GameObject(fTimeDelta);

		m_tCameraInfo.vEye.TransformCoord(_vec3(0.0f, 0.0f, -1.0f), m_pTransCom->m_matWorld);

		if (nullptr != m_pCameraAtSkinningMatrix)
		{
			_matrix matBoneFinalTransform = ((m_pCameraAtSkinningMatrix->matBoneScale
											* m_pCameraAtSkinningMatrix->matBoneRotation
											* m_pCameraAtSkinningMatrix->matBoneTrans)
											* m_pCameraAtSkinningMatrix->matParentTransform)
											* m_pCameraAtSkinningMatrix->matRootTransform;

			_matrix matWorld = (matBoneFinalTransform) * (m_pTarget->Get_Transform()->m_matWorld);
			m_tCameraInfo.vAt = _vec3(matWorld._41, matWorld._42, matWorld._43);
		}
	}
}

void CDynamicCamera::SetUp_DynamicCameraArcherUltimate(const _float& fTimeDelta)
{
	if (nullptr != m_pTarget && PC_ARCHER == m_pTarget->Get_OType())
	{
		_vec3 vRight = m_pTarget->Get_Transform()->Get_RightVector();
		_vec3 vLook  = m_pTarget->Get_Transform()->Get_LookVector();
		vRight.Normalize();
		vLook.Normalize();

		m_pTransCom->m_vScale = _vec3(0.0f, 0.0f, 3.0f);
		m_pTransCom->m_vAngle = _vec3(45.0f, m_tThirdPersonViewOriginDesc.vOriginAngle.y - 45.0f, 0.0f);
		m_pTransCom->m_vPos   = m_pTarget->Get_Transform()->m_vPos;
		Engine::CGameObject::Update_GameObject(fTimeDelta);

		m_tCameraInfo.vEye.TransformCoord(_vec3(0.0f, 0.0f, -1.0f), m_pTransCom->m_matWorld);
		m_tCameraInfo.vEye  += vRight * 3.0f;
		m_tCameraInfo.vEye.y += 0.5f;

		if (nullptr != m_pCameraAtSkinningMatrix)
		{
			_matrix matBoneFinalTransform = ((m_pCameraAtSkinningMatrix->matBoneScale
											* m_pCameraAtSkinningMatrix->matBoneRotation
											* m_pCameraAtSkinningMatrix->matBoneTrans)
											* m_pCameraAtSkinningMatrix->matParentTransform)
											* m_pCameraAtSkinningMatrix->matRootTransform;

			_matrix matWorld = (matBoneFinalTransform) * (m_pTarget->Get_Transform()->m_matWorld);
			m_tCameraInfo.vAt = _vec3(m_pTransCom->m_vPos.x, matWorld._42, m_pTransCom->m_vPos.z);
			m_tCameraInfo.vAt += vLook * 3.0f;
			m_tCameraInfo.vAt.y += 0.5f;
		}

		// Camera Shaking
		SetUp_CameraShaking(fTimeDelta);
		m_tCameraInfo.vEye.x += m_tCameraShakingDesc.vEyeOffset.x;
		m_tCameraInfo.vEye.y += m_tCameraShakingDesc.vEyeOffset.y;

		// Camera Zoom
		SetUp_CameraZoom(fTimeDelta);
	}
}

void CDynamicCamera::SetUp_DynamicCameraPriestBuffSkill(const _float& fTimeDelta)
{
	if (nullptr != m_pTarget && PC_PRIEST == m_pTarget->Get_OType())
	{
		m_pTransCom->m_vScale = _vec3(0.0f, 0.0f, 6.0f);
		m_pTransCom->m_vAngle = _vec3(20.0f, m_tThirdPersonViewOriginDesc.vOriginAngle.y + 180.0f, 0.0f);
		m_pTransCom->m_vPos = m_pTarget->Get_Transform()->m_vPos;
		Engine::CGameObject::Update_GameObject(fTimeDelta);

		m_tCameraInfo.vEye.TransformCoord(_vec3(0.0f, 0.0f, -1.0f), m_pTransCom->m_matWorld);

		if (nullptr != m_pCameraAtSkinningMatrix)
		{
			_matrix matBoneFinalTransform = ((m_pCameraAtSkinningMatrix->matBoneScale
											* m_pCameraAtSkinningMatrix->matBoneRotation
											* m_pCameraAtSkinningMatrix->matBoneTrans)
											* m_pCameraAtSkinningMatrix->matParentTransform)
											* m_pCameraAtSkinningMatrix->matRootTransform;

			_matrix matWorld = (matBoneFinalTransform) * (m_pTarget->Get_Transform()->m_matWorld);
			m_tCameraInfo.vAt = _vec3(matWorld._41, matWorld._42, matWorld._43);
		}
	}
}

void CDynamicCamera::SetUp_DynamicCameraCinematicLakanAll(const _float& fTimeDelta)
{
	if (!m_bIsSettingCinematicValue)
	{
		m_bIsSettingCinematicValue = true;

		m_vEyeInterpolationDesc.is_start_interpolation = true;
		m_vEyeInterpolationDesc.linear_ratio           = 0.0f;
		m_vEyeInterpolationDesc.interpolation_speed    = 0.15f;
		m_vEyeInterpolationDesc.v1                     = _vec3(405, 1.0f, 373.0f);
		m_vEyeInterpolationDesc.v2                     = _vec3(375.0f, 2.0f, 375.0f);

		m_vAtInterpolationDesc.is_start_interpolation = true;
		m_vAtInterpolationDesc.linear_ratio           = 0.0f;
		m_vAtInterpolationDesc.interpolation_speed    = 0.15f;
		m_vAtInterpolationDesc.v1                     = _vec3(380.0f, 32.0f, 316.0f);
		m_vAtInterpolationDesc.v2                     = _vec3(405.0f, 31.0f, 315.0f);

		m_pRenderer->Set_IsRenderShadow(false);
	}

	Engine::SetUp_LinearInterpolation(fTimeDelta, m_tCameraInfo.vEye, m_vEyeInterpolationDesc);
	Engine::SetUp_LinearInterpolation(fTimeDelta, m_tCameraInfo.vAt, m_vAtInterpolationDesc);

	// Complete CameraCinematic
	if (m_vEyeInterpolationDesc.linear_ratio == 1.0f && m_vAtInterpolationDesc.linear_ratio == 1.0f)
	{
		m_bIsSettingCinematicValue = false;
		m_eCameraState = CAMERA_STATE::CINEMATIC_PRION_BERSERKER_ALL;
	}
}

void CDynamicCamera::SetUp_DynamicCameraCinematicPrionBerserkerAll(const _float& fTimeDelta)
{
	CPrionBerserkerBoss* pTarget = static_cast<CPrionBerserkerBoss*>(CCinemaMgr::Get_Instance()->Get_PrionBerserkerBoss());

	if (!m_bIsSettingCinematicValue)
	{
		m_bIsSettingCinematicValue = true;

		m_vEyeInterpolationDesc.is_start_interpolation = true;
		m_vEyeInterpolationDesc.linear_ratio           = 0.0f;
		m_vEyeInterpolationDesc.interpolation_speed    = 0.16f;
		m_vEyeInterpolationDesc.v1                     = _vec3(370.0f, 2.5f, 395.0f);
		m_vEyeInterpolationDesc.v2                     = _vec3(405.0f, 3.0f, 397.0f);

		pTarget->Set_State(PrionBerserkerBoss::ANGRY);
		m_pRenderer->Set_IsRenderShadow(true);
	}

	Engine::SetUp_LinearInterpolation(fTimeDelta, m_tCameraInfo.vEye, m_vEyeInterpolationDesc);
	m_tCameraInfo.vAt = _vec3(pTarget->Get_Transform()->m_vPos.x, 11.0f, pTarget->Get_Transform()->m_vPos.z);

	// Complete CameraCinematic
	if (m_vEyeInterpolationDesc.linear_ratio == 1.0f)
	{
		m_bIsSettingCinematicValue = false;
		m_eCameraState = CAMERA_STATE::CINEMATIC_LAKAN_CENTER;
	}
}

void CDynamicCamera::SetUp_DynamicCameraCinematicLakanCenter(const _float& fTimeDelta)
{
	if (!m_bIsSettingCinematicValue)
	{
		m_bIsSettingCinematicValue = true;

		m_fCameraTime              = 0.0f;
		m_fUpdateCameraTime        = 3.0f;
		m_pRenderer->Set_IsRenderShadow(false);
	}
	CLakan* pTarget = static_cast<CLakan*>(CCinemaMgr::Get_Instance()->Get_CenterLakan());
	m_pCameraAtSkinningMatrix = pTarget->Get_MeshComponent()->Find_SkinningMatrix("Bip01-Head");

	m_pTransCom->m_vScale = _vec3(0.0f, 0.0f, 4.0f);
	m_pTransCom->m_vAngle = _vec3(34.0f, 208.0f, 0.0f);
	m_pTransCom->m_vPos   = pTarget->Get_Transform()->m_vPos;
	Engine::CGameObject::Update_GameObject(fTimeDelta);

	m_tCameraInfo.vEye.TransformCoord(_vec3(0.0f, 0.0f, -1.0f), m_pTransCom->m_matWorld);

	if (nullptr != m_pCameraAtSkinningMatrix)
	{
		_matrix matBoneFinalTransform = ((m_pCameraAtSkinningMatrix->matBoneScale
										* m_pCameraAtSkinningMatrix->matBoneRotation
										* m_pCameraAtSkinningMatrix->matBoneTrans)
										* m_pCameraAtSkinningMatrix->matParentTransform)
										* m_pCameraAtSkinningMatrix->matRootTransform;

		_matrix matWorld = (matBoneFinalTransform) * (pTarget->Get_Transform()->m_matWorld);
		m_tCameraInfo.vAt = _vec3(matWorld._41, matWorld._42, matWorld._43);
	}

	m_fCameraTime += fTimeDelta;
	if (m_fCameraTime >= m_fUpdateCameraTime)
	{
		m_fCameraTime = 0.0f;
		m_bIsSettingCinematicValue = false;
		m_eCameraState = CAMERA_STATE::CINEMATIC_PRION_BERSERKER_BOSS;

	}
}

void CDynamicCamera::SetUp_DynamicCameraCinematicPrionBerserkerBoss(const _float& fTimeDelta)
{
	CPrionBerserkerBoss* pTarget = static_cast<CPrionBerserkerBoss*>(CCinemaMgr::Get_Instance()->Get_PrionBerserkerBoss());
	m_pCameraAtSkinningMatrix = pTarget->Get_MeshComponent()->Find_SkinningMatrix("Bip01-Head");

	if (!m_bIsSettingCinematicValue)
	{
		m_bIsSettingCinematicValue = true;

		m_fCameraTime              = 0.0f;
		m_fUpdateCameraTime        = 5.0f;

		pTarget->Set_State(PrionBerserkerBoss::COMMAND);
		m_pRenderer->Set_IsRenderShadow(true);
	}

	m_pTransCom->m_vScale = _vec3(0.0f, 0.0f, -19.0f);
	m_pTransCom->m_vAngle = _vec3(-17.0f, 201.0f, 0.0f);
	m_pTransCom->m_vPos   = pTarget->Get_Transform()->m_vPos;
	Engine::CGameObject::Update_GameObject(fTimeDelta);

	m_tCameraInfo.vEye.TransformCoord(_vec3(0.0f, 0.0f, -1.0f), m_pTransCom->m_matWorld);

	if (nullptr != m_pCameraAtSkinningMatrix)
	{
		_matrix matBoneFinalTransform = ((m_pCameraAtSkinningMatrix->matBoneScale
										* m_pCameraAtSkinningMatrix->matBoneRotation
										* m_pCameraAtSkinningMatrix->matBoneTrans)
										* m_pCameraAtSkinningMatrix->matParentTransform)
										* m_pCameraAtSkinningMatrix->matRootTransform;

		_matrix matWorld = (matBoneFinalTransform) * (pTarget->Get_Transform()->m_matWorld);
		m_tCameraInfo.vAt = _vec3(matWorld._41, matWorld._42, matWorld._43);
	}

	m_fCameraTime += fTimeDelta;
	if (m_fCameraTime >= m_fUpdateCameraTime)
	{
		m_fCameraTime = 0.0f;
		m_bIsSettingCinematicValue = false;
		m_eCameraState = CAMERA_STATE::CINEMATIC_VERGOS_SPAWN_FLYING;

		Engine::CGameObject* pGameObj = nullptr;
		pGameObj = Pop_Instance(CInstancePoolMgr::Get_Instance()->Get_FadeInOutPool());
		if (nullptr != pGameObj)
		{
			static_cast<CFadeInOut*>(pGameObj)->Set_FadeInOutEventType(EVENT_TYPE::FADE_IN);
			m_pObjectMgr->Add_GameObject(L"Layer_UI", L"FadeInOut", pGameObj);
		}
	}
}

void CDynamicCamera::SetUp_DynamicCameraCinematicVergosFlying(const _float& fTimeDelta)
{
	CCinemaVergos* pTarget = static_cast<CCinemaVergos*>(CCinemaMgr::Get_Instance()->Get_CinemaVergos());
	_uint uiCurAnimationFrame = pTarget->Get_CurAnimationFrame();
	m_pCameraAtSkinningMatrix = pTarget->Get_MeshComponent()->Find_SkinningMatrix("Bip01-Head");

	if (!m_bIsSettingCinematicValue)
	{
		m_bIsSettingCinematicValue = true;

		pTarget->Set_IsUpdate(true);
		CCinemaMgr::Get_Instance()->Spawn_Vergos();
		m_tCameraInfo.vEye = _vec3(380.0f, 0.0f, 380.0f);
		m_pRenderer->Set_IsRenderShadow(false);
	}

	if (nullptr != m_pCameraAtSkinningMatrix)
	{
		_matrix matBoneFinalTransform = ((m_pCameraAtSkinningMatrix->matBoneScale
										* m_pCameraAtSkinningMatrix->matBoneRotation
										* m_pCameraAtSkinningMatrix->matBoneTrans)
										* m_pCameraAtSkinningMatrix->matParentTransform)
										* m_pCameraAtSkinningMatrix->matRootTransform;

		_matrix matWorld = (matBoneFinalTransform) * (pTarget->Get_Transform()->m_matWorld);
		m_tCameraInfo.vAt = _vec3(matWorld._41, matWorld._42, matWorld._43);
	}

	if (uiCurAnimationFrame >= 90)
	{
		m_bIsSettingCinematicValue = false;
		m_eCameraState = CAMERA_STATE::CINEMATIC_VERGOS_SPAWN_SCREAMING;
	}
}

void CDynamicCamera::SetUp_DynamicCameraCinematicVergosScreaming(const _float& fTimeDelta)
{
	CCinemaVergos* pTarget = static_cast<CCinemaVergos*>(CCinemaMgr::Get_Instance()->Get_CinemaVergos());
	_uint uiCurAnimationFrame = pTarget->Get_CurAnimationFrame();
	m_pCameraAtSkinningMatrix = pTarget->Get_MeshComponent()->Find_SkinningMatrix("Bip01-Head");

	if (!m_bIsSettingCinematicValue)
	{
		m_bIsSettingCinematicValue = true;
		m_tCameraInfo.vEye = _vec3(232.0f, 96.0f, 600.0f);
		m_pRenderer->Set_IsRenderShadow(true);
	}

	m_tCameraInfo.vEye = _vec3(232.0f, 96.0f, 600.0f);

	// Camera Shaking
	SetUp_CameraShaking(fTimeDelta);
	m_tCameraInfo.vEye.x += m_tCameraShakingDesc.vEyeOffset.x;
	m_tCameraInfo.vEye.y += m_tCameraShakingDesc.vEyeOffset.y;

	if (nullptr != m_pCameraAtSkinningMatrix)
	{
		_matrix matBoneFinalTransform = ((m_pCameraAtSkinningMatrix->matBoneScale
										* m_pCameraAtSkinningMatrix->matBoneRotation
										* m_pCameraAtSkinningMatrix->matBoneTrans)
										* m_pCameraAtSkinningMatrix->matParentTransform)
										* m_pCameraAtSkinningMatrix->matRootTransform;

		_matrix matWorld = (matBoneFinalTransform) * (pTarget->Get_Transform()->m_matWorld);
		m_tCameraInfo.vAt = _vec3(matWorld._41, matWorld._42, matWorld._43);
	}

	if (uiCurAnimationFrame >= 565)
	{
		m_bIsSettingCinematicValue = false;
		m_fCameraTime = 0.0f;
		m_eCameraState = CAMERA_STATE::CINEMATIC_SCREAMING_PRIONBERSERKER;

		CCinemaMgr::Get_Instance()->Reset_Vergos();
	}
}

void CDynamicCamera::SetUp_DynamicCameraCinematicPrionBerserkerScreaming(const _float& fTimeDelta)
{
	CPrionBerserkerBoss* pTarget = static_cast<CPrionBerserkerBoss*>(CCinemaMgr::Get_Instance()->Get_PrionBerserkerBoss());
	m_pCameraAtSkinningMatrix = pTarget->Get_MeshComponent()->Find_SkinningMatrix("Bip01-Head");

	if (!m_bIsSettingCinematicValue)
	{
		m_bIsSettingCinematicValue = true;

		m_tProjInfo.fFovY = 50.0f;
		pTarget->Set_State(PrionBerserkerBoss::ANGRY);
		pTarget->Set_IsPrionBerserkerScreaming(true);
		m_pRenderer->Set_IsRenderShadow(true);
	}

	m_pTransCom->m_vScale = _vec3(0.0f, 0.0f, -22.8f);
	m_pTransCom->m_vAngle = _vec3(-6.0f, 556.0f, 0.0f);
	m_pTransCom->m_vPos   = pTarget->Get_Transform()->m_vPos;
	Engine::CGameObject::Update_GameObject(fTimeDelta);

	m_tCameraInfo.vEye.TransformCoord(_vec3(0.0f, 0.0f, -1.0f), m_pTransCom->m_matWorld);

	// Camera Shaking
	SetUp_CameraShaking(fTimeDelta);
	m_tCameraInfo.vEye.x += m_tCameraShakingDesc.vEyeOffset.x;
	m_tCameraInfo.vEye.y += m_tCameraShakingDesc.vEyeOffset.y;

	if (nullptr != m_pCameraAtSkinningMatrix)
	{
		_matrix matBoneFinalTransform = ((m_pCameraAtSkinningMatrix->matBoneScale
										* m_pCameraAtSkinningMatrix->matBoneRotation
										* m_pCameraAtSkinningMatrix->matBoneTrans)
										* m_pCameraAtSkinningMatrix->matParentTransform)
										* m_pCameraAtSkinningMatrix->matRootTransform;

		_matrix matWorld = (matBoneFinalTransform) * (pTarget->Get_Transform()->m_matWorld);
		m_tCameraInfo.vAt = _vec3(pTarget->Get_Transform()->m_vPos.x, matWorld._42, pTarget->Get_Transform()->m_vPos.z);
	}
}

void CDynamicCamera::SetUp_DynamicCameraCinematicPrionBerserkerRush(const _float& fTimeDelta)
{
	CPrionBerserkerBoss* pTarget = static_cast<CPrionBerserkerBoss*>(CCinemaMgr::Get_Instance()->Get_PrionBerserkerBoss());
	m_pCameraAtSkinningMatrix = pTarget->Get_MeshComponent()->Find_SkinningMatrix("Bip01-Head");

	if (!m_bIsSettingCinematicValue)
	{
		m_bIsSettingCinematicValue = true;

		m_vEyeInterpolationDesc.is_start_interpolation = true;
		m_vEyeInterpolationDesc.linear_ratio           = 0.0f;
		m_vEyeInterpolationDesc.interpolation_speed    = 0.35f;
		m_vEyeInterpolationDesc.v1                     = m_tCameraInfo.vEye;
		m_vEyeInterpolationDesc.v2                     = _vec3(387.0f, 12.0f, 470.0f);

		m_tProjInfo.fFovY = 50.0f;
		CCinemaMgr::Get_Instance()->Rush_Prion();
		m_pRenderer->Set_IsRenderShadow(true);
	}
	 
	if (pTarget->Get_Info()->m_fSpeed > 0.f)
		Engine::SetUp_LinearInterpolation(fTimeDelta, m_tCameraInfo.vEye, m_vEyeInterpolationDesc);

	if (nullptr != m_pCameraAtSkinningMatrix)
	{
		_matrix matBoneFinalTransform = ((m_pCameraAtSkinningMatrix->matBoneScale
										* m_pCameraAtSkinningMatrix->matBoneRotation
										* m_pCameraAtSkinningMatrix->matBoneTrans)
										* m_pCameraAtSkinningMatrix->matParentTransform)
										* m_pCameraAtSkinningMatrix->matRootTransform;

		_matrix matWorld = (matBoneFinalTransform) * (pTarget->Get_Transform()->m_matWorld);
		m_tCameraInfo.vAt = _vec3(pTarget->Get_Transform()->m_vPos.x, matWorld._42, pTarget->Get_Transform()->m_vPos.z);
	}

	if (m_vEyeInterpolationDesc.linear_ratio == 1.0f)
	{
		CCinemaMgr::Get_Instance()->Set_IsMoveStopPrionBerserker(true);
		m_bIsSettingCinematicValue = false;
		m_eCameraState = CAMERA_STATE::CINEMATIC_RUSH_LAKAN;
	}
}

void CDynamicCamera::SetUp_DynamicCameraCinematicLakanRush(const _float& fTimeDelta)
{
	if (!m_bIsSettingCinematicValue)
	{
		m_bIsSettingCinematicValue = true;

		m_tProjInfo.fFovY = 45.0f;
		m_tCameraInfo.vEye = _vec3(395.0f, 1.0f, 370.0f);
		CCinemaMgr::Get_Instance()->Rush_Lakan();
		m_pRenderer->Set_IsRenderShadow(true);
	}

	CLakan* pTarget = static_cast<CLakan*>(CCinemaMgr::Get_Instance()->Get_CenterLakan());
	m_pCameraAtSkinningMatrix = pTarget->Get_MeshComponent()->Find_SkinningMatrix("Bip01-Head");

	if (nullptr != m_pCameraAtSkinningMatrix)
	{
		_matrix matBoneFinalTransform = ((m_pCameraAtSkinningMatrix->matBoneScale
										* m_pCameraAtSkinningMatrix->matBoneRotation
										* m_pCameraAtSkinningMatrix->matBoneTrans)
										* m_pCameraAtSkinningMatrix->matParentTransform)
										* m_pCameraAtSkinningMatrix->matRootTransform;

		_matrix matWorld = (matBoneFinalTransform) * (pTarget->Get_Transform()->m_matWorld);
		m_tCameraInfo.vAt = _vec3(matWorld._41, matWorld._42, matWorld._43);
	}
}

void CDynamicCamera::SetUp_DynamicCameraCinematicEnding(const _float& fTimeDelta)
{
	if (!m_bIsSettingCinematicValue)
	{
		m_bIsSettingCinematicValue = true;
		m_bIsCinematicEnding = false;

		m_vEyeInterpolationDesc.is_start_interpolation = true;
		m_vEyeInterpolationDesc.linear_ratio           = 0.0f;
		m_vEyeInterpolationDesc.interpolation_speed    = 0.25f;
		m_vEyeInterpolationDesc.v1                     = m_tCameraInfo.vEye;
		m_vEyeInterpolationDesc.v2                     = _vec3(392.0f, 8.0f, 327.0f);
		m_pRenderer->Set_IsRenderShadow(true);
	}
	Engine::SetUp_LinearInterpolation(fTimeDelta, m_tCameraInfo.vEye, m_vEyeInterpolationDesc);

	if (m_vEyeInterpolationDesc.linear_ratio > 0.35f && !m_bIsCinematicEnding)
	{
		m_bIsCinematicEnding = true;

		Engine::CGameObject* pGameObj = nullptr;
		pGameObj = Pop_Instance(CInstancePoolMgr::Get_Instance()->Get_FadeInOutPool());
		if (nullptr != pGameObj)
		{
			static_cast<CFadeInOut*>(pGameObj)->Set_FadeInOutEventType(EVENT_TYPE::EVENT_CINEMATIC_ENDING);
			m_pObjectMgr->Add_GameObject(L"Layer_UI", L"FadeInOut", pGameObj);
		}

		// PrionBerserker MoveStop false
		CCinemaMgr::Get_Instance()->Set_IsMoveStopPrionBerserker(false);
	}
}

void CDynamicCamera::SetUp_DynamicCameraCinematicVergosDeath(const _float& fTimeDelta)
{
	if (!m_bIsSettingCinematicValue)
	{
		m_bIsSettingCinematicValue = true;

		if (nullptr != m_pVergos)
			m_pCameraAtSkinningMatrix = m_pVergos->Get_MeshComponent()->Find_SkinningMatrix("Head_Bone01");

		m_tCameraInfo.vEye = _vec3(400.0f, 1.25f, 350.0f);
	}

	if (nullptr != m_pVergos)
	{
		_uint uiCurFrame = m_pVergos->Get_CurAnimationFrame();
		if (uiCurFrame > 235)
			m_tCameraInfo.vEye = _vec3(368.0f, 3.0f, 352.0f);
		else
			m_tCameraInfo.vEye = _vec3(400.0f, 1.25f, 350.0f);
	}

	// Camera Shaking
	SetUp_CameraShaking(fTimeDelta);
	m_tCameraInfo.vEye.x += m_tCameraShakingDesc.vEyeOffset.x;
	m_tCameraInfo.vEye.y += m_tCameraShakingDesc.vEyeOffset.y;


	if (nullptr != m_pCameraAtSkinningMatrix)
	{
		_matrix matBoneFinalTransform = ((m_pCameraAtSkinningMatrix->matBoneScale
										* m_pCameraAtSkinningMatrix->matBoneRotation
										* m_pCameraAtSkinningMatrix->matBoneTrans)
										* m_pCameraAtSkinningMatrix->matParentTransform)
										* m_pCameraAtSkinningMatrix->matRootTransform;

		_matrix matWorld = (matBoneFinalTransform) * (m_pVergos->Get_Transform()->m_matWorld);
		m_tCameraInfo.vAt = _vec3(matWorld._41, matWorld._42, matWorld._43);
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
	CCamera::Free();
	Engine::Safe_Release(m_pFont);
	
	m_pVergos = nullptr;
}
