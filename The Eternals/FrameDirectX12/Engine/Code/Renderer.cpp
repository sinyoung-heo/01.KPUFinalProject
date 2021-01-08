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
	[ PipelineState�� �ٲ���� ���� SetPipelineState ���� ]
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
	[ PipelineState�� �ٲ���� ���� SetPipelineState ���� ]
	____________________________________________________________________________________________________________*/
	if (m_arrContextPrePipelineState[iContextIdx] != m_arrContextCurPipelineState[iContextIdx])
	{
		m_arrContextPrePipelineState[iContextIdx] = m_arrContextCurPipelineState[iContextIdx];

		CGraphicDevice::Get_Instance()->Set_PipelineState(m_arrContextCurPipelineState[iContextIdx]);
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

	m_mapRenderOnOff[L"RenderTarget"]	= true;
	m_mapRenderOnOff[L"Font"]			= true;
	m_mapRenderOnOff[L"Collider"]		= true;

	/*__________________________________________________________________________________________________________
	2020.06.07 MultiThreadRendering
	Thread CommandAllocators & CommandList ����.
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

HRESULT CRenderer::Render_Renderer(const _float& fTimeDelta)
{
	/*__________________________________________________________________________________________________________
	- GPU�� ��� ���� ����Ʈ�� ������ �� ���� ��ٸ���.
	____________________________________________________________________________________________________________*/
	FAILED_CHECK_RETURN(CGraphicDevice::Get_Instance()->Wait_ForGpuComplete(), E_FAIL);

	// [ Render Begin ]
	FAILED_CHECK_RETURN(CGraphicDevice::Get_Instance()->Render_Begin(_rgba(0.0f, 0.0f, 0.0f, 1.0f)), E_FAIL);

	/*__________________________________________________________________________________________________________
	2020.06.07 MultiThreadRendering
	LoadingThread�� ������ ���� �����带 �������� �Ѵ�.
	� ���������� �𸣰����� �̸� �����θ� Work_Trhead�Լ� ������ �ȵȴ�.
	____________________________________________________________________________________________________________*/
	if (m_bIsLoadingFinish)
	{
		// 2020.06.07 MultiThreadRendering.
		if (!m_bIsCreateThread)
		{
			m_bIsCreateThread = true;
			Create_ThreadContext();
		}

		// 2020.06.08 MultiThreadRendering
		FAILED_CHECK_RETURN(Reset_ThreadCommandList(), E_FAIL);

		// 2020.06.08 MultiThreadRendering
		/*__________________________________________________________________________________________________________
		[ Render ShadowDepth Pass ]
		____________________________________________________________________________________________________________*/
		// Begin Render ShadowDepth Pass.
		m_pShadowDepthTarget->Begin_RenderTargetOnContext(m_pPreShadowCommandList, THREADID::SHADOW);

		// Start WorkThread.
		// None-Signal -> Signal
		for (_int i = 0; i < CONTEXT::CONTEXT_END; ++i)
			SetEvent(m_hWorkerBeginRender[i]);

		// 2020.06.08 MultiThreadRendering
		// ShadowDepth�� ���� �׷��� �� ���� �������. ���� �׷������� ���� ����.
		WaitForMultipleObjects(CONTEXT::CONTEXT_END, m_hWorkerFinishShadow, TRUE, INFINITE);

		// End Render ShadowDepth Pass.
		m_pShadowDepthTarget->End_RenderTargetOnContext(m_pEndShadowCommandList, THREADID::SHADOW);
		

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
		m_pDeferredTarget->Begin_RenderTargetOnContext(m_pPreSceneCommandList, THREADID::SCENE);
		
		// 2020.06.13 MultiThreadRendering
		// Scene�� ���� �׷��� �� ���� �������. ���� �׷������� ���� ����.
		WaitForMultipleObjects(CONTEXT::CONTEXT_END, m_hWorkerFinishedRender, TRUE, INFINITE);

		// End Render Scene Pass.
		m_pDeferredTarget->End_RenderTargetOnContext(m_pEndSceneCommandList, THREADID::SCENE);

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

	Render_Priority(fTimeDelta);
	// Render_ShadowDepth(fTimeDelta);	// Shadow Depth
	// Render_NonAlpha(fTimeDelta);		// Diffuse, Normal, Specular, Depth
	Render_Light();						// Shade, Specular
	Render_Blend();						// Target Blend
	Render_Alpha(fTimeDelta);
	Render_Collider(fTimeDelta);		// Collider Render
	Render_UI(fTimeDelta);
	Render_RenderTarget();				// Debug RenderTarget

	// [ Excute CommandList ]
	FAILED_CHECK_RETURN(CGraphicDevice::Get_Instance()->Render_ExcuteCmdList(), E_FAIL);
	
	Render_Font(fTimeDelta);			// 11on12 Text Render

	// [ Render End ]
	FAILED_CHECK_RETURN(CGraphicDevice::Get_Instance()->Render_End(), E_FAIL);
	Clear_RenderGroup();

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
	m_pShadowDepthTarget->SetUp_ShadowDepthOnGraphicDevice();

	for (auto& pGameObject : m_RenderList[RENDER_NONALPHA])
		pGameObject->Render_ShadowDepth(fTimeDelta);

	m_pShadowDepthTarget->Release_ShadowDepthOnGraphicDevice();
}


void CRenderer::Render_NonAlpha(const _float& fTimeDelta)
{
	m_pDeferredTarget->SetUp_OnGraphicDevice();

	for (auto& pGameObject : m_RenderList[RENDER_NONALPHA])
		pGameObject->Render_GameObject(fTimeDelta);

	m_pDeferredTarget->Release_OnGraphicDevice();
}

void CRenderer::Render_Light()
{
	m_pLightTarget->SetUp_LightOnGraphicDevice();

	CLightMgr::Get_Instance()->Render_Light(m_pDeferredTarget->Get_TargetTexture());

	m_pLightTarget->Release_OnGraphicDevice();

}

void CRenderer::Render_Blend()
{
	if (!m_bIsSetBlendTexture)
	{
		m_bIsSetBlendTexture = true;

		vector<ComPtr<ID3D12Resource>> vecDeferredTarget	= m_pDeferredTarget->Get_TargetTexture();
		vector<ComPtr<ID3D12Resource>> vecShadeTarget		= m_pLightTarget->Get_TargetTexture();

		vector<ComPtr<ID3D12Resource>> vecBlendTarget;
		vecBlendTarget.emplace_back(vecDeferredTarget[0]);	// RenderTarget - Diffuse
		vecBlendTarget.emplace_back(vecShadeTarget[0]);		// RenderTarget - Shade
		vecBlendTarget.emplace_back(vecShadeTarget[1]);		// RenderTarget - Specular

		m_pBlendShader->SetUp_ShaderTexture(vecBlendTarget);
	}

	m_pBlendShader->Begin_Shader();
	m_pBlendBuffer->Begin_Buffer();

	m_pBlendBuffer->Render_Buffer();

}

void CRenderer::Render_Alpha(const _float& fTimeDelta)
{
	sort(m_RenderList[RENDER_ALPHA].begin(), m_RenderList[RENDER_ALPHA].end(), [](CGameObject* pSour, CGameObject* pDest)->_bool 
		{ 
			return pSour->Get_DepthOfView() > pDest->Get_DepthOfView(); 
		});

	//m_RenderList[RENDER_ALPHA].sort([](CGameObject* pSour, CGameObject* pDest)->_bool 
	//								{ 
	//									return pSour->Get_DepthOfView() > pDest->Get_DepthOfView(); 
	//								});

	for (auto& pGameObject : m_RenderList[RENDER_ALPHA])
		pGameObject->Render_GameObject(fTimeDelta);
}

void CRenderer::Render_UI(const _float& fTimeDelta)
{
	//m_RenderList[RENDER_UI].sort([](CGameObject* pSour, CGameObject* pDest)->_bool
	//	{
	//		return pSour->Get_UIDepth() < pDest->Get_UIDepth(); 
	//	});

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
		if (nullptr != m_pDeferredTarget)
			m_pDeferredTarget->Render_RenderTarget();

		if (nullptr != m_pLightTarget)
			m_pLightTarget->Render_RenderTarget();

		if (nullptr != m_pShadowDepthTarget)
			m_pShadowDepthTarget->Render_RenderTarget();
	}

}

