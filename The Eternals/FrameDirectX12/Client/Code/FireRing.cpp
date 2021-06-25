#include "stdafx.h"
#include "FireRing.h"
#include "GraphicDevice.h"
#include "DirectInput.h"
#include "ObjectMgr.h"
#include "LightMgr.h"
#include "DynamicCamera.h"
#include "RenderTarget.h"
#include "TimeMgr.h"
#include "DescriptorHeapMgr.h"
CFireRing::CFireRing(ID3D12Device * pGraphicDevice, ID3D12GraphicsCommandList * pCommandList)
	: Engine::CGameObject(pGraphicDevice, pCommandList)
{
}


HRESULT CFireRing::Ready_GameObject(wstring wstrMeshTag,
											 const _vec3 & vScale,
											 const _vec3 & vAngle, 
											 const _vec3 & vPos, 
											const FRAME& tFrame)
{
	Engine::FAILED_CHECK_RETURN(Engine::CGameObject::Ready_GameObject(), E_FAIL);
	Engine::FAILED_CHECK_RETURN(Add_Component(wstrMeshTag), E_FAIL);

	m_wstrMeshTag = wstrMeshTag;
	m_pTransCom->m_vScale	= vScale;
	m_pTransCom->m_vAngle	= vAngle;
	m_pTransCom->m_vPos		= vPos;
	m_tFrame = tFrame;
	return S_OK;
}

HRESULT CFireRing::LateInit_GameObject()
{

	m_pShaderCom->SetUp_ShaderConstantBuffer((_uint)(m_pMeshCom->Get_DiffTexture().size()));
	m_pCrossFilterShaderCom->SetUp_ShaderConstantBuffer((_uint)(m_pMeshCom->Get_DiffTexture().size()));

	Engine::CTexture* pTexture = static_cast<Engine::CTexture*>(m_pComponentMgr->Clone_Component(L"EffectPublic", Engine::COMPONENTID::ID_STATIC));
	SetUp_DescriptorHeap(pTexture->Get_Texture(), m_pRenderer->Get_TargetShadowDepth()->Get_TargetTexture());


	m_uiDiffuse =8;
	m_uiNormal = 11;//NormIdx
	m_uiSpec = 11;//SpecIdx
	return S_OK;	
}

_int CFireRing::Update_GameObject(const _float & fTimeDelta)
{
	Engine::FAILED_CHECK_RETURN(Engine::CGameObject::LateInit_GameObject(), E_FAIL);

	if (m_bIsDead)
		return DEAD_OBJ;

	m_pTransCom->m_vAngle.y += 540.f*fTimeDelta;
	m_pTransCom->m_vScale = _vec3(sinf(XMConvertToRadians(m_fDeltaDegree* 0.25)*0.8f));
	m_tFrame.fCurFrame += fTimeDelta * m_tFrame.fFrameSpeed;

	// Sprite X��
	if (m_tFrame.fCurFrame > m_tFrame.fFrameCnt)
	{
		m_tFrame.fCurFrame = 0.0f;
		m_tFrame.fCurScene += 1.0f;
	}

	// Sprite Y��
	if (m_tFrame.fCurScene >= m_tFrame.fSceneCnt)
	{
		m_tFrame.fCurScene = 0.0f;
	}
	/*__________________________________________________________________________________________________________
	[ Renderer - Add Render Group ]
	____________________________________________________________________________________________________________*/

	Engine::FAILED_CHECK_RETURN(m_pRenderer->Add_Renderer(Engine::CRenderer::RENDER_MAGICCIRCLE, this), -1);
	

	_vec4 vPosInWorld = _vec4(m_pTransCom->m_vPos, 1.0f);
	Engine::CGameObject::Compute_ViewZ(vPosInWorld);
	/*____________________________________________________________________
	TransCom - Update WorldMatrix.
	______________________________________________________________________*/
	Engine::CGameObject::Update_GameObject(fTimeDelta);

	return NO_EVENT;
}

_int CFireRing::LateUpdate_GameObject(const _float & fTimeDelta)
{
	Engine::NULL_CHECK_RETURN(m_pRenderer, -1);

	return NO_EVENT;
}


