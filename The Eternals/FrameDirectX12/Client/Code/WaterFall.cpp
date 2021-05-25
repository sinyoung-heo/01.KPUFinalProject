#include "stdafx.h"
#include "WaterFall.h"
#include "GraphicDevice.h"
#include "DirectInput.h"
#include "ObjectMgr.h"
#include "LightMgr.h"
#include "DynamicCamera.h"
#include "RenderTarget.h"
#include "TimeMgr.h"
#include "DescriptorHeapMgr.h"
CWaterFall::CWaterFall(ID3D12Device * pGraphicDevice, ID3D12GraphicsCommandList * pCommandList)
	: Engine::CGameObject(pGraphicDevice, pCommandList)
	, m_pShaderShadowInstancing(Engine::CShaderShadowInstancing::Get_Instance())
	, m_pShaderMeshInstancing(Engine::CShaderMeshInstancing::Get_Instance())
{
}


HRESULT CWaterFall::Ready_GameObject(wstring wstrMeshTag,
											 const _vec3 & vScale,
											 const _vec3 & vAngle, 
											 const _vec3 & vPos, const _vec3& vPosOffset)
{
	Engine::FAILED_CHECK_RETURN(Engine::CGameObject::Ready_GameObject(true, false, true), E_FAIL);
	Engine::FAILED_CHECK_RETURN(Add_Component(wstrMeshTag), E_FAIL);

	m_wstrMeshTag           = wstrMeshTag;
	m_pTransCom->m_vScale	= vScale;
	m_pTransCom->m_vAngle	= vAngle;
	m_pTransCom->m_vPos     = vPos + vPosOffset;

	// BoundingBox.
	Engine::CGameObject::SetUp_BoundingBox(&(m_pTransCom->m_matWorld),
										   m_pTransCom->m_vScale,
										   m_pMeshCom->Get_CenterPos(),
										   m_pMeshCom->Get_MinVector(),
										   m_pMeshCom->Get_MaxVector());

	// PipelineState.
	m_iMeshPipelineStatePass	= 2;
	m_iShadowPipelineStatePass	= 0;

	return S_OK;
}

HRESULT CWaterFall::LateInit_GameObject()
{
	m_pShaderCom->SetUp_ShaderConstantBuffer();
	Engine::CTexture* pTexture = static_cast<Engine::CTexture*>(m_pComponentMgr->Clone_Component(L"TexWaterFall", Engine::COMPONENTID::ID_STATIC));
	SetUp_DescriptorHeap(pTexture->Get_Texture(), m_pRenderer->Get_TargetShadowDepth()->Get_TargetTexture());
	m_fNormalMapDeltatime  = 4;
	m_fPatternMapDeltatime = 6;
	return S_OK;
}

_int CWaterFall::Update_GameObject(const _float & fTimeDelta)
{
	Engine::FAILED_CHECK_RETURN(Engine::CGameObject::LateInit_GameObject(), E_FAIL);

	if (m_bIsDead)
		return DEAD_OBJ;

	/*__________________________________________________________________________________________________________
	[ Renderer - Add Render Group ]
	____________________________________________________________________________________________________________*/
	if (m_pRenderer->Get_Frustum().Contains(m_pBoundingBoxCom->Get_BoundingInfo()) != DirectX::DISJOINT)
		Engine::FAILED_CHECK_RETURN(m_pRenderer->Add_Renderer(Engine::CRenderer::RENDER_NONALPHA, this), -1);

	/*____________________________________________________________________
	TransCom - Update WorldMatrix.
	______________________________________________________________________*/
	Engine::CGameObject::Update_GameObject(fTimeDelta);

	return NO_EVENT;
}

_int CWaterFall::LateUpdate_GameObject(const _float & fTimeDelta)
{
	Engine::NULL_CHECK_RETURN(m_pRenderer, -1);


	return NO_EVENT;
}

void CWaterFall::Render_GameObject(const _float& fTimeDelta,
										   ID3D12GraphicsCommandList * pCommandList,
										   const _int& iContextIdx)
{

	Set_ConstantTable();
	m_pMeshCom->Render_WaterMesh(pCommandList, iContextIdx, m_pShaderCom, m_pDescriptorHeaps, (_int)m_fNormalMapDeltatime, (_int)m_fPatternMapDeltatime);
}

