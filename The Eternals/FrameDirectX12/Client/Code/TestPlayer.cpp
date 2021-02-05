#include "stdafx.h"
#include "TestPlayer.h"

#include "GraphicDevice.h"
#include "DirectInput.h"
#include "ObjectMgr.h"
#include "LightMgr.h"
#include "DynamicCamera.h"
#include "Font.h"
#include "RenderTarget.h"

CTestPlayer::CTestPlayer(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList)
	: Engine::CGameObject(pGraphicDevice, pCommandList)
{
}

CTestPlayer::CTestPlayer(const CTestPlayer& rhs)
	: Engine::CGameObject(rhs)
	, m_wstrMeshTag(rhs.m_wstrMeshTag)
{
}

HRESULT CTestPlayer::Ready_GameObject(wstring wstrMeshTag, const _vec3& vScale, const _vec3& vAngle, const _vec3& vPos)
{
	Engine::FAILED_CHECK_RETURN(Engine::CGameObject::Ready_GameObject(true, true, true), E_FAIL);
	Engine::FAILED_CHECK_RETURN(Add_Component(wstrMeshTag), E_FAIL);
	m_pTransCom->m_vScale = vScale;
	m_pTransCom->m_vAngle = vAngle;
	m_pTransCom->m_vPos = vPos;
	Engine::CGameObject::SetUp_BoundingBox(&(m_pTransCom->m_matWorld),
		m_pTransCom->m_vScale,
		m_pMeshCom->Get_CenterPos(),
		m_pMeshCom->Get_MinVector(),
		m_pMeshCom->Get_MaxVector());


	m_pInfoCom->m_fSpeed = 5.0f;
	m_pInfoCom->m_arrBezierPoint[3] = { m_pTransCom->m_vPos };
	m_eKeyState = MVKEY::K_END;
	m_bIsKeyUp = false;

	/*__________________________________________________________________________________________________________
	[ 애니메이션 설정 ]
	____________________________________________________________________________________________________________*/
	m_uiAnimIdx = 1;
	m_pMeshCom->Set_AnimationKey(m_uiAnimIdx);

	/*__________________________________________________________________________________________________________
	[ Collider Bone Setting ]
	____________________________________________________________________________________________________________*/
	Engine::SKINNING_MATRIX* pmatSkinning = nullptr;
	_matrix* pmatParent = nullptr;

	// ColliderSphere
	// pmatSkinning	= m_pMeshCom->Find_SkinningMatrix("Bip01-L-Hand");
	pmatSkinning = m_pMeshCom->Find_SkinningMatrix("Bip01-R-Hand");
	pmatParent = &(m_pTransCom->m_matWorld);
	Engine::NULL_CHECK_RETURN(pmatSkinning, E_FAIL);
	m_pColliderSphereCom->Set_SkinningMatrix(pmatSkinning);		// Bone Matrix
	m_pColliderSphereCom->Set_ParentMatrix(pmatParent);			// Parent Matrix
	m_pColliderSphereCom->Set_Scale(_vec3(3.f, 3.f, 3.f));		// Collider Scale
	m_pColliderSphereCom->Set_Radius(m_pTransCom->m_vScale);	// Collider Radius

	// ColliderBox
	pmatSkinning = m_pMeshCom->Find_SkinningMatrix("Bip01-R-Hand");
	pmatParent = &(m_pTransCom->m_matWorld);
	Engine::NULL_CHECK_RETURN(pmatSkinning, E_FAIL);
	m_pColliderBoxCom->Set_SkinningMatrix(pmatSkinning);	// Bone Matrix
	m_pColliderBoxCom->Set_ParentMatrix(pmatParent);		// Parent Matrix
	m_pColliderBoxCom->Set_Scale(_vec3(3.f, 3.f, 3.f));		// Collider Scale
	m_pColliderBoxCom->Set_Extents(m_pTransCom->m_vScale);	// Box Offset From Center

	/*__________________________________________________________________________________________________________
	[ Font 생성 ]
	____________________________________________________________________________________________________________*/
	m_pFont = static_cast<Engine::CFont*>(Engine::CObjectMgr::Get_Instance()->Clone_GameObjectPrototype(L"Font_NetmarbleLight"));
	Engine::NULL_CHECK_RETURN(m_pFont, E_FAIL);
	Engine::FAILED_CHECK_RETURN(m_pFont->Ready_GameObject(L"", _vec2(900.f, 0.f), D2D1::ColorF::Yellow), E_FAIL);

	return S_OK;
}

