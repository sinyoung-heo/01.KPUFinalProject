#include "Renderer.h"
#include "GraphicDevice.h"
#include "DirectInput.h"
#include "ObjectMgr.h"
#include "TimeMgr.h"
#include "ComponentMgr.h"
#include "LightMgr.h"
#include "GameObject.h"
#include "RenderTarget.h"
#include "Font.h"

USING(Engine)
IMPLEMENT_SINGLETON(CRenderer)

CRenderer::CRenderer()
	: m_pComponentMgr(CComponentMgr::Get_Instance())
{
}

_uint CRenderer::Get_RenderLstSize(const RENDERGROUP & eRenderGroup)
{
	if (RENDER_END == eRenderGroup)
		return 0;

	return m_uiRenderListSize = (_uint)(m_RenderList[eRenderGroup].size());
}

_uint CRenderer::Get_ColliderLstSize()
{
	return m_uiColliderListSize = (_uint)(m_ColliderList.size());
}

_bool CRenderer::Get_RenderOnOff(wstring wstrTag)
{
	auto iter_find = m_mapRenderOnOff.find(wstrTag);

	if (iter_find == m_mapRenderOnOff.end())
		return false;

	return iter_find->second;
}

HRESULT CRenderer::Set_CurPipelineState(ID3D12PipelineState * pPipelineState)
{
	NULL_CHECK_RETURN(pPipelineState, E_FAIL);
	m_pCurPipelineState = pPipelineState;

	/*__________________________________________________________________________________________________________
	[ PipelineState가 바뀌었을 때만 SetPipelineState 설정 ]
	____________________________________________________________________________________________________________*/
	if (m_pPrePipelineState != m_pCurPipelineState)
	{
		m_pPrePipelineState = m_pCurPipelineState;

		CGraphicDevice::Get_Instance()->Set_PipelineState(m_pCurPipelineState);
		m_pCommandList->SetPipelineState(m_pCurPipelineState);

		++m_uiCnt_SetPipelineState;
	}

	return S_OK;
}

HRESULT CRenderer::Set_CurPipelineState(ID3D12GraphicsCommandList * pCommandList,
										ID3D12PipelineState * pPipelineState,
										const _int& iContextIdx)
{
	NULL_CHECK_RETURN(pPipelineState, E_FAIL);
	m_arrContextCurPipelineState[iContextIdx] = pPipelineState;

	/*__________________________________________________________________________________________________________
	[ PipelineState가 바뀌었을 때만 SetPipelineState 설정 ]
	____________________________________________________________________________________________________________*/
	if (m_arrContextPrePipelineState[iContextIdx] != m_arrContextCurPipelineState[iContextIdx])
	{
		m_arrContextPrePipelineState[iContextIdx] = m_arrContextCurPipelineState[iContextIdx];

		// CGraphicDevice::Get_Instance()->Set_PipelineState(m_arrContextCurPipelineState[iContextIdx]);
		pCommandList->SetPipelineState(m_arrContextCurPipelineState[iContextIdx]);

		++m_uiCnt_SetPipelineState;
	}

	return S_OK;
}

HRESULT CRenderer::Set_RenderOnOff(wstring wstrTag)
{
	auto iter_find = m_mapRenderOnOff.find(wstrTag);

	if (iter_find == m_mapRenderOnOff.end())
		return E_FAIL;

	iter_find->second = !(iter_find->second);

	return S_OK;
}

HRESULT CRenderer::Ready_Renderer(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList)
{
	m_pGraphicDevice = pGraphicDevice;
	NULL_CHECK_RETURN(m_pGraphicDevice, E_FAIL);

	m_pCommandList = pCommandList;
	NULL_CHECK_RETURN(m_pCommandList, E_FAIL);

	m_uiCnt_ShaderFile			= 0;
	m_uiCnt_PipelineState		= 0;
	m_uiCnt_SetPipelineState	= 0;

	FAILED_CHECK_RETURN(Ready_ShaderPrototype(), E_FAIL);
	FAILED_CHECK_RETURN(Ready_RenderTarget(), E_FAIL);

	m_mapRenderOnOff[L"RenderTarget"]	= false;
	m_mapRenderOnOff[L"DebugFont"]		= false;
	m_mapRenderOnOff[L"Collider"]		= false;

	/*__________________________________________________________________________________________________________
	2020.06.07 MultiThreadRendering
	Thread CommandAllocators & CommandList 생성.
	____________________________________________________________________________________________________________*/
	FAILED_CHECK_RETURN(Create_ThreadCommandList(), E_FAIL);

	return S_OK;
}

HRESULT CRenderer::Add_Renderer(const RENDERGROUP & eRenderID, CGameObject * pGameObject)
{
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	m_RenderList[eRenderID].emplace_back(pGameObject);

	return S_OK;
}

HRESULT CRenderer::Add_Renderer(CComponent * pComponent)
{
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_ColliderList.emplace_back(pComponent);

	return S_OK;
}

HRESULT CRenderer::Render_Renderer(const _float& fTimeDelta, const RENDERID& eID)
{
	/*__________________________________________________________________________________________________________
	- GPU가 모든 명령 리스트를 실행할 때 까지 기다린다.
	____________________________________________________________________________________________________________*/
	FAILED_CHECK_RETURN(CGraphicDevice::Get_Instance()->Wait_ForGpuComplete(), E_FAIL);

	// [ Render Begin ]
	FAILED_CHECK_RETURN(CGraphicDevice::Get_Instance()->Render_Begin(_rgba(0.0f, 0.0f, 0.0f, 1.0f)), E_FAIL);

	/*__________________________________________________________________________________________________________
	LoadingThread가 끝나고 나서 쓰레드를 만들어줘야 한다.
	____________________________________________________________________________________________________________*/
	if(RENDERID::MULTI_THREAD == eID)
		Render_MultiThread(fTimeDelta);

	Render_Priority(fTimeDelta);		// SkyBox

	if (RENDERID::SINGLE_THREAD == eID)
	{
		Render_ShadowDepth(fTimeDelta);	// Shadow Depth
		Render_NonAlpha(fTimeDelta);	// Diffuse, Normal, Specular, Depth
	}

	Render_Light();						// Shade, Specular
	Render_Luminance();					// Luminance(고휘도추출)
	Render_DownSampling();
	Render_SSAO();
	Render_Blur();
	Render_Distortion(fTimeDelta);
	Render_Blend();						// Target Blend

	Render_Collider(fTimeDelta);		// Collider Render
	Render_Alpha(fTimeDelta);			// Effect Texture, Mesh

	Render_UI(fTimeDelta);				// UI Render
	Render_RenderTarget();				// Debug RenderTarget

	// [ Excute CommandList ]
	FAILED_CHECK_RETURN(CGraphicDevice::Get_Instance()->Render_ExcuteCmdList(), E_FAIL);
	
	Render_Font(fTimeDelta);			// 11on12 Text Render

	// [ Render End ]
	FAILED_CHECK_RETURN(CGraphicDevice::Get_Instance()->Render_End(), E_FAIL);

	Clear_RenderGroup();
	CShaderShadowInstancing::Get_Instance()->Reset_Instance();
	CShaderMeshInstancing::Get_Instance()->Reset_Instance();

	return S_OK;
}