void CWaterFall::Render_ShadowDepth(const _float& fTimeDelta, 
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

HRESULT CWaterFall::Add_Component(wstring wstrMeshTag)
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
	Engine::FAILED_CHECK_RETURN(m_pShaderCom->Set_PipelineStatePass(8), E_FAIL);
	m_pShaderCom->AddRef();
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Shader", m_pShaderCom);


	return S_OK;
}

void CWaterFall::Set_ConstantTable()
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
	tCB_ShaderMesh.vEmissiveColor = _rgba(1.0f, 1.0f, 1.0f, 1.0f);

	m_fDeltaTime += (Engine::CTimerMgr::Get_Instance()->Get_TimeDelta(L"Timer_TimeDelta"))*0.3f;
	m_fDeltatime2 += (Engine::CTimerMgr::Get_Instance()->Get_TimeDelta(L"Timer_TimeDelta"));
	m_fDeltatime3 += (Engine::CTimerMgr::Get_Instance()->Get_TimeDelta(L"Timer_TimeDelta"));
	tCB_ShaderMesh.fOffset1 = m_fDeltaTime;
	tCB_ShaderMesh.fOffset2 = m_fDeltatime2;
	tCB_ShaderMesh.fDissolve = sin(m_fDeltatime3);
	m_pShaderCom->Get_UploadBuffer_ShaderMesh()->CopyData(0, tCB_ShaderMesh);


	
}

void CWaterFall::Set_ConstantTableShadowDepth()
{
}

void CWaterFall::Set_ConstantTable(const _int& iContextIdx, const _int& iInstanceIdx)
{
	/*__________________________________________________________________________________________________________
	[ Set ShaderResource Data ]
	____________________________________________________________________________________________________________*/
	Engine::SHADOW_DESC tShadowDesc = CShadowLightMgr::Get_Instance()->Get_ShadowDesc();

	m_fDeltatime += (Engine::CTimerMgr::Get_Instance()->Get_TimeDelta(L"Timer_TimeDelta")) * 0.1f;
	Engine::CB_SHADER_MESH_INSTANCEING tCB_ShaderMesh;
	ZeroMemory(&tCB_ShaderMesh, sizeof(Engine::CB_SHADER_MESH_INSTANCEING));
	tCB_ShaderMesh.matWorld      = Engine::CShader::Compute_MatrixTranspose(m_pTransCom->m_matWorld);
	tCB_ShaderMesh.matLightView  = Engine::CShader::Compute_MatrixTranspose(tShadowDesc.matLightView);
	tCB_ShaderMesh.matLightProj  = Engine::CShader::Compute_MatrixTranspose(tShadowDesc.matLightProj);
	tCB_ShaderMesh.vLightPos     = tShadowDesc.vLightPosition;
	tCB_ShaderMesh.fLightPorjFar = tShadowDesc.fLightPorjFar;
	tCB_ShaderMesh.fOffset1 = m_fDeltatime;
	m_pShaderMeshInstancing->Get_UploadBuffer_ShaderMesh(iContextIdx, m_wstrMeshTag, m_iMeshPipelineStatePass)->CopyData(iInstanceIdx, tCB_ShaderMesh);
}

void CWaterFall::Set_ConstantTableShadowDepth(const _int& iContextIdx, const _int& iInstanceIdx)
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

HRESULT CWaterFall::SetUp_DescriptorHeap(vector<ComPtr<ID3D12Resource>> vecTexture, vector<ComPtr<ID3D12Resource>> vecShadowDepth)
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


Engine::CGameObject* CWaterFall::Create(ID3D12Device * pGraphicDevice, ID3D12GraphicsCommandList * pCommandList,
												wstring wstrMeshTag, 
												const _vec3 & vScale,
												const _vec3 & vAngle,
												const _vec3 & vPos, const _vec3& vPosOffset)
{
	CWaterFall* pInstance = new CWaterFall(pGraphicDevice, pCommandList);

	if (FAILED(pInstance->Ready_GameObject(wstrMeshTag, vScale, vAngle, vPos, vPosOffset)))
		Engine::Safe_Release(pInstance);

	return pInstance;
}

void CWaterFall::Free()
{
	Engine::CGameObject::Free();
	Engine::Safe_Release(m_pMeshCom);
	Engine::Safe_Release(m_pDescriptorHeaps);
	Engine::Safe_Release(m_pShaderCom);
}