void CFireRing::Render_GameObject(const _float& fTimeDelta)
{
	Set_ConstantTable();
	m_pMeshCom->Render_MagicCircleMesh(m_pShaderCom, m_pDescriptorHeaps, m_uiDiffuse, m_uiNormal,m_uiSpec
		,0,4);
}
void CFireRing::Render_CrossFilterGameObject(const _float& fTimeDelta)
{
	Set_ConstantTable();
	m_pMeshCom->Render_StaticMesh(m_pCrossFilterShaderCom);
}
HRESULT CFireRing::Add_Component(wstring wstrMeshTag)
{
	Engine::NULL_CHECK_RETURN(m_pComponentMgr, E_FAIL);

	// Mesh
	m_pMeshCom = static_cast<Engine::CMesh*>(m_pComponentMgr->Clone_Component(wstrMeshTag.c_str(), Engine::COMPONENTID::ID_STATIC));
	Engine::NULL_CHECK_RETURN(m_pMeshCom, E_FAIL);
	m_pMeshCom->AddRef();
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Mesh", m_pMeshCom);


	// Shader
	m_pShaderCom = static_cast<Engine::CShaderMeshEffect*>(m_pComponentMgr->Clone_Component(L"ShaderMeshEffect", Engine::COMPONENTID::ID_STATIC));
	Engine::NULL_CHECK_RETURN(m_pShaderCom, E_FAIL);
	Engine::FAILED_CHECK_RETURN(m_pShaderCom->Set_PipelineStatePass(8), E_FAIL);
	m_pShaderCom->AddRef();
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Shader", m_pShaderCom);

	m_pCrossFilterShaderCom = static_cast<Engine::CShaderMesh*>(m_pComponentMgr->Clone_Component(L"ShaderMesh", Engine::COMPONENTID::ID_STATIC));
	Engine::NULL_CHECK_RETURN(m_pCrossFilterShaderCom, E_FAIL);
	m_pCrossFilterShaderCom->AddRef();
	Engine::FAILED_CHECK_RETURN(m_pCrossFilterShaderCom->Set_PipelineStatePass(3), E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Shader2", m_pCrossFilterShaderCom);

	return S_OK;
}

void CFireRing::Set_ConstantTable()
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

	//tCB_ShaderTexture.fFrameCnt = m_tFrame.fFrameCnt;
	//tCB_ShaderTexture.fCurFrame = (_float)(_int)m_tFrame.fCurFrame;
	//tCB_ShaderTexture.fSceneCnt = m_tFrame.fSceneCnt;
	//tCB_ShaderTexture.fCurScene = (_int)m_tFrame.fCurScene;

	//g_fOffSet1 = AnimationUV X  // g_fOffSet3 = Degree Delta��
	//g_fOffSet5 = Detail��
	m_fDeltaTime += Engine::CTimerMgr::Get_Instance()->Get_TimeDelta(L"Timer_TimeDelta") * 0.5f;
	m_fDeltaDegree += Engine::CTimerMgr::Get_Instance()->Get_TimeDelta(L"Timer_TimeDelta") * 180.f;
	tCB_ShaderMesh.fOffset6 = m_fDeltaDegree / 180.f;
	if (m_fDeltaDegree > 360.f)
		m_bIsDead = true;
	tCB_ShaderMesh.fOffset1 = -m_fDeltaTime;
	tCB_ShaderMesh.fOffset3 = m_fDeltaDegree;
	/*
	tCB_ShaderMesh.vEmissiveColor = _rgba(sinf(XMConvertToRadians(m_fDeltaDegree * 0.25)), sinf(XMConvertToRadians(m_fDeltaDegree* 0.25)*0.5f)
		, 0, 0.f);*/
	tCB_ShaderMesh.fOffset5 = 1.f;
	tCB_ShaderMesh.vAfterImgColor.x = m_tFrame.fFrameCnt;
	tCB_ShaderMesh.vAfterImgColor.y = (_float)(_int)m_tFrame.fCurFrame;
	tCB_ShaderMesh.vAfterImgColor.z = m_tFrame.fSceneCnt;
	tCB_ShaderMesh.vAfterImgColor.w = (_int)m_tFrame.fCurScene;
	if(m_pShaderCom->Get_UploadBuffer_ShaderMesh()!=nullptr)
		m_pShaderCom->Get_UploadBuffer_ShaderMesh()->CopyData(0, tCB_ShaderMesh);


	
}

void CFireRing::Set_ConstantTableShadowDepth()
{

}


HRESULT CFireRing::SetUp_DescriptorHeap(vector<ComPtr<ID3D12Resource>> vecTexture, vector<ComPtr<ID3D12Resource>> vecShadowDepth)
{
	_uint m_uiTexSize = vecTexture.size() + vecShadowDepth.size();

	D3D12_DESCRIPTOR_HEAP_DESC SRV_HeapDesc = {};
	SRV_HeapDesc.NumDescriptors             = m_uiTexSize;	// �ؽ�ó�� ���� ��ŭ ����.
	SRV_HeapDesc.Type                       = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	SRV_HeapDesc.Flags                      = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

	Engine::FAILED_CHECK_RETURN(m_pGraphicDevice->CreateDescriptorHeap(&SRV_HeapDesc,
																	   IID_PPV_ARGS(&m_pDescriptorHeaps)),
																	   E_FAIL);

	// ���� ������ ����Ű�� �����͸� ��´�.
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

		// ���� ���� �����ڷ� �Ѿ��.
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


Engine::CGameObject* CFireRing::Create(ID3D12Device * pGraphicDevice, ID3D12GraphicsCommandList * pCommandList,
												wstring wstrMeshTag, 
												const _vec3 & vScale,
												const _vec3 & vAngle,
												const _vec3 & vPos, const FRAME& tFrame)
{
	CFireRing* pInstance = new CFireRing(pGraphicDevice, pCommandList);

	if (FAILED(pInstance->Ready_GameObject(wstrMeshTag, vScale, vAngle, vPos, tFrame)))
		Engine::Safe_Release(pInstance);

	return pInstance;
}

void CFireRing::Free()
{
	Engine::CGameObject::Free();
	Engine::Safe_Release(m_pMeshCom);
//	Engine::Safe_Release(m_pDescriptorHeaps);
	Engine::Safe_Release(m_pShaderCom);
	Engine::Safe_Release(m_pCrossFilterShaderCom);
}
