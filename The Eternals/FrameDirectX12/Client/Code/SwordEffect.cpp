#include "stdafx.h"
#include "SwordEffect.h"
#include "GraphicDevice.h"
#include "DirectInput.h"
#include "ObjectMgr.h"
#include "LightMgr.h"
#include "DynamicCamera.h"
#include "RenderTarget.h"
#include "TimeMgr.h"
#include "DescriptorHeapMgr.h"
#include "SnowParticle.h"
#include "EffectMgr.h"
CSwordEffect::CSwordEffect(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList)
	: Engine::CGameObject(pGraphicDevice, pCommandList)
{
}


HRESULT CSwordEffect::Ready_GameObject(wstring wstrMeshTag,
	const _vec3& vScale,
	const _vec3& vAngle,
	const _vec3& vPos)
{
	Engine::FAILED_CHECK_RETURN(Engine::CGameObject::Ready_GameObject(), E_FAIL);
	Engine::FAILED_CHECK_RETURN(Add_Component(wstrMeshTag), E_FAIL);

	m_wstrMeshTag = wstrMeshTag;
	m_pTransCom->m_vScale = vScale;
	m_pTransCom->m_vAngle = vAngle;
	m_pTransCom->m_vPos = vPos;

	return S_OK;
}

HRESULT CSwordEffect::LateInit_GameObject()
{
	m_pShaderCom->SetUp_ShaderConstantBuffer();
	Engine::CTexture* pTexture = static_cast<Engine::CTexture*>(m_pComponentMgr->Clone_Component(L"EffectPublic", Engine::COMPONENTID::ID_STATIC));
	SetUp_DescriptorHeap(pTexture->Get_Texture(), m_pRenderer->Get_TargetShadowDepth()->Get_TargetTexture());

	m_pCrossFilterShaderCom->SetUp_ShaderConstantBuffer((_uint)(m_pMeshCom->Get_DiffTexture().size()));

	return S_OK;
}

_int CSwordEffect::Update_GameObject(const _float& fTimeDelta)
{
	if (m_pTransCom->m_vScale.x < m_fLimitScale)
	{
		m_pTransCom->m_vScale.x += (Engine::CTimerMgr::Get_Instance()->Get_TimeDelta(L"Timer_TimeDelta")) * 0.6f;
		m_pTransCom->m_vScale.y += (Engine::CTimerMgr::Get_Instance()->Get_TimeDelta(L"Timer_TimeDelta")) * 0.6f;
		m_pTransCom->m_vScale.z += (Engine::CTimerMgr::Get_Instance()->Get_TimeDelta(L"Timer_TimeDelta")) * 0.6f;
	}
	Engine::FAILED_CHECK_RETURN(Engine::CGameObject::LateInit_GameObject(), E_FAIL);

	m_fLifeTime += (Engine::CTimerMgr::Get_Instance()->Get_TimeDelta(L"Timer_TimeDelta"));

	if (m_pTransCom->m_vPos.y > 10.f)
	{
		
		m_pTransCom->m_vAngle.y += fTimeDelta * 960.f;
		m_pTransCom->m_vPos.y -= 100.f * fTimeDelta;
	}
	else
	{
		if (!m_bisEffect)
		{
			_vec3 Pos = m_pTransCom->m_vPos;
			Pos.y = 0.2f;
			CEffectMgr::Get_Instance()->Effect_IceDecal(Pos);
			CEffectMgr::Get_Instance()->Effect_IceStorm(Pos,6,2.f);
			m_bisEffect = true;
		}
	}
	if (m_fLifeTime > 6.f)
		m_bIsReturn = true;
	if (m_bIsDead)
		return DEAD_OBJ;

	if (m_bIsReturn)
	{
		//초기화가 필요한 값들
		m_fDeltatime = -1.f;
		m_fDeltatimeVelocity = 0.f;
		m_fDeltatimeVelocity2 = 1.f;
		m_fDeltatime2 = 0.f;
		m_fDeltatime3 = 0.f;
		m_fLifeTime = 0.f;
		m_bisLifeInit = false;
		m_bisEffect = false;

		Return_Instance(CInstancePoolMgr::Get_Instance()->Get_Effect_SwordEffect(), m_uiInstanceIdx);
		return RETURN_OBJ;
	}
	/*__________________________________________________________________________________________________________
	[ Renderer - Add Render Group ]
	____________________________________________________________________________________________________________*/
	Engine::FAILED_CHECK_RETURN(m_pRenderer->Add_Renderer(Engine::CRenderer::RENDER_MAGICCIRCLE, this), -1);
	Engine::FAILED_CHECK_RETURN(m_pRenderer->Add_Renderer(Engine::CRenderer::RENDER_CROSSFILTER, this), -1);

	/*____________________________________________________________________
	TransCom - Update WorldMatrix.
	______________________________________________________________________*/


	_vec4 vPosInWorld = _vec4(m_pTransCom->m_vPos, 1.0f);
	Engine::CGameObject::Compute_ViewZ(vPosInWorld);

	Engine::CGameObject::Update_GameObject(fTimeDelta);

	return NO_EVENT;
}

