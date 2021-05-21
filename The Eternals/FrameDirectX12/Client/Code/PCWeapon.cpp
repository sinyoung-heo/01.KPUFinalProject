#include "stdafx.h"
#include "PCWeapon.h"
#include "GraphicDevice.h"
#include "DirectInput.h"
#include "ObjectMgr.h"
#include "LightMgr.h"

CPCWeapon::CPCWeapon(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList)
	: Engine::CGameObject(pGraphicDevice, pCommandList)
{
}

void CPCWeapon::Set_DissolveInterpolation(const _float& fDissolveSpeed)
{
	m_fDissolveSpeed = fDissolveSpeed;
	m_bIsStartInterpolation = true;
}

HRESULT CPCWeapon::Ready_GameObject(wstring wstrMeshTag,
									const _vec3& vScale, 
									const _vec3& vAngle, 
									const _vec3& vPos,
									Engine::HIERARCHY_DESC* pHierarchyDesc,
									_matrix* pParentMatrix,
									const _rgba& vEmissiveColor)
{
	Engine::FAILED_CHECK_RETURN(Engine::CGameObject::Ready_GameObject(true, false, true), E_FAIL);
	Engine::FAILED_CHECK_RETURN(Add_Component(wstrMeshTag), E_FAIL);
	SetUp_WeaponType();

	m_wstrMeshTag		  = wstrMeshTag;
	m_pTransCom->m_vScale = vScale;
	m_pTransCom->m_vAngle = vAngle;
	m_pTransCom->m_vPos	  = vPos;
	m_pHierarchyDesc      = pHierarchyDesc;
	m_pParentMatrix       = pParentMatrix;
	m_vEmissiveColor      = vEmissiveColor;

	// PipelineState.
	m_iMeshPipelineStatePass   = 7;
	m_iShadowPipelineStatePass = 0;
	Engine::FAILED_CHECK_RETURN(m_pShaderCom->Set_PipelineStatePass(m_iMeshPipelineStatePass), E_FAIL);
	Engine::FAILED_CHECK_RETURN(m_pShadowCom->Set_PipelineStatePass(m_iShadowPipelineStatePass), E_FAIL);

	return S_OK;
}

HRESULT CPCWeapon::LateInit_GameObject()
{
	// SetUp Shader ConstantBuffer
	m_pShaderCom->SetUp_ShaderConstantBuffer((_uint)(m_pMeshCom->Get_DiffTexture().size()));
	m_pShadowCom->SetUp_ShaderConstantBuffer((_uint)(m_pMeshCom->Get_DiffTexture().size()));

	return S_OK;
}

_int CPCWeapon::Update_GameObject(const _float& fTimeDelta)
{
	if (m_bIsDead)
		return DEAD_OBJ;

	if (fTimeDelta > TIME_OFFSET)
		return NO_EVENT;

	SetUp_Dissolve(fTimeDelta);

	/*__________________________________________________________________________________________________________
	[ Renderer - Add Render Group ]
	____________________________________________________________________________________________________________*/
	Engine::FAILED_CHECK_RETURN(m_pRenderer->Add_Renderer(Engine::CRenderer::RENDER_NONALPHA, this), -1);

	/*____________________________________________________________________
	TransCom - Update WorldMatrix.
	______________________________________________________________________*/
	Engine::CGameObject::Update_GameObject(fTimeDelta);

	if (nullptr != m_pHierarchyDesc && nullptr != m_pParentMatrix)
	{
		m_matBoneFinalTransform = (m_pHierarchyDesc->matScale * m_pHierarchyDesc->matRotate * m_pHierarchyDesc->matTrans) * m_pHierarchyDesc->matGlobalTransform;
		m_pTransCom->m_matWorld *= m_matBoneFinalTransform * (*m_pParentMatrix);
	}

	m_pBoundingBoxCom->Update_Component(fTimeDelta);

	return NO_EVENT;
}

_int CPCWeapon::LateUpdate_GameObject(const _float& fTimeDelta)
{
	Set_ConstantTable();

	return NO_EVENT;
}