void CRenderer::Clear_RenderGroup()
{
	for (size_t i = 0; i < RENDER_END; ++i)
		m_RenderList[i].clear();

	m_ColliderList.clear();
}

void CRenderer::Render_Priority(const _float& fTimeDelta)
{
	for (auto& pGameObject : m_RenderList[RENDER_PRIORITY])
		pGameObject->Render_GameObject(fTimeDelta);
}

void CRenderer::Render_ShadowDepth(const _float & fTimeDelta)
{
	m_pTargetShadowDepth->SetUp_OnGraphicDevice(TARGETID::TYPE_SHADOWDEPTH);

	for (auto& pGameObject : m_RenderList[RENDER_NONALPHA])
		pGameObject->Render_ShadowDepth(fTimeDelta);

	m_pTargetShadowDepth->Release_OnGraphicDevice(TARGETID::TYPE_SHADOWDEPTH);
}


void CRenderer::Render_NonAlpha(const _float& fTimeDelta)
{
	m_pTargetDeferred->SetUp_OnGraphicDevice(TARGETID::TYPE_DEFAULT);

	for (auto& pGameObject : m_RenderList[RENDER_NONALPHA])
		pGameObject->Render_GameObject(fTimeDelta);

	m_pTargetDeferred->Release_OnGraphicDevice(TARGETID::TYPE_DEFAULT);
}

void CRenderer::Render_Light()
{
	m_pTargetLight->SetUp_OnGraphicDevice(TARGETID::TYPE_LIGHTING);

	CLightMgr::Get_Instance()->Render_Light(m_pTargetDeferred->Get_TargetTexture());

	m_pTargetLight->Release_OnGraphicDevice(TARGETID::TYPE_LIGHTING);

}

void CRenderer::Render_Blend()
{
	if (!m_bIsSetBlendTexture)
	{
		m_bIsSetBlendTexture = true;

		vector<ComPtr<ID3D12Resource>> vecDeferredTarget	= m_pTargetDeferred->Get_TargetTexture();
		vector<ComPtr<ID3D12Resource>> vecShadeTarget		= m_pTargetLight->Get_TargetTexture();

		vector<ComPtr<ID3D12Resource>> vecBlurTarget = m_pTargetBlur->Get_TargetTexture();
		vector<ComPtr<ID3D12Resource>> vecSSAOTarget = m_pTargetSSAO->Get_TargetTexture();
		vector<ComPtr<ID3D12Resource>> vecDistortionTarget = m_pTargetDistortion->Get_TargetTexture();


		vector<ComPtr<ID3D12Resource>> vecBlendTarget;
		vecBlendTarget.emplace_back(vecDeferredTarget[0]);	// RenderTarget - Diffuse
		vecBlendTarget.emplace_back(vecShadeTarget[0]);		// RenderTarget - Shade
		vecBlendTarget.emplace_back(vecShadeTarget[1]);		// RenderTarget - Specular
		vecBlendTarget.emplace_back(vecBlurTarget[0]);		// RenderTarget - Blur 
		vecBlendTarget.emplace_back(vecDeferredTarget[4]);		// RenderTarget - Emissive
		vecBlendTarget.emplace_back(vecBlurTarget[3]);		// RenderTarget - SSAO
		vecBlendTarget.emplace_back(vecDistortionTarget[0]);		// RenderTarget - SSAO

		m_pBlendShader->SetUp_ShaderTexture(vecBlendTarget);
	}

	m_pBlendShader->Begin_Shader();
	m_pBlendBuffer->Begin_Buffer();

	m_pBlendBuffer->Render_Buffer();

}

void CRenderer::Render_Distortion(const _float& fTimeDelta)
{
	sort(m_RenderList[RENDER_DISTORTION].begin(), m_RenderList[RENDER_DISTORTION ].end(), [](CGameObject* pSour, CGameObject* pDest)->_bool
		{
			return pSour->Get_DepthOfView() > pDest->Get_DepthOfView();
		});

	m_pTargetDistortion->SetUp_OnGraphicDevice(TARGETID::TYPE_DEFAULT);

	for (auto& pGameObject : m_RenderList[RENDER_DISTORTION])
		pGameObject->Render_GameObject(fTimeDelta);

	m_pTargetDistortion->Release_OnGraphicDevice(TARGETID::TYPE_DEFAULT);
}

void CRenderer::Render_Luminance()
{
	if (!m_bIsSetLuminanceTexture)
	{
		m_bIsSetLuminanceTexture = true;
		vector<ComPtr<ID3D12Resource>> vecDeferredTarget = m_pTargetDeferred->Get_TargetTexture();
		vector<ComPtr<ID3D12Resource>> vecLuminanceTarget;
		vecLuminanceTarget.emplace_back(vecDeferredTarget[0]);	// RenderTarget - Albedo
		m_pLuminanceShader->SetUp_ShaderTexture(vecLuminanceTarget);
	}
	m_pTargetLuminance->SetUp_OnGraphicDevice();
	m_pLuminanceShader->Begin_Shader();
	m_pLuminanceBuffer->Begin_Buffer();
	m_pLuminanceBuffer->Render_Buffer();
	m_pTargetLuminance->Release_OnGraphicDevice();
}

void CRenderer::Render_DownSampling()
{
	if (!m_bIsSetDownSamplingTexture)
	{
		m_bIsSetDownSamplingTexture = true; 
		vector<ComPtr<ID3D12Resource>> vecDeferredTarget = m_pTargetDeferred->Get_TargetTexture();\
		vector<ComPtr<ID3D12Resource>> vecSSAOTarget = m_pTargetSSAO->Get_TargetTexture();
		vector<ComPtr<ID3D12Resource>> vecDownSamplingTarget;
		vecDownSamplingTarget.emplace_back(vecDeferredTarget[4]);	// RenderTarget - Emissive
		vecDownSamplingTarget.emplace_back(vecDeferredTarget[4]);	// RenderTarget - Emissive
		vecDownSamplingTarget.emplace_back(vecSSAOTarget[0]);	// RenderTarget - SSAO
		m_pDownSamplingShader->SetUp_ShaderTexture(vecDownSamplingTarget);
	
	}
	m_pTargetDownSampling->SetUp_OnGraphicDevice_DownSampling(4);
	m_pDownSamplingShader->Begin_Shader();
	m_pDownSamplingBuffer->Begin_Buffer();
	m_pDownSamplingBuffer->Render_Buffer();
	m_pTargetDownSampling->Release_OnGraphicDevice_DownSampling();
}

