#include "stdafx.h"
#include "StaticMeshObject.h"
#include "GraphicDevice.h"
#include "DirectInput.h"
#include "ObjectMgr.h"
#include "LightMgr.h"
#include "TimeMgr.h"
#include "DynamicCamera.h"
#include "RenderTarget.h"
#include "ShaderMgr.h"
#include "NaviMesh.h"

CStaticMeshObject::CStaticMeshObject(ID3D12Device * pGraphicDevice, ID3D12GraphicsCommandList * pCommandList)
	: Engine::CGameObject(pGraphicDevice, pCommandList)
	, m_pShaderShadowInstancing(Engine::CShaderShadowInstancing::Get_Instance())
	, m_pShaderMeshInstancing(Engine::CShaderMeshInstancing::Get_Instance())
{
}

HRESULT CStaticMeshObject::Ready_GameObject(wstring wstrMeshTag, 
											const _vec3 & vScale, 
											const _vec3 & vAngle, 
											const _vec3 & vPos,
											const _bool& bIsRenderShadow,
											const _bool& bIsCollision,
											const _vec3& vBoundingSphereScale,
											const _vec3& vBoundingSpherePos,
											const _vec3& vPosOffset)
{
	Engine::FAILED_CHECK_RETURN(Engine::CGameObject::Ready_GameObject(true, false, true, true), E_FAIL);
	Engine::FAILED_CHECK_RETURN(Add_Component(wstrMeshTag), E_FAIL);
	m_wstrMeshTag			= wstrMeshTag;
	m_pTransCom->m_vScale	= vScale;
	m_pTransCom->m_vAngle	= vAngle;
	m_pTransCom->m_vPos		= vPos + vPosOffset;
	m_pTransCom->Update_Component(0.016f);
	m_pTransCom->Set_IsStatic(true);

	m_bIsCollision = bIsCollision;
	m_eRenderGroup = Engine::CRenderer::RENDERGROUP::RENDER_NONALPHA;

	// BoundingBox.
	Engine::CGameObject::SetUp_BoundingBox(&(m_pTransCom->m_matWorld),
										   m_pTransCom->m_vScale,
										   m_pMeshCom->Get_CenterPos(),
										   m_pMeshCom->Get_MinVector(),
										   m_pMeshCom->Get_MaxVector());
	// BoundingSphere.
	Engine::CGameObject::SetUp_BoundingSphere(&(m_pTransCom->m_matWorld),
											  m_pTransCom->m_vScale,
											  vBoundingSphereScale,
											  vBoundingSpherePos);

	if (m_wstrMeshTag == L"VK_CON_Floor_04_SM" ||
		m_wstrMeshTag == L"VK_CON_Floor_03_SM")
	{
		m_iMeshPipelineStatePass = 1;
	}
	else
		m_iMeshPipelineStatePass = 3;

	m_iShadowPipelineStatePass = 0;

	m_iRandomnumber = rand() % 10;

	return S_OK;
}

HRESULT CStaticMeshObject::LateInit_GameObject()
{
	m_pShaderCom->SetUp_ShaderConstantBuffer((_uint)(m_pMeshCom->Get_DiffTexture().size()));
	m_pCrossFilterShaderCom->SetUp_ShaderConstantBuffer((_uint)(m_pMeshCom->Get_DiffTexture().size()));
	m_pEdgeObjectShaderCom->SetUp_ShaderConstantBuffer((_uint)(m_pMeshCom->Get_DiffTexture().size()));

	return S_OK;
}

_int CStaticMeshObject::Update_GameObject(const _float & fTimeDelta)
{
	Engine::FAILED_CHECK_RETURN(Engine::CGameObject::LateInit_GameObject(), E_FAIL);

	if (m_bIsDead)
		return DEAD_OBJ;

	/*__________________________________________________________________________________________________________
	[ Renderer - Add Render Group ]
	____________________________________________________________________________________________________________*/
	// Frustum Culling
	if (m_wstrMeshTag == L"VK_CON_Wall_B_SM" ||
		m_wstrMeshTag == L"Cliff_Rock2" ||
		m_wstrMeshTag == L"Cliff_Rock4" ||
		m_wstrMeshTag == L"Cliff_Rock7" ||
		m_wstrMeshTag == L"SnowMountain01_SM" || 
		m_wstrMeshTag == L"SnowMountain02_SM" || 
		m_wstrMeshTag == L"Beast_Ice_06_SM" || 
		m_wstrMeshTag == L"Beast_Ice_07_SM")
	{
		Engine::FAILED_CHECK_RETURN(m_pRenderer->Add_Renderer(Engine::CRenderer::RENDER_NONALPHA, this), -1);
	}
	else
	{
		if (m_pRenderer->Get_Frustum().Contains(m_pBoundingBoxCom->Get_BoundingInfo()) != DirectX::DISJOINT)
		{
			Engine::FAILED_CHECK_RETURN(m_pRenderer->Add_Renderer(Engine::CRenderer::RENDER_NONALPHA, this), -1);
		}
	}

	/*____________________________________________________________________
	TransCom - Update WorldMatrix.
	______________________________________________________________________*/
	Engine::CGameObject::Update_GameObject(fTimeDelta);

	return NO_EVENT;
}

