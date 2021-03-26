#include "stdafx.h"
#include "Popori_F.h"
#include "GraphicDevice.h"
#include "DirectInput.h"
#include "ObjectMgr.h"
#include "LightMgr.h"
#include "DynamicCamera.h"
#include "Font.h"
#include "RenderTarget.h"
#include "TimeMgr.h"

CPopori_F::CPopori_F(ID3D12Device * pGraphicDevice, ID3D12GraphicsCommandList * pCommandList)
	: Engine::CGameObject(pGraphicDevice, pCommandList)
{
}

HRESULT CPopori_F::Ready_GameObject(wstring wstrMeshTag,
									wstring wstrNaviMeshTag,
									const _vec3 & vScale, 
									const _vec3 & vAngle, 
									const _vec3 & vPos)
{
	Engine::FAILED_CHECK_RETURN(Engine::CGameObject::Ready_GameObject(true, true, true), E_FAIL);
	Engine::FAILED_CHECK_RETURN(Add_Component(wstrMeshTag, wstrNaviMeshTag), E_FAIL);
	m_pTransCom->m_vScale	= vScale;
	m_pTransCom->m_vAngle	= vAngle;
	m_pTransCom->m_vPos		= vPos;
	m_pNaviMeshCom->Set_CurrentCellIndex(m_pNaviMeshCom->Get_CurrentPositionCellIndex(vPos));
	Engine::CGameObject::SetUp_BoundingBox(&(m_pTransCom->m_matWorld),
										   m_pTransCom->m_vScale,
										   m_pMeshCom->Get_CenterPos(),
										   m_pMeshCom->Get_MinVector(),
										   m_pMeshCom->Get_MaxVector());


	m_pInfoCom->m_fSpeed = 10.0f;

	/*__________________________________________________________________________________________________________
	[ PipelineStatePass ]
	____________________________________________________________________________________________________________*/
	Engine::FAILED_CHECK_RETURN(m_pShaderCom->Set_PipelineStatePass(0), E_FAIL);
	m_iMeshPipelineStatePass = 0;
	
	Engine::FAILED_CHECK_RETURN(m_pShadowCom->Set_PipelineStatePass(0), E_FAIL);
	m_iShadowPipelineStatePass = 0;

	Engine::FAILED_CHECK_RETURN(m_pEdgeObjectShaderCom->Set_PipelineStatePass(4), E_FAIL);

	/*__________________________________________________________________________________________________________
	[ 애니메이션 설정 ]
	____________________________________________________________________________________________________________*/
	m_uiAnimIdx = 1;

	/*__________________________________________________________________________________________________________
	[ Collider Bone Setting ]
	____________________________________________________________________________________________________________*/
	Engine::SKINNING_MATRIX*	pmatSkinning	= nullptr;
	_matrix*					pmatParent		= nullptr;

	// ColliderSphere
	// pmatSkinning	= m_pMeshCom->Find_SkinningMatrix("Bip01-L-Hand");
	pmatSkinning	= m_pMeshCom->Find_SkinningMatrix("Bip01-R-Hand");
	pmatParent		= &(m_pTransCom->m_matWorld);
	Engine::NULL_CHECK_RETURN(pmatSkinning, E_FAIL);
	m_pColliderSphereCom->Set_SkinningMatrix(pmatSkinning);		// Bone Matrix
	m_pColliderSphereCom->Set_ParentMatrix(pmatParent);			// Parent Matrix
	m_pColliderSphereCom->Set_Scale(_vec3(3.f, 3.f, 3.f));		// Collider Scale
	m_pColliderSphereCom->Set_Radius(m_pTransCom->m_vScale);	// Collider Radius

	// ColliderBox
	pmatSkinning	= m_pMeshCom->Find_SkinningMatrix("Bip01-R-Hand");
	pmatParent		= &(m_pTransCom->m_matWorld);
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


	m_uiAfterImgSize = 20;
	m_fAfterImgMakeTime = 0.1f;
	m_fAfterSubAlpha = 0.05f;
	m_pMeshCom->Set_AfterImgMakeTime(m_fAfterImgMakeTime);
	m_pMeshCom->Set_AfterImgSize(m_uiAfterImgSize);
	m_pMeshCom->Set_AfterImgSubAlpha(m_fAfterSubAlpha);
	return S_OK;
}

