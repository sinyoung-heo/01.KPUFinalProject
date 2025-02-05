#include "stdafx.h"
#include "TestOthers.h"

#include "GraphicDevice.h"
#include "DirectInput.h"
#include "ObjectMgr.h"
#include "LightMgr.h"
#include "Font.h"
#include "RenderTarget.h"

CTestOthers::CTestOthers(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList)
	: Engine::CGameObject(pGraphicDevice, pCommandList)
	, m_pPacketMgr(CPacketMgr::Get_Instance())
	, m_pServerMath(CServerMath::Get_Instance())
{
}

HRESULT CTestOthers::Ready_GameObject(wstring wstrMeshTag, const _vec3& vScale, const _vec3& vAngle, const _vec3& vPos)
{
	Engine::FAILED_CHECK_RETURN(Engine::CGameObject::Ready_GameObject(true, true, true), E_FAIL);
	Engine::FAILED_CHECK_RETURN(Add_Component(wstrMeshTag), E_FAIL);
	m_pTransCom->m_vScale = vScale;
	m_pTransCom->m_vAngle = vAngle;
	m_pTransCom->m_vPos = vPos;
	m_pNaviMeshCom->Set_CurrentCellIndex(m_pNaviMeshCom->Get_CurrentPositionCellIndex(vPos));
	Engine::CGameObject::SetUp_BoundingBox(&(m_pTransCom->m_matWorld),
										   m_pTransCom->m_vScale,
										   m_pMeshCom->Get_CenterPos(),
										   m_pMeshCom->Get_MinVector(),
										   m_pMeshCom->Get_MaxVector());

	m_pInfoCom->m_fSpeed = 0.0f;
	m_pInfoCom->m_vArrivePos = m_pTransCom->m_vPos;

	/*__________________________________________________________________________________________________________
	[ 애니메이션 설정 ]
	____________________________________________________________________________________________________________*/
	m_uiAnimIdx = 1;

	/*__________________________________________________________________________________________________________
	[ Collider Bone Setting ]
	____________________________________________________________________________________________________________*/
	Engine::SKINNING_MATRIX* pmatSkinning = nullptr;
	_matrix* pmatParent = nullptr;

	// ColliderSphere
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

	return S_OK;
}

HRESULT CTestOthers::LateInit_GameObject()
{
	// SetUp Shader ConstantBuffer
	m_pShaderCom->SetUp_ShaderConstantBuffer((_uint)(m_pMeshCom->Get_DiffTexture().size()));
	m_pShadowCom->SetUp_ShaderConstantBuffer((_uint)(m_pMeshCom->Get_DiffTexture().size()));

	return S_OK;
}

_int CTestOthers::Update_GameObject(const _float& fTimeDelta)
{
	Engine::FAILED_CHECK_RETURN(Engine::CGameObject::LateInit_GameObject(), E_FAIL);

	if (m_bIsDead)
		return DEAD_OBJ;

	/*__________________________________________________________________________________________________________
	[ Renderer - Add Render Group ]
	____________________________________________________________________________________________________________*/
	Engine::FAILED_CHECK_RETURN(m_pRenderer->Add_Renderer(Engine::CRenderer::RENDER_NONALPHA, this), -1);

	/*__________________________________________________________________________________________________________
	[ Play Animation ]
	____________________________________________________________________________________________________________*/
	m_pMeshCom->Set_AnimationKey(m_uiAnimIdx);
	m_pMeshCom->Play_Animation(fTimeDelta * TPS);

	/*__________________________________________________________________________________________________________
	[ TransCom - Update WorldMatrix ]
	____________________________________________________________________________________________________________*/
	Move_OnNaviMesh(fTimeDelta);
	Engine::CGameObject::Update_GameObject(fTimeDelta);

	return NO_EVENT;
}

_int CTestOthers::LateUpdate_GameObject(const _float& fTimeDelta)
{
	Engine::NULL_CHECK_RETURN(m_pRenderer, -1);

	return NO_EVENT;
}

void CTestOthers::Send_PacketToServer()
{

}

