#include "stdafx.h"
#include "TestColPlayer.h"
#include "DirectInput.h"
#include "DynamicCamera.h"
#include "ObjectMgr.h"
#include <random>

CTestColPlayer::CTestColPlayer(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList)
	: Engine::CGameObject(pGraphicDevice, pCommandList)
	, m_pShaderColorInstancing(Engine::CShaderColorInstancing::Get_Instance())
{
}

CTestColPlayer::CTestColPlayer(const CTestColPlayer& rhs)
	: Engine::CGameObject(rhs)
{
}

HRESULT CTestColPlayer::Ready_GameObject(const _vec3& vScale, const _vec3& vAngle, const _vec3& vPos)
{
	Engine::FAILED_CHECK_RETURN(Engine::CGameObject::Ready_GameObject(true, true, true, true), E_FAIL);
	Engine::FAILED_CHECK_RETURN(Add_Component(), E_FAIL);
	m_pTransCom->m_vScale	= vScale;
	m_pTransCom->m_vAngle	= vAngle;
	m_pTransCom->m_vPos		= vPos;
	m_wstrCollisionTag      = L"TestCollision";

	m_pInfoCom->m_vArrivePos = m_pTransCom->m_vPos;

	m_eKeyState = MVKEY::K_END;
	m_bIsKeyDown = false;
	m_bIsSameDir = false;
	m_vecPreAngle = m_pTransCom->m_vAngle.y;

	m_fAttackTime = 3.f;
	m_bIsAttack = false;

	m_bIsCollision = true;

	// BoundingBox.
	Engine::CGameObject::SetUp_BoundingBox(&(m_pTransCom->m_matWorld),
										   m_pTransCom->m_vScale,
										   _vec3(0.0f),
										   _vec3(0.5f, 0.5f, 0.5f),
										   _vec3(-0.5f, -0.5f, -0.5f));
	// BoundingSphere
	Engine::CGameObject::SetUp_BoundingSphere(&(m_pTransCom->m_matWorld),
											  m_pTransCom->m_vScale,
											  _vec3(1.0f),
											  _vec3(0.0f));

	m_pInfoCom->m_fSpeed = 5.f;

	return S_OK;
}

HRESULT CTestColPlayer::LateInit_GameObject()
{
	/*__________________________________________________________________________________________________________
	[ Get GameObject - DynamicCamera ]
	____________________________________________________________________________________________________________*/
	m_pDynamicCamera = static_cast<CDynamicCamera*>(m_pObjectMgr->Get_GameObject(L"Layer_Camera", L"DynamicCamera"));
	Engine::NULL_CHECK_RETURN(m_pDynamicCamera, E_FAIL);
	m_pDynamicCamera->AddRef();

	return S_OK;
}

_int CTestColPlayer::Update_GameObject(const _float& fTimeDelta)
{
	Engine::FAILED_CHECK_RETURN(Engine::CGameObject::LateInit_GameObject(), E_FAIL);

	if (m_bIsDead)
		return DEAD_OBJ;

	m_pBoundingSphereCom->Set_Color(_rgba(0.0f, 1.0f, 0.0f, 1.0f));
	m_pBoundingBoxCom->Set_Color(_rgba(0.0f, 1.0f, 0.0f, 1.0f));

	/*__________________________________________________________________________________________________________
	[ Key Input ]
	____________________________________________________________________________________________________________*/
	if (!g_bIsOnDebugCaemra)
	{
		Key_Input(fTimeDelta);
	}

	/*__________________________________________________________________________________________________________
	[ TransCom - Update WorldMatrix ]
	____________________________________________________________________________________________________________*/
	Engine::CGameObject::Update_GameObject(fTimeDelta);

	/*__________________________________________________________________________________________________________
	[ Collision - Add Collision List ]
	____________________________________________________________________________________________________________*/
	m_pCollisonMgr->Add_CollisionCheckList(this);

	return NO_EVENT;
}