void CRenderer::Render_Blur()
{
	if (!m_bIsSetBlurTexture)
	{
		m_bIsSetBlurTexture = true;
		vector<ComPtr<ID3D12Resource>> vecDownSampleTarget = m_pTargetDownSampling->Get_TargetTexture();
		vector<ComPtr<ID3D12Resource>> vecDeferredTarget = m_pTargetDeferred->Get_TargetTexture();
		vector<ComPtr<ID3D12Resource>> vecSSAOTarget = m_pTargetSSAO->Get_TargetTexture();

		vector<ComPtr<ID3D12Resource>> vecBlurTarget;
		vecBlurTarget.emplace_back(vecDownSampleTarget[0]);	// RenderTarget - DS Emissive
		vecBlurTarget.emplace_back(vecDownSampleTarget[1]);	// RenderTarget - DS CrossFilter
		vecBlurTarget.emplace_back(vecDeferredTarget[0]);	// RenderTarget - Albedo
		vecBlurTarget.emplace_back(vecDeferredTarget[3]);	// RenderTarget - Depth
		vecBlurTarget.emplace_back(vecSSAOTarget[0]);	// RenderTarget - SSAO
		m_pBlurShader->SetUp_ShaderTexture(vecBlurTarget);
	}
	m_pTargetBlur->SetUp_OnGraphicDevice();
	m_pBlurShader->Begin_Shader();
	m_pBlurBuffer->Begin_Buffer();
	m_pBlurBuffer->Render_Buffer();
	m_pTargetBlur->Release_OnGraphicDevice();
}

void CRenderer::Render_SSAO()
{
	if (!m_bIsSetSSAOTexture)
	{
		m_bIsSetSSAOTexture = true;
		vector<ComPtr<ID3D12Resource>> vecDeferredTarget = m_pTargetDeferred->Get_TargetTexture();
		vector<ComPtr<ID3D12Resource>> vecSSAOTarget;
		vecSSAOTarget.emplace_back(vecDeferredTarget[1]);	// RenderTarget - Normal
		vecSSAOTarget.emplace_back(vecDeferredTarget[3]);	// RenderTarget - Depth
		m_pSSAOShader->SetUp_ShaderTexture(vecSSAOTarget);
	}
	m_pTargetSSAO->SetUp_OnGraphicDevice();
	m_pSSAOShader->Begin_Shader();
	m_pSSAOBuffer->Begin_Buffer();
	m_pSSAOBuffer->Render_Buffer();
	m_pTargetSSAO->Release_OnGraphicDevice();
}

void CRenderer::Render_Alpha(const _float& fTimeDelta)
{
	sort(m_RenderList[RENDER_ALPHA].begin(), m_RenderList[RENDER_ALPHA].end(), [](CGameObject* pSour, CGameObject* pDest)->_bool 
		{ 
			return pSour->Get_DepthOfView() > pDest->Get_DepthOfView(); 
		});

	for (auto& pGameObject : m_RenderList[RENDER_ALPHA])
		pGameObject->Render_GameObject(fTimeDelta);
}

void CRenderer::Render_UI(const _float& fTimeDelta)
{
	sort(m_RenderList[RENDER_UI].begin(), m_RenderList[RENDER_UI].end(), [](CGameObject* pSour, CGameObject* pDest)->_bool
		{ 
			return pSour->Get_UIDepth() > pDest->Get_UIDepth();
		});

	for (auto& pGameObject : m_RenderList[RENDER_UI])
		pGameObject->Render_GameObject(fTimeDelta);
}

void CRenderer::Render_Collider(const _float & fTimeDelta)
{
	for (auto& pCollider : m_ColliderList)
		pCollider->Render_Component(fTimeDelta);
}

void CRenderer::Render_RenderTarget()
{
	if (m_mapRenderOnOff[L"RenderTarget"])
	{
		if (nullptr != m_pTargetDeferred)
			m_pTargetDeferred->Render_RenderTarget();

		if (nullptr != m_pTargetLight)
			m_pTargetLight->Render_RenderTarget();

		if (nullptr != m_pTargetShadowDepth)
			m_pTargetShadowDepth->Render_RenderTarget();

		if (nullptr != m_pTargetLuminance)
			m_pTargetLuminance->Render_RenderTarget();

		if (nullptr != m_pTargetDownSampling)
			m_pTargetDownSampling->Render_RenderTarget();

		if (nullptr != m_pTargetBlur)
			m_pTargetBlur->Render_RenderTarget();
		if (nullptr != m_pTargetSSAO)
			m_pTargetSSAO->Render_RenderTarget();


		if (nullptr != m_pTargetDistortion)
			m_pTargetDistortion->Render_RenderTarget();
	}

}

void CRenderer::Render_Font(const _float & fTimeDelta)
{
	CGraphicDevice::Get_Instance()->Render_TextBegin();

	for (auto& pGameObject : m_RenderList[RENDER_FONT])
		pGameObject->Render_GameObject(fTimeDelta);

	CGraphicDevice::Get_Instance()->Render_TextEnd();
}


