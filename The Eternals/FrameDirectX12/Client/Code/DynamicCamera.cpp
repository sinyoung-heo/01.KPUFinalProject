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

	// Font ����.
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
		if (nullptr != m_pTarget)
		{
			/*__________________________________________________________________________________________________________
			[ Key Input ]
			____________________________________________________________________________________________________________*/
			// MouseCursor Ȱ��ȭ ��, ī�޶� ������ X.
			if (!CMouseCursorMgr::Get_Instance()->Get_IsActiveMouse())
				Key_Input(fTimeDelta);

			/*__________________________________________________________________________________________________________
			[ Target�� Position�� Distance�� �������� Camera�� Eye�� ���. ]
			____________________________________________________________________________________________________________*/
			m_pTransCom->m_vScale = _vec3(0.0f, 0.0f, m_fDistFromTarget);
			m_pTransCom->m_vPos   = m_pTarget->Get_Transform()->m_vPos;
			Engine::CGameObject::Update_GameObject(fTimeDelta);

			m_tCameraInfo.vEye.TransformCoord(_vec3(0.0f, 0.0f, -1.0f), m_pTransCom->m_matWorld);
			if (nullptr == m_pCameraAtSkinningMatrix)
			{
				m_tCameraInfo.vAt	= m_pTransCom->m_vPos;
				m_tCameraInfo.vAt.y = m_pTarget->Get_BoundingBox()->Get_MaxConerPosY();
			}
			else
			{
				_matrix matWorld = m_pTransCom->m_matWorld;
				_matrix matBoneFinalTransform = ((m_pCameraAtSkinningMatrix->matBoneScale
											  * m_pCameraAtSkinningMatrix->matBoneRotation
											  * m_pCameraAtSkinningMatrix->matBoneTrans)
											  * m_pCameraAtSkinningMatrix->matParentTransform)
											  * m_pCameraAtSkinningMatrix->matRootTransform;

				matWorld = (matBoneFinalTransform) * (m_pTarget->Get_Transform()->m_matWorld);
				m_tCameraInfo.vAt = _vec3(m_pTransCom->m_vPos.x, matWorld._42, m_pTransCom->m_vPos.z);
			}
		}
		else
		{
			m_pTarget = m_pObjectMgr->Get_GameObject(L"Layer_GameObject", L"ThisPlayer");
			m_pTarget->Get_BoundingBox()->Set_SetUpCameraAt(true);
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
						 wstring(L"Eye (%d, %d, %d) \n") +
						 wstring(L"At (%d, %d, %d)\n") +
						 wstring(L"Angle (%d, %d) \n") +
						 wstring(L"MoveSpeed (%d, %d, %d) \n") +
						 wstring(L"TargetDist : %d \n");

			wsprintf(m_szText, m_wstrText.c_str(),
					(_int)m_tCameraInfo.vEye.x, (_int)m_tCameraInfo.vEye.y, (_int)m_tCameraInfo.vEye.z,
					(_int)m_tCameraInfo.vAt.x, (_int)m_tCameraInfo.vAt.y, (_int)m_tCameraInfo.vAt.z,
					(_int)m_pTransCom->m_vAngle.x, (_int)m_pTransCom->m_vAngle.y,
					(_int)(m_fCameraMoveResponsiveness.x * 100.0f), (_int)(m_fCameraMoveResponsiveness.y * 100.0f), (_int)(m_fCameraMoveResponsiveness.z * 100.0f),
					(_int)m_fDistFromTarget);

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

	/*__________________________________________________________________________________________________________
	[ ���콺 ��, �� �̵� ]
	____________________________________________________________________________________________________________*/
	_long dwMouseMoveX      = 0;
	if (dwMouseMoveX = Engine::GetDIMouseMove(Engine::MOUSEMOVESTATE::DIMS_X))
	{
		m_pTransCom->m_vAngle.y += static_cast<_float>(dwMouseMoveX) * (1.0f + m_fCameraMoveResponsiveness.x) * fTimeDelta;
	}

	/*__________________________________________________________________________________________________________
	[ ���콺 ��, �� �̵� ]
	___________________________________________________________________________________________________________*/
	_long dwMouseMoveY = 0;
	if (dwMouseMoveY = Engine::GetDIMouseMove(Engine::MOUSEMOVESTATE::DIMS_Y))
	{
		m_pTransCom->m_vAngle.x += static_cast<_float>(dwMouseMoveY) * (1.0f + m_fCameraMoveResponsiveness.y) * fTimeDelta;
	}


	/*____________________________________________________________________
	[ Angle ����. ]
	______________________________________________________________________*/
	if (m_pTransCom->m_vAngle.y < 0.f)
		m_pTransCom->m_vAngle.y = 359.9f;

	if (m_pTransCom->m_vAngle.y >= 360.f)
		m_pTransCom->m_vAngle.y = (_float)(static_cast<_int>(m_pTransCom->m_vAngle.y) % 360);

	if (m_pTransCom->m_vAngle.x > CAM_ANGLE_OFFSETMAX_X)
		m_pTransCom->m_vAngle.x = CAM_ANGLE_OFFSETMAX_X;
	else if (m_pTransCom->m_vAngle.x < CAM_ANGLE_OFFSETMIN_X)
		m_pTransCom->m_vAngle.x = CAM_ANGLE_OFFSETMIN_X;
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