_int CTestColPlayer::LateUpdate_GameObject(const _float& fTimeDelta)
{
	Engine::NULL_CHECK_RETURN(m_pRenderer, -1);

	Process_Collision();

	/* 움직이고 있는 중일 경우 */
	if (m_bIsKeyDown)
	{
		if (CPacketMgr::Get_Instance()->change_MoveKey(m_eKeyState) || m_bIsSameDir == true)
		{
			Send_Player_Move();
			m_bIsSameDir = false;
		}

		if (Is_Change_CamDirection())
		{
			Send_Player_Move();
		}

		// NaviMesh 이동.
		if (!CServerMath::Get_Instance()->Is_Arrive_Point(m_pTransCom->m_vPos, m_pInfoCom->m_vArrivePos))
		{
			m_pTransCom->m_vPos += m_pTransCom->m_vDir * m_pInfoCom->m_fSpeed * fTimeDelta;
		}
	}

	Attack(fTimeDelta);

	return NO_EVENT;
}

void CTestColPlayer::Render_GameObject(const _float& fTimeDelta)
{
}

void CTestColPlayer::Process_Collision()
{
	for (auto& pDst : m_lstCollisionDst)
	{
		if (L"TestCollision" == pDst->Get_CollisionTag())
		{
			m_pBoundingSphereCom->Set_Color(_rgba(1.0f, 0.0f, 0.0f, 1.0f));
			m_pBoundingBoxCom->Set_Color(_rgba(1.0f, 0.0f, 0.0f, 1.0f));
			pDst->Get_BoundingSphere()->Set_Color(_rgba(1.0f, 0.0f, 0.0f, 1.0f));
			pDst->Get_BoundingBox()->Set_Color(_rgba(1.0f, 0.0f, 0.0f, 1.0f));

			/* Player HP Decrease */
			CPacketMgr::Get_Instance()->send_attackByMonster(pDst->Get_ServerNumber());

			/* Player Attack to Monster */
			CPacketMgr::Get_Instance()->send_attackToMonster(pDst->Get_ServerNumber());
		}
	}
}

HRESULT CTestColPlayer::Add_Component()
{
	return S_OK;
}