_int CSwordEffect::LateUpdate_GameObject(const _float& fTimeDelta)
{
	Engine::NULL_CHECK_RETURN(m_pRenderer, -1);


	return NO_EVENT;
}


void CSwordEffect::Render_GameObject(const _float& fTimeDelta)
{
	Set_ConstantTable();
	m_pMeshCom->Render_MagicCircleMesh(m_pShaderCom, m_pDescriptorHeaps, 0, 2, 3, 0, 0);
}
void CSwordEffect::Render_CrossFilterGameObject(const _float& fTimeDelta)
{
	Set_ConstantTable();
	m_pMeshCom->Render_StaticMesh(m_pCrossFilterShaderCom);
}
HRESULT CSwordEffect::Add_Component(wstring wstrMeshTag)
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
	Engine::FAILED_CHECK_RETURN(m_pShaderCom->Set_PipelineStatePass(3), E_FAIL);
	m_pShaderCom->AddRef();
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Shader", m_pShaderCom);

	m_pCrossFilterShaderCom = static_cast<Engine::CShaderMesh*>(m_pComponentMgr->Clone_Component(L"ShaderMesh", Engine::COMPONENTID::ID_STATIC));
	Engine::NULL_CHECK_RETURN(m_pCrossFilterShaderCom, E_FAIL);
	m_pCrossFilterShaderCom->AddRef();
	Engine::FAILED_CHECK_RETURN(m_pCrossFilterShaderCom->Set_PipelineStatePass(3), E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Shader2", m_pCrossFilterShaderCom);
	return S_OK;
}

