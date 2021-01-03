#pragma once
#include "Include.h"
#include "Camera.h"

namespace Engine
{
	class CFont;
}

class CDebugCamera final : public Engine::CCamera
{
private:
	explicit CDebugCamera(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList);
	explicit CDebugCamera(const CDebugCamera& rhs);
	virtual ~CDebugCamera() = default;

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
	void			Key_Input(const _float& fTimeDelta);

private:
	/*__________________________________________________________________________________________________________
	[ Value ]
	____________________________________________________________________________________________________________*/
	_bool	m_bIsResetDebugCamera		= false;	// Debug Camera <-> Dynamic Camera
	_float	m_fSpeed					= 35.0f;	// Camera 이동속도.

	/*__________________________________________________________________________________________________________
	[ Font ]
	____________________________________________________________________________________________________________*/
	Engine::CFont*	m_pFont				= nullptr;
	wstring			m_wstrText			= L"";
	_tchar			m_szText[MAX_STR]	= L"";

public:
	static CDebugCamera* Create(ID3D12Device* pGraphicDevice, 
								ID3D12GraphicsCommandList* pCommandList,
								const Engine::CAMERA_DESC& tCameraInfo,
								const Engine::PROJ_DESC& tProjInfo,
								const Engine::ORTHO_DESC& tOrthoInfo);
private:
	virtual void Free();
};