HRESULT CPopori_F::LateInit_GameObject()
{
	/*__________________________________________________________________________________________________________
	[ Get GameObject - DynamicCamera ]
	____________________________________________________________________________________________________________*/
	m_pDynamicCamera = static_cast<CDynamicCamera*>(m_pObjectMgr->Get_GameObject(L"Layer_Camera", L"DynamicCamera"));
	Engine::NULL_CHECK_RETURN(m_pDynamicCamera, E_FAIL);
	m_pDynamicCamera->AddRef();

	// SetUp Shader ConstantBuffer
	
	m_pShaderCom->SetUp_ShaderConstantBuffer((_uint)(m_pMeshCom->Get_DiffTexture().size()), m_uiAfterImgSize);
	

	m_pEdgeObjectShaderCom->SetUp_ShaderConstantBuffer((_uint)(m_pMeshCom->Get_DiffTexture().size()));

	m_pShadowCom->SetUp_ShaderConstantBuffer((_uint)(m_pMeshCom->Get_DiffTexture().size()));

	return S_OK;
}

_int CPopori_F::Update_GameObject(const _float & fTimeDelta)
{
	Engine::FAILED_CHECK_RETURN(Engine::CGameObject::LateInit_GameObject(), E_FAIL);

	if (m_bIsDead)
		return DEAD_OBJ;

	/*__________________________________________________________________________________________________________
	[ Key Input ]
	____________________________________________________________________________________________________________*/
	if(!g_bIsOnDebugCaemra)
		Key_Input(fTimeDelta);

	/*__________________________________________________________________________________________________________
	[ Change Animation ]
	____________________________________________________________________________________________________________*/
	Change_Animation(fTimeDelta);

	/*__________________________________________________________________________________________________________
	[ Renderer - Add Render Group ]
	____________________________________________________________________________________________________________*/
	Engine::FAILED_CHECK_RETURN(m_pRenderer->Add_Renderer(Engine::CRenderer::RENDER_NONALPHA, this), -1);
	
	Engine::FAILED_CHECK_RETURN(m_pRenderer->Add_Renderer(Engine::CRenderer::RENDER_EDGE, this), -1);
	
	Engine::FAILED_CHECK_RETURN(m_pRenderer->Add_Renderer(Engine::CRenderer::RENDER_ALPHA, this), -1);

	/*__________________________________________________________________________________________________________
	[ TransCom - Update WorldMatrix ]
	____________________________________________________________________________________________________________*/
	Engine::CGameObject::Update_GameObject(fTimeDelta);

	// AfterImage
	m_fAfterImgTime += (Engine::CTimerMgr::Get_Instance()->Get_TimeDelta(L"Timer_TimeDelta"));
	m_pMeshCom->Set_AfterImgTime(m_fAfterImgTime);
	if (m_fAfterImgTime > m_fAfterImgMakeTime)
	{
		m_fAfterImgTime = 0.f;
		m_lstAFWorldMatrix.emplace_back(m_pTransCom->m_matWorld);
		m_lstAFAlpha.emplace_back(_rgba(1.f,1.f,1.f,1.f));
	}
	if (m_lstAFWorldMatrix.size())
	{
		for (list<_rgba>::iterator& iterFade = m_lstAFAlpha.begin(); iterFade != m_lstAFAlpha.end();)
		{
			(*iterFade).w -= m_fAfterSubAlpha;
			(*iterFade).x -= (Engine::CTimerMgr::Get_Instance()->Get_TimeDelta(L"Timer_TimeDelta"))*2.f;
			(*iterFade).y -= (Engine::CTimerMgr::Get_Instance()->Get_TimeDelta(L"Timer_TimeDelta"));
			if (0 > (*iterFade).w)
			{
				m_lstAFWorldMatrix.pop_front();
				iterFade = m_lstAFAlpha.erase(iterFade);
				continue;
			}
			else
				++iterFade;
		}
	}

	//if (m_lstAFWorldMatrix.size() > m_uiAfterImgSize )
	//	m_lstAFWorldMatrix.pop_front();

	return NO_EVENT;
}