HRESULT CRenderer::Ready_ShaderPrototype()
{
	CShader* pShader = nullptr;

	// ShaderColor
	pShader = CShaderColor::Create(m_pGraphicDevice, m_pCommandList);
	NULL_CHECK_RETURN(pShader, E_FAIL);
	FAILED_CHECK_RETURN(m_pComponentMgr->Add_ComponentPrototype(L"ShaderColor", ID_STATIC, pShader), E_FAIL);
	++m_uiCnt_ShaderFile;

	// ShaderTexture
	pShader = CShaderTexture::Create(m_pGraphicDevice, m_pCommandList);
	NULL_CHECK_RETURN(pShader, E_FAIL);
	FAILED_CHECK_RETURN(m_pComponentMgr->Add_ComponentPrototype(L"ShaderTexture", ID_STATIC, pShader), E_FAIL);
	++m_uiCnt_ShaderFile;

	// ShaderSkyBox
	pShader = CShaderSkyBox::Create(m_pGraphicDevice, m_pCommandList);
	NULL_CHECK_RETURN(pShader, E_FAIL);
	FAILED_CHECK_RETURN(m_pComponentMgr->Add_ComponentPrototype(L"ShaderSkyBox", ID_STATIC, pShader), E_FAIL);
	++m_uiCnt_ShaderFile;

	// ShaderMesh
	pShader = CShaderMesh::Create(m_pGraphicDevice, m_pCommandList);
	NULL_CHECK_RETURN(pShader, E_FAIL);
	FAILED_CHECK_RETURN(m_pComponentMgr->Add_ComponentPrototype(L"ShaderMesh", ID_STATIC, pShader), E_FAIL);
	++m_uiCnt_ShaderFile;

	// ShaderShadow
	pShader = CShaderShadow::Create(m_pGraphicDevice, m_pCommandList);
	NULL_CHECK_RETURN(pShader, E_FAIL);
	FAILED_CHECK_RETURN(m_pComponentMgr->Add_ComponentPrototype(L"ShaderShadow", ID_STATIC, pShader), E_FAIL);
	++m_uiCnt_ShaderFile;

	// ShaderLighting
	pShader = CShaderLighting::Create(m_pGraphicDevice, m_pCommandList);
	NULL_CHECK_RETURN(pShader, E_FAIL);
	FAILED_CHECK_RETURN(m_pComponentMgr->Add_ComponentPrototype(L"ShaderLighting", ID_STATIC, pShader), E_FAIL);
	++m_uiCnt_ShaderFile;

	// ShaderBlend
	pShader = CShaderBlend::Create(m_pGraphicDevice, m_pCommandList);
	NULL_CHECK_RETURN(pShader, E_FAIL);
	FAILED_CHECK_RETURN(m_pComponentMgr->Add_ComponentPrototype(L"ShaderBlend", ID_STATIC, pShader), E_FAIL);
	++m_uiCnt_ShaderFile;

	// ShaderLuminance
	pShader = CShaderLuminance::Create(m_pGraphicDevice, m_pCommandList);
	NULL_CHECK_RETURN(pShader, E_FAIL);
	FAILED_CHECK_RETURN(m_pComponentMgr->Add_ComponentPrototype(L"ShaderLuminance", ID_STATIC, pShader), E_FAIL);
	++m_uiCnt_ShaderFile;

	// ShaderDownSampling
	pShader = CShaderDownSampling::Create(m_pGraphicDevice, m_pCommandList);
	NULL_CHECK_RETURN(pShader, E_FAIL);
	FAILED_CHECK_RETURN(m_pComponentMgr->Add_ComponentPrototype(L"ShaderDownSampling", ID_STATIC, pShader), E_FAIL);
	++m_uiCnt_ShaderFile;

	// ShaderBlur
	pShader = CShaderBlur::Create(m_pGraphicDevice, m_pCommandList);
	NULL_CHECK_RETURN(pShader, E_FAIL);
	FAILED_CHECK_RETURN(m_pComponentMgr->Add_ComponentPrototype(L"ShaderBlur", ID_STATIC, pShader), E_FAIL);
	++m_uiCnt_ShaderFile;

	// ShaderSSAO
	pShader = CShaderSSAO::Create(m_pGraphicDevice, m_pCommandList);
	NULL_CHECK_RETURN(pShader, E_FAIL);
	FAILED_CHECK_RETURN(m_pComponentMgr->Add_ComponentPrototype(L"ShaderSSAO", ID_STATIC, pShader), E_FAIL);
	++m_uiCnt_ShaderFile;

	// ShaderShadowInstancing
	CShaderShadowInstancing::Get_Instance()->Ready_Shader(m_pGraphicDevice, m_pCommandList);
	++m_uiCnt_ShaderFile;

	// ShaderMeshInstancing
	CShaderMeshInstancing::Get_Instance()->Ready_Shader(m_pGraphicDevice, m_pCommandList);
	++m_uiCnt_ShaderFile;

	// ShaderTextureInstancing
	CShaderTextureInstancing::Get_Instance()->Ready_Shader(m_pGraphicDevice, m_pCommandList);
	++m_uiCnt_ShaderFile;

	return S_OK;
}