_int CStaticMeshObject::LateUpdate_GameObject(const _float & fTimeDelta)
{
	Engine::NULL_CHECK_RETURN(m_pRenderer, -1);
	Process_Collision(fTimeDelta);

	Set_ConstantTable();

	return NO_EVENT;
}

void CStaticMeshObject::Process_Collision(const _float& fTimeDelta)
{
	if (m_bIsCollision)
	{
		Engine::CGameObject* pThisPlayer = m_pObjectMgr->Get_GameObject(L"Layer_GameObject", L"ThisPlayer");

		if (Engine::CCollisionMgr::Check_Sphere(m_pBoundingSphereCom->Get_BoundingInfo(), pThisPlayer->Get_BoundingSphere()->Get_BoundingInfo()))
		{
			// Process Collision Event
			m_pBoundingSphereCom->Set_Color(_rgba(1.0f, 0.0f, 0.0f, 1.0f));
			pThisPlayer->Get_BoundingSphere()->Set_Color(_rgba(1.0f, 0.0f, 0.0f, 1.0f));

			_float fSpeed = pThisPlayer->Get_Info()->m_fSpeed;
			_vec3 vDir = pThisPlayer->Get_Transform()->m_vDir;
			vDir.Normalize();

			pThisPlayer->Get_Transform()->m_vPos += (-1.0f) * vDir * fTimeDelta * fSpeed * 1.05f;

			Engine::CNaviMesh* pNaviMeshCom = static_cast<Engine::CNaviMesh*>(pThisPlayer->Get_Component(L"Com_NaviMesh", Engine::ID_DYNAMIC));
			if (nullptr != pNaviMeshCom)
				pNaviMeshCom->Set_CurrentCellIndex(pNaviMeshCom->Get_CurrentPositionCellIndex(pThisPlayer->Get_Transform()->m_vPos));

			// Send Packet
			CPacketMgr::Get_Instance()->send_move_stop(pThisPlayer->Get_Transform()->m_vPos,
													   pThisPlayer->Get_Transform()->m_vDir,
													   pThisPlayer->Get_AnimationIdx());
		}
	}

}

void CStaticMeshObject::Render_GameObject(const _float& fTimeDelta)
{
	//m_pMeshCom->Render_StaticMesh(m_pShaderCom);	
}
void CStaticMeshObject::Render_EdgeGameObject(const _float& fTimeDelta)
{
	m_pMeshCom->Render_StaticMesh(m_pEdgeObjectShaderCom);
}
void CStaticMeshObject::Render_CrossFilterGameObject(const _float& fTimeDelta)
{
	m_pMeshCom->Render_StaticMesh(m_pCrossFilterShaderCom);
}


void CStaticMeshObject::Render_GameObject(const _float& fTimeDelta, 
										  ID3D12GraphicsCommandList * pCommandList,
										  const _int& iContextIdx)
{
	/*__________________________________________________________________________________________________________
	[ Add Instance ]
	____________________________________________________________________________________________________________*/
	m_pShaderMeshInstancing->Add_Instance(iContextIdx, m_wstrMeshTag, m_iMeshPipelineStatePass);
	_uint iInstanceIdx = m_pShaderMeshInstancing->Get_InstanceCount(iContextIdx, m_wstrMeshTag, m_iMeshPipelineStatePass) - 1;
	Set_ConstantTable(iContextIdx, iInstanceIdx);
}