void CSwordEffect::Set_ConstantTable()
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

	m_fDeltaTime += (Engine::CTimerMgr::Get_Instance()->Get_TimeDelta(L"Timer_TimeDelta")) * 0.5f * m_fDeltatimeVelocity;

	m_fDeltatime3 += (Engine::CTimerMgr::Get_Instance()->Get_TimeDelta(L"Timer_TimeDelta")) * 0.5f * m_fDeltatimeVelocity2;
	tCB_ShaderMesh.fOffset1 = sin(m_fDeltaTime);//번짐효과
	tCB_ShaderMesh.fOffset2 = m_fDeltatime2;
	tCB_ShaderMesh.fOffset3 = m_fDeltatime3;

	if (m_fLifeTime > 4.f)
	{
		m_fDeltatime2 += (Engine::CTimerMgr::Get_Instance()->Get_TimeDelta(L"Timer_TimeDelta")) * 0.5f;
		tCB_ShaderMesh.fDissolve = m_fDeltatime2;
		if (!m_bisLifeInit)
		{
			m_bisLifeInit = true;
			m_fDeltatimeVelocity2 = 3;

			CEffectMgr::Get_Instance()->Effect_Particle(m_pTransCom->m_vPos, 20, L"Lighting1", _vec3(0.1f));
		
		}
	}
	m_pShaderCom->Get_UploadBuffer_ShaderMesh()->CopyData(0, tCB_ShaderMesh);


	tCB_ShaderMesh.vEmissiveColor.x = 135.f / 255.f;
	tCB_ShaderMesh.vEmissiveColor.y = 150.f / 255.f;
	tCB_ShaderMesh.vEmissiveColor.z = 220.f / 255.f;
	tCB_ShaderMesh.vEmissiveColor.w = 1.f;
	m_pCrossFilterShaderCom->Get_UploadBuffer_ShaderMesh()->CopyData(0, tCB_ShaderMesh);
	if (m_fDeltaTime > 1.f)
		m_fDeltatimeVelocity = -1.f;
	else if (m_fDeltaTime < 0.f)
		m_fDeltatimeVelocity = 1.f;

	if (m_fDeltatime3 > 1.f)
		m_fDeltatimeVelocity2 = -1.f;
	else if (m_fDeltatime3 < 0.f)
	{
		m_fDeltatime3 = 0.0f;
		m_fDeltatimeVelocity2 = 0.f;
	}
}
HRESULT CSwordEffect::SetUp_DescriptorHeap(vector<ComPtr<ID3D12Resource>> vecTexture, vector<ComPtr<ID3D12Resource>> vecShadowDepth)
{
	_uint m_uiTexSize = vecTexture.size() + vecShadowDepth.size();
	D3D12_DESCRIPTOR_HEAP_DESC SRV_HeapDesc = {};
	SRV_HeapDesc.NumDescriptors = m_uiTexSize;	// 텍스처의 개수 만큼 설정.
	SRV_HeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	SRV_HeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

	Engine::FAILED_CHECK_RETURN(m_pGraphicDevice->CreateDescriptorHeap(&SRV_HeapDesc,
		IID_PPV_ARGS(&m_pDescriptorHeaps)),
		E_FAIL);
	// 힙의 시작을 가리키는 포인터를 얻는다.
	CD3DX12_CPU_DESCRIPTOR_HANDLE SRV_DescriptorHandle(m_pDescriptorHeaps->GetCPUDescriptorHandleForHeapStart());
	for (_uint i = 0; i < m_uiTexSize - vecShadowDepth.size(); ++i)
	{
		D3D12_SHADER_RESOURCE_VIEW_DESC SRV_Desc = {};
		SRV_Desc.Format = vecTexture[i]->GetDesc().Format;
		SRV_Desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		SRV_Desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		SRV_Desc.Texture2D.MostDetailedMip = 0;
		SRV_Desc.Texture2D.MipLevels = vecTexture[i]->GetDesc().MipLevels;
		SRV_Desc.Texture2D.ResourceMinLODClamp = 0.0f;

		m_pGraphicDevice->CreateShaderResourceView(vecTexture[i].Get(), &SRV_Desc, SRV_DescriptorHandle);

		// 힙의 다음 서술자로 넘어간다.
		SRV_DescriptorHandle.Offset(1, Engine::CGraphicDevice::Get_Instance()->Get_CBV_SRV_UAV_DescriptorSize());
	}

	D3D12_SHADER_RESOURCE_VIEW_DESC SRV_Desc = {};
	SRV_Desc.Format = vecShadowDepth[0]->GetDesc().Format;
	SRV_Desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	SRV_Desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	SRV_Desc.Texture2D.MostDetailedMip = 0;
	SRV_Desc.Texture2D.MipLevels = vecShadowDepth[0]->GetDesc().MipLevels;
	SRV_Desc.Texture2D.ResourceMinLODClamp = 0.0f;
	m_pGraphicDevice->CreateShaderResourceView(vecShadowDepth[0].Get(), &SRV_Desc, SRV_DescriptorHandle);

	return S_OK;
}


void CSwordEffect::Set_CreateInfo(const _vec3& vScale, const _vec3& vAngle, const _vec3& vPos)
{
	m_pTransCom->m_vScale = vScale;
	m_pTransCom->m_vAngle = vAngle;
	m_pTransCom->m_vPos = vPos;
	m_fDeltaTime = -1.f;
	m_fLimitScale = 0.3f;
}

Engine::CGameObject* CSwordEffect::Create(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList,
	wstring wstrMeshTag,
	const _vec3& vScale,
	const _vec3& vAngle,
	const _vec3& vPos)
{
	CSwordEffect* pInstance = new CSwordEffect(pGraphicDevice, pCommandList);

	if (FAILED(pInstance->Ready_GameObject(wstrMeshTag, vScale, vAngle, vPos)))
		Engine::Safe_Release(pInstance);

	return pInstance;
}

CSwordEffect** CSwordEffect::Create_InstancePool(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList, const _uint& uiInstanceCnt)
{
	CSwordEffect** ppInstance = new (CSwordEffect * [uiInstanceCnt]);

	for (_uint i = 0; i < uiInstanceCnt; ++i)
	{
		ppInstance[i] = new CSwordEffect(pGraphicDevice, pCommandList);
		ppInstance[i]->m_uiInstanceIdx = i;
		ppInstance[i]->Ready_GameObject(L"Twohand19_A_SM",_vec3(0.f),_vec3(0.f),_vec3(0.f));
	}

	return ppInstance;
}

void CSwordEffect::Free()
{
	Engine::CGameObject::Free();
	Engine::Safe_Release(m_pMeshCom);
	//Engine::Safe_Release(m_pDescriptorHeaps);
	Engine::Safe_Release(m_pShaderCom);
	Engine::Safe_Release(m_pCrossFilterShaderCom);
}
