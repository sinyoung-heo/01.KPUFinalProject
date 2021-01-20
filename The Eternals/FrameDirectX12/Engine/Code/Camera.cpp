#include "Camera.h"
#include "GraphicDevice.h"

#include "ShaderColor.h"
#include "ShaderTexture.h"
#include "ShaderMesh.h"
#include "ShaderSkyBox.h"
#include "ShaderShadow.h"

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

	/*__________________________________________________________________________________________________________
	[ 원근 투영 행렬 ]
	____________________________________________________________________________________________________________*/
	m_tProjInfo.matProj = XMMatrixPerspectiveFovLH(XMConvertToRadians(m_tProjInfo.fFovY),
												   m_tProjInfo.fAspect,
												   m_tProjInfo.fNearZ,
												   m_tProjInfo.fFarZ);

	/*__________________________________________________________________________________________________________
	[ 직교 투영 행렬 ]
	____________________________________________________________________________________________________________*/
	m_tOrthoInfo.matProj = XMMatrixOrthographicLH(m_tOrthoInfo.fWidth,
												  m_tOrthoInfo.fHeight,
												  m_tOrthoInfo.fNearZ, 
												  m_tOrthoInfo.fFarZ);

	/*__________________________________________________________________________________________________________
	[ Set Transform ]
	____________________________________________________________________________________________________________*/
	CGraphicDevice::Get_Instance()->Set_Transform(MATRIXID::PROJECTION, &m_tProjInfo.matProj);
	CGraphicDevice::Get_Instance()->Set_Transform(MATRIXID::ORTHO, &m_tOrthoInfo.matProj);


	/*__________________________________________________________________________________________________________
	[ Shader Component 생성 ]
	____________________________________________________________________________________________________________*/
	m_pShaderColor = static_cast<CShaderColor*>(m_pComponentMgr->Clone_Component(L"ShaderColor", COMPONENTID::ID_STATIC));
	NULL_CHECK_RETURN(m_pShaderColor, E_FAIL);

	m_pShaderTexture = static_cast<CShaderTexture*>(m_pComponentMgr->Clone_Component(L"ShaderTexture", COMPONENTID::ID_STATIC));
	NULL_CHECK_RETURN(m_pShaderTexture, E_FAIL);;

	m_pShaderMesh = static_cast<CShaderMesh*>(m_pComponentMgr->Clone_Component(L"ShaderMesh", COMPONENTID::ID_STATIC));
	NULL_CHECK_RETURN(m_pShaderMesh, E_FAIL);

	m_pShaderSkyBox = static_cast<CShaderSkyBox*>(m_pComponentMgr->Clone_Component(L"ShaderSkyBox", COMPONENTID::ID_STATIC));
	NULL_CHECK_RETURN(m_pShaderSkyBox, E_FAIL);

	m_pShaderShadow = static_cast<CShaderShadow*>(m_pComponentMgr->Clone_Component(L"ShaderShadow", COMPONENTID::ID_STATIC));
	NULL_CHECK_RETURN(m_pShaderShadow, E_FAIL);

	return S_OK;
}

_int CCamera::Update_GameObject(const _float & fTimeDelta)
{
	CGameObject::Update_GameObject(fTimeDelta);

	/*__________________________________________________________________________________________________________
	[ 뷰 행렬 Update ]
	____________________________________________________________________________________________________________*/
	m_tCameraInfo.matView = XMMatrixLookAtLH(m_tCameraInfo.vEye.Get_XMVECTOR(), 
											 m_tCameraInfo.vAt.Get_XMVECTOR(), 
											 m_tCameraInfo.vUp.Get_XMVECTOR());

	/*__________________________________________________________________________________________________________
	[ Shader ConstantBuffer Update ]
	____________________________________________________________________________________________________________*/
	Set_ConstantTable();

	return NO_EVENT;
}

void CCamera::Set_ConstantTable()
{
	CB_CAMERA_MATRIX tCB_CameraMatrix;
	ZeroMemory(&tCB_CameraMatrix, sizeof(CB_CAMERA_MATRIX));

	XMStoreFloat4x4(&tCB_CameraMatrix.matView, XMMatrixTranspose(m_tCameraInfo.matView));
	XMStoreFloat4x4(&tCB_CameraMatrix.matProj, XMMatrixTranspose(m_tProjInfo.matProj));
	XMStoreFloat4x4(&tCB_CameraMatrix.matOrtho, XMMatrixTranspose(m_tOrthoInfo.matProj));
	tCB_CameraMatrix.vCameraPos = _vec4(m_tCameraInfo.vEye, 1.0f);
	tCB_CameraMatrix.fProjFar	= m_tProjInfo.fFarZ;

	m_pShaderColor->Get_UploadBuffer_CameraMatrix()->CopyData(0, tCB_CameraMatrix);

}

void CCamera::Free()
{
	CGameObject::Free();

	Safe_Release(m_pShaderColor);
	Safe_Release(m_pShaderTexture);
	Safe_Release(m_pShaderMesh);
	Safe_Release(m_pShaderSkyBox);
	Safe_Release(m_pShaderShadow);
}