void CStaticMeshObject::Render_ShadowDepth(const _float& fTimeDelta, 
										   ID3D12GraphicsCommandList * pCommandList,
										   const _int& iContextIdx)
{
	/*__________________________________________________________________________________________________________
	[ Add Instance ]
	____________________________________________________________________________________________________________*/
	if (m_wstrMeshTag != L"VK_CON_Floor_04_SM" &&
		m_wstrMeshTag != L"VK_CON_Floor_03_SM" &&
		m_wstrMeshTag != L"VK_CON_Wall_B_SM" &&
		m_wstrMeshTag != L"VK_CON3_OBJ_01_SM" &&
		m_wstrMeshTag != L"Cliff_Rock1" &&
		m_wstrMeshTag != L"Cliff_Rock2" &&
		m_wstrMeshTag != L"Cliff_Rock3" &&
		m_wstrMeshTag != L"Cliff_Rock4" &&
		m_wstrMeshTag != L"Cliff_Rock5" &&
		m_wstrMeshTag != L"Cliff_Rock6" &&
		m_wstrMeshTag != L"Cliff_Rock7" &&
		m_wstrMeshTag != L"Cliff_Rock8" &&
		m_wstrMeshTag != L"Cliff_Rock9" &&
		m_wstrMeshTag != L"SnowMountain01_SM" &&
		m_wstrMeshTag != L"SnowMountain02_SM" &&
		m_wstrMeshTag != L"Beast_Ice_06_SM" &&
		m_wstrMeshTag != L"Beast_Ice_07_SM" &&
		m_wstrMeshTag != L"Ur_Oriyn_Statue_01_SM")
	{
		m_pShaderShadowInstancing->Add_Instance(iContextIdx, m_wstrMeshTag, m_iShadowPipelineStatePass);
		_uint iInstanceIdx = m_pShaderShadowInstancing->Get_InstanceCount(iContextIdx, m_wstrMeshTag, m_iShadowPipelineStatePass) - 1;
		Set_ConstantTableShadowDepth(iContextIdx, iInstanceIdx);
	}
}