void CRenderer::Render_Font(const _float & fTimeDelta)
{
	if (!m_mapRenderOnOff[L"Font"])
		return;

	CGraphicDevice::Get_Instance()->Render_TextBegin();

	for (auto& pGameObject : m_RenderList[RENDER_FONT])
		pGameObject->Render_GameObject(fTimeDelta);

	CGraphicDevice::Get_Instance()->Render_TextEnd();
}


HRESULT CRenderer::Ready_ShaderPrototype()
{
	CComponent*	pShader	= nullptr;

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

	// ShaderMesh
	pShader = CShaderMesh::Create(m_pGraphicDevice, m_pCommandList);
	NULL_CHECK_RETURN(pShader, E_FAIL);
	FAILED_CHECK_RETURN(m_pComponentMgr->Add_ComponentPrototype(L"ShaderMesh", ID_STATIC, pShader), E_FAIL);
	++m_uiCnt_ShaderFile;

	// ShaderSKyBox
	pShader = CShaderSkyBox::Create(m_pGraphicDevice, m_pCommandList);
	NULL_CHECK_RETURN(pShader, E_FAIL);
	FAILED_CHECK_RETURN(m_pComponentMgr->Add_ComponentPrototype(L"ShaderSkyBox", ID_STATIC, pShader), E_FAIL);
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

	// ShaderShadow
	pShader = CShaderShadow::Create(m_pGraphicDevice, m_pCommandList);
	NULL_CHECK_RETURN(pShader, E_FAIL);
	FAILED_CHECK_RETURN(m_pComponentMgr->Add_ComponentPrototype(L"ShaderShadow", ID_STATIC, pShader), E_FAIL);
	++m_uiCnt_ShaderFile;

	return S_OK;
}