HRESULT CRenderer::Ready_RenderTarget()
{
	/*__________________________________________________________________________________________________________
	[ Deferred RenderTarget ]
	____________________________________________________________________________________________________________*/
	// Diffuse, Normal, Specular, Depth
	m_pTargetDeferred = CRenderTarget::Create(m_pGraphicDevice, m_pCommandList, 5);
	NULL_CHECK_RETURN(m_pTargetDeferred, E_FAIL);
	m_pTargetDeferred->Set_TargetClearColor(0, _rgba(0.0f, 0.0f, 0.0f, 0.0f), DXGI_FORMAT_R8G8B8A8_UNORM);		// Diffuse
	m_pTargetDeferred->Set_TargetClearColor(1, _rgba(0.0f, 0.0f, 0.0f, 1.0f), DXGI_FORMAT_R8G8B8A8_UNORM);		// Normal
	m_pTargetDeferred->Set_TargetClearColor(2, _rgba(0.0f, 0.0f, 0.0f, 0.0f), DXGI_FORMAT_R8G8B8A8_UNORM);		// Specular
	m_pTargetDeferred->Set_TargetClearColor(3, _rgba(1.0f, 1.0f, 1.0f, 1.0f), DXGI_FORMAT_R32G32B32A32_FLOAT);	// Depth
	m_pTargetDeferred->Set_TargetClearColor(4, _rgba(0.0f, 0.0f, 0.0f, 0.0f), DXGI_FORMAT_R8G8B8A8_UNORM);	// Emissive
	FAILED_CHECK_RETURN(m_pTargetDeferred->SetUp_DefaultSetting(TARGETID::TYPE_DEFAULT), E_FAIL);

	/*__________________________________________________________________________________________________________
	[ Light RnderTarget ]
	____________________________________________________________________________________________________________*/
	m_pTargetLight = CRenderTarget::Create(m_pGraphicDevice, m_pCommandList, 2);
	NULL_CHECK_RETURN(m_pTargetLight, E_FAIL);
	m_pTargetLight->Set_TargetClearColor(0, _rgba(0.0f, 0.0f, 0.0f, 1.0f), DXGI_FORMAT_R32G32B32A32_FLOAT);	// Shade
	m_pTargetLight->Set_TargetClearColor(1, _rgba(0.0f, 0.0f, 0.0f, 0.0f), DXGI_FORMAT_R8G8B8A8_UNORM);		// Specular
	FAILED_CHECK_RETURN(m_pTargetLight->SetUp_DefaultSetting(TARGETID::TYPE_LIGHTING), E_FAIL);

	m_pTargetLight->Set_TargetRenderPos(_vec3(WIDTH_SECOND, HEIGHT_FIRST , 1.0f));

	/*__________________________________________________________________________________________________________
	[ ShadowDepth RenderTarget ]
	____________________________________________________________________________________________________________*/
	m_pTargetShadowDepth = CRenderTarget::Create(m_pGraphicDevice, m_pCommandList, 1);
	NULL_CHECK_RETURN(m_pTargetShadowDepth, E_FAIL);
	m_pTargetShadowDepth->Set_TargetClearColor(0, _rgba(1.0f, 1.0f, 1.0f, 1.0f), DXGI_FORMAT_R32G32B32A32_FLOAT);
	m_pTargetShadowDepth->Set_TargetTextureSize(0, SHADOWTEX_WIDTH, SHADOWTEX_HEIGHT);
	FAILED_CHECK_RETURN(m_pTargetShadowDepth->SetUp_DefaultSetting(TARGETID::TYPE_SHADOWDEPTH), E_FAIL);

	m_pTargetShadowDepth->Set_TargetRenderScale(_vec3(90.0f, 90.0f, 90.0f));
	m_pTargetShadowDepth->Set_TargetRenderPos(_vec3(WINCX - 90.0f, WINCY - 90.0f, 1.0f));

	/*__________________________________________________________________________________________________________
	[ Luminance RenderTarget ]
	____________________________________________________________________________________________________________*/
	m_pTargetLuminance = CRenderTarget::Create(m_pGraphicDevice, m_pCommandList, 1);
	NULL_CHECK_RETURN(m_pTargetLuminance, E_FAIL);
	m_pTargetLuminance->Set_TargetClearColor(0, _rgba(0.0f, 0.0f, 0.0f, 0.0f), DXGI_FORMAT_R16G16B16A16_UNORM);
	FAILED_CHECK_RETURN(m_pTargetLuminance->SetUp_DefaultSetting(), E_FAIL);
	m_pTargetLuminance->Set_TargetRenderPos(_vec3(WIDTH_THIRD, HEIGHT_FIRST, 1.0f));

	m_pLuminanceShader = static_cast<CShaderLuminance*>(m_pComponentMgr->Clone_Component(L"ShaderLuminance", COMPONENTID::ID_STATIC));
	NULL_CHECK_RETURN(m_pLuminanceShader, E_FAIL);
	FAILED_CHECK_RETURN(m_pLuminanceShader->Set_PipelineStatePass(0), E_FAIL);

	m_pLuminanceBuffer = static_cast<CScreenTex*>(m_pComponentMgr->Clone_Component(L"ScreenTex", COMPONENTID::ID_STATIC));
	NULL_CHECK_RETURN(m_pLuminanceBuffer, E_FAIL);
	/*__________________________________________________________________________________________________________
	[ DownSample RenderTarget ]
	____________________________________________________________________________________________________________*/
	m_pTargetDownSampling = CRenderTarget::Create(m_pGraphicDevice, m_pCommandList, 3);
	NULL_CHECK_RETURN(m_pTargetDownSampling, E_FAIL);
	m_pTargetDownSampling->Set_TargetClearColor(0, _rgba(0.0f, 0.0f, 0.0f, 0.0f), DXGI_FORMAT_R8G8B8A8_UNORM);
	m_pTargetDownSampling->Set_TargetClearColor(1, _rgba(0.0f, 0.0f, 0.0f, 0.0f), DXGI_FORMAT_R8G8B8A8_UNORM);
	m_pTargetDownSampling->Set_TargetClearColor(2, _rgba(0.0f, 0.0f, 0.0f, 0.0f), DXGI_FORMAT_R8G8B8A8_UNORM);
	m_pTargetDownSampling->Set_TargetTextureSize(0, WINCX / 4 , WINCY / 4, false);
	m_pTargetDownSampling->Set_TargetTextureSize(1, WINCX / 4 , WINCY / 4, false);
	m_pTargetDownSampling->Set_TargetTextureSize(2, WINCX / 4, WINCY / 4, false);

	FAILED_CHECK_RETURN(m_pTargetDownSampling->SetUp_DefaultSetting(), E_FAIL);
	m_pTargetDownSampling->Set_TargetRenderPos(_vec3(WIDTH_FOURTH, HEIGHT_FIRST, 1.0f));

	m_pDownSamplingShader = static_cast<CShaderDownSampling*>(m_pComponentMgr->Clone_Component(L"ShaderDownSampling", COMPONENTID::ID_STATIC));
	NULL_CHECK_RETURN(m_pDownSamplingShader, E_FAIL);
	FAILED_CHECK_RETURN(m_pDownSamplingShader->Set_PipelineStatePass(0), E_FAIL);

	m_pDownSamplingBuffer = static_cast<CScreenTex*>(m_pComponentMgr->Clone_Component(L"ScreenTex", COMPONENTID::ID_STATIC));
	NULL_CHECK_RETURN(m_pDownSamplingBuffer, E_FAIL);

	/*__________________________________________________________________________________________________________
	[ Blur RenderTarget ]
	____________________________________________________________________________________________________________*/
	m_pTargetBlur = CRenderTarget::Create(m_pGraphicDevice, m_pCommandList, 4);
	NULL_CHECK_RETURN(m_pTargetBlur, E_FAIL);
	m_pTargetBlur->Set_TargetClearColor(0, _rgba(0.0f, 0.0f, 0.0f, 0.0f), DXGI_FORMAT_R8G8B8A8_UNORM);
	m_pTargetBlur->Set_TargetClearColor(1, _rgba(0.0f, 0.0f, 0.0f, 0.0f), DXGI_FORMAT_R8G8B8A8_UNORM);
	m_pTargetBlur->Set_TargetClearColor(2, _rgba(0.0f, 0.0f, 0.0f, 0.0f), DXGI_FORMAT_R8G8B8A8_UNORM);
	m_pTargetBlur->Set_TargetClearColor(3, _rgba(0.0f, 0.0f, 0.0f, 0.0f), DXGI_FORMAT_R8G8B8A8_UNORM);
	FAILED_CHECK_RETURN(m_pTargetBlur->SetUp_DefaultSetting(), E_FAIL);
	m_pTargetBlur->Set_TargetRenderPos(_vec3(WIDTH_FIFTH, HEIGHT_FIRST, 1.0f));

	m_pBlurShader = static_cast<CShaderBlur*>(m_pComponentMgr->Clone_Component(L"ShaderBlur", COMPONENTID::ID_STATIC));
	NULL_CHECK_RETURN(m_pBlurShader, E_FAIL);
	FAILED_CHECK_RETURN(m_pBlurShader->Set_PipelineStatePass(0), E_FAIL);

	m_pBlurBuffer = static_cast<CScreenTex*>(m_pComponentMgr->Clone_Component(L"ScreenTex", COMPONENTID::ID_STATIC));
	NULL_CHECK_RETURN(m_pBlurBuffer, E_FAIL);
	/*__________________________________________________________________________________________________________
	[ SSAO RenderTarget ]
	____________________________________________________________________________________________________________*/
	m_pTargetSSAO = CRenderTarget::Create(m_pGraphicDevice, m_pCommandList, 1);
	NULL_CHECK_RETURN(m_pTargetSSAO, E_FAIL);
	m_pTargetSSAO->Set_TargetClearColor(0, _rgba(0.0f, 0.0f, 0.0f, 1.0f), DXGI_FORMAT_R8G8B8A8_UNORM);
	FAILED_CHECK_RETURN(m_pTargetSSAO->SetUp_DefaultSetting(), E_FAIL);
	m_pTargetSSAO->Set_TargetRenderPos(_vec3(WIDTH_SECOND, HEIGHT_THIRD, 1.0f));
	m_pSSAOShader = static_cast<CShaderSSAO*>(m_pComponentMgr->Clone_Component(L"ShaderSSAO", COMPONENTID::ID_STATIC));
	NULL_CHECK_RETURN(m_pSSAOShader, E_FAIL);
	FAILED_CHECK_RETURN(m_pSSAOShader->Set_PipelineStatePass(0), E_FAIL);
	m_pSSAOBuffer = static_cast<CScreenTex*>(m_pComponentMgr->Clone_Component(L"ScreenTex", COMPONENTID::ID_STATIC));
	NULL_CHECK_RETURN(m_pSSAOBuffer, E_FAIL);

	//Distortion

	m_pTargetDistortion = CRenderTarget::Create(m_pGraphicDevice, m_pCommandList, 1);
	NULL_CHECK_RETURN(m_pTargetDistortion, E_FAIL);
	m_pTargetDistortion->Set_TargetClearColor(0, _rgba(0.0f, 0.0f, 0.0f, 1.0f), DXGI_FORMAT_R8G8B8A8_UNORM);
	FAILED_CHECK_RETURN(m_pTargetDistortion->SetUp_DefaultSetting(), E_FAIL);
	m_pTargetDistortion->Set_TargetRenderPos(_vec3(WIDTH_SECOND, HEIGHT_FOURTH, 1.0f));

	/*__________________________________________________________________________________________________________
	[ Blend Resource ]
	____________________________________________________________________________________________________________*/
	m_pBlendBuffer = static_cast<CScreenTex*>(m_pComponentMgr->Clone_Component(L"ScreenTex", COMPONENTID::ID_STATIC));
	NULL_CHECK_RETURN(m_pBlendBuffer, E_FAIL);

	m_pBlendShader = static_cast<CShaderBlend*>(m_pComponentMgr->Clone_Component(L"ShaderBlend", COMPONENTID::ID_STATIC));
	NULL_CHECK_RETURN(m_pBlendShader, E_FAIL);
	FAILED_CHECK_RETURN(m_pBlendShader->Set_PipelineStatePass(0), E_FAIL);

	
	return S_OK;
}