HRESULT CTestPlayer::LateInit_GameObject()
{
	/*__________________________________________________________________________________________________________
	[ Get GameObject - DynamicCamera ]
	____________________________________________________________________________________________________________*/
	m_pDynamicCamera = static_cast<CDynamicCamera*>(m_pObjectMgr->Get_GameObject(L"Layer_Camera", L"DynamicCamera"));
	Engine::NULL_CHECK_RETURN(m_pDynamicCamera, E_FAIL);
	m_pDynamicCamera->AddRef();

	// SetUp Shader ConstantBuffer
	m_pShaderCom->SetUp_ShaderConstantBuffer((_uint)(m_pMeshCom->Get_DiffTexture().size()));
	m_pShadowCom->SetUp_ShaderConstantBuffer((_uint)(m_pMeshCom->Get_DiffTexture().size()));

	return S_OK;
}

_int CTestPlayer::Update_GameObject(const _float& fTimeDelta)
{
	Engine::FAILED_CHECK_RETURN(Engine::CGameObject::LateInit_GameObject(), E_FAIL);

	if (m_bIsDead)
		return DEAD_OBJ;

	/*__________________________________________________________________________________________________________
	[ Key Input ]
	____________________________________________________________________________________________________________*/
	if (!g_bIsOnDebugCaemra)
	{
		Key_Input(fTimeDelta);

		/* 움직이고 있는 중일 경우 */
		if (m_bIsKeyUp)
		{
			//if (CPacketMgr::Get_Instance()->change_MoveKey(m_eKeyState))
				Send_Player_Move();

			//if (!CServerMath::Get_Instance()->Is_Arrive_Point(m_pTransCom->m_vPos, m_pInfoCom->m_arrBezierPoint[3]))
			{
				//m_pTransCom->m_vPos += m_pTransCom->m_vDir /** fTimeDelta*/ * 1.f;
			}
		}
	}

	/*__________________________________________________________________________________________________________
	[ TransCom - Update WorldMatrix ]
	____________________________________________________________________________________________________________*/
	Engine::CGameObject::Update_GameObject(fTimeDelta);

	return NO_EVENT;
}

_int CTestPlayer::LateUpdate_GameObject(const _float& fTimeDelta)
{
	Engine::NULL_CHECK_RETURN(m_pRenderer, -1);

	/*__________________________________________________________________________________________________________
	[ Renderer - Add Render Group ]
	____________________________________________________________________________________________________________*/
	Engine::FAILED_CHECK_RETURN(m_pRenderer->Add_Renderer(Engine::CRenderer::RENDER_NONALPHA, this), -1);

	/*__________________________________________________________________________________________________________
	[ Font Update ]
	____________________________________________________________________________________________________________*/
	if (Engine::CRenderer::Get_Instance()->Get_RenderOnOff(L"DebugFont"))
	{
		m_wstrText = wstring(L"[ Character Info ] \n") +
			wstring(L"Num Animation \t %d \n") +
			wstring(L"Current Ani Index \t %d \n") +
			wstring(L"Max Frame \t %d \n") +
			wstring(L"Current Frame \t %d");

		wsprintf(m_szText, m_wstrText.c_str(),
			*(m_pMeshCom->Get_NumAnimation()),
			m_uiAnimIdx,
			m_ui3DMax_NumFrame,
			m_ui3DMax_CurFrame);

		m_pFont->Update_GameObject(fTimeDelta);
		m_pFont->Set_Text(wstring(m_szText));
	}

	/*__________________________________________________________________________________________________________
	[ Animation KeyFrame Index ]
	____________________________________________________________________________________________________________*/
	m_ui3DMax_NumFrame = *(m_pMeshCom->Get_3DMaxNumFrame());
	m_ui3DMax_CurFrame = *(m_pMeshCom->Get_3DMaxCurFrame());

	return NO_EVENT;
}

void CTestPlayer::Render_GameObject(const _float& fTimeDelta)
{
	/*__________________________________________________________________________________________________________
	[ Play Animation ]
	____________________________________________________________________________________________________________*/
	m_pMeshCom->Set_AnimationKey(m_uiAnimIdx);
	m_pMeshCom->Play_Animation(fTimeDelta * TPS);

	Set_ConstantTable();
	m_pMeshCom->Render_DynamicMesh(m_pShaderCom);
}