void CPCWeapon::Render_GameObject(const _float& fTimeDelta, 
								  ID3D12GraphicsCommandList* pCommandList, 
								  const _int& iContextIdx)
{
	///*__________________________________________________________________________________________________________
	//[ Add Instance ]
	//____________________________________________________________________________________________________________*/
	//m_pShaderMeshInstancing->Add_Instance(iContextIdx, m_wstrMeshTag, m_iMeshPipelineStatePass);
	//_uint iInstanceIdx = m_pShaderMeshInstancing->Get_InstanceCount(iContextIdx, m_wstrMeshTag, m_iMeshPipelineStatePass) - 1;

	//Set_ConstantTable(iContextIdx, iInstanceIdx);

	Set_ConstantTable();
	m_pMeshCom->Render_StaticMesh(pCommandList, iContextIdx, m_pShaderCom);
}

void CPCWeapon::Render_ShadowDepth(const _float& fTimeDelta, 
								   ID3D12GraphicsCommandList* pCommandList, 
								   const _int& iContextIdx)
{
	if (m_bIsRenderShadow)
	{
		///*__________________________________________________________________________________________________________
		//[ Add Instance ]
		//____________________________________________________________________________________________________________*/
		//m_pShaderShadowInstancing->Add_Instance(iContextIdx, m_wstrMeshTag, m_iShadowPipelineStatePass);
		//_uint iInstanceIdx = m_pShaderShadowInstancing->Get_InstanceCount(iContextIdx, m_wstrMeshTag, m_iShadowPipelineStatePass) - 1;

		//Set_ConstantTableShadowDepth(iContextIdx, iInstanceIdx);

		Set_ConstantTableShadowDepth();
		m_pMeshCom->Render_StaticMeshShadowDepth(pCommandList, iContextIdx, m_pShadowCom);
	}
}

HRESULT CPCWeapon::Add_Component(wstring wstrMeshTag)
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

	// Shadow
	m_pShadowCom = static_cast<Engine::CShaderShadow*>(m_pComponentMgr->Clone_Component(L"ShaderShadow", Engine::COMPONENTID::ID_STATIC));
	Engine::NULL_CHECK_RETURN(m_pShadowCom, E_FAIL);
	m_pShadowCom->AddRef();
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Shadow", m_pShadowCom);

	return S_OK;
}

void CPCWeapon::SetUp_WeaponType()
{
	// WeaponTwoHand
	if (L"Twohand19_A_SM" == m_wstrMeshTag)
		m_chWeaponType = Twohand19_A_SM;
	else if (L"TwoHand27_SM" == m_wstrMeshTag)
		m_chWeaponType = TwoHand27_SM;
	else if (L"TwoHand29_SM" == m_wstrMeshTag)
		m_chWeaponType = TwoHand29_SM;
	else if (L"TwoHand31_SM" == m_wstrMeshTag)
		m_chWeaponType = TwoHand31_SM;
	else if (L"TwoHand33_B_SM" == m_wstrMeshTag)
		m_chWeaponType = TwoHand33_B_SM;
}

void CPCWeapon::Set_ConstantTable()
{
	/*__________________________________________________________________________________________________________
	[ Set ConstantBuffer Data ]
	____________________________________________________________________________________________________________*/
	Engine::SHADOW_DESC tShadowDesc = CShadowLightMgr::Get_Instance()->Get_ShadowDesc();

	Engine::CB_SHADER_MESH tCB_ShaderMesh;
	ZeroMemory(&tCB_ShaderMesh, sizeof(Engine::CB_SHADER_MESH));
	tCB_ShaderMesh.matWorld       = Engine::CShader::Compute_MatrixTranspose(m_pTransCom->m_matWorld);
	tCB_ShaderMesh.matLightView   = Engine::CShader::Compute_MatrixTranspose(tShadowDesc.matLightView);
	tCB_ShaderMesh.matLightProj   = Engine::CShader::Compute_MatrixTranspose(tShadowDesc.matLightProj);
	tCB_ShaderMesh.vLightPos      = tShadowDesc.vLightPosition;
	tCB_ShaderMesh.fLightPorjFar  = tShadowDesc.fLightPorjFar;
	tCB_ShaderMesh.fDissolve	  = m_fDissolve;
	tCB_ShaderMesh.vEmissiveColor = m_vEmissiveColor;

	m_pShaderCom->Get_UploadBuffer_ShaderMesh()->CopyData(0, tCB_ShaderMesh);
}

