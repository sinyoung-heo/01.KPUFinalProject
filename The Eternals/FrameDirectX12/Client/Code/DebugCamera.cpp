#include "stdafx.h"
#include "DebugCamera.h"

#include "GraphicDevice.h"
#include "ObjectMgr.h"
#include "DirectInput.h"
#include "DynamicCamera.h"
#include "Font.h"


CDebugCamera::CDebugCamera(ID3D12Device * pGraphicDevice, ID3D12GraphicsCommandList * pCommandList)
	: Engine::CCamera(pGraphicDevice, pCommandList)
{
}

HRESULT CDebugCamera::Ready_GameObject(const Engine::CAMERA_DESC& tCameraInfo,
									   const Engine::PROJ_DESC& tProjInfo,
									   const Engine::ORTHO_DESC& tOrthoInfo)
{
	Engine::FAILED_CHECK_RETURN(Engine::CCamera::Ready_GameObject(tCameraInfo, tProjInfo, tOrthoInfo), E_FAIL);

	m_pFont = static_cast<Engine::CFont*>(Engine::CObjectMgr::Get_Instance()->Clone_GameObjectPrototype(L"Font_NetmarbleLight"));
	Engine::NULL_CHECK_RETURN(m_pFont, E_FAIL);
	Engine::FAILED_CHECK_RETURN(m_pFont->Ready_GameObject(L"", _vec2(1150.f, 0.f), D2D1::ColorF::Cyan), E_FAIL);

	return S_OK;
}

HRESULT CDebugCamera::LateInit_GameObject()
{
	return S_OK;
}

_int CDebugCamera::Update_GameObject(const _float & fTimeDelta)
{
	Engine::FAILED_CHECK_RETURN(Engine::CGameObject::LateInit_GameObject(), E_FAIL);


	if (g_bIsOnDebugCaemra)
	{
		if (!m_bIsResetDebugCamera)
		{
			m_bIsResetDebugCamera = true;

			CDynamicCamera* pDynamicCamera = static_cast<CDynamicCamera*>(m_pObjectMgr->Get_GameObject(L"Layer_Camera", L"DynamicCamera"));
			if (nullptr != pDynamicCamera)
				m_tCameraInfo = pDynamicCamera->Get_CameraInfo();

			pDynamicCamera = nullptr;
		}

		/*__________________________________________________________________________________________________________
		[ Key Input ]
		____________________________________________________________________________________________________________*/
		Key_Input(fTimeDelta);

		/*__________________________________________________________________________________________________________
		[ View Matrix Update ]
		____________________________________________________________________________________________________________*/
		Engine::CCamera::Update_GameObject(fTimeDelta);

		/*__________________________________________________________________________________________________________
		[ Set Transform ]
		____________________________________________________________________________________________________________*/
		Engine::CGraphicDevice::Get_Instance()->Set_Transform(Engine::MATRIXID::VIEW, &m_tCameraInfo.matView);

	}
	else
		m_bIsResetDebugCamera = false;


	return NO_EVENT;
}

_int CDebugCamera::LateUpdate_GameObject(const _float & fTimeDelta)
{
	/*__________________________________________________________________________________________________________
	[ Font Update ]
	____________________________________________________________________________________________________________*/
	if (Engine::CRenderer::Get_Instance()->Get_RenderOnOff(L"DebugFont"))
	{
		if (g_bIsOnDebugCaemra)
		{
			m_wstrText = wstring(L"[ Camera Info ] \n") +
						 wstring(L"Eye\t(%d, %d, %d) \n") +
						 wstring(L"At\t(%d, %d, %d)\n") +
						 wstring(L"Speed\t %d\n");

			wsprintf(m_szText, m_wstrText.c_str(),
					(_int)m_tCameraInfo.vEye.x, (_int)m_tCameraInfo.vEye.y, (_int)m_tCameraInfo.vEye.z,
					(_int)m_tCameraInfo.vAt.x, (_int)m_tCameraInfo.vAt.y, (_int)m_tCameraInfo.vAt.z,
					(_int)m_fSpeed);

			m_pFont->Update_GameObject(fTimeDelta);
			m_pFont->Set_Text(wstring(m_szText));
		}
	}

	return NO_EVENT;
}