void CTestColPlayer::Key_Input(const _float& fTimeDelta)
{
#ifdef SERVER
	if (!g_bIsActive || m_bIsAttack) return;
#endif 

	m_pTransCom->m_vDir = m_pTransCom->Get_LookVector();
	m_pTransCom->m_vDir.Normalize();

	if (Engine::KEY_PRESSING(DIK_W))
	{
		// 대각선 - 우 상단.
		if (Engine::KEY_PRESSING(DIK_D))
		{
#ifndef SERVER
#else
			m_pTransCom->m_vAngle.y = m_pDynamicCamera->Get_Transform()->m_vAngle.y + RIGHT_UP;
			m_eKeyState = MVKEY::K_RIGHT_UP;
#endif 
		}
		// 대각선 - 좌 상단.
		else if (Engine::KEY_PRESSING(DIK_A))
		{
#ifndef SERVER
#else
			m_pTransCom->m_vAngle.y = m_pDynamicCamera->Get_Transform()->m_vAngle.y + LEFT_UP;
			m_eKeyState = MVKEY::K_LEFT_UP;
#endif		
		}
		// 직진.
		else
		{
#ifndef SERVER
#else			
			m_pTransCom->m_vAngle.y = m_pDynamicCamera->Get_Transform()->m_vAngle.y + FRONT;
			m_eKeyState = MVKEY::K_FRONT;
#endif 	
		}

#ifndef SERVER
		// NaviMesh 이동.
		_vec3 vPos = m_pNaviMeshCom->Move_OnNaviMesh(&m_pTransCom->m_vPos,
			&m_pTransCom->m_vDir,
			m_pInfoCom->m_fSpeed * fTimeDelta);
		m_pTransCom->m_vPos = vPos;
#endif 		
		m_last_move_time = high_resolution_clock::now();
		m_bIsKeyDown = true;
	}

	else if (Engine::KEY_PRESSING(DIK_S))
	{
		// 대각선 - 우 하단.
		if (Engine::KEY_PRESSING(DIK_D))
		{
#ifndef SERVER
#else
			m_pTransCom->m_vAngle.y = m_pDynamicCamera->Get_Transform()->m_vAngle.y + RIGHT_DOWN;
			m_eKeyState = MVKEY::K_RIGHT_DOWN;
#endif 			
		}
		// 대각선 - 좌 하단.
		else if (Engine::KEY_PRESSING(DIK_A))
		{
#ifndef SERVER
#else
			m_pTransCom->m_vAngle.y = m_pDynamicCamera->Get_Transform()->m_vAngle.y + LEFT_DOWN;
			m_eKeyState = MVKEY::K_LEFT_DOWN;
#endif 			
		}
		// 후진.
		else
		{
#ifndef SERVER
#else
			m_pTransCom->m_vAngle.y = m_pDynamicCamera->Get_Transform()->m_vAngle.y + BACK;
			m_eKeyState = MVKEY::K_BACK;
#endif			
		}

#ifndef SERVER
		_vec3 vPos = m_pNaviMeshCom->Move_OnNaviMesh(&m_pTransCom->m_vPos,
			&m_pTransCom->m_vDir,
			m_pInfoCom->m_fSpeed * fTimeDelta);
		m_pTransCom->m_vPos = vPos;
#endif
		m_last_move_time = high_resolution_clock::now();
		m_bIsKeyDown = true;
	}
	// 좌로 이동.
	else if (Engine::KEY_PRESSING(DIK_A))
	{

#ifndef SERVER
		m_pTransCom->m_vAngle.y = m_pDynamicCamera->Get_Transform()->m_vAngle.y + LEFT;

		_vec3 vPos = m_pNaviMeshCom->Move_OnNaviMesh(&m_pTransCom->m_vPos,
			&m_pTransCom->m_vDir,
			m_pInfoCom->m_fSpeed * fTimeDelta);
		m_pTransCom->m_vPos = vPos;
#else
		m_pTransCom->m_vAngle.y = m_pDynamicCamera->Get_Transform()->m_vAngle.y + LEFT;
		m_last_move_time = high_resolution_clock::now();
		m_eKeyState = MVKEY::K_LEFT;
		m_bIsKeyDown = true;
#endif 
	}
	// 우로 이동.	
	else if (Engine::KEY_PRESSING(DIK_D))
	{

#ifndef SERVER
		m_pTransCom->m_vAngle.y = m_pDynamicCamera->Get_Transform()->m_vAngle.y + RIGHT;

		_vec3 vPos = m_pNaviMeshCom->Move_OnNaviMesh(&m_pTransCom->m_vPos,
			&m_pTransCom->m_vDir,
			m_pInfoCom->m_fSpeed * fTimeDelta);
		m_pTransCom->m_vPos = vPos;
#else
		m_pTransCom->m_vAngle.y = m_pDynamicCamera->Get_Transform()->m_vAngle.y + RIGHT;
		m_last_move_time = high_resolution_clock::now();
		m_eKeyState = MVKEY::K_RIGHT;
		m_bIsKeyDown = true;
#endif
	}
	// 움직임 Stop
	else
	{
		if (m_bIsKeyDown)
		{
#ifdef SERVER
			CPacketMgr::Get_Instance()->send_move_stop(m_pTransCom->m_vPos, m_pTransCom->m_vDir);
#endif
			m_bIsKeyDown = false;
			m_bIsSameDir = true;
		}
	}

}