void CTestPlayer::Render_ShadowDepth(const _float& fTimeDelta)
{
	Set_ConstantTableShadowDepth();
	m_pMeshCom->Render_DynamicMeshShadowDepth(m_pShadowCom);
}

void CTestPlayer::Render_GameObject(const _float& fTimeDelta, ID3D12GraphicsCommandList* pCommandList, const _int& iContextIdx)
{
	/*__________________________________________________________________________________________________________
	[ Play Animation ]
	____________________________________________________________________________________________________________*/
	m_pMeshCom->Set_AnimationKey(m_uiAnimIdx);
	m_pMeshCom->Play_Animation(fTimeDelta * TPS);

	Set_ConstantTable();
	m_pMeshCom->Render_DynamicMesh(pCommandList, iContextIdx, m_pShaderCom);
}

void CTestPlayer::Render_ShadowDepth(const _float& fTimeDelta, ID3D12GraphicsCommandList* pCommandList, const _int& iContextIdx)
{
	Set_ConstantTableShadowDepth();
	m_pMeshCom->Render_DynamicMeshShadowDepth(pCommandList, iContextIdx, m_pShadowCom);
}

void CTestPlayer::Set_DeadReckoning(const _vec3& vPos1, const _vec3& vPos2, const _vec3& vPos3, const _vec3& vPos4)
{
	m_pInfoCom->m_arrBezierPoint[0] = vPos1;
	m_pInfoCom->m_arrBezierPoint[1] = vPos2;
	m_pInfoCom->m_arrBezierPoint[2] = vPos3;
	m_pInfoCom->m_arrBezierPoint[3] = vPos4;
}

