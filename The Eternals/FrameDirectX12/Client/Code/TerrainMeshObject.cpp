#include "stdafx.h"
#include "TerrainMeshObject.h"
#include "GraphicDevice.h"
#include "DirectInput.h"
#include "ObjectMgr.h"
#include "LightMgr.h"
#include "DynamicCamera.h"
#include "RenderTarget.h"
#include "TimeMgr.h"

CTerrainMeshObject::CTerrainMeshObject(ID3D12Device * pGraphicDevice, ID3D12GraphicsCommandList * pCommandList)
	: Engine::CGameObject(pGraphicDevice, pCommandList)
	, m_pShaderShadowInstancing(Engine::CShaderShadowInstancing::Get_Instance())
	, m_pShaderMeshInstancing(Engine::CShaderMeshInstancing::Get_Instance())
{
}


HRESULT CTerrainMeshObject::Ready_GameObject(wstring wstrMeshTag,
											 const _vec3 & vScale,
											 const _vec3 & vAngle, 
											 const _vec3 & vPos,
											 const _vec3& vPosOffset)
{
	Engine::FAILED_CHECK_RETURN(Engine::CGameObject::Ready_GameObject(), E_FAIL);
	Engine::FAILED_CHECK_RETURN(Add_Component(wstrMeshTag), E_FAIL);

	m_wstrMeshTag = wstrMeshTag;
	m_pTransCom->m_vScale	= vScale;
	m_pTransCom->m_vAngle	= vAngle;
	m_pTransCom->m_vPos		= vPos + vPosOffset;

	// PipelineState.
	m_iMeshPipelineStatePass	= 2;
	m_iShadowPipelineStatePass	= 0;
	m_fDeltatime2 = 4.f;
	m_fWave = 1.f;
	return S_OK;
}

HRESULT CTerrainMeshObject::LateInit_GameObject()
{

	return S_OK;
}

_int CTerrainMeshObject::Update_GameObject(const _float & fTimeDelta)
{
	Engine::FAILED_CHECK_RETURN(Engine::CGameObject::LateInit_GameObject(), E_FAIL);

	if (m_bIsDead)
		return DEAD_OBJ;

	/*__________________________________________________________________________________________________________
	[ Renderer - Add Render Group ]
	____________________________________________________________________________________________________________*/
	Engine::FAILED_CHECK_RETURN(m_pRenderer->Add_Renderer(Engine::CRenderer::RENDER_NONALPHA, this), -1);

	/*____________________________________________________________________
	TransCom - Update WorldMatrix.
	______________________________________________________________________*/
	Engine::CGameObject::Update_GameObject(fTimeDelta);

	return NO_EVENT;
}

_int CTerrainMeshObject::LateUpdate_GameObject(const _float & fTimeDelta)
{
	Engine::NULL_CHECK_RETURN(m_pRenderer, -1);

	return NO_EVENT;
}

