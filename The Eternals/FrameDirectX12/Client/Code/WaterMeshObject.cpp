#include "stdafx.h"
#include "WaterMeshObject.h"
#include "GraphicDevice.h"
#include "DirectInput.h"
#include "ObjectMgr.h"
#include "LightMgr.h"
#include "DynamicCamera.h"
#include "RenderTarget.h"
#include "TimeMgr.h"
#include "DescriptorHeapMgr.h"
CWaterMeshObject::CWaterMeshObject(ID3D12Device * pGraphicDevice, ID3D12GraphicsCommandList * pCommandList)
	: Engine::CGameObject(pGraphicDevice, pCommandList)
	, m_pShaderShadowInstancing(Engine::CShaderShadowInstancing::Get_Instance())
	, m_pShaderMeshInstancing(Engine::CShaderMeshInstancing::Get_Instance())
{
}


HRESULT CWaterMeshObject::Ready_GameObject(wstring wstrMeshTag,
											 const _vec3 & vScale,
											 const _vec3 & vAngle, 
											 const _vec3 & vPos)
{
	Engine::FAILED_CHECK_RETURN(Engine::CGameObject::Ready_GameObject(), E_FAIL);
	Engine::FAILED_CHECK_RETURN(Add_Component(wstrMeshTag), E_FAIL);

	m_wstrMeshTag = wstrMeshTag;
	m_pTransCom->m_vScale	= vScale;
	m_pTransCom->m_vAngle	= vAngle;
	m_pTransCom->m_vPos		= vPos;

	// PipelineState.
	m_iMeshPipelineStatePass	= 2;
	m_iShadowPipelineStatePass	= 0;

	return S_OK;
}

HRESULT CWaterMeshObject::LateInit_GameObject()
{
	m_pShaderCom->SetUp_ShaderConstantBuffer();
	Engine::CTexture* pTexture = static_cast<Engine::CTexture*>(m_pComponentMgr->Clone_Component(L"WaterNormal", Engine::COMPONENTID::ID_STATIC));
	SetUp_DescriptorHeap(pTexture->Get_Texture(), m_pRenderer->Get_TargetShadowDepth()->Get_TargetTexture());

	m_fPatternMapDeltatime = 123.f;
	return S_OK;
}

_int CWaterMeshObject::Update_GameObject(const _float & fTimeDelta)
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

_int CWaterMeshObject::LateUpdate_GameObject(const _float & fTimeDelta)
{
	Engine::NULL_CHECK_RETURN(m_pRenderer, -1);


	return NO_EVENT;
}

void CWaterMeshObject::Render_GameObject(const _float& fTimeDelta,
										   ID3D12GraphicsCommandList * pCommandList,
										   const _int& iContextIdx)
{
	///*__________________________________________________________________________________________________________
	//[ Add Instance ]
	//____________________________________________________________________________________________________________*/
	//m_pShaderMeshInstancing->Add_Instance(iContextIdx, m_wstrMeshTag, m_iMeshPipelineStatePass);
	//_uint iInstanceIdx = m_pShaderMeshInstancing->Get_InstanceCount(iContextIdx, m_wstrMeshTag, m_iMeshPipelineStatePass) - 1;

	//Set_ConstantTable(iContextIdx, iInstanceIdx);

	// ID3D12DescriptorHeap* pDescriptorHeap= Engine::CDescriptorHeapMgr::Get_Instance()->Find_DescriptorHeap(L"WaterNormal");

	Set_ConstantTable();
	m_fPatternMapDeltatime+=(Engine::CTimerMgr::Get_Instance()->Get_TimeDelta(L"Timer_TimeDelta")) * 15.f;
	if (m_fPatternMapDeltatime > 138.f)
		m_fPatternMapDeltatime = 123.f;
	m_fNormalMapDeltatime += (Engine::CTimerMgr::Get_Instance()->Get_TimeDelta(L"Timer_TimeDelta")) * 15.f;
	if (m_fNormalMapDeltatime > 120.f)
		m_fNormalMapDeltatime = 0.f;
	m_pMeshCom->Render_WaterMesh(pCommandList, iContextIdx, m_pShaderCom, m_pDescriptorHeaps, (_int)m_fNormalMapDeltatime, (_int)m_fPatternMapDeltatime);
}

void CWaterMeshObject::Render_ShadowDepth(const _float& fTimeDelta, 
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

HRESULT CWaterMeshObject::Add_Component(wstring wstrMeshTag)
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
	Engine::FAILED_CHECK_RETURN(m_pShaderCom->Set_PipelineStatePass(6), E_FAIL);
	m_pShaderCom->AddRef();
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Shader", m_pShaderCom);


	return S_OK;
}

void CWaterMeshObject::Set_ConstantTable()
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

	m_fDeltaTime += (Engine::CTimerMgr::Get_Instance()->Get_TimeDelta(L"Timer_TimeDelta")) * 0.005f;
	tCB_ShaderMesh.fOffset1 = m_fDeltaTime;
	m_pShaderCom->Get_UploadBuffer_ShaderMesh()->CopyData(0, tCB_ShaderMesh);


	if (m_fDeltaTime > 1.f)
		m_fDeltaTime = 0.f;
}

void CWaterMeshObject::Set_ConstantTableShadowDepth()
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