void CRenderer::Create_ThreadContext()
{
	// Create Thread Context
	struct thread_wrapper
	{
		static unsigned int WINAPI thunk(LPVOID lpParameter)
		{
			ThreadParameter* pParameter = reinterpret_cast<ThreadParameter*>(lpParameter);
			CRenderer::Get_Instance()->Worker_Thread(pParameter->threadIdx);

			return 0;
		}
	};

	for (int i = 0; i < CONTEXT::CONTEXT_END; ++i)
	{
		m_hWorkerBeginRender[i]		= CreateEvent(NULL,	// 핸들 상속과 보안 디스크립터.
												  FALSE,// TRUE면 수동 리셋, FALSE면 자동 리셋 이벤트.
												  FALSE,// TRUE면 신호, FALSE면 비신호 상태로 시작.
												  NULL);// 이벤트에 부여할 이름.
		m_hWorkerFinishShadow[i]	= CreateEvent(NULL, FALSE, FALSE, NULL);
		m_hWorkerFinishedRender[i]	= CreateEvent(NULL, FALSE, FALSE, NULL);

		m_tThreadParameter[i].threadIdx = i;

		m_hThreadHandle[i] = reinterpret_cast<HANDLE>(_beginthreadex(nullptr,
																	 0,
																	 thread_wrapper::thunk,
																	 reinterpret_cast<LPVOID>(&m_tThreadParameter[i]),
																	 0,
																	 nullptr));

		assert(m_hWorkerBeginRender[i]	!= NULL);
		assert(m_hWorkerFinishShadow[i] != NULL);
		assert(m_hThreadHandle[i]		!= NULL);
	}
}

HRESULT CRenderer::Create_ThreadCommandList()
{
	for (_int i = 0; i < CONTEXT::CONTEXT_END; ++i)
	{
		FAILED_CHECK_RETURN(m_pGraphicDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, 
																	 IID_PPV_ARGS(&m_arrShadowCommandAllocator[i])), 
																	 E_FAIL);
		FAILED_CHECK_RETURN(m_pGraphicDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT,
																	 IID_PPV_ARGS(&m_arrSceneCommandAllocator[i])),
																	 E_FAIL);

		FAILED_CHECK_RETURN(m_pGraphicDevice->CreateCommandList(0,
																D3D12_COMMAND_LIST_TYPE_DIRECT,
																m_arrShadowCommandAllocator[i],// Associated command allocator
																m_pCurPipelineState,			// Initial PipelineStateObject
																IID_PPV_ARGS(&m_arrShadowCommandList[i])),
																E_FAIL);
		FAILED_CHECK_RETURN(m_pGraphicDevice->CreateCommandList(0,
																D3D12_COMMAND_LIST_TYPE_DIRECT,
																m_arrSceneCommandAllocator[i],	// Associated command allocator
																m_pCurPipelineState,			// Initial PipelineStateObject
																IID_PPV_ARGS(&m_arrSceneCommandList[i])),
																E_FAIL);

		FAILED_CHECK_RETURN(m_arrShadowCommandList[i]->Close(), E_FAIL);
		FAILED_CHECK_RETURN(m_arrSceneCommandList[i]->Close(), E_FAIL);

		_tchar szName[MIN_STR];

		wsprintf(szName, L"COMMANDLIST_MULTITHREAD_SHADOW%d", i);
		m_arrShadowCommandList[i]->SetName(szName);

		wsprintf(szName, L"COMMANDLIST_MULTITHREAD_SCENE%d", i);
		m_arrSceneCommandList[i]->SetName(szName);
	}

	// PreCommandList - ShadowDepth
	FAILED_CHECK_RETURN(m_pGraphicDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, 
																	 IID_PPV_ARGS(&m_pPreShadowCommandAllocator)),
																	 E_FAIL);
	FAILED_CHECK_RETURN(m_pGraphicDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, 
																	 IID_PPV_ARGS(&m_pEndShadowCommandAllocator)),
																	 E_FAIL);

	FAILED_CHECK_RETURN(m_pGraphicDevice->CreateCommandList(0,
															D3D12_COMMAND_LIST_TYPE_DIRECT,
															m_pPreShadowCommandAllocator,	// Associated command allocator
															m_pCurPipelineState,			// Initial PipelineStateObject
															IID_PPV_ARGS(&m_pPreShadowCommandList)),
															E_FAIL);
	FAILED_CHECK_RETURN(m_pGraphicDevice->CreateCommandList(0,
															D3D12_COMMAND_LIST_TYPE_DIRECT,
															m_pEndShadowCommandAllocator,	// Associated command allocator
															m_pCurPipelineState,			// Initial PipelineStateObject
															IID_PPV_ARGS(&m_pEndShadowCommandList)),
															E_FAIL);

	FAILED_CHECK_RETURN(m_pPreShadowCommandList->Close(), E_FAIL);
	m_pPreShadowCommandList->SetName(L"COMMANDLIST_MULTITHREAD_PRE_SHADOW");
	FAILED_CHECK_RETURN(m_pEndShadowCommandList->Close(), E_FAIL);
	m_pEndShadowCommandList->SetName(L"COMMANDLIST_MULTITHREAD_END_SHADOW");


	// PreCommandList - Scene
	FAILED_CHECK_RETURN(m_pGraphicDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, 
																 IID_PPV_ARGS(&m_pPreSceneCommandAllocator)),
																 E_FAIL);
	FAILED_CHECK_RETURN(m_pGraphicDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, 
																 IID_PPV_ARGS(&m_pEndSceneCommandAllocator)),
																 E_FAIL);

	FAILED_CHECK_RETURN(m_pGraphicDevice->CreateCommandList(0,
															D3D12_COMMAND_LIST_TYPE_DIRECT,
															m_pPreSceneCommandAllocator,	// Associated command allocator
															m_pCurPipelineState,			// Initial PipelineStateObject
															IID_PPV_ARGS(&m_pPreSceneCommandList)),
															E_FAIL);
	FAILED_CHECK_RETURN(m_pGraphicDevice->CreateCommandList(0,
															D3D12_COMMAND_LIST_TYPE_DIRECT,
															m_pEndSceneCommandAllocator,	// Associated command allocator
															m_pCurPipelineState,			// Initial PipelineStateObject
															IID_PPV_ARGS(&m_pEndSceneCommandList)),
															E_FAIL);

	FAILED_CHECK_RETURN(m_pPreSceneCommandList->Close(), E_FAIL);
	m_pPreSceneCommandList->SetName(L"COMMANDLIST_MULTITHREAD_PRE_SCENE");
	FAILED_CHECK_RETURN(m_pEndSceneCommandList->Close(), E_FAIL);
	m_pEndSceneCommandList->SetName(L"COMMANDLIST_MULTITHREAD_END_SCENE");

	return S_OK;
}