HRESULT CRenderer::Ready_RenderTarget()
{
	/*__________________________________________________________________________________________________________
	[ Deferred RenderTarget ]
	____________________________________________________________________________________________________________*/
	// Diffuse, Normal, Specular, Depth
	m_pDeferredTarget = CRenderTarget::Create(m_pGraphicDevice, m_pCommandList, 4);
	NULL_CHECK_RETURN(m_pDeferredTarget, E_FAIL);

	m_pDeferredTarget->Set_TargetClearColor(0, _rgba(0.0f, 0.0f, 0.0f, 0.0f), DXGI_FORMAT_R8G8B8A8_UNORM);		// Diffuse
	m_pDeferredTarget->Set_TargetClearColor(1, _rgba(0.0f, 0.0f, 0.0f, 1.0f), DXGI_FORMAT_R8G8B8A8_UNORM);		// Normal
	m_pDeferredTarget->Set_TargetClearColor(2, _rgba(0.0f, 0.0f, 0.0f, 0.0f), DXGI_FORMAT_R8G8B8A8_UNORM);		// Specular
	m_pDeferredTarget->Set_TargetClearColor(3, _rgba(1.0f, 1.0f, 1.0f, 1.0f), DXGI_FORMAT_R32G32B32A32_FLOAT);	// Depth
	FAILED_CHECK_RETURN(m_pDeferredTarget->SetUp_DefaultSetting(), E_FAIL);
	FAILED_CHECK_RETURN(m_pDeferredTarget->Create_TextureDescriptorHeap(), E_FAIL);
	m_pDeferredTarget->SetUp_ShaderConstantBuffer();

	/*__________________________________________________________________________________________________________
	[ Light RnderTarget ]
	____________________________________________________________________________________________________________*/
	m_pLightTarget = CRenderTarget::Create(m_pGraphicDevice, m_pCommandList, 2);
	NULL_CHECK_RETURN(m_pLightTarget, E_FAIL);

	m_pLightTarget->Set_TargetClearColor(0, _rgba(0.0f, 0.0f, 0.0f, 1.0f), DXGI_FORMAT_R32G32B32A32_FLOAT);	// Shade
	m_pLightTarget->Set_TargetClearColor(1, _rgba(0.0f, 0.0f, 0.0f, 0.0f), DXGI_FORMAT_R8G8B8A8_UNORM);		// Specular
	FAILED_CHECK_RETURN(m_pLightTarget->SetUp_LightSetting(), E_FAIL);
	FAILED_CHECK_RETURN(m_pLightTarget->Create_TextureDescriptorHeap(), E_FAIL);
	m_pLightTarget->SetUp_ShaderConstantBuffer();
	m_pLightTarget->Set_TargetRenderPos(_vec3(480.0f, 90.0f /*+ (90.0f * 8.0f)*/, 1.0f));

	/*__________________________________________________________________________________________________________
	[ ShadowDepth RenderTarget ]
	____________________________________________________________________________________________________________*/
	m_pShadowDepthTarget = CRenderTarget::Create(m_pGraphicDevice, m_pCommandList, 1);
	NULL_CHECK_RETURN(m_pShadowDepthTarget, E_FAIL);

	m_pShadowDepthTarget->Set_TargetClearColor(0, _rgba(1.0f, 1.0f, 1.0f, 1.0f), DXGI_FORMAT_R32G32B32A32_FLOAT);
	m_pShadowDepthTarget->Set_TargetTextureSize(0, SHADOWTEX_WIDTH, SHADOWTEX_HEIGHT);
	FAILED_CHECK_RETURN(m_pShadowDepthTarget->SetUp_ShadowDepthSetting(), E_FAIL);
	FAILED_CHECK_RETURN(m_pShadowDepthTarget->Create_TextureDescriptorHeap(), E_FAIL);
	m_pShadowDepthTarget->SetUp_ShaderConstantBuffer();
	m_pShadowDepthTarget->Set_TargetRenderScale(_vec3(90.0f, 90.0f, 90.0f));
	m_pShadowDepthTarget->Set_TargetRenderPos(_vec3(720.0f, 90.0f, 1.0f));

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
		m_hWorkerBeginRender[i]		= CreateEvent(NULL,	// �ڵ� ��Ӱ� ���� ��ũ����.
												  FALSE,// TRUE�� ���� ����, FALSE�� �ڵ� ���� �̺�Ʈ.
												  FALSE,// TRUE�� ��ȣ, FALSE�� ���ȣ ���·� ����.
												  NULL);// �̺�Ʈ�� �ο��� �̸�.
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
	FAILED_CHECK_RETURN(m_pEndShadowCommandList->Close(), E_FAIL);


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
	FAILED_CHECK_RETURN(m_pEndSceneCommandList->Close(), E_FAIL);

	return S_OK;
}