void CTestOthers::Render_GameObject(const _float& fTimeDelta, ID3D12GraphicsCommandList* pCommandList, const _int& iContextIdx)
{
	Set_ConstantTable();
	m_pMeshCom->Render_DynamicMesh(pCommandList, iContextIdx, m_pShaderCom);
}

void CTestOthers::Render_ShadowDepth(const _float& fTimeDelta, ID3D12GraphicsCommandList* pCommandList, const _int& iContextIdx)
{
	Set_ConstantTableShadowDepth();
	m_pMeshCom->Render_DynamicMeshShadowDepth(pCommandList, iContextIdx, m_pShadowCom);
}

HRESULT CTestOthers::Add_Component(wstring wstrMeshTag)
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
	m_pNaviMeshCom = static_cast<Engine::CNaviMesh*>(m_pComponentMgr->Clone_Component(L"StageVelika_NaviMesh", Engine::ID_DYNAMIC));
	Engine::NULL_CHECK_RETURN(m_pNaviMeshCom, E_FAIL);
	m_pNaviMeshCom->AddRef();
	m_pNaviMeshCom->Set_CurrentCellIndex(0);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_NaviMesh", m_pNaviMeshCom);

	return S_OK;
}

void CTestOthers::Set_ConstantTable()
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

void CTestOthers::Set_ConstantTableShadowDepth()
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

void CTestOthers::Move_OnNaviMesh(const _float& fTimeDelta)
{
	m_pTransCom->m_vDir = m_pTransCom->Get_LookVector();
	m_pTransCom->m_vDir.Normalize();

	SetUp_MoveSpeed(fTimeDelta);

	if (!m_bIsMoveStop /*|| 
		m_pInfoCom->m_fSpeed == PCOthersConst::MIN_SPEED*/)
	{
		// NaviMesh 이동.		
		if (!CServerMath::Get_Instance()->Is_Arrive_Point(m_pTransCom->m_vPos, m_pInfoCom->m_vArrivePos))
		{
			_vec3 vPos = m_pNaviMeshCom->Move_OnNaviMesh(&m_pTransCom->m_vPos,
														 &m_pTransCom->m_vDir,
														 m_pInfoCom->m_fSpeed * fTimeDelta);
			m_pTransCom->m_vPos = vPos;
		}
	}
}

void CTestOthers::SetUp_MoveSpeed(const _float& fTimeDelta)
{
	//if (!m_bIsMoveStop)
	//{
	//	m_pInfoCom->m_fSpeed += ((PCOthersConst::MAX_SPEED + PCOthersConst::MAX_SPEED) / 2.0f) * fTimeDelta;
	//	if (m_pInfoCom->m_fSpeed > PCOthersConst::MAX_SPEED)
	//		m_pInfoCom->m_fSpeed = PCOthersConst::MAX_SPEED;
	//}
	//else
	//{
	//	m_pInfoCom->m_fSpeed -= PCOthersConst::MAX_SPEED * 3.0f * fTimeDelta;
	//	if (m_pInfoCom->m_fSpeed < PCOthersConst::MIN_SPEED)
	//		m_pInfoCom->m_fSpeed = PCOthersConst::MIN_SPEED;
	//}
}

Engine::CGameObject* CTestOthers::Create(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList, 
										 wstring wstrMeshTag, 
										 const _vec3& vScale, 
										 const _vec3& vAngle, 
										 const _vec3& vPos)
{
	CTestOthers* pInstance = new CTestOthers(pGraphicDevice, pCommandList);

	if (FAILED(pInstance->Ready_GameObject(wstrMeshTag, vScale, vAngle, vPos)))
		Engine::Safe_Release(pInstance);

	return pInstance;
}

void CTestOthers::Free()
{
	Engine::CGameObject::Free();

	Engine::Safe_Release(m_pMeshCom);
	Engine::Safe_Release(m_pShaderCom);
	Engine::Safe_Release(m_pShadowCom);
	Engine::Safe_Release(m_pColliderSphereCom);
	Engine::Safe_Release(m_pColliderBoxCom);
	Engine::Safe_Release(m_pNaviMeshCom);
}