HRESULT CStaticMeshObject::Add_Component(wstring wstrMeshTag)
{
	Engine::NULL_CHECK_RETURN(m_pComponentMgr, E_FAIL);

	// Mesh
	m_pMeshCom = static_cast<Engine::CMesh*>(m_pComponentMgr->Clone_Component(wstrMeshTag.c_str(), Engine::COMPONENTID::ID_STATIC));
	Engine::NULL_CHECK_RETURN(m_pMeshCom, E_FAIL);
	m_pMeshCom->AddRef();
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Mesh", m_pMeshCom);

	m_pShaderCom = static_cast<Engine::CShaderMesh*>(m_pComponentMgr->Clone_Component(L"ShaderMesh", Engine::COMPONENTID::ID_STATIC));
	Engine::NULL_CHECK_RETURN(m_pShaderCom, E_FAIL);
	m_pShaderCom->AddRef();
	Engine::FAILED_CHECK_RETURN(m_pShaderCom->Set_PipelineStatePass(0), E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Shader", m_pShaderCom);

	m_pCrossFilterShaderCom = static_cast<Engine::CShaderMesh*>(m_pComponentMgr->Clone_Component(L"ShaderMesh", Engine::COMPONENTID::ID_STATIC));
	Engine::NULL_CHECK_RETURN(m_pCrossFilterShaderCom, E_FAIL);
	m_pCrossFilterShaderCom->AddRef();
	Engine::FAILED_CHECK_RETURN(m_pCrossFilterShaderCom->Set_PipelineStatePass(3), E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Shader2", m_pCrossFilterShaderCom);

	m_pEdgeObjectShaderCom = static_cast<Engine::CShaderMesh*>(m_pComponentMgr->Clone_Component(L"ShaderMesh", Engine::COMPONENTID::ID_STATIC));
	Engine::NULL_CHECK_RETURN(m_pEdgeObjectShaderCom, E_FAIL);
	m_pEdgeObjectShaderCom->AddRef();
	Engine::FAILED_CHECK_RETURN(m_pEdgeObjectShaderCom->Set_PipelineStatePass(4), E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Shader3", m_pEdgeObjectShaderCom);

	return S_OK;
}

void CStaticMeshObject::Set_ConstantTable(const _int& iContextIdx, const _int& iInstanceIdx)
{
	/*__________________________________________________________________________________________________________
	[ Set ShaderResource Data ]
	____________________________________________________________________________________________________________*/
	Engine::SHADOW_DESC tShadowDesc = CShadowLightMgr::Get_Instance()->Get_ShadowDesc();
	_matrix matView = *(Engine::CGraphicDevice::Get_Instance()->Get_Transform(Engine::MATRIXID::VIEW));
	_matrix matProj = *(Engine::CGraphicDevice::Get_Instance()->Get_Transform(Engine::MATRIXID::PROJECTION));

	Engine::CB_SHADER_MESH_INSTANCEING tCB_ShaderMesh;
	ZeroMemory(&tCB_ShaderMesh, sizeof(Engine::CB_SHADER_MESH_INSTANCEING));
	tCB_ShaderMesh.matWorld       = Engine::CShader::Compute_MatrixTranspose(m_pTransCom->m_matWorld);
	tCB_ShaderMesh.matView        = Engine::CShader::Compute_MatrixTranspose(matView);
	tCB_ShaderMesh.matProj        = Engine::CShader::Compute_MatrixTranspose(matProj);
	tCB_ShaderMesh.matLightView   = Engine::CShader::Compute_MatrixTranspose(tShadowDesc.matLightView);
	tCB_ShaderMesh.matLightProj   = Engine::CShader::Compute_MatrixTranspose(tShadowDesc.matLightProj);
	tCB_ShaderMesh.vLightPos      = tShadowDesc.vLightPosition;
	tCB_ShaderMesh.fLightPorjFar  = tShadowDesc.fLightPorjFar;
	tCB_ShaderMesh.vEmissiveColor = _rgba(1.1f, 1.1f, 1.1f, 1.0f);

	m_fDeltaTime += (Engine::CTimerMgr::Get_Instance()->Get_TimeDelta(L"Timer_TimeDelta")) * 0.05f;
	tCB_ShaderMesh.fDissolve = m_fDeltaTime;
	if (m_fDeltaTime > 1.f)
		m_fDeltaTime = 0.f;

	m_pShaderMeshInstancing->Get_UploadBuffer_ShaderMesh(iContextIdx, m_wstrMeshTag, m_iMeshPipelineStatePass)->CopyData(iInstanceIdx, tCB_ShaderMesh);
}

void CStaticMeshObject::Set_ConstantTable()
{
	/*__________________________________________________________________________________________________________
	[ Set ConstantBuffer Data ]
	____________________________________________________________________________________________________________*/
	Engine::SHADOW_DESC tShadowDesc = CShadowLightMgr::Get_Instance()->Get_ShadowDesc();

	Engine::CB_SHADER_MESH tCB_ShaderMesh;
	ZeroMemory(&tCB_ShaderMesh, sizeof(Engine::CB_SHADER_MESH));
	tCB_ShaderMesh.matWorld = Engine::CShader::Compute_MatrixTranspose(m_pTransCom->m_matWorld);

	m_pShaderCom->Get_UploadBuffer_ShaderMesh()->CopyData(0,tCB_ShaderMesh);

	int randR = rand() % 70 + 30, randG = rand() % 70 + 30, randB = rand() % 70 + 30;
	tCB_ShaderMesh.vLightPos.x =1.f;//randR *0.01f;
	tCB_ShaderMesh.vLightPos.y =1.f;//randG * 0.01f;
	tCB_ShaderMesh.vLightPos.z =1.f;//randB * 0.01f;
	m_pCrossFilterShaderCom->Get_UploadBuffer_ShaderMesh()->CopyData(0, tCB_ShaderMesh);
	m_pEdgeObjectShaderCom->Get_UploadBuffer_ShaderMesh()->CopyData(0, tCB_ShaderMesh);
}

void CStaticMeshObject::Set_ConstantTableShadowDepth(const _int& iContextIdx, const _int& iInstanceIdx)
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


	tCB_ShaderShadow.fOffset1 = Engine::CShaderMgr::Get_Instance()->Get_ShaderInfo().TreeAlphaTest;
	m_pShaderShadowInstancing->Get_UploadBuffer_ShaderShadow(iContextIdx, m_wstrMeshTag, m_iShadowPipelineStatePass)->CopyData(iInstanceIdx, tCB_ShaderShadow);
}

Engine::CGameObject* CStaticMeshObject::Create(ID3D12Device * pGraphicDevice, ID3D12GraphicsCommandList * pCommandList,
											   wstring wstrMeshTag, 
											   const _vec3 & vScale, 
											   const _vec3 & vAngle, 
											   const _vec3 & vPos,
											   const _bool& bIsRenderShadow,
											   const _bool& bIsCollision,
											   const _vec3& vBoundingSphereScale,
											   const _vec3& vBoundingSpherePos,
											   const _vec3& vPosOffset)
{
	CStaticMeshObject* pInstance = new CStaticMeshObject(pGraphicDevice, pCommandList);

	if (FAILED(pInstance->Ready_GameObject(wstrMeshTag, 
										   vScale, 
										   vAngle,
										   vPos, 
										   bIsRenderShadow,
										   bIsCollision, 
										   vBoundingSphereScale,
										   vBoundingSpherePos,
										   vPosOffset)))
		Engine::Safe_Release(pInstance);

	// SetUp InstanceShader and Add Instance Count.
	Engine::CShaderShadowInstancing::Get_Instance()->SetUp_Instancing(wstrMeshTag);
	Engine::CShaderShadowInstancing::Get_Instance()->Add_TotalInstanceCount(wstrMeshTag);
	Engine::CShaderMeshInstancing::Get_Instance()->SetUp_Instancing(wstrMeshTag);
	Engine::CShaderMeshInstancing::Get_Instance()->Add_TotalInstanceCount(wstrMeshTag);

	return pInstance;
}

void CStaticMeshObject::Free()
{
	Engine::CGameObject::Free();
	Engine::Safe_Release(m_pMeshCom);
	Engine::Safe_Release(m_pShaderCom);
	Engine::Safe_Release(m_pCrossFilterShaderCom);
	Engine::Safe_Release(m_pEdgeObjectShaderCom);
}