_int CPopori_F::LateUpdate_GameObject(const _float & fTimeDelta)
{
	Engine::NULL_CHECK_RETURN(m_pRenderer, -1);

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
	[ Play Animation ]
	____________________________________________________________________________________________________________*/
	m_pMeshCom->Set_AnimationKey(m_uiAnimIdx);
	m_pMeshCom->Play_Animation(fTimeDelta * TPS);
	m_ui3DMax_NumFrame = *(m_pMeshCom->Get_3DMaxNumFrame());
	m_ui3DMax_CurFrame = *(m_pMeshCom->Get_3DMaxCurFrame());

	return NO_EVENT;
}

void CPopori_F::Render_GameObject(const _float& fTimeDelta)
{
	// Render AfterImage
	if (m_uiAfterImgSize)
	{
		m_pShaderCom->Set_PipelineStatePass(5);
		Render_AfterImage(fTimeDelta);
	}
}

void CPopori_F::Render_AfterImage(const _float& fTimeDelta)
{
	auto iter_begin = m_lstAFWorldMatrix.begin();
	auto iter_end = m_lstAFWorldMatrix.end();

	auto Alpha_begin = m_lstAFAlpha.begin();
	auto Alpha_end = m_lstAFAlpha.end();
	for (_uint i = 0; iter_begin != iter_end; ++i, ++iter_begin)
	{
		/*__________________________________________________________________________________________________________
		[ Set ConstantBuffer Data ]
		____________________________________________________________________________________________________________*/
		Engine::CB_SHADER_MESH tCB_ShaderMesh;
		ZeroMemory(&tCB_ShaderMesh, sizeof(Engine::CB_SHADER_MESH));
		tCB_ShaderMesh.matWorld = Engine::CShader::Compute_MatrixTranspose(*iter_begin);
		tCB_ShaderMesh.fAfterImgColor = *Alpha_begin;
		if (Alpha_begin != Alpha_end)
			Alpha_begin++;
		m_pShaderCom->Get_UploadBuffer_AFShaderMesh()->CopyData(i, tCB_ShaderMesh);

	//	m_vAfterImgColorAry[i].w -= (Engine::CTimerMgr::Get_Instance()->Get_TimeDelta(L"Timer_TimeDelta"))*0.5f;
		// Render Buffer
		m_pMeshCom->Render_DynamicMeshAfterImage(m_pShaderCom, i);

	}
}

void CPopori_F::Render_EdgeGameObject(const _float & fTimeDelta)
{
	Set_ConstantTable();
	m_pMeshCom->Render_DynamicMesh(m_pEdgeObjectShaderCom);
}

void CPopori_F::Render_ShadowDepth(const _float & fTimeDelta)
{
	Set_ConstantTableShadowDepth();
	m_pMeshCom->Render_DynamicMeshShadowDepth(m_pShadowCom);
}

void CPopori_F::Render_GameObject(const _float& fTimeDelta,
								  ID3D12GraphicsCommandList * pCommandList, 
								  const _int& iContextIdx)
{
	Set_ConstantTable();
	m_pShaderCom->Set_PipelineStatePass(0);
	m_pMeshCom->Render_DynamicMesh(pCommandList, iContextIdx, m_pShaderCom);

	//// Render AfterImage
	//if (m_uiAfterImgSize)
	//{
	//	m_pShaderCom->Set_PipelineStatePass(5);
	//	Render_AfterImage(fTimeDelta, pCommandList, iContextIdx);
	//}
}

void CPopori_F::Render_ShadowDepth(const _float& fTimeDelta, ID3D12GraphicsCommandList * pCommandList, const _int& iContextIdx)
{
	Set_ConstantTableShadowDepth();
	m_pMeshCom->Render_DynamicMeshShadowDepth(pCommandList, iContextIdx, m_pShadowCom);
}