HRESULT CRenderer::Reset_ThreadCommandList()
{
	// Thread CommandList.
	for (_int i = 0; i < CONTEXT::CONTEXT_END; ++i)
	{
		FAILED_CHECK_RETURN(m_arrShadowCommandAllocator[i]->Reset(), E_FAIL);
		FAILED_CHECK_RETURN(m_arrShadowCommandList[i]->Reset(m_arrShadowCommandAllocator[i], m_pCurPipelineState), E_FAIL);

		FAILED_CHECK_RETURN(m_arrSceneCommandAllocator[i]->Reset(), E_FAIL);
		FAILED_CHECK_RETURN(m_arrSceneCommandList[i]->Reset(m_arrSceneCommandAllocator[i], m_pCurPipelineState), E_FAIL);
	}

	// PreCommandList - ShadowDepth.
	FAILED_CHECK_RETURN(m_pPreShadowCommandAllocator->Reset(), E_FAIL);
	FAILED_CHECK_RETURN(m_pPreShadowCommandList->Reset(m_pPreShadowCommandAllocator, m_pCurPipelineState), E_FAIL);
	FAILED_CHECK_RETURN(m_pEndShadowCommandAllocator->Reset(), E_FAIL);
	FAILED_CHECK_RETURN(m_pEndShadowCommandList->Reset(m_pEndShadowCommandAllocator, m_pCurPipelineState), E_FAIL);

	// PreCommandList - Scene.
	FAILED_CHECK_RETURN(m_pPreSceneCommandAllocator->Reset(), E_FAIL);
	FAILED_CHECK_RETURN(m_pPreSceneCommandList->Reset(m_pPreSceneCommandAllocator, m_pCurPipelineState), E_FAIL);
	FAILED_CHECK_RETURN(m_pEndSceneCommandAllocator->Reset(), E_FAIL);
	FAILED_CHECK_RETURN(m_pEndSceneCommandList->Reset(m_pEndSceneCommandAllocator, m_pCurPipelineState), E_FAIL);

	return S_OK;
}