HRESULT CRenderer::Reset_ThreadCommandList()
{
	// Thread CommandList.
	for (_int i = 0; i < CONTEXT::CONTEXT_END; ++i)
	{
		FAILED_CHECK_RETURN(m_arrShadowCommandAllocator[i]->Reset(), E_FAIL);
		FAILED_CHECK_RETURN(m_arrShadowCommandList[i]->Reset(m_arrShadowCommandAllocator[i], m_arrContextCurPipelineState[i]), E_FAIL);

		FAILED_CHECK_RETURN(m_arrSceneCommandAllocator[i]->Reset(), E_FAIL);
		FAILED_CHECK_RETURN(m_arrSceneCommandList[i]->Reset(m_arrSceneCommandAllocator[i], m_arrContextCurPipelineState[i]), E_FAIL);
	}

	// PreCommandList - ShadowDepth.
	FAILED_CHECK_RETURN(m_pPreShadowCommandAllocator->Reset(), E_FAIL);
	FAILED_CHECK_RETURN(m_pPreShadowCommandList->Reset(m_pPreShadowCommandAllocator, nullptr), E_FAIL);
	FAILED_CHECK_RETURN(m_pEndShadowCommandAllocator->Reset(), E_FAIL);
	FAILED_CHECK_RETURN(m_pEndShadowCommandList->Reset(m_pEndShadowCommandAllocator, nullptr), E_FAIL);

	// PreCommandList - Scene.
	FAILED_CHECK_RETURN(m_pPreSceneCommandAllocator->Reset(), E_FAIL);
	FAILED_CHECK_RETURN(m_pPreSceneCommandList->Reset(m_pPreSceneCommandAllocator, nullptr), E_FAIL);
	FAILED_CHECK_RETURN(m_pEndSceneCommandAllocator->Reset(), E_FAIL);
	FAILED_CHECK_RETURN(m_pEndSceneCommandList->Reset(m_pEndSceneCommandAllocator, nullptr), E_FAIL);

	return S_OK;
}

HRESULT CRenderer::Render_MultiThread(const _float& fTimeDelta)
{
	if (m_bIsLoadingFinish)
	{
		if (!m_bIsCreateThread)
		{
			m_bIsCreateThread = true;
			Create_ThreadContext();
		}

		FAILED_CHECK_RETURN(Reset_ThreadCommandList(), E_FAIL);

		/*__________________________________________________________________________________________________________
		[ Render ShadowDepth Pass ]
		____________________________________________________________________________________________________________*/
		// Begin Render ShadowDepth Pass.
		m_pTargetShadowDepth->Begin_RenderTargetOnContext(m_pPreShadowCommandList, THREADID::SHADOW);

		// Start WorkThread.
		// None-Signal -> Signal
		for (_int i = 0; i < CONTEXT::CONTEXT_END; ++i)
			SetEvent(m_hWorkerBeginRender[i]);

		// ShadowDepth가 전부 그려질 때 까지 대기하자. 전부 그려졌으면 명령 제출.
		WaitForMultipleObjects(CONTEXT::CONTEXT_END, m_hWorkerFinishShadow, TRUE, INFINITE);

		// End Render ShadowDepth Pass.
		m_pTargetShadowDepth->End_RenderTargetOnContext(m_pEndShadowCommandList, THREADID::SHADOW);
		

		// Submit Begin RenderShadow Pass.
		m_pPreShadowCommandList->Close();
		ID3D12CommandList* ppPreShadowCommandLists[] = { m_pPreShadowCommandList };
		CGraphicDevice::Get_Instance()->Get_CommandQueue()->ExecuteCommandLists(_countof(ppPreShadowCommandLists), ppPreShadowCommandLists);

		// Submit RenderShadow Pass.
		ID3D12CommandList* ppShadowCommandLists[CONTEXT::CONTEXT_END] = 
		{ 
			m_arrShadowCommandList[CONTEXT::CONTEXT0],
			m_arrShadowCommandList[CONTEXT::CONTEXT1],
			m_arrShadowCommandList[CONTEXT::CONTEXT2],
			m_arrShadowCommandList[CONTEXT::CONTEXT3]
		};
		CGraphicDevice::Get_Instance()->Get_CommandQueue()->ExecuteCommandLists(_countof(ppShadowCommandLists), ppShadowCommandLists);

		// Submit End RenderShadow Pass.
		m_pEndShadowCommandList->Close();
		ID3D12CommandList* ppEndShadowCommandLists[] = { m_pEndShadowCommandList };
		CGraphicDevice::Get_Instance()->Get_CommandQueue()->ExecuteCommandLists(_countof(ppEndShadowCommandLists), ppEndShadowCommandLists);


		/*__________________________________________________________________________________________________________
		[ Render Scene Pass ]
		____________________________________________________________________________________________________________*/
		// Begin Render Scene Pass.
		m_pTargetDeferred->Begin_RenderTargetOnContext(m_pPreSceneCommandList, THREADID::SCENE);
		
		// Scene이 전부 그려질 때 까지 대기하자. 전부 그려졌으면 명령 제출.
		WaitForMultipleObjects(CONTEXT::CONTEXT_END, m_hWorkerFinishedRender, TRUE, INFINITE);

		// End Render Scene Pass.
		m_pTargetDeferred->End_RenderTargetOnContext(m_pEndSceneCommandList, THREADID::SCENE);

		// Submit Begin RenderScene Pass.
		m_pPreSceneCommandList->Close();
		ID3D12CommandList* ppPreSceneCommandLists[] = { m_pPreSceneCommandList };
		CGraphicDevice::Get_Instance()->Get_CommandQueue()->ExecuteCommandLists(_countof(ppPreSceneCommandLists), ppPreSceneCommandLists);

		// Submit RenderScene Pass.
		ID3D12CommandList* ppSceneCommandLists[CONTEXT::CONTEXT_END] = 
		{
			m_arrSceneCommandList[CONTEXT::CONTEXT0],
			m_arrSceneCommandList[CONTEXT::CONTEXT1],
			m_arrSceneCommandList[CONTEXT::CONTEXT2],
			m_arrSceneCommandList[CONTEXT::CONTEXT3]
		};
		CGraphicDevice::Get_Instance()->Get_CommandQueue()->ExecuteCommandLists(_countof(ppSceneCommandLists), ppSceneCommandLists);

		// Submit End RenderScene Pass.
		m_pEndSceneCommandList->Close();
		ID3D12CommandList* ppEndSceneCommandLists[] = { m_pEndSceneCommandList };
		CGraphicDevice::Get_Instance()->Get_CommandQueue()->ExecuteCommandLists(_countof(ppEndSceneCommandLists), ppEndSceneCommandLists);
	}

	return S_OK;
}


