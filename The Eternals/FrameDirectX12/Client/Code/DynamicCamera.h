#pragma once
#include "Include.h"
#include "Camera.h"

namespace Engine
{
	class CFont;
}

const _float CAM_AT_HEIFHT_MIN        = 1.0f;
const _float CAM_AT_HEIFHT_MAX        = 6.0f;
const _float CAM_ANGLE_OFFSETMIN_X    = 1.0f;
const _float CAM_ANGLE_OFFSETMAX_X    = 75.0f;
const _float MIN_TARGETDIST           = 3.0f;
const _float MAX_TARGETDIST           = 12.0f;

typedef struct tagCameraShakingDesc
{
	_bool	bIsStartCameraShaking       = false;
	_bool	bIsCameraShaking            = false;
	_bool	bIsReSetting                = false;
	_float	fShakingTime                = 0.0f;			// 몇 초동안 카메라 쉐이킹을 할 것인가?
	_float	fUpdateShakingTime          = 0.0f;
	_vec2	vMin                        = _vec2(0.0f);	// Range Min
	_vec2	vMax                        = _vec2(0.0f);	// Range Max
	_vec2	vEyeOffset                  = _vec2(0.0f);
	_vec2	vPreOffsetRange             = _vec2(0.0f);
	Engine::LINEAR_INTERPOLATION_DESC<_vec2> tOffsetInterpolationDesc;

} CAMERA_SHAKING_DESC;

typedef struct tagCameraZoomDesc
{
	_bool		bIsStartZoomInOut = false;
	_bool		bIsCameraZoom     = false;
	CAMERA_ZOOM eZoomState        = CAMERA_ZOOM::ZOOM_END;
	_float		fPower            = 0.0f;
	Engine::LINEAR_INTERPOLATION_DESC<_float> tFovYInterpolationDesc;

} CAMERA_ZOOM_DESC;

typedef struct tagCameraOriginDesc
{
	_bool bIsResetting = true;
	_vec3 vOriginScale = _vec3(0.0f);
	_vec3 vOriginAngle = _vec3(0.0f);

} CAMERA_ORIGIN_DESC;

class CVergos;

class CDynamicCamera final : public Engine::CCamera
{
private:
	explicit CDynamicCamera(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList);
	virtual ~CDynamicCamera() = default;

public:
	void Set_Target(Engine::CGameObject* pTarget)							{ m_pTarget = pTarget; }
	void Set_Vergos(CVergos* pVergos)										{ m_pVergos = pVergos; }
	void Set_CameraAtParentMatrix(Engine::SKINNING_MATRIX* pSkinningMatrix)	{ m_pCameraAtSkinningMatrix = pSkinningMatrix; }
	void Set_CameraState(const CAMERA_STATE& eState)						{ m_eCameraState = eState; }
	void Set_CameraStateCinematicEnding()									{ if (m_eCameraState != CAMERA_STATE::CINEMATIC_ENDING) m_eCameraState = CAMERA_STATE::CINEMATIC_ENDING; }
	void Set_IsSettingCameraCinematicValue(const _bool& bIsSetting)			{ m_bIsSettingCinematicValue = bIsSetting; }
	void Set_IsCinematicEnding(const _bool& bIsEnding)						{ m_bIsCinematicEnding = bIsEnding; }
	void SetUp_ThirdPersonViewOriginData();
public:
	const CAMERA_SHAKING_DESC& Get_CameraShakingDesc() { return m_tCameraShakingDesc; }
	void Set_CameraShakingDesc(const CAMERA_SHAKING_DESC& tDesc);
	void Set_CameraZoomDesc(const CAMERA_ZOOM_DESC& tDesc);