void CRenderer::Worker_Thread(_int threadIndex)
{
	while (threadIndex >= 0 && threadIndex < CONTEXT::CONTEXT_END)
	{
		/*__________________________________________________________________________________________________________
		[ Wait for main thread to tell us to draw. ]
		���� �����尡 �츮���� �׸���� ���� ������ ��ٷ���.
		____________________________________________________________________________________________________________*/
		WaitForSingleObject(m_hWorkerBeginRender[threadIndex], INFINITE);

		/*__________________________________________________________________________________________________________
		[ Render ShadowDepth ]
		____________________________________________________________________________________________________________*/
		// Bind RenderTarget.
		m_pShadowDepthTarget->Bind_RenderTargetOnContext(m_arrShadowCommandList[threadIndex], THREADID::SHADOW);

		// Start Render ShadowDepth.
		for (_int i = threadIndex; i < m_RenderList[RENDER_NONALPHA].size(); i += CONTEXT::CONTEXT_END)
		{
			m_RenderList[RENDER_NONALPHA][i]->Render_ShadowDepth(CTimerMgr::Get_Instance()->Get_TimeDelta(L"Timer_TimeDelta"),
																 m_arrShadowCommandList[threadIndex], 
																 threadIndex);
		}
		
		// Submit Shadow Pass.
		m_arrShadowCommandList[threadIndex]->Close();
		
		// End Render ShadowDepth.
		// None-Signal -> Signal
		SetEvent(m_hWorkerFinishShadow[threadIndex]);





		/*__________________________________________________________________________________________________________
		[ Render Scene ]
		These can only be sent after the shadow passes for this frame have been submitted.
		�� �����ӿ� ���ؼ��� ������ �н��� ������ �Ŀ��� ���� �� �ִ�.
		____________________________________________________________________________________________________________*/
		// Bind RenderTarget
		m_pDeferredTarget->Bind_RenderTargetOnContext(m_arrSceneCommandList[threadIndex], THREADID::SCENE);

		// Start Render Scene.
		for (_int i = threadIndex; i < m_RenderList[RENDER_NONALPHA].size(); i += CONTEXT::CONTEXT_END)
		{
			m_RenderList[RENDER_NONALPHA][i]->Render_GameObject(CTimerMgr::Get_Instance()->Get_TimeDelta(L"Timer_TimeDelta"), 
																m_arrSceneCommandList[threadIndex],
																threadIndex);
		}

		// End Scene Pass.
		m_arrSceneCommandList[threadIndex]->Close();
	
		// Tell main thread that we are done.
		// ���� �����忡 �츮�� �����ٰ� ���ض�.
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

	Safe_Release(m_pDeferredTarget);
	Safe_Release(m_pLightTarget);
	Safe_Release(m_pShadowDepthTarget);

	Safe_Release(m_pBlendBuffer);
	Safe_Release(m_pBlendShader);

	/*__________________________________________________________________________________________________________
	2020.06.07 MultiThreadRendering
	- CommandAllocators & CommandList ����.
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

	for (_int i = 0; i < CONTEXT::CONTEXT_END; ++i)
	{
		CloseHandle(m_hWorkerBeginRender[i]);
		CloseHandle(m_hWorkerFinishShadow[i]);
		CloseHandle(m_hWorkerFinishedRender[i]);
		CloseHandle(m_hThreadHandle[i]);
	}

}