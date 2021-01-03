#include "Camera.h"
#include "GraphicDevice.h"

USING(Engine)

CCamera::CCamera(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList)
	: CGameObject(pGraphicDevice, pCommandList)
{
	ZeroMemory(&m_tCameraInfo, sizeof(CAMERA_DESC));
	ZeroMemory(&m_tProjInfo, sizeof(PROJ_DESC));
	ZeroMemory(&m_tOrthoInfo, sizeof(ORTHO_DESC));
}

CCamera::CCamera(const CCamera & rhs)
	: CGameObject(rhs)
	, m_tCameraInfo(rhs.m_tCameraInfo)
	, m_tProjInfo(rhs.m_tProjInfo)
	, m_tOrthoInfo(rhs.m_tOrthoInfo)
{
}


HRESULT CCamera::Ready_GameObject(const CAMERA_DESC& tCameraInfo,
								  const PROJ_DESC& tProjInfo,
								  const ORTHO_DESC& tOrthoInfo)
{
	m_tCameraInfo	= tCameraInfo;
	m_tProjInfo		= tProjInfo;
	m_tOrthoInfo	= tOrthoInfo;

	/*____________________________________________________________________
	[ ���� ���� ��� ]
	______________________________________________________________________*/
	m_tProjInfo.matProj = XMMatrixPerspectiveFovLH(XMConvertToRadians(m_tProjInfo.fFovY),
												   m_tProjInfo.fAspect,
												   m_tProjInfo.fNearZ,
												   m_tProjInfo.fFarZ);

	/*____________________________________________________________________
	[ ���� ���� ��� ]
	______________________________________________________________________*/
	m_tOrthoInfo.matProj = XMMatrixOrthographicLH(m_tOrthoInfo.fWidth,
												  m_tOrthoInfo.fHeight,
												  m_tOrthoInfo.fNearZ, 
												  m_tOrthoInfo.fFarZ);

	/*__________________________________________________________________________________________________________
	[ Set Transform ]
	____________________________________________________________________________________________________________*/
	CGraphicDevice::Get_Instance()->Set_Transform(MATRIXID::PROJECTION, &m_tProjInfo.matProj);
	CGraphicDevice::Get_Instance()->Set_Transform(MATRIXID::ORTHO, &m_tOrthoInfo.matProj);

	return S_OK;
}

_int CCamera::Update_GameObject(const _float & fTimeDelta)
{
	CGameObject::Update_GameObject(fTimeDelta);

	/*____________________________________________________________________
	[ �� ��� Update ]
	______________________________________________________________________*/
	m_tCameraInfo.matView = XMMatrixLookAtLH(m_tCameraInfo.vEye.Get_XMVECTOR(), 
											 m_tCameraInfo.vAt.Get_XMVECTOR(), 
											 m_tCameraInfo.vUp.Get_XMVECTOR());

	return NO_EVENT;
}

void CCamera::Free()
{
	CGameObject::Free();
}
