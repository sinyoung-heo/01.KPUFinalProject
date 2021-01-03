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

CDynamicCamera::CDynamicCamera(const CDynamicCamera & rhs)
	: Engine::CCamera(rhs)
{
}


HRESULT CDynamicCamera::Ready_GameObject(const Engine::CAMERA_DESC& tCameraInfo,
										 const Engine::PROJ_DESC& tProjInfo,
										 const Engine::ORTHO_DESC& tOrthoInfo)
{
	Engine::FAILED_CHECK_RETURN(Engine::CCamera::Ready_GameObject(tCameraInfo, tProjInfo, tOrthoInfo), E_FAIL);

	// Font 생성.
	m_pFont = static_cast<Engine::CFont*>(Engine::CObjectMgr::Get_Instance()->Clone_GameObjectPrototype(L"Prototype_Font_NetmarbleLight"));
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
		/*__________________________________________________________________________________________________________
		[ Key Input ]
		____________________________________________________________________________________________________________*/
		Key_Input(fTimeDelta);

		/*__________________________________________________________________________________________________________
		[ View Matrix Update ]
		____________________________________________________________________________________________________________*/
		Engine::CCamera::Update_GameObject(fTimeDelta);

		/*__________________________________________________________________________________________________________
		[ Font Update ]
		____________________________________________________________________________________________________________*/
		if (Engine::CRenderer::Get_Instance()->Get_RenderOnOff(L"Font"))
		{
			m_wstrText = wstring(L"[ Camera Info ] \n") +
						 wstring(L"Eye\t(%d, %d, %d) \n") +
						 wstring(L"At\t(%d, %d, %d)\n");

			wsprintf(m_szText, m_wstrText.c_str(),
					(_int)m_tCameraInfo.vEye.x, (_int)m_tCameraInfo.vEye.y, (_int)m_tCameraInfo.vEye.z,
					(_int)m_tCameraInfo.vAt.x, (_int)m_tCameraInfo.vAt.y, (_int)m_tCameraInfo.vAt.z);

			m_pFont->Update_GameObject(fTimeDelta);
			m_pFont->Set_Text(wstring(m_szText));
		}

		/*__________________________________________________________________________________________________________
		[ Set Transform ]
		____________________________________________________________________________________________________________*/
		Engine::CGraphicDevice::Get_Instance()->Set_Transform(Engine::MATRIXID::VIEW, &m_tCameraInfo.matView);
	}

	return NO_EVENT;
}

_int CDynamicCamera::LateUpdate_GameObject(const _float & fTimeDelta)
{
	return NO_EVENT;
}

void CDynamicCamera::Render_GameObject(const _float & fTimeDelta)
{
}

void CDynamicCamera::Key_Input(const _float & fTimeDelta)
{
	_matrix matWorld = INIT_MATRIX;
	matWorld = MATRIX_INVERSE(m_tCameraInfo.matView);

	_long   dwMouseMove = 0;

	/*__________________________________________________________________________________________________________
	[ 마우스 상, 하 이동 ]
	____________________________________________________________________________________________________________*/
	if (dwMouseMove = Engine::GetDIMouseMove(Engine::MOUSEMOVESTATE::DIMS_Y))
	{
		_vec3 vRight;
		memcpy(&vRight, &matWorld.m[0][0], sizeof(_vec3));

		_matrix matRot;
		matRot = XMMatrixRotationAxis(vRight.Get_XMVECTOR(), XMConvertToRadians(dwMouseMove / 10.f));

		_vec3 vLook = m_tCameraInfo.vAt - m_tCameraInfo.vEye;
		vLook.TransformCoord(vLook, matRot);

		m_tCameraInfo.vAt = m_tCameraInfo.vEye + vLook;
	}

	/*__________________________________________________________________________________________________________
	[ 마우스 좌, 우 이동 ]
	____________________________________________________________________________________________________________*/
	if (dwMouseMove = Engine::GetDIMouseMove(Engine::MOUSEMOVESTATE::DIMS_X))
	{
		_vec3 vUp = _vec3(0.f, 1.f, 0.f);

		_matrix matRot;
		matRot = XMMatrixRotationAxis(vUp.Get_XMVECTOR(), XMConvertToRadians(dwMouseMove / 10.f));

		_vec3   vLook = m_tCameraInfo.vAt - m_tCameraInfo.vEye;
		vLook.TransformCoord(vLook, matRot);

		m_tCameraInfo.vAt = m_tCameraInfo.vEye + vLook;
	}
}

CDynamicCamera * CDynamicCamera::Create(ID3D12Device * pGraphicDevice,
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
}