HRESULT CPopori_F::Add_Component(wstring wstrMeshTag, wstring wstrNaviMeshTag)
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
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Shader", m_pShaderCom);

	// Shader
	m_pEdgeObjectShaderCom = static_cast<Engine::CShaderMesh*>(m_pComponentMgr->Clone_Component(L"ShaderMesh", Engine::COMPONENTID::ID_STATIC));
	Engine::NULL_CHECK_RETURN(m_pEdgeObjectShaderCom, E_FAIL);
	m_pEdgeObjectShaderCom->AddRef();
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Shader2", m_pEdgeObjectShaderCom);

	// Shadow
	m_pShadowCom = static_cast<Engine::CShaderShadow*>(m_pComponentMgr->Clone_Component(L"ShaderShadow", Engine::COMPONENTID::ID_STATIC));
	Engine::NULL_CHECK_RETURN(m_pShadowCom, E_FAIL);
	m_pShadowCom->AddRef();
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
	m_pNaviMeshCom = static_cast<Engine::CNaviMesh*>(m_pComponentMgr->Clone_Component(wstrNaviMeshTag, Engine::ID_DYNAMIC));
	Engine::NULL_CHECK_RETURN(m_pNaviMeshCom, E_FAIL);
	m_pNaviMeshCom->AddRef();
	m_mapComponent[Engine::ID_DYNAMIC].emplace(L"Com_NaviMesh", m_pNaviMeshCom);

	return S_OK;
}

void CPopori_F::Set_ConstantTable()
{
	/*__________________________________________________________________________________________________________
	[ Set ConstantBuffer Data ]
	____________________________________________________________________________________________________________*/
	Engine::SHADOW_DESC tShadowDesc = CShadowLightMgr::Get_Instance()->Get_ShadowDesc();
	
	Engine::CB_SHADER_MESH tCB_ShaderMesh;
	ZeroMemory(&tCB_ShaderMesh, sizeof(Engine::CB_SHADER_MESH));
	tCB_ShaderMesh.matWorld			= Engine::CShader::Compute_MatrixTranspose(m_pTransCom->m_matWorld);
	tCB_ShaderMesh.matLightView		= Engine::CShader::Compute_MatrixTranspose(tShadowDesc.matLightView);
	tCB_ShaderMesh.matLightProj		= Engine::CShader::Compute_MatrixTranspose(tShadowDesc.matLightProj);
	tCB_ShaderMesh.vLightPos		= tShadowDesc.vLightPosition;
	tCB_ShaderMesh.fLightPorjFar	= tShadowDesc.fLightPorjFar;

	m_fDeltaTime += (Engine::CTimerMgr::Get_Instance()->Get_TimeDelta(L"Timer_TimeDelta")) * 0.15f;
	tCB_ShaderMesh.fDeltaTime = m_fDeltaTime;
	m_pShaderCom->Get_UploadBuffer_ShaderMesh()->CopyData(0, tCB_ShaderMesh);
	m_pEdgeObjectShaderCom->Get_UploadBuffer_ShaderMesh()->CopyData(0, tCB_ShaderMesh);

	
	if (m_fDeltaTime > 1.f)
		m_fDeltaTime = 0.f;
}

void CPopori_F::Set_ConstantTableShadowDepth()
{
	/*__________________________________________________________________________________________________________
	[ Set ConstantBuffer Data ]
	____________________________________________________________________________________________________________*/
	Engine::SHADOW_DESC tShadowDesc = CShadowLightMgr::Get_Instance()->Get_ShadowDesc();

	Engine::CB_SHADER_SHADOW tCB_ShaderShadow;
	ZeroMemory(&tCB_ShaderShadow, sizeof(Engine::CB_SHADER_SHADOW));
	tCB_ShaderShadow.matWorld	= Engine::CShader::Compute_MatrixTranspose(m_pTransCom->m_matWorld);
	tCB_ShaderShadow.matView	= Engine::CShader::Compute_MatrixTranspose(tShadowDesc.matLightView);
	tCB_ShaderShadow.matProj	= Engine::CShader::Compute_MatrixTranspose(tShadowDesc.matLightProj);
	tCB_ShaderShadow.fProjFar	= tShadowDesc.fLightPorjFar;

	m_pShadowCom->Get_UploadBuffer_ShaderShadow()->CopyData(0, tCB_ShaderShadow);
}