void CTerrainMeshObject::Render_GameObject(const _float& fTimeDelta,
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

void CTerrainMeshObject::Render_ShadowDepth(const _float& fTimeDelta, 
											ID3D12GraphicsCommandList * pCommandList, 
											const _int& iContextIdx)
{
	///*__________________________________________________________________________________________________________
	//[ Add Instance ]
	//____________________________________________________________________________________________________________*/
	//m_pShaderShadowInstancing->Add_Instance(iContextIdx, m_wstrMeshTag, m_iShadowPipelineStatePass);
	//_uint iInstanceIdx = m_pShaderShadowInstancing->Get_InstanceCount(iContextIdx, m_wstrMeshTag, m_iShadowPipelineStatePass) - 1;

	//Set_ConstantTableShadowDepth(iContextIdx, iInstanceIdx);
}

HRESULT CTerrainMeshObject::Add_Component(wstring wstrMeshTag)
{
	Engine::NULL_CHECK_RETURN(m_pComponentMgr, E_FAIL);

	// Mesh
	m_pMeshCom = static_cast<Engine::CMesh*>(m_pComponentMgr->Clone_Component(wstrMeshTag.c_str(), Engine::COMPONENTID::ID_STATIC));
	Engine::NULL_CHECK_RETURN(m_pMeshCom, E_FAIL);
	m_pMeshCom->AddRef();
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Mesh", m_pMeshCom);

	return S_OK;
}

void CTerrainMeshObject::Set_ConstantTable()
{
	/*__________________________________________________________________________________________________________
	[ Set ConstantBuffer Data ]
	____________________________________________________________________________________________________________*/
	//Engine::SHADOW_DESC tShadowDesc = CShadowLightMgr::Get_Instance()->Get_ShadowDesc();
	//
	//Engine::CB_SHADER_MESH tCB_ShaderMesh;
	//ZeroMemory(&tCB_ShaderMesh, sizeof(Engine::CB_SHADER_MESH));
	//tCB_ShaderMesh.matWorld			= Engine::CShader::Compute_MatrixTranspose(m_pTransCom->m_matWorld);
	//tCB_ShaderMesh.matLightView		= Engine::CShader::Compute_MatrixTranspose(tShadowDesc.matLightView);
	//tCB_ShaderMesh.matLightProj		= Engine::CShader::Compute_MatrixTranspose(tShadowDesc.matLightProj);
	//tCB_ShaderMesh.vLightPos		= tShadowDesc.vLightPosition;
	//tCB_ShaderMesh.fLightPorjFar	= tShadowDesc.fLightPorjFar;

	//m_pShaderCom->Get_UploadBuffer_ShaderMesh()->CopyData(0, tCB_ShaderMesh);
}

void CTerrainMeshObject::Set_ConstantTableShadowDepth()
{
	/*__________________________________________________________________________________________________________
	[ Set ConstantBuffer Data ]
	____________________________________________________________________________________________________________*/
	//Engine::SHADOW_DESC tShadowDesc = CShadowLightMgr::Get_Instance()->Get_ShadowDesc();

	//Engine::CB_SHADER_SHADOW tCB_ShaderShadow;
	//ZeroMemory(&tCB_ShaderShadow, sizeof(Engine::CB_SHADER_SHADOW));
	//tCB_ShaderShadow.matWorld	= Engine::CShader::Compute_MatrixTranspose(m_pTransCom->m_matWorld);
	//tCB_ShaderShadow.matView	= Engine::CShader::Compute_MatrixTranspose(tShadowDesc.matLightView);
	//tCB_ShaderShadow.matProj	= Engine::CShader::Compute_MatrixTranspose(tShadowDesc.matLightProj);
	//tCB_ShaderShadow.fProjFar	= tShadowDesc.fLightPorjFar;

	//m_pShadowCom->Get_UploadBuffer_ShaderShadow()->CopyData(0, tCB_ShaderShadow);
}

void CTerrainMeshObject::Set_ConstantTable(const _int& iContextIdx, const _int& iInstanceIdx)
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
	if (m_wstrMeshTag == L"BumpTerrainMesh01")
		tCB_ShaderMesh.vEmissiveColor = _rgba(1.1f, 1.1f, 1.1f, 1.1f);
	else
		tCB_ShaderMesh.vEmissiveColor = _rgba(1.0f, 1.0f, 1.0f, 1.0f);

	// PipelineState.
	m_fDeltaTime += (Engine::CTimerMgr::Get_Instance()->Get_TimeDelta(L"Timer_TimeDelta")) * 0.005f;
	m_fDeltatime2 += (Engine::CTimerMgr::Get_Instance()->Get_TimeDelta(L"Timer_TimeDelta"));
	m_fDeltatime3 += (Engine::CTimerMgr::Get_Instance()->Get_TimeDelta(L"Timer_TimeDelta"));
	tCB_ShaderMesh.fOffset1 = m_fDeltaTime;
	tCB_ShaderMesh.fOffset2 = 0.5f + m_fDeltatime2;
	tCB_ShaderMesh.fOffset3 = m_fWave;
	tCB_ShaderMesh.fDissolve = sin(m_fDeltatime3);

	m_pShaderMeshInstancing->Get_UploadBuffer_ShaderMesh(iContextIdx, m_wstrMeshTag, m_iMeshPipelineStatePass)->CopyData(iInstanceIdx, tCB_ShaderMesh);
}

void CTerrainMeshObject::Set_ConstantTableShadowDepth(const _int& iContextIdx, const _int& iInstanceIdx)
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

	m_pShaderShadowInstancing->Get_UploadBuffer_ShaderShadow(iContextIdx, m_wstrMeshTag, m_iShadowPipelineStatePass)->CopyData(iInstanceIdx, tCB_ShaderShadow);
}


Engine::CGameObject* CTerrainMeshObject::Create(ID3D12Device * pGraphicDevice, ID3D12GraphicsCommandList * pCommandList,
												wstring wstrMeshTag, 
												const _vec3 & vScale,
												const _vec3 & vAngle,
												const _vec3 & vPos,
												const _vec3& vPosOffset)
{
	CTerrainMeshObject* pInstance = new CTerrainMeshObject(pGraphicDevice, pCommandList);

	if (FAILED(pInstance->Ready_GameObject(wstrMeshTag, vScale, vAngle, vPos, vPosOffset)))
		Engine::Safe_Release(pInstance);

	// SetUp InstanceShader and Add Instance Count.
	Engine::CShaderShadowInstancing::Get_Instance()->SetUp_Instancing(wstrMeshTag);
	Engine::CShaderShadowInstancing::Get_Instance()->Add_TotalInstanceCount(wstrMeshTag);
	Engine::CShaderMeshInstancing::Get_Instance()->SetUp_Instancing(wstrMeshTag);
	Engine::CShaderMeshInstancing::Get_Instance()->Add_TotalInstanceCount(wstrMeshTag);

	return pInstance;
}

void CTerrainMeshObject::Free()
{
	Engine::CGameObject::Free();
	Engine::Safe_Release(m_pMeshCom);
}