void CWaterMeshObject::Set_ConstantTable(const _int& iContextIdx, const _int& iInstanceIdx)
{
	/*__________________________________________________________________________________________________________
	[ Set ShaderResource Data ]
	____________________________________________________________________________________________________________*/
	Engine::SHADOW_DESC tShadowDesc = CShadowLightMgr::Get_Instance()->Get_ShadowDesc();

	m_fDeltatime += (Engine::CTimerMgr::Get_Instance()->Get_TimeDelta(L"Timer_TimeDelta")) * 0.1f;
	Engine::CB_SHADER_MESH tCB_ShaderMesh;
	ZeroMemory(&tCB_ShaderMesh, sizeof(Engine::CB_SHADER_MESH));
	tCB_ShaderMesh.matWorld      = Engine::CShader::Compute_MatrixTranspose(m_pTransCom->m_matWorld);
	tCB_ShaderMesh.matLightView  = Engine::CShader::Compute_MatrixTranspose(tShadowDesc.matLightView);
	tCB_ShaderMesh.matLightProj  = Engine::CShader::Compute_MatrixTranspose(tShadowDesc.matLightProj);
	tCB_ShaderMesh.vLightPos     = tShadowDesc.vLightPosition;
	tCB_ShaderMesh.fLightPorjFar = tShadowDesc.fLightPorjFar;
	tCB_ShaderMesh.fOffset1 = m_fDeltatime;
	m_pShaderMeshInstancing->Get_UploadBuffer_ShaderMesh(iContextIdx, m_wstrMeshTag, m_iMeshPipelineStatePass)->CopyData(iInstanceIdx, tCB_ShaderMesh);
}

void CWaterMeshObject::Set_ConstantTableShadowDepth(const _int& iContextIdx, const _int& iInstanceIdx)
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

HRESULT CWaterMeshObject::SetUp_DescriptorHeap(vector<ComPtr<ID3D12Resource>> vecTexture, vector<ComPtr<ID3D12Resource>> vecShadowDepth)
{
	_uint m_uiTexSize = vecTexture.size() + vecShadowDepth.size();

	D3D12_DESCRIPTOR_HEAP_DESC SRV_HeapDesc = {};
	SRV_HeapDesc.NumDescriptors             = m_uiTexSize;	// 텍스처의 개수 만큼 설정.
	SRV_HeapDesc.Type                       = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	SRV_HeapDesc.Flags                      = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

	Engine::FAILED_CHECK_RETURN(m_pGraphicDevice->CreateDescriptorHeap(&SRV_HeapDesc,
																	   IID_PPV_ARGS(&m_pDescriptorHeaps)),
																	   E_FAIL);

	// 힙의 시작을 가리키는 포인터를 얻는다.
	CD3DX12_CPU_DESCRIPTOR_HANDLE SRV_DescriptorHandle(m_pDescriptorHeaps->GetCPUDescriptorHandleForHeapStart());

	for (_uint i = 0; i < m_uiTexSize - vecShadowDepth.size(); ++i)
	{
		D3D12_SHADER_RESOURCE_VIEW_DESC SRV_Desc = {};
		SRV_Desc.Format                        = vecTexture[i]->GetDesc().Format;
		SRV_Desc.ViewDimension                 = D3D12_SRV_DIMENSION_TEXTURE2D;
		SRV_Desc.Shader4ComponentMapping       = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		SRV_Desc.Texture2D.MostDetailedMip     = 0;
		SRV_Desc.Texture2D.MipLevels           = vecTexture[i]->GetDesc().MipLevels;
		SRV_Desc.Texture2D.ResourceMinLODClamp = 0.0f;

		m_pGraphicDevice->CreateShaderResourceView(vecTexture[i].Get(), &SRV_Desc, SRV_DescriptorHandle);

		// 힙의 다음 서술자로 넘어간다.
		SRV_DescriptorHandle.Offset(1, Engine::CGraphicDevice::Get_Instance()->Get_CBV_SRV_UAV_DescriptorSize());
	}

	D3D12_SHADER_RESOURCE_VIEW_DESC SRV_Desc = {};
	SRV_Desc.Format							= vecShadowDepth[0]->GetDesc().Format;
	SRV_Desc.ViewDimension					= D3D12_SRV_DIMENSION_TEXTURE2D;
	SRV_Desc.Shader4ComponentMapping		= D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	SRV_Desc.Texture2D.MostDetailedMip		= 0;
	SRV_Desc.Texture2D.MipLevels			= vecShadowDepth[0]->GetDesc().MipLevels;
	SRV_Desc.Texture2D.ResourceMinLODClamp	= 0.0f;
	m_pGraphicDevice->CreateShaderResourceView(vecShadowDepth[0].Get(), &SRV_Desc, SRV_DescriptorHandle);

	return S_OK;
}


Engine::CGameObject* CWaterMeshObject::Create(ID3D12Device * pGraphicDevice, ID3D12GraphicsCommandList * pCommandList,
												wstring wstrMeshTag, 
												const _vec3 & vScale,
												const _vec3 & vAngle,
												const _vec3 & vPos)
{
	CWaterMeshObject* pInstance = new CWaterMeshObject(pGraphicDevice, pCommandList);

	if (FAILED(pInstance->Ready_GameObject(wstrMeshTag, vScale, vAngle, vPos)))
		Engine::Safe_Release(pInstance);

	//// SetUp InstanceShader and Add Instance Count.
	//Engine::CShaderShadowInstancing::Get_Instance()->SetUp_Instancing(wstrMeshTag);
	//Engine::CShaderShadowInstancing::Get_Instance()->Add_TotalInstanceCount(wstrMeshTag);
	//Engine::CShaderMeshInstancing::Get_Instance()->SetUp_Instancing(wstrMeshTag);
	//Engine::CShaderMeshInstancing::Get_Instance()->Add_TotalInstanceCount(wstrMeshTag);

	return pInstance;
}

void CWaterMeshObject::Free()
{
	Engine::CGameObject::Free();
	Engine::Safe_Release(m_pMeshCom);
	Engine::Safe_Release(m_pDescriptorHeaps);
	Engine::Safe_Release(m_pShaderCom);
}