HRESULT CTestPlayer::Add_Component(wstring wstrMeshTag)
{
	Engine::NULL_CHECK_RETURN(m_pComponentMgr, E_FAIL);

	// Mesh
	m_pMeshCom = static_cast<Engine::CMesh*>(m_pComponentMgr->Clone_Component(wstrMeshTag.c_str(), Engine::COMPONENTID::ID_STATIC));
	Engine::NULL_CHECK_RETURN(m_pMeshCom, E_FAIL);
	m_pMeshCom->AddRef();
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Mesh", m_pMeshCom);

	// Shader
	m_pShaderCom = static_cast<Engine::CShaderMesh*>(m_pComponentMgr->Clone_Component(L"ShaderMesh", Engine::COMPONENTID::ID_STATIC));
	Engine::NULL_CHECK_RETURN(m_pShaderCom, E_FAIL);
	m_pShaderCom->AddRef();
	Engine::FAILED_CHECK_RETURN(m_pShaderCom->Set_PipelineStatePass(0), E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Shader", m_pShaderCom);

	// Shadow
	m_pShadowCom = static_cast<Engine::CShaderShadow*>(m_pComponentMgr->Clone_Component(L"ShaderShadow", Engine::COMPONENTID::ID_STATIC));
	Engine::NULL_CHECK_RETURN(m_pShadowCom, E_FAIL);
	m_pShadowCom->AddRef();
	Engine::FAILED_CHECK_RETURN(m_pShadowCom->Set_PipelineStatePass(0), E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Shadow", m_pShadowCom);

	// Collider - Sphere
	m_pColliderSphereCom = static_cast<Engine::CColliderSphere*>(m_pComponentMgr->Clone_Component(L"ColliderSphere", Engine::COMPONENTID::ID_DYNAMIC));
	Engine::NULL_CHECK_RETURN(m_pColliderSphereCom, E_FAIL);
	m_pColliderSphereCom->AddRef();
	m_mapComponent[Engine::ID_DYNAMIC].emplace(L"Com_ColliderSphere", m_pColliderSphereCom);

	// Collider - Box
	m_pColliderBoxCom = static_cast<Engine::CColliderBox*>(m_pComponentMgr->Clone_Component(L"ColliderBox", Engine::COMPONENTID::ID_DYNAMIC));
	Engine::NULL_CHECK_RETURN(m_pColliderBoxCom, E_FAIL);
	m_pColliderBoxCom->AddRef();
	m_mapComponent[Engine::ID_DYNAMIC].emplace(L"Com_ColliderBox", m_pColliderBoxCom);

	// NaviMesh
	m_pNaviMeshCom = static_cast<Engine::CNaviMesh*>(m_pComponentMgr->Clone_Component(L"TestNaviMesh", Engine::ID_DYNAMIC));
	Engine::NULL_CHECK_RETURN(m_pNaviMeshCom, E_FAIL);
	m_pNaviMeshCom->AddRef();
	m_pNaviMeshCom->Set_CurrentCellIndex(0);
	m_mapComponent[Engine::ID_DYNAMIC].emplace(L"Com_NaviMesh", m_pNaviMeshCom);

	return S_OK;
}

void CTestPlayer::Set_ConstantTable()
{	
	/*__________________________________________________________________________________________________________
	[ Set ConstantBuffer Data ]
	____________________________________________________________________________________________________________*/
	Engine::SHADOW_DESC tShadowDesc = CShadowLightMgr::Get_Instance()->Get_ShadowDesc();

	Engine::CB_SHADER_MESH tCB_ShaderMesh;
	ZeroMemory(&tCB_ShaderMesh, sizeof(Engine::CB_SHADER_MESH));
	tCB_ShaderMesh.matWorld = Engine::CShader::Compute_MatrixTranspose(m_pTransCom->m_matWorld);
	tCB_ShaderMesh.matLightView = Engine::CShader::Compute_MatrixTranspose(tShadowDesc.matLightView);
	tCB_ShaderMesh.matLightProj = Engine::CShader::Compute_MatrixTranspose(tShadowDesc.matLightProj);
	tCB_ShaderMesh.vLightPos = tShadowDesc.vLightPosition;
	tCB_ShaderMesh.fLightPorjFar = tShadowDesc.fLightPorjFar;

	m_pShaderCom->Get_UploadBuffer_ShaderMesh()->CopyData(0, tCB_ShaderMesh);
}

void CTestPlayer::Set_ConstantTableShadowDepth()
{
	/*__________________________________________________________________________________________________________
	[ Set ConstantBuffer Data ]
	____________________________________________________________________________________________________________*/
	Engine::SHADOW_DESC tShadowDesc = CShadowLightMgr::Get_Instance()->Get_ShadowDesc();

	Engine::CB_SHADER_SHADOW tCB_ShaderShadow;
	ZeroMemory(&tCB_ShaderShadow, sizeof(Engine::CB_SHADER_SHADOW));
	tCB_ShaderShadow.matWorld = Engine::CShader::Compute_MatrixTranspose(m_pTransCom->m_matWorld);
	tCB_ShaderShadow.matView = Engine::CShader::Compute_MatrixTranspose(tShadowDesc.matLightView);
	tCB_ShaderShadow.matProj = Engine::CShader::Compute_MatrixTranspose(tShadowDesc.matLightProj);
	tCB_ShaderShadow.fProjFar = tShadowDesc.fLightPorjFar;

	m_pShadowCom->Get_UploadBuffer_ShaderShadow()->CopyData(0, tCB_ShaderShadow);
}

void CTestPlayer::Key_Input(const _float& fTimeDelta)
{
#ifdef SERVER
	if (!g_bIsActive) return;
#endif 

	if (Engine::KEY_DOWN(DIK_1))
		m_uiAnimIdx = 0;

	else if (Engine::KEY_DOWN(DIK_2))
		m_uiAnimIdx = 1;

	m_pTransCom->m_vDir = m_pTransCom->Get_LookVector();
	m_pTransCom->m_vDir.Normalize();

	if (Engine::KEY_PRESSING(DIK_W))
	{
		// 대각선 - 우 상단.
		if (Engine::KEY_PRESSING(DIK_D))
		{
#ifndef SERVER
			m_pTransCom->m_vAngle.y = m_pDynamicCamera->Get_Transform()->m_vAngle.y + RIGHT_UP;
#else
			m_eKeyState = MVKEY::K_RIGHT_UP;			
#endif 
		}
		// 대각선 - 좌 상단.
		else if (Engine::KEY_PRESSING(DIK_A))
		{
#ifndef SERVER
			m_pTransCom->m_vAngle.y = m_pDynamicCamera->Get_Transform()->m_vAngle.y + LEFT_UP;
#else
			m_eKeyState = MVKEY::K_LEFT_UP;
#endif		
		}
		// 직진.
		else
		{
#ifndef SERVER
			m_pTransCom->m_vAngle.y = m_pDynamicCamera->Get_Transform()->m_vAngle.y + FRONT;
#else			
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
		m_bIsKeyUp = true;
	}

	else if (Engine::KEY_PRESSING(DIK_S))
	{
		// 대각선 - 우 하단.
		if (Engine::KEY_PRESSING(DIK_D))
		{
#ifndef SERVER
			m_pTransCom->m_vAngle.y = m_pDynamicCamera->Get_Transform()->m_vAngle.y + RIGHT_DOWN;
#else
			m_eKeyState = MVKEY::K_RIGHT_DOWN;
#endif 			
		}
		// 대각선 - 좌 하단.
		else if (Engine::KEY_PRESSING(DIK_A))
		{
#ifndef SERVER
			m_pTransCom->m_vAngle.y = m_pDynamicCamera->Get_Transform()->m_vAngle.y + LEFT_DOWN;
#else
			m_eKeyState = MVKEY::K_LEFT_DOWN;
#endif 			
		}
		// 후진.
		else
		{
#ifndef SERVER
			m_pTransCom->m_vAngle.y = m_pDynamicCamera->Get_Transform()->m_vAngle.y + BACK;
#else
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
		m_bIsKeyUp = true;
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
		m_last_move_time = high_resolution_clock::now();
		m_eKeyState = MVKEY::K_LEFT;
		m_bIsKeyUp = true;
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
		m_last_move_time = high_resolution_clock::now();
		m_eKeyState = MVKEY::K_RIGHT;
		m_bIsKeyUp = true;
#endif
	}
	// 움직임 Stop
	else
	{
		if (m_bIsKeyUp)
		{
#ifdef SERVER
			cout << "send_move_stop" << endl;
			CPacketMgr::Get_Instance()->send_move_stop(m_pTransCom->m_vPos, m_pTransCom->m_vAngle);
#endif
			m_bIsKeyUp = false;
		}
	}
}

void CTestPlayer::Send_Player_Move()
{
	cout << "send_move" << endl;
	switch (m_eKeyState)
	{
	case K_FRONT:
		CPacketMgr::Get_Instance()->send_move(MV_FRONT, m_pTransCom->m_vDir, m_pDynamicCamera->Get_Transform()->m_vAngle);
		break;
	case K_BACK:
		CPacketMgr::Get_Instance()->send_move(MV_BACK, m_pTransCom->m_vDir, m_pDynamicCamera->Get_Transform()->m_vAngle);
		break;
	case K_RIGHT:
		CPacketMgr::Get_Instance()->send_move(MV_RIGHT, m_pTransCom->m_vDir, m_pDynamicCamera->Get_Transform()->m_vAngle);
		break;
	case K_RIGHT_UP:
		CPacketMgr::Get_Instance()->send_move(MV_RIGHT_UP, m_pTransCom->m_vDir, m_pDynamicCamera->Get_Transform()->m_vAngle);
		break;
	case K_RIGHT_DOWN:
		CPacketMgr::Get_Instance()->send_move(MV_RIGHT_DOWN, m_pTransCom->m_vDir, m_pDynamicCamera->Get_Transform()->m_vAngle);
		break;
	case K_LEFT:
		CPacketMgr::Get_Instance()->send_move(MV_LEFT, m_pTransCom->m_vDir, m_pDynamicCamera->Get_Transform()->m_vAngle);
		break;
	case K_LEFT_UP:
		CPacketMgr::Get_Instance()->send_move(MV_LEFT_UP, m_pTransCom->m_vDir, m_pDynamicCamera->Get_Transform()->m_vAngle);
		break;
	case K_LEFT_DOWN:
		CPacketMgr::Get_Instance()->send_move(MV_LEFT_DOWN, m_pTransCom->m_vDir, m_pDynamicCamera->Get_Transform()->m_vAngle);
		break;
	}
}

Engine::CGameObject* CTestPlayer::Create(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList, wstring wstrMeshTag, const _vec3& vScale, const _vec3& vAngle, const _vec3& vPos)
{
	CTestPlayer* pInstance = new CTestPlayer(pGraphicDevice, pCommandList);

	if (FAILED(pInstance->Ready_GameObject(wstrMeshTag, vScale, vAngle, vPos)))
		Engine::Safe_Release(pInstance);

	return pInstance;
}

void CTestPlayer::Free()
{
	Engine::CGameObject::Free();

	Engine::Safe_Release(m_pDynamicCamera);

	Engine::Safe_Release(m_pMeshCom);
	Engine::Safe_Release(m_pShaderCom);
	Engine::Safe_Release(m_pShadowCom);
	Engine::Safe_Release(m_pColliderSphereCom);
	Engine::Safe_Release(m_pColliderBoxCom);
	Engine::Safe_Release(m_pNaviMeshCom);

	Engine::Safe_Release(m_pFont);
}
