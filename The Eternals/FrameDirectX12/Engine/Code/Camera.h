#pragma once
#include "GameObject.h"

BEGIN(Engine)

class ENGINE_DLL CCamera : public CGameObject
{
protected:
	explicit CCamera(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList);
	explicit CCamera(const CCamera& rhs);
	virtual ~CCamera() = default;

public:
	// Get
	const CAMERA_DESC&	Get_CameraInfo()								{ return m_tCameraInfo; }
	const PROJ_DESC&	Get_ProjInfo()									{ return m_tProjInfo; }
	const ORTHO_DESC&	Get_OrthInfo()									{ return m_tOrthoInfo; }

	// Set
	void				Set_CameraInfo(const CAMERA_DESC& tCameraInfo)	{ m_tCameraInfo = tCameraInfo; }
	void				Set_Eye(const _vec3& vEye)						{ m_tCameraInfo.vEye = vEye; }
	void				Set_At(const _vec3& vAt)						{ m_tCameraInfo.vEye = vAt; }
	void				Set_Up(const _vec3& vUp)						{ m_tCameraInfo.vEye = vUp; }
	void				Set_ProjInfo(const PROJ_DESC& tProjInfo)		{ m_tProjInfo = tProjInfo; }
	void				Set_OrthoInfo(const ORTHO_DESC& tOrthoInfo)		{ m_tOrthoInfo = tOrthoInfo; }

	// CGameObject��(��) ���� ��ӵ�
	virtual HRESULT		Ready_GameObject(const CAMERA_DESC& tCameraInfo,
										 const PROJ_DESC& tProjInfo,
										 const ORTHO_DESC& tOrthoInfo);
	virtual _int		Update_GameObject(const _float& fTimeDelta);

protected:
	/*____________________________________________________________________
	[ Value ]
	______________________________________________________________________*/
	CAMERA_DESC	m_tCameraInfo	{ };	// Camera - Eyte, At, Up
	PROJ_DESC	m_tProjInfo		{ };	// ���� ���� ������ ���� ����ü.
	ORTHO_DESC	m_tOrthoInfo	{ };	// ���� ���� ������ ���� ����ü.

protected:
	virtual void			Free();
};

END