void CTestColPlayer::Send_Player_Move()
{
	m_pTransCom->m_vDir = m_pTransCom->Get_LookVector();
	m_pTransCom->m_vDir.Normalize();

	switch (m_eKeyState)
	{
	case K_FRONT:
		CPacketMgr::Get_Instance()->send_move(m_pTransCom->m_vDir, m_pTransCom->m_vPos);
		break;
	case K_BACK:
		CPacketMgr::Get_Instance()->send_move(m_pTransCom->m_vDir, m_pTransCom->m_vPos);
		break;
	case K_RIGHT:
		CPacketMgr::Get_Instance()->send_move(m_pTransCom->m_vDir, m_pTransCom->m_vPos);
		break;
	case K_RIGHT_UP:
		CPacketMgr::Get_Instance()->send_move(m_pTransCom->m_vDir, m_pTransCom->m_vPos);
		break;
	case K_RIGHT_DOWN:
		CPacketMgr::Get_Instance()->send_move(m_pTransCom->m_vDir, m_pTransCom->m_vPos);
		break;
	case K_LEFT:
		CPacketMgr::Get_Instance()->send_move(m_pTransCom->m_vDir, m_pTransCom->m_vPos);
		break;
	case K_LEFT_UP:
		CPacketMgr::Get_Instance()->send_move(m_pTransCom->m_vDir, m_pTransCom->m_vPos);
		break;
	case K_LEFT_DOWN:
		CPacketMgr::Get_Instance()->send_move(m_pTransCom->m_vDir, m_pTransCom->m_vPos);
		break;
	}

	m_vecPreAngle = m_pTransCom->m_vAngle.y;
}

bool CTestColPlayer::Is_Change_CamDirection()
{
	if (m_vecPreAngle + 5.f < m_pTransCom->m_vAngle.y)
		return true;
	else if (m_vecPreAngle - 5.f > m_pTransCom->m_vAngle.y)
		return true;

	return false;
}

void CTestColPlayer::Attack(const _float& fTimeDelta)
{
	m_pTransCom->m_vDir = m_pTransCom->Get_LookVector();
	m_pTransCom->m_vDir.Normalize();

	m_fAttackTime += fTimeDelta;

	/* 마우스 왼쪽 버튼 - 공격 */
	if (Engine::MOUSE_PRESSING(Engine::DIM_LB))
	{
		m_bIsAttack = true;
		if (m_fAttackTime > 2.f)
		{
			CPacketMgr::Get_Instance()->send_attack(m_pTransCom->m_vDir, m_pTransCom->m_vPos);
			m_fAttackTime = 0.f;
		}
	}

	/* 공격 중지 */
	if (Engine::MOUSE_KEYUP(Engine::DIM_LB))
	{
		m_bIsAttack = false;
		m_pBoundingSphereCom->Set_Color(_rgba(0.0f, 1.0f, 0.0f, 1.0f));
		m_pBoundingBoxCom->Set_Color(_rgba(0.0f, 1.0f, 0.0f, 1.0f));
		CPacketMgr::Get_Instance()->send_attack_stop(m_pTransCom->m_vDir, m_pTransCom->m_vPos);
	}

	/* Attack Moving */
	if (m_bIsAttack)
	{
		m_bIsKeyDown = false;
		m_bIsSameDir = true;

		m_pBoundingSphereCom->Set_Color(_rgba(1.0f, 0.0f, 1.0f, 1.0f));
		m_pBoundingBoxCom->Set_Color(_rgba(1.0f, 0.0f, 1.0f, 1.0f));

		// NaviMesh 이동.
		if (!CServerMath::Get_Instance()->Is_Arrive_Point(m_pTransCom->m_vPos, m_pInfoCom->m_vArrivePos))
		{
			m_pTransCom->m_vPos += m_pTransCom->m_vDir * 2.f * fTimeDelta;
		}
	}
}

Engine::CGameObject* CTestColPlayer::Create(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList, 
												 const _vec3& vScale, 
												 const _vec3& vAngle, 
												 const _vec3& vPos)
{
	CTestColPlayer* pInstance = new CTestColPlayer(pGraphicDevice, pCommandList);

	if (FAILED(pInstance->Ready_GameObject(vScale, vAngle, vPos)))
		Engine::Safe_Release(pInstance);

	return pInstance;
}

void CTestColPlayer::Free()
{
	Engine::CGameObject::Free();
	Engine::Safe_Release(m_pDynamicCamera);
}