void CRenderer::Worker_Thread(_int threadIndex)
{
	while (threadIndex >= 0 && threadIndex < CONTEXT::CONTEXT_END)
	{
		/*__________________________________________________________________________________________________________
		[ Wait for main thread to tell us to draw. ]
		메인 쓰레드가 우리에게 그리라고 말할 때까지 기다려라.
		____________________________________________________________________________________________________________*/
		WaitForSingleObject(m_hWorkerBeginRender[threadIndex], INFINITE);

		/*__________________________________________________________________________________________________________
		[ Render ShadowDepth ]
		____________________________________________________________________________________________________________*/
		// Bind RenderTarget.
		m_pTargetShadowDepth->Bind_RenderTargetOnContext(m_arrShadowCommandList[threadIndex], THREADID::SHADOW);

		// Start Render ShadowDepth.
		for (_int i = threadIndex; i < m_RenderList[RENDER_NONALPHA].size(); i += CONTEXT::CONTEXT_END)
		{
			m_RenderList[RENDER_NONALPHA][i]->Render_ShadowDepth(CTimerMgr::Get_Instance()->Get_TimeDelta(L"Timer_TimeDelta"),
																 m_arrShadowCommandList[threadIndex], 
																 threadIndex);
		}

		// Render Shadow Instance
		CShaderShadowInstancing::Get_Instance()->Render_Instance(m_arrShadowCommandList[threadIndex], threadIndex);

		// Submit Shadow Pass.
		m_arrShadowCommandList[threadIndex]->Close();
		
		// End Render ShadowDepth.
		// None-Signal -> Signal
		SetEvent(m_hWorkerFinishShadow[threadIndex]);



		/*__________________________________________________________________________________________________________
		[ Render Scene ]
		These can only be sent after the shadow passes for this frame have been submitted.
		이 프레임에 대해서는 쉐도우 패스를 제출한 후에만 보낼 수 있다.
		____________________________________________________________________________________________________________*/
		// Bind RenderTarget
		m_pTargetDeferred->Bind_RenderTargetOnContext(m_arrSceneCommandList[threadIndex], THREADID::SCENE);

		// Start Render Scene.
		for (_int i = threadIndex; i < m_RenderList[RENDER_NONALPHA].size(); i += CONTEXT::CONTEXT_END)
		{
			m_RenderList[RENDER_NONALPHA][i]->Render_GameObject(CTimerMgr::Get_Instance()->Get_TimeDelta(L"Timer_TimeDelta"), 
																m_arrSceneCommandList[threadIndex],
																threadIndex);
		}

		// Render Mesh Instance
		CShaderMeshInstancing::Get_Instance()->Render_Instance(m_arrSceneCommandList[threadIndex], threadIndex);

		// End Scene Pass.
		m_arrSceneCommandList[threadIndex]->Close();
	
		// Tell main thread that we are done.
		// 메인 쓰레드에 우리는 끝났다고 말해라.
		// None-Signal -> Signal
		SetEvent(m_hWorkerFinishedRender[threadIndex]);
	}

}

void CRenderer::Free()
{
#ifdef ENGINE_LOG
	COUT_STR("Destroy Renderer");
#endif

	Clear_RenderGroup();

	Safe_Release(m_pTargetDeferred);
	Safe_Release(m_pTargetLight);
	Safe_Release(m_pTargetShadowDepth);
	

	Safe_Release(m_pBlendBuffer);
	Safe_Release(m_pBlendShader);

	//Luminance
	Safe_Release(m_pTargetLuminance);
	Safe_Release(m_pLuminanceShader);
	Safe_Release(m_pLuminanceBuffer);
	//DownSampling
	Safe_Release(m_pTargetDownSampling);
	Safe_Release(m_pDownSamplingShader);
	Safe_Release(m_pDownSamplingBuffer);
	//Blur
	Safe_Release(m_pTargetBlur);
	Safe_Release(m_pBlurShader);
	Safe_Release(m_pBlurBuffer);
	//SSAO
	Safe_Release(m_pTargetSSAO);
	Safe_Release(m_pSSAOShader);
	Safe_Release(m_pSSAOBuffer);

	//Distort
	Safe_Release(m_pTargetDistortion);

	/*__________________________________________________________________________________________________________
	2020.06.07 MultiThreadRendering
	- CommandAllocators & CommandList 제거.
	____________________________________________________________________________________________________________*/
	for (_uint i = 0; i < CONTEXT::CONTEXT_END; ++i)
	{
		Safe_Release(m_arrShadowCommandAllocator[i]);
		Safe_Release(m_arrSceneCommandAllocator[i]);
		Safe_Release(m_arrShadowCommandList[i]);
		Safe_Release(m_arrSceneCommandList[i]);
	}

	Safe_Release(m_pPreShadowCommandAllocator);
	Safe_Release(m_pPreShadowCommandList);
	Safe_Release(m_pEndShadowCommandAllocator);
	Safe_Release(m_pEndShadowCommandList);

	Safe_Release(m_pPreSceneCommandAllocator);
	Safe_Release(m_pPreSceneCommandList);
	Safe_Release(m_pEndSceneCommandAllocator);
	Safe_Release(m_pEndSceneCommandList);

	CGraphicDevice::Get_Instance()->Wait_ForGpuComplete();

	if (m_bIsCreateThread)
	{
		for (_int i = 0; i < CONTEXT::CONTEXT_END; ++i)
		{
			CloseHandle(m_hWorkerBeginRender[i]);
			CloseHandle(m_hWorkerFinishShadow[i]);
			CloseHandle(m_hWorkerFinishedRender[i]);
			CloseHandle(m_hThreadHandle[i]);
		}
	}
}
