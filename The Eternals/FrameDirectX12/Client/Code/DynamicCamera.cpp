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
	Engine::FAILED_CHECK_RETURN(Engine::CGameObject::Ready_GameObject(true, false), E_FAIL);
	Engine::FAILED_CHECK_RETURN(Engine::CCamera::Ready_GameObject(tCameraInfo, tProjInfo, tOrthoInfo), E_FAIL);

	m_pTransCom->m_vAngle.x = 22.5f;
	m_pTransCom->m_vAngle.y = 0.0f;

	// Font 생성.
	m_pFont = static_cast<Engine::CFont*>(Engine::CObjectMgr::Get_Instance()->Clone_GameObjectPrototype(L"Font_NetmarbleLight"));
	Engine::NULL_CHECK_RETURN(m_pFont, E_FAIL);
	Engine::FAILED_CHECK_RETURN(m_pFont->Ready_GameObject(L"", _vec2(1150.f, 0.f), D2D1::ColorF::Cyan), E_FAIL);

	return S_OK;
}

HRESULT CDynamicCamera::LateInit_GameObject()
{
	m_pTarget = m_pObjectMgr->Get_GameObject(L"Layer_GameObject", L"Popori_F");
	Engine::NULL_CHECK_RETURN(m_pTarget, E_FAIL);

	return S_OK;
}

_int CDynamicCamera::Update_GameObject(const _float & fTimeDelta)
{
#ifndef SERVER
	Engine::FAILED_CHECK_RETURN(Engine::CGameObject::LateInit_GameObject(), E_FAIL);
#endif // !SERVER


	if (!g_bIsOnDebugCaemra)
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
			m_pTransCom->m_vPos = m_pTarget->Get_Transform()->m_vPos;
			Engine::CGameObject::Update_GameObject(fTimeDelta);

			m_tCameraInfo.vEye.TransformCoord(_vec3(0.0f, 0.0f, -1.0f), m_pTransCom->m_matWorld);
			m_tCameraInfo.vAt = m_pTransCom->m_vPos;
		}

		/*__________________________________________________________________________________________________________
		[ Update ViewMatrix ]
		____________________________________________________________________________________________________________*/
		Engine::CCamera::Update_GameObject(fTimeDelta);

		/*__________________________________________________________________________________________________________
		[ Set Transform ]
		____________________________________________________________________________________________________________*/
		Engine::CGraphicDevice::Get_Instance()->Set_Transform(Engine::MATRIXID::VIEW, &m_tCameraInfo.matView);
	}

	return NO_EVENT;
}

_int CDynamicCamera::LateUpdate_GameObject(const _float & fTimeDelta)
{
	if (!g_bIsOnDebugCaemra)
	{
		/*__________________________________________________________________________________________________________
		[ Font Update ]
		____________________________________________________________________________________________________________*/
		if (Engine::CRenderer::Get_Instance()->Get_RenderOnOff(L"DebugFont"))
		{
			m_wstrText = wstring(L"[ Camera Info ] \n") +
						 wstring(L"Eye\t(%d, %d, %d) \n") +
						 wstring(L"At\t(%d, %d, %d)\n") +
						 wstring(L"Angle X    %d \n") +
						 wstring(L"Angle Y    %d \n") +
						 wstring(L"TargetDist %d \n");

			wsprintf(m_szText, m_wstrText.c_str(),
					(_int)m_tCameraInfo.vEye.x, (_int)m_tCameraInfo.vEye.y, (_int)m_tCameraInfo.vEye.z,
					(_int)m_tCameraInfo.vAt.x, (_int)m_tCameraInfo.vAt.y, (_int)m_tCameraInfo.vAt.z,
					(_int)m_pTransCom->m_vAngle.x, (_int)m_pTransCom->m_vAngle.y, (_int)m_fDistFromTarget);

			m_pFont->Update_GameObject(fTimeDelta);
			m_pFont->Set_Text(wstring(m_szText));
		}
	}

	return NO_EVENT;
}

void CDynamicCamera::Render_GameObject(const _float & fTimeDelta)
{
}



void CDynamicCamera::Key_Input(const _float & fTimeDelta)
{
#ifdef SERVER
	if (!g_bIsActive) return;
#endif 

	_matrix matWorld = INIT_MATRIX;
	matWorld = MATRIX_INVERSE(m_tCameraInfo.matView);

	_long   dwMouseMove = 0;

	/*__________________________________________________________________________________________________________
	[ 마우스 상, 하 이동 ]
	____________________________________________________________________________________________________________*/
	if (dwMouseMove = Engine::GetDIMouseMove(Engine::MOUSEMOVESTATE::DIMS_Y))
	{
		m_pTransCom->m_vAngle.x += static_cast<_float>(dwMouseMove) / 30.f;

		//// At의 Y값 조정.
		//m_pTransCom->m_vAt.y += static_cast<_float>(-dwMouseMove) / 150.f;
	}

	/*__________________________________________________________________________________________________________
	[ 마우스 좌, 우 이동 ]
	____________________________________________________________________________________________________________*/
	if (dwMouseMove = Engine::GetDIMouseMove(Engine::MOUSEMOVESTATE::DIMS_X))
	{
		m_pTransCom->m_vAngle.y += static_cast<_float>(dwMouseMove) / 24.f;
		//_vec3 vUp = _vec3(0.f, 1.f, 0.f);
	}

	/*____________________________________________________________________
	[ Angle Y값 보정. ]
	______________________________________________________________________*/
	if (m_pTransCom->m_vAngle.y < 0.f)
		m_pTransCom->m_vAngle.y = 359.9f;

	if (m_pTransCom->m_vAngle.y >= 360.f)
		m_pTransCom->m_vAngle.y = (_float)(static_cast<_int>(m_pTransCom->m_vAngle.y) % 360);
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