void CPopori_F::Key_Input(const _float & fTimeDelta)
{
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
			m_pTransCom->m_vAngle.y = m_pDynamicCamera->Get_Transform()->m_vAngle.y + RIGHT_UP;
		// 대각선 - 좌 상단.
		else if (Engine::KEY_PRESSING(DIK_A))
			m_pTransCom->m_vAngle.y = m_pDynamicCamera->Get_Transform()->m_vAngle.y + LEFT_UP;
		// 직진.
		else
			m_pTransCom->m_vAngle.y = m_pDynamicCamera->Get_Transform()->m_vAngle.y + FRONT;

		// NaviMesh 이동.
		_vec3 vPos = m_pNaviMeshCom->Move_OnNaviMesh(&m_pTransCom->m_vPos, 
													 &m_pTransCom->m_vDir,
													 m_pInfoCom->m_fSpeed * fTimeDelta);
		m_pTransCom->m_vPos = vPos;
	}

	else if (Engine::KEY_PRESSING(DIK_S))
	{
		// 대각선 - 우 하단.
		if (Engine::KEY_PRESSING(DIK_D))
			m_pTransCom->m_vAngle.y = m_pDynamicCamera->Get_Transform()->m_vAngle.y + RIGHT_DOWN;
		// 대각선 - 좌 하단.
		else if (Engine::KEY_PRESSING(DIK_A))
			m_pTransCom->m_vAngle.y = m_pDynamicCamera->Get_Transform()->m_vAngle.y + LEFT_DOWN;
		// 후진.
		else
			m_pTransCom->m_vAngle.y = m_pDynamicCamera->Get_Transform()->m_vAngle.y + BACK;

		_vec3 vPos = m_pNaviMeshCom->Move_OnNaviMesh(&m_pTransCom->m_vPos,
													 &m_pTransCom->m_vDir,
													 m_pInfoCom->m_fSpeed * fTimeDelta);
		m_pTransCom->m_vPos = vPos;
	}

	else if (Engine::KEY_PRESSING(DIK_A))
	{
		// 좌로 이동.
		m_pTransCom->m_vAngle.y = m_pDynamicCamera->Get_Transform()->m_vAngle.y + LEFT;

		_vec3 vPos = m_pNaviMeshCom->Move_OnNaviMesh(&m_pTransCom->m_vPos,
													 &m_pTransCom->m_vDir, 
													 m_pInfoCom->m_fSpeed * fTimeDelta);
		m_pTransCom->m_vPos = vPos;
	}

	else if (Engine::KEY_PRESSING(DIK_D))
	{
		// 우로 이동.
		m_pTransCom->m_vAngle.y = m_pDynamicCamera->Get_Transform()->m_vAngle.y + RIGHT;

		_vec3 vPos = m_pNaviMeshCom->Move_OnNaviMesh(&m_pTransCom->m_vPos, 
													 &m_pTransCom->m_vDir,
													 m_pInfoCom->m_fSpeed * fTimeDelta);
		m_pTransCom->m_vPos = vPos;
	}

}

void CPopori_F::Change_Animation(const _float& fTimeDelta)
{
	if (0 == m_uiAnimIdx && m_pMeshCom->Is_AnimationSetEnd(fTimeDelta))
	{
		m_uiAnimIdx = 1;
	}

	if (1 == m_uiAnimIdx && m_pMeshCom->Is_AnimationSetEnd(fTimeDelta))
	{
		m_uiAnimIdx = 0;
	}
}

Engine::CGameObject* CPopori_F::Create(ID3D12Device * pGraphicDevice, ID3D12GraphicsCommandList * pCommandList,
									   wstring wstrMeshTag, 
									   wstring wstrNaviMeshTag,
									   const _vec3 & vScale, 
									   const _vec3 & vAngle, 
									   const _vec3 & vPos)
{
	CPopori_F* pInstance = new CPopori_F(pGraphicDevice, pCommandList);

	if (FAILED(pInstance->Ready_GameObject(wstrMeshTag, wstrNaviMeshTag, vScale, vAngle, vPos)))
		Engine::Safe_Release(pInstance);

	return pInstance;
}

void CPopori_F::Free()
{
	Engine::CGameObject::Free();

	Engine::Safe_Release(m_pDynamicCamera);

	Engine::Safe_Release(m_pMeshCom);
	Engine::Safe_Release(m_pShaderCom);
	Engine::Safe_Release(m_pEdgeObjectShaderCom);
	Engine::Safe_Release(m_pShadowCom);
	Engine::Safe_Release(m_pColliderSphereCom);
	Engine::Safe_Release(m_pColliderBoxCom);
	Engine::Safe_Release(m_pNaviMeshCom);

	Engine::Safe_Release(m_pFont);
}