void CPCWeapon::Set_ConstantTableShadowDepth()
{
	/*__________________________________________________________________________________________________________
	[ Set ConstantBuffer Data ]
	____________________________________________________________________________________________________________*/
	Engine::SHADOW_DESC tShadowDesc = CShadowLightMgr::Get_Instance()->Get_ShadowDesc();

	Engine::CB_SHADER_SHADOW tCB_ShaderShadow;
	ZeroMemory(&tCB_ShaderShadow, sizeof(Engine::CB_SHADER_SHADOW));
	tCB_ShaderShadow.matWorld = Engine::CShader::Compute_MatrixTranspose(m_pTransCom->m_matWorld);
	tCB_ShaderShadow.matView  = Engine::CShader::Compute_MatrixTranspose(tShadowDesc.matLightView);
	tCB_ShaderShadow.matProj  = Engine::CShader::Compute_MatrixTranspose(tShadowDesc.matLightProj);
	tCB_ShaderShadow.fProjFar = tShadowDesc.fLightPorjFar;

	m_pShadowCom->Get_UploadBuffer_ShaderShadow()->CopyData(0, tCB_ShaderShadow);
}

void CPCWeapon::Set_ConstantTable(const _int& iContextIdx, const _int& iInstanceIdx)
{
	/*__________________________________________________________________________________________________________
	[ Set ShaderResource Data ]
	____________________________________________________________________________________________________________*/
	Engine::SHADOW_DESC tShadowDesc = CShadowLightMgr::Get_Instance()->Get_ShadowDesc();

	Engine::CB_SHADER_MESH_INSTANCEING tCB_ShaderMesh;
	ZeroMemory(&tCB_ShaderMesh, sizeof(Engine::CB_SHADER_MESH_INSTANCEING));
	tCB_ShaderMesh.matWorld      = Engine::CShader::Compute_MatrixTranspose(m_pTransCom->m_matWorld);
	tCB_ShaderMesh.matLightView  = Engine::CShader::Compute_MatrixTranspose(tShadowDesc.matLightView);
	tCB_ShaderMesh.matLightProj  = Engine::CShader::Compute_MatrixTranspose(tShadowDesc.matLightProj);
	tCB_ShaderMesh.vLightPos     = tShadowDesc.vLightPosition;
	tCB_ShaderMesh.fLightPorjFar = tShadowDesc.fLightPorjFar;
	tCB_ShaderMesh.fDissolve     = m_fDissolve;


	m_pShaderMeshInstancing->Get_UploadBuffer_ShaderMesh(iContextIdx, m_wstrMeshTag, m_iMeshPipelineStatePass)->CopyData(iInstanceIdx, tCB_ShaderMesh);
}

void CPCWeapon::Set_ConstantTableShadowDepth(const _int& iContextIdx, const _int& iInstanceIdx)
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

void CPCWeapon::SetUp_Dissolve(const _float& fTimeDelta)
{
	m_fLinearRatio += m_fDissolveSpeed * fTimeDelta;

	if (m_fLinearRatio >= 1.0f)
	{
		m_fLinearRatio = 1.0f;
		m_bIsStartInterpolation = false;
	}
	else if (m_fLinearRatio <= 0.0f)
	{
		m_fLinearRatio = 0.0f;
		m_bIsStartInterpolation = false;
	}

	m_fDissolve = m_fMinDissolve * (1.0f - m_fLinearRatio) + m_fMaxDissolve * m_fLinearRatio;
}

void CPCWeapon::Free()
{
	Engine::CGameObject::Free();
	Engine::Safe_Release(m_pMeshCom);
	Engine::Safe_Release(m_pShaderCom);
	Engine::Safe_Release(m_pShadowCom);
}
