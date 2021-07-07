#include "stdafx.h"
#include "MagicCircle.h"
#include "GraphicDevice.h"
#include "DirectInput.h"
#include "ObjectMgr.h"
#include "LightMgr.h"
#include "DynamicCamera.h"
#include "RenderTarget.h"
#include "TimeMgr.h"
#include "DescriptorHeapMgr.h"
CMagicCircle::CMagicCircle(ID3D12Device * pGraphicDevice, ID3D12GraphicsCommandList * pCommandList)
	: Engine::CGameObject(pGraphicDevice, pCommandList)
{
}


HRESULT CMagicCircle::Ready_GameObject(wstring wstrMeshTag,
											 const _vec3 & vScale,
											 const _vec3 & vAngle, 
											 const _vec3 & vPos, const int& iPipeLineIdx )
{
	m_uiPipeLineIdx = iPipeLineIdx;
	Engine::FAILED_CHECK_RETURN(Engine::CGameObject::Ready_GameObject(), E_FAIL);
	Engine::FAILED_CHECK_RETURN(Add_Component(wstrMeshTag), E_FAIL);

	m_wstrMeshTag = wstrMeshTag;
	m_pTransCom->m_vScale	= vScale;
	m_pTransCom->m_vAngle	= vAngle;
	m_pTransCom->m_vPos		= vPos;

	m_fLimitLifeTime = 5.5f;
	m_fLimitScale = 0.02f;
	return S_OK;
}

HRESULT CMagicCircle::LateInit_GameObject()
{

	m_pShaderCom->SetUp_ShaderConstantBuffer((_uint)(m_pMeshCom->Get_DiffTexture().size()));
	Engine::CTexture* pTexture = static_cast<Engine::CTexture*>(m_pComponentMgr->Clone_Component(L"PublicMagic", Engine::COMPONENTID::ID_STATIC));
	SetUp_DescriptorHeap(pTexture->Get_Texture(), m_pRenderer->Get_TargetShadowDepth()->Get_TargetTexture());


	return S_OK;	
}

_int CMagicCircle::Update_GameObject(const _float & fTimeDelta)
{
	if(m_bisRotate)
		m_pTransCom->m_vAngle.y += 60.f*fTimeDelta;
	if (m_bisFollowPlayer)
	{
		m_pTransCom->m_vPos.x = m_pParentTransform->m_vPos.x;
		m_pTransCom->m_vPos.z = m_pParentTransform->m_vPos.z;
	}

	if (m_fLifeTime< m_fLimitLifeTime &&m_bisScaleAnim && m_pTransCom->m_vScale.x<m_fLimitScale)
		m_pTransCom->m_vScale += _vec3(fTimeDelta* m_fLimitScale*2);
	m_fLifeTime += fTimeDelta;
	if (m_fLifeTime > m_fLimitLifeTime)
	{
		m_pTransCom->m_vScale-= _vec3(fTimeDelta * m_fLimitScale*2);
		if (m_pTransCom->m_vScale.x < 0.00)
			m_bIsReturn = true;
	}
	Engine::FAILED_CHECK_RETURN(Engine::CGameObject::LateInit_GameObject(), E_FAIL);

	if (m_bIsDead)
		return DEAD_OBJ;
	if (m_bIsReturn)
	{
		Return_Instance(CInstancePoolMgr::Get_Instance()->Get_Effect_MagicCircleEffect(), m_uiInstanceIdx);
		return RETURN_OBJ;
	}

	/*__________________________________________________________________________________________________________
	[ Renderer - Add Render Group ]
	____________________________________________________________________________________________________________*/

	Engine::FAILED_CHECK_RETURN(m_pRenderer->Add_Renderer(Engine::CRenderer::RENDER_MAGICCIRCLE, this), -1);
	/*Engine::FAILED_CHECK_RETURN(m_pRenderer->Add_Renderer(Engine::CRenderer::RENDER_ALPHA, this), -1);*/
	/*____________________________________________________________________
	TransCom - Update WorldMatrix.
	______________________________________________________________________*/
	Engine::CGameObject::Update_GameObject(fTimeDelta);

	
	
	_vec4 vPosInWorld = _vec4(m_pTransCom->m_vPos, 1.0f);
	Engine::CGameObject::Compute_ViewZ(vPosInWorld);
	
	//
	m_fAlpha += fTimeDelta;
	m_fDegree += 180.f * fTimeDelta;
	return NO_EVENT;
}