void CDebugCamera::Render_GameObject(const _float & fTimeDelta)
{
}

void CDebugCamera::Key_Input(const _float & fTimeDelta)
{
#ifdef SERVER
	if (!g_bIsActive) return;
#endif 

	_matrix matWorld = INIT_MATRIX;
	matWorld = MATRIX_INVERSE(m_tCameraInfo.matView);

	if (Engine::KEY_DOWN(DIK_MINUS))
	{
		--m_fSpeed;
		if (m_fSpeed < 0.0f)
			m_fSpeed = 1.0f;
	}
	else if (Engine::KEY_DOWN(DIK_EQUALS))
		++m_fSpeed;

	_long   dwMouseMove = 0;
	/*__________________________________________________________________________________________________________
	[ 마우스 상, 하 이동 ]
	____________________________________________________________________________________________________________*/
	if (dwMouseMove = Engine::GetDIMouseMove(Engine::MOUSEMOVESTATE::DIMS_Y))
	{
		_vec3 vRight;
		memcpy(&vRight, &matWorld.m[0][0], sizeof(_vec3));

		_matrix matRot;
		matRot = XMMatrixRotationAxis(vRight.Get_XMVECTOR(), XMConvertToRadians(dwMouseMove / 20.f));

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
		matRot = XMMatrixRotationAxis(vUp.Get_XMVECTOR(), XMConvertToRadians(dwMouseMove / 20.f));

		_vec3   vLook = m_tCameraInfo.vAt - m_tCameraInfo.vEye;
		vLook.TransformCoord(vLook, matRot);

		m_tCameraInfo.vAt = m_tCameraInfo.vEye + vLook;
	}


	if (Engine::KEY_PRESSING(DIK_W))
	{
		_vec3 vLook;
		memcpy(&vLook, &matWorld.m[2][0], sizeof(_vec3));

		vLook.Normalize();

		m_tCameraInfo.vEye	+= vLook * m_fSpeed * fTimeDelta;
		m_tCameraInfo.vAt	+= vLook * m_fSpeed * fTimeDelta;
	}

	if (Engine::KEY_PRESSING(DIK_S))
	{
		_vec3 vLook;
		memcpy(&vLook, &matWorld.m[2][0], sizeof(_vec3));

		vLook.Normalize();

		m_tCameraInfo.vEye	-= vLook * m_fSpeed * fTimeDelta;
		m_tCameraInfo.vAt	-= vLook * m_fSpeed * fTimeDelta;
	}

	if (Engine::KEY_PRESSING(DIK_D))
	{
		_vec3 vLook;
		memcpy(&vLook, &matWorld.m[0][0], sizeof(_vec3));

		vLook.Normalize();

		m_tCameraInfo.vEye	+= vLook * m_fSpeed * fTimeDelta;
		m_tCameraInfo.vAt	+= vLook * m_fSpeed * fTimeDelta;
	}

	if (Engine::KEY_PRESSING(DIK_A))
	{
		_vec3 vLook;
		memcpy(&vLook, &matWorld.m[0][0], sizeof(_vec3));

		vLook.Normalize();

		m_tCameraInfo.vEye	-= vLook * m_fSpeed * fTimeDelta;
		m_tCameraInfo.vAt	-= vLook * m_fSpeed * fTimeDelta;
	}

}

Engine::CGameObject* CDebugCamera::Create(ID3D12Device * pGraphicDevice,
										  ID3D12GraphicsCommandList * pCommandList,
										  const Engine::CAMERA_DESC & tCameraInfo,
										  const Engine::PROJ_DESC & tProjInfo,
										  const Engine::ORTHO_DESC & tOrthoInfo)
{
	CDebugCamera* pInstance = new CDebugCamera(pGraphicDevice, pCommandList);

	if (FAILED(pInstance->Ready_GameObject(tCameraInfo, tProjInfo, tOrthoInfo)))
		Engine::Safe_Release(pInstance);

	return pInstance;
}

void CDebugCamera::Free()
{
	Engine::CCamera::Free();

	Engine::Safe_Release(m_pFont);
}
