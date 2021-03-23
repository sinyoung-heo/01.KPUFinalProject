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

class CDynamicCamera final : public Engine::CCamera
{
private:
	explicit CDynamicCamera(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList);
	explicit CDynamicCamera(const CDynamicCamera& rhs);
	virtual ~CDynamicCamera() = default;

public:
	void Set_Target(Engine::CGameObject* pTarget)							{ m_pTarget = pTarget; }
	void Set_CameraAtParentMatrix(Engine::SKINNING_MATRIX* pSkinningMatrix)	{ m_pCameraAtSkinningMatrix = pSkinningMatrix; }
public:
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