_int CMagicCircle::LateUpdate_GameObject(const _float & fTimeDelta)
{
	Engine::NULL_CHECK_RETURN(m_pRenderer, -1);

	Set_ConstantTable();

	return NO_EVENT;
}


void CMagicCircle::Render_GameObject(const _float& fTimeDelta)
{
	m_pMeshCom->Render_MagicCircleMesh(m_pShaderCom, m_pDescriptorHeaps, m_uiDiffuse, m_uiNormal, m_uiSpec,0,0);
}

HRESULT CMagicCircle::Add_Component(wstring wstrMeshTag)
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
	Engine::FAILED_CHECK_RETURN(m_pShaderCom->Set_PipelineStatePass(m_uiPipeLineIdx), E_FAIL);
	m_pShaderCom->AddRef();
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Shader", m_pShaderCom);

	return S_OK;
}

void CMagicCircle::Set_ConstantTable()
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

	//Offset3 = Degree;
	//Offset1 = Alpha;

	tCB_ShaderMesh.fOffset1 = m_fAlpha;
	tCB_ShaderMesh.fOffset3 = m_fDegree;
	if(m_pShaderCom->Get_UploadBuffer_ShaderMesh()!=nullptr)
		m_pShaderCom->Get_UploadBuffer_ShaderMesh()->CopyData(0, tCB_ShaderMesh);

	
}

void CMagicCircle::Set_ConstantTableShadowDepth()
{

}


HRESULT CMagicCircle::SetUp_DescriptorHeap(vector<ComPtr<ID3D12Resource>> vecTexture, vector<ComPtr<ID3D12Resource>> vecShadowDepth)
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


void CMagicCircle::Set_CreateInfo(const _vec3& vScale, const _vec3& vAngle, const _vec3& vPos, bool bisRotate, bool bisScaleAnim, const Engine::CTransform* ParentTransform, bool bisFollowPlayer)
{
	m_pTransCom->m_vScale = vScale;
	m_pTransCom->m_vAngle = vAngle;
	m_pTransCom->m_vPos = vPos;

	m_bisRotate = bisRotate;
	m_bisScaleAnim = bisScaleAnim;
	m_bisFollowPlayer = bisFollowPlayer;
	m_pParentTransform = ParentTransform;

	m_fDeltatime = 0.f;
	m_fDeltatime2 = 0.f;
	m_fDeltatime3 = 0.f;
	m_fLifeTime = 0.f;

	m_fAlpha = 0.f;
	m_fDegree = 0.f;
}

Engine::CGameObject* CMagicCircle::Create(ID3D12Device * pGraphicDevice, ID3D12GraphicsCommandList * pCommandList,
												wstring wstrMeshTag, 
												const _vec3 & vScale,
												const _vec3 & vAngle,
												const _vec3 & vPos, const int& iPipeLineIdx)
{
	CMagicCircle* pInstance = new CMagicCircle(pGraphicDevice, pCommandList);

	if (FAILED(pInstance->Ready_GameObject(wstrMeshTag, vScale, vAngle, vPos, iPipeLineIdx)))
		Engine::Safe_Release(pInstance);

	return pInstance;
}

CMagicCircle** CMagicCircle::Create_InstancePool(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList, const _uint& uiInstanceCnt)
{
	CMagicCircle** ppInstance = new (CMagicCircle * [uiInstanceCnt]);
	for (_uint i = 0; i < uiInstanceCnt; ++i)
	{
		ppInstance[i] = new CMagicCircle(pGraphicDevice, pCommandList);
		ppInstance[i]->m_uiInstanceIdx = i;
		ppInstance[i]->Ready_GameObject(L"PublicPlane00", _vec3(0.f), _vec3(0.f), _vec3(0.f), 0.f);
	}
	return ppInstance;

}

void CMagicCircle::Free()
{
	Engine::CGameObject::Free();
	Engine::Safe_Release(m_pMeshCom);
	//Engine::Safe_Release(m_pDescriptorHeaps);
	Engine::Safe_Release(m_pShaderCom);
}