	// CGameObject을(를) 통해 상속됨
	virtual HRESULT	Ready_GameObject(const Engine::CAMERA_DESC& tCameraInfo,
									 const Engine::PROJ_DESC& tProjInfo,
									 const Engine::ORTHO_DESC& tOrthoInfo);
	virtual HRESULT	LateInit_GameObject();
	virtual _int	Update_GameObject(const _float& fTimeDelta);
	virtual _int	LateUpdate_GameObject(const _float& fTimeDelta);
	virtual void	Render_GameObject(const _float& fTimeDelta);
private:
	void SetUp_DynamicCameraFromTarget(const _float& fTimeDelta);
	void SetUp_DynamicCameraGladiatorUltimate(const _float& fTimeDelta);
	void SetUp_DynamicCameraArcherArrowFall(const _float& fTimeDelta);
	void SetUp_DynamicCameraArcherUltimate(const _float& fTimeDelta);
	void SetUp_DynamicCameraPriestBuffSkill(const _float& fTimeDelta);
	void SetUp_DynamicCameraCinematicLakanAll(const _float& fTimeDelta);
	void SetUp_DynamicCameraCinematicPrionBerserkerAll(const _float& fTimeDelta);
	void SetUp_DynamicCameraCinematicLakanCenter(const _float& fTimeDelta);
	void SetUp_DynamicCameraCinematicPrionBerserkerBoss(const _float& fTimeDelta);
	void SetUp_DynamicCameraCinematicVergosFlying(const _float& fTimeDelta);
	void SetUp_DynamicCameraCinematicVergosScreaming(const _float& fTimeDelta);
	void SetUp_DynamicCameraCinematicPrionBerserkerScreaming(const _float& fTimeDelta);
	void SetUp_DynamicCameraCinematicPrionBerserkerRush(const _float& fTimeDelta);
	void SetUp_DynamicCameraCinematicLakanRush(const _float& fTimeDelta);
	void SetUp_DynamicCameraCinematicEnding(const _float& fTimeDelta);
	void SetUp_DynamicCameraCinematicVergosDeath(const _float& fTimeDelta);
	void SetUp_CameraShaking(const _float& fTimeDelta);
	void SetUp_CameraZoom(const _float& fTimeDelta);
	void SetUp_CameraFont(const _float& fTimeDelta);
	void Key_Input(const _float& fTimeDelta);
	void SetUp_TargetFromDist(const _float& fTimeDelta);
	void SetUp_CameraAngle();
	void SetUp_CameraAtHeightByTargetDist();
private:
	/*__________________________________________________________________________________________________________
	[ Value ]
	____________________________________________________________________________________________________________*/
	Engine::CGameObject*		m_pTarget					= nullptr;
	Engine::SKINNING_MATRIX*	m_pCameraAtSkinningMatrix	= nullptr;
	_float						m_fAtHeightOffset			= 0.0f;
	_float						m_fDistFromTarget			= (MIN_TARGETDIST + MAX_TARGETDIST) / 2.0f;
	_float						m_fTarget_DistFromTarget	= (MIN_TARGETDIST + MAX_TARGETDIST) / 2.0f;
	_vec3						m_fCameraMoveResponsiveness = _vec3(0.0f, 0.0f, 6.0f);

	CAMERA_STATE		m_eCameraState = CAMERA_STATE::THIRD_PERSON_VIEW;
	CAMERA_SHAKING_DESC	m_tCameraShakingDesc;
	CAMERA_ZOOM_DESC	m_tCameraZoomDesc;
	CAMERA_ORIGIN_DESC	m_tThirdPersonViewOriginDesc;

	// Cinematic Camera
	_bool m_bIsSettingCinematicValue = false;
	_bool m_bIsCinematicEnding       = false;
	Engine::LINEAR_INTERPOLATION_DESC<_vec3> m_vEyeInterpolationDesc;
	Engine::LINEAR_INTERPOLATION_DESC<_vec3> m_vAtInterpolationDesc;
	_float m_fCameraTime       = 0.0f;
	_float m_fUpdateCameraTime = 2.5f;

	CVergos* m_pVergos = nullptr;

	/*__________________________________________________________________________________________________________
	[ Font ]
	____________________________________________________________________________________________________________*/
	Engine::CFont*	m_pFont				= nullptr;
	wstring			m_wstrText			= L"";
	_tchar			m_szText[MAX_STR]	= L"";
public:
	static Engine::CGameObject* Create(ID3D12Device* pGraphicDevice,
									   ID3D12GraphicsCommandList* pCommandList,
									   const Engine::CAMERA_DESC& tCameraInfo,
									   const Engine::PROJ_DESC& tProjInfo,
									   const Engine::ORTHO_DESC& tOrthoInfo);
private:
	virtual void Free();
};

