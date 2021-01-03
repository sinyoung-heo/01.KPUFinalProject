#include "GraphicDevice.h"
#include "Renderer.h"

USING(Engine)
IMPLEMENT_SINGLETON(CGraphicDevice)


CGraphicDevice::CGraphicDevice()
{
	for (_int i = 0; i < SWAPCHAIN::BUFFER_END; i++)
		m_arrSwapChainBuffer[i] = nullptr;

	for (_int i = 0; i < CMDID::CMD_END; ++i)
	{
		m_arrCommandList[i]			= nullptr;
		m_arrCommandAllocator[i]	= nullptr;
	}

	for (_int i = 0; i < MATRIXID::MATRIX_END; ++i)
		m_arrTransform[i] = nullptr;

	ZeroMemory(&m_Viewport, sizeof(D3D12_VIEWPORT));
	ZeroMemory(&m_ScissorRect, sizeof(D3D12_RECT));
}


HRESULT CGraphicDevice::Ready_GraphicDevice(HWND hWnd,
											HANDLE hHandle,
											WINMODE eMode, 
											const _uint & iWidth, 
											const _uint & iHeight)
{
	// DirectX 12 Device
	FAILED_CHECK_RETURN(Create_GraphicDevice(iWidth, iHeight), E_FAIL);
	FAILED_CHECK_RETURN(Create_CommandQueueAndList(), E_FAIL);
	FAILED_CHECK_RETURN(Create_SwapChain(hWnd, eMode, iWidth, iHeight), E_FAIL);
	FAILED_CHECK_RETURN(Create_RtvAndDsvDescriptorHeaps(), E_FAIL);
	FAILED_CHECK_RETURN(Create_FenceObject(), E_FAIL);
	FAILED_CHECK_RETURN(Create_RenderTargetAndDepthStencilBuffer(iWidth, iHeight), E_FAIL);

	// DirectX 11 Device
	FAILED_CHECK_RETURN(Create_11On12GraphicDevice(), E_FAIL);

	return S_OK;
}

HRESULT CGraphicDevice::Render_Begin(const _rgba& vRGBA)
{
	/*__________________________________________________________________________________________________________
	- 명령 할당자와 명령 리스트 재설정.
	- 재설정은 GPU가 관련 명령 목록들을 모두 처리한 후에 일어난다.
	____________________________________________________________________________________________________________*/
	FAILED_CHECK_RETURN(m_arrCommandAllocator[CMDID::CMD_MAIN]->Reset(), E_FAIL);
	FAILED_CHECK_RETURN(m_arrCommandList[CMDID::CMD_MAIN]->Reset(m_arrCommandAllocator[CMDID::CMD_MAIN], m_pPipelineState), E_FAIL);


	/*__________________________________________________________________________________________________________
	- 뷰포트와 씨저 사각형을 설정한다. 
	- 명령 목록(CommandList)을 재설정할 때마다 이들도 재설정해야 한다.
	____________________________________________________________________________________________________________*/
	m_arrCommandList[CMDID::CMD_MAIN]->RSSetViewports(1, &m_Viewport);
	m_arrCommandList[CMDID::CMD_MAIN]->RSSetScissorRects(1, &m_ScissorRect);


	/*__________________________________________________________________________________________________________
	Indicate a state transition on the resource usage.
	- 자원 용도에 관련된 상태전이를 Direct3D에 통지한다.
	- 현재 렌더 타겟에 대한 프리젠트가 끝나기를 기다린다. 
	- 프리젠트가 끝나면 렌더 타겟 버퍼의 상태는 
	  프리젠트 상태(D3D12_RESOURCE_STATE_PRESENT)에서 렌더 타겟 상태(D3D12_RESOURCE_STATE_RENDER_TARGET)로 바뀔 것이다.
	____________________________________________________________________________________________________________*/
	m_RenterTargetResourceBarrier.Transition.pResource   = m_arrSwapChainBuffer[m_iCurrBackBuffer];
	m_RenterTargetResourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
	m_RenterTargetResourceBarrier.Transition.StateAfter  = D3D12_RESOURCE_STATE_RENDER_TARGET;
	m_arrCommandList[CMDID::CMD_MAIN]->ResourceBarrier(1, &m_RenterTargetResourceBarrier);

	/*__________________________________________________________________________________________________________
	 Clear the back buffer and depth buffer.
	- 현재의 렌더 타겟에 해당하는 서술자의 CPU 주소(핸들)를 계산.
	- 원하는 색상으로 렌더 타겟(뷰)을 지운다.
	____________________________________________________________________________________________________________*/
	_float arrClearColor[4] = { vRGBA.x, vRGBA.y, vRGBA.z, vRGBA.w };
	m_arrCommandList[CMDID::CMD_MAIN]->ClearRenderTargetView(CD3DX12_CPU_DESCRIPTOR_HANDLE(m_pRTV_Heap->GetCPUDescriptorHandleForHeapStart(),
																						   m_iCurrBackBuffer,
																						   m_uiRTV_DescriptorSize),
															 arrClearColor,
															 0,
															 nullptr);

	/*__________________________________________________________________________________________________________
	- 깊이-스텐실 서술자의 CPU 주소를 계산.
	- 원하는 값으로 깊이-스텐실(뷰)을 지운다.
	____________________________________________________________________________________________________________*/
	// 2020.06.14 MultiThreadRendering.
	//m_arrCommandList[CMDID::CMD_MAIN]->ClearDepthStencilView(m_pDSV_Heap->GetCPUDescriptorHandleForHeapStart(),
	//														  D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 
	//														  1.0f,
	//														  0,
	//														  0, 
	//														  nullptr);

	/*__________________________________________________________________________________________________________
	Specify the buffers we are going to render to.
	- 렌더링 결과가 기록될 렌더 타겟 버퍼들을 지정한다.
	- RTV(서술자)와 DSV(서술자)를 출력-병합 단계(OM)에 연결한다.
	- OMSetRenderTargets : 렌더링에 사용할 렌더 타겟과 깊이,스텐실 버퍼를 파이프라인에 묶는다.

	즉, 현재 설정된 렌더타겟 텍스쳐에 장면과 깊이를 그리기 시작.
	____________________________________________________________________________________________________________*/
	m_arrCommandList[CMDID::CMD_MAIN]->OMSetRenderTargets(1,
														  &CD3DX12_CPU_DESCRIPTOR_HANDLE(m_pRTV_Heap->GetCPUDescriptorHandleForHeapStart(),
														  								 m_iCurrBackBuffer,
														  								 m_uiRTV_DescriptorSize), 
														  true,
														  &m_pDSV_Heap->GetCPUDescriptorHandleForHeapStart());



	/* ------------------------------------ 렌더링 코드는 여기에 추가될 것이다. ----------------------------------- */



	return S_OK;
}

HRESULT CGraphicDevice::Render_ExcuteCmdList()
{
	/*__________________________________________________________________________________________________________
	 Indicate a state transition on the resource usage.
	- 자원 용도에 관련된 상태 전이를 Direct3D에 통지한다.
	- 현재 렌더 타겟에 대한 렌더링이 끝나기를 기다린다. 
	- GPU가 렌더 타겟(버퍼)을 더 이상 사용하지 않으면 렌더 타겟의 상태는 
	  프리젠트 상태(D3D12_RESOURCE_STATE_PRESENT)로 바뀔 것이다.
	____________________________________________________________________________________________________________*/
	if (false == CRenderer::Get_Instance()->Get_RenderOnOff(L"Font"))
	{
		m_RenterTargetResourceBarrier.Transition.pResource   = m_arrSwapChainBuffer[m_iCurrBackBuffer];
		m_RenterTargetResourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
		m_RenterTargetResourceBarrier.Transition.StateAfter  = D3D12_RESOURCE_STATE_PRESENT;
		m_arrCommandList[CMDID::CMD_MAIN]->ResourceBarrier(1, &m_RenterTargetResourceBarrier);
	}

	/*__________________________________________________________________________________________________________
	- 명령들의 기록을 마친다.
	- ExecuteCommandLists로 명령 목록을 제출하기 전에 
	  반드시 이 메서드를 이용해서 명령 목록을 닫아야 한다.
	____________________________________________________________________________________________________________*/
	// 2020.06.14 MultiThreadRendering.
	m_arrCommandList[CMDID::CMD_MAIN]->ClearDepthStencilView(m_pDSV_Heap->GetCPUDescriptorHandleForHeapStart(),
															 D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 
															 1.0f,
															 0,
															 0, 
															 nullptr);
	
	FAILED_CHECK_RETURN(m_arrCommandList[CMDID::CMD_MAIN]->Close(), E_FAIL);

	/*__________________________________________________________________________________________________________
	- 명령 리스트를 GPU큐에 제출한다.
	____________________________________________________________________________________________________________*/
	ID3D12CommandList* ppCommandLists[] = { m_arrCommandList[CMDID::CMD_MAIN] };
	m_pCommandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

	return S_OK;
}

HRESULT CGraphicDevice::Render_TextBegin()
{
	/*__________________________________________________________________________________________________________
	- 먼저 모든 D3D12 명령 목록을 실행하여 3D 장면을 렌더링한 다음, 그 위에 UI를 렌더링한다.
	다른 샘플에서는 명령 목록을 닫기 전에 일반적으로 리소스 장벽을 적용하여 렌더링 대상 상태에서 현재 상태로 백 버퍼를 전환합니다. 
	그러나 이 샘플에서는 D2D를 사용하여 백 버퍼로 계속 렌더링해야 하므로 해당 리소스 장벽을 제거합니다. 

	- 백 버퍼의 래핑된 리소스를 만들 때 렌더링 대상 상태를 "IN" 상태로 지정하고, 현재 상태를 "OUT" 상태로 지정.
	(m_arrWrappedBackBuffers를 PRESENT 상태로 래핑하여 ResourceBarrier - PRESENT 대체.)
	____________________________________________________________________________________________________________*/
	m_p11On12Device->AcquireWrappedResources(&m_arrWrappedBackBuffers[m_iCurrBackBuffer], 1);

	m_pD2D_Context->SetTarget(m_arrD2DRenderTargets[m_iCurrBackBuffer]);
	m_pD2D_Context->BeginDraw();
	m_pD2D_Context->SetTransform(D2D1::Matrix3x2F::Identity());

	return S_OK;
}

HRESULT CGraphicDevice::Render_TextEnd()
{
	/*__________________________________________________________________________________________________________
	- 마지막으로, 11On12 디바이스에서 수행된 모든 명령을 공유 ID3D12CommandQueue로 제출하기 위해 
	ID3D11DeviceContext에서 Flush를 호출해야 합니다.
	____________________________________________________________________________________________________________*/
	m_pD2D_Context->EndDraw();
	m_pD2D_Context->SetTarget(nullptr);

	m_p11On12Device->ReleaseWrappedResources(&m_arrWrappedBackBuffers[m_iCurrBackBuffer], 1);

	m_p11Context->Flush();

	return S_OK;
}

HRESULT CGraphicDevice::Render_End()
{
	/*__________________________________________________________________________________________________________
	- 스왑체인을 프리젠트한다. -> 후면 버퍼와 전면 버퍼를 교환.
	- 프리젠트를 하면 현재 렌더 타겟(후면버퍼)의 내용이 전면버퍼로 옮겨지고, 렌더 타겟 인덱스가 바뀔 것이다.
	____________________________________________________________________________________________________________*/
	FAILED_CHECK_RETURN(m_pSwapChain->Present(0, 0), E_FAIL);
	m_iCurrBackBuffer = (m_iCurrBackBuffer + 1) % SWAPCHAIN::BUFFER_END;

	return S_OK;
}

HRESULT CGraphicDevice::Wait_ForGpuComplete()
{
	/*__________________________________________________________________________________________________________
	 [ Advance the fence value to mark commands up to this fence point ]
	- CPU 펜스값 증가.
	- 현재 Fence 지점까지의 명령들을 표시하도록 울타리 값을 전진시킨다.
	____________________________________________________________________________________________________________*/
	m_uiCurrentFence++;

	/*__________________________________________________________________________________________________________
	  Add an instruction to the command queue to set a new fence point.
	 Because we are on the GPU timeline, the new fence point won't be set
	 until the GPU finishes processing all the commands prior to this Signal().

	- CPU가 펜스의 값을 설정하는 명령을 명령 큐에 추가.
	- 새 Fence 지점을 설정하는 명령(Signal)을 명령 대기열에 추가한다. 지금 우리는 GPU 시간선 상에 있으므로,
	새 Fence 지점은 GPU가 Signal()명령까지의 모든 명령을 처리하기 전까지는 설정되지 않는다.
	____________________________________________________________________________________________________________*/
	FAILED_CHECK_RETURN(m_pCommandQueue->Signal(m_pFence, m_uiCurrentFence), E_FAIL);

	/*__________________________________________________________________________________________________________
	[ Wait until the GPU has completed commands up to this fence point ]
	- GPU가 이 Fence 지점까지의 명령들을 완료할 때 까지 기다린다.
	____________________________________________________________________________________________________________*/
	if (m_pFence->GetCompletedValue() < m_uiCurrentFence)
	{
		HANDLE eventHandle = CreateEventEx(NULL, 
										   FALSE, // Event Name
										   FALSE, // Flags 
										   EVENT_ALL_ACCESS);

		// GPU가 현재 Fence 지점에 도달했으면 이벤트를 발동한다.
		FAILED_CHECK_RETURN(m_pFence->SetEventOnCompletion(m_uiCurrentFence, eventHandle), E_FAIL);


		// GPU가 현재 울타리 지점에 도달했음을 뜻하는 이벤트를 기다린다.
		WaitForSingleObject(eventHandle, INFINITE);
		CloseHandle(eventHandle);
	}

	return S_OK;
}

HRESULT CGraphicDevice::Begin_ResetCmdList(const CMDID& eCmdID)
{
	FAILED_CHECK_RETURN(m_arrCommandList[eCmdID]->Reset(m_arrCommandAllocator[eCmdID], nullptr), E_FAIL);

	return S_OK;
}

HRESULT CGraphicDevice::End_ResetCmdList(const CMDID& eCmdID)
{
	FAILED_CHECK_RETURN(m_arrCommandList[eCmdID]->Close(), E_FAIL);

	ID3D12CommandList* CommandLists[] = { m_arrCommandList[eCmdID] };
	m_pCommandQueue->ExecuteCommandLists(_countof(CommandLists), CommandLists);

	Wait_ForGpuComplete();

	return S_OK;
}

void CGraphicDevice::Begin_BackBufferSetting()
{
	m_RenterTargetResourceBarrier.Transition.pResource   = m_arrSwapChainBuffer[m_iCurrBackBuffer];
	m_RenterTargetResourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	m_RenterTargetResourceBarrier.Transition.StateAfter  = D3D12_RESOURCE_STATE_PRESENT;
	m_arrCommandList[CMDID::CMD_MAIN]->ResourceBarrier(1, &m_RenterTargetResourceBarrier);

}

void CGraphicDevice::End_BackBufferSetting()
{
	m_RenterTargetResourceBarrier.Transition.pResource   = m_arrSwapChainBuffer[m_iCurrBackBuffer];
	m_RenterTargetResourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
	m_RenterTargetResourceBarrier.Transition.StateAfter  = D3D12_RESOURCE_STATE_RENDER_TARGET;
	m_arrCommandList[CMDID::CMD_MAIN]->ResourceBarrier(1, &m_RenterTargetResourceBarrier);

	m_arrCommandList[CMDID::CMD_MAIN]->OMSetRenderTargets(1,
														  &CD3DX12_CPU_DESCRIPTOR_HANDLE(m_pRTV_Heap->GetCPUDescriptorHandleForHeapStart(),
														  							   m_iCurrBackBuffer,
														  							   m_uiRTV_DescriptorSize),
														  true,
														  &m_pDSV_Heap->GetCPUDescriptorHandleForHeapStart());

}

void CGraphicDevice::Clear_DepthStencilBuffer(const CMDID & eCmdID)
{
	m_arrCommandList[eCmdID]->ClearDepthStencilView(m_pDSV_Heap->GetCPUDescriptorHandleForHeapStart(),
													D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 
													1.0f,
													0,
													0, 
													nullptr);
}


HRESULT CGraphicDevice::Create_GraphicDevice(const _uint & iWidth, const _uint & iHeight)
{
#if defined(_DEBUG)
	ID3D12Debug* pDebugController = nullptr;
	D3D12GetDebugInterface(__uuidof(ID3D12Debug), (void**)&pDebugController);
	pDebugController->EnableDebugLayer();
#endif

	/*__________________________________________________________________________________________________________
	[ DXGI 팩토리를 생성 ]
	____________________________________________________________________________________________________________*/
	FAILED_CHECK_RETURN(CreateDXGIFactory1(IID_PPV_ARGS(&m_pFactory)), E_FAIL);

	/*__________________________________________________________________________________________________________
	 [ Try to create hardware device ]
	____________________________________________________________________________________________________________*/
	IDXGIAdapter1* pHardwareAdapter;
	Get_HardwareAdapter(m_pFactory, &pHardwareAdapter);

	HRESULT hResult = D3D12CreateDevice(pHardwareAdapter,
										D3D_FEATURE_LEVEL_12_0,
										IID_PPV_ARGS(&m_pGraphicDevice));

	/*__________________________________________________________________________________________________________
	[ Fallback to WARP device ]
	- 특성 레벨 12.0을 지원하는 하드웨어 디바이스를 생성할 수 없으면 WARP 디바이스를 생성.
	____________________________________________________________________________________________________________*/
	if(FAILED(hResult))
	{
		IDXGIAdapter* pWarpAdapter;
		FAILED_CHECK_RETURN(m_pFactory->EnumWarpAdapter(IID_PPV_ARGS(&pWarpAdapter)), E_FAIL);

		FAILED_CHECK_RETURN(D3D12CreateDevice(pWarpAdapter,
											  D3D_FEATURE_LEVEL_11_0,
											  IID_PPV_ARGS(&m_pGraphicDevice)), 
											  E_FAIL);
	}

	m_uiRTV_DescriptorSize			= m_pGraphicDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	m_uiDSV_DescriptorSize			= m_pGraphicDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
	m_uiCBV_SRV_UAV_DescriptorSize	= m_pGraphicDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

#if defined(_DEBUG)
	Log_Adapters();
	Safe_Release(pDebugController);
#endif

	return S_OK;
}

HRESULT CGraphicDevice::Create_CommandQueueAndList()
{
	D3D12_COMMAND_QUEUE_DESC QueueDesc = {};
	QueueDesc.Type	= D3D12_COMMAND_LIST_TYPE_DIRECT;
	QueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	FAILED_CHECK_RETURN(m_pGraphicDevice->CreateCommandQueue(&QueueDesc, IID_PPV_ARGS(&m_pCommandQueue)), E_FAIL);

	/*__________________________________________________________________________________________________________
	[ CommandList ]
	____________________________________________________________________________________________________________*/
	for (_int i = 0; i < CMDID::CMD_END; ++i)
	{
		FAILED_CHECK_RETURN(m_pGraphicDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, 
																	 IID_PPV_ARGS(&m_arrCommandAllocator[i])),
																	 E_FAIL);

		FAILED_CHECK_RETURN(m_pGraphicDevice->CreateCommandList(0,
																D3D12_COMMAND_LIST_TYPE_DIRECT,
																m_arrCommandAllocator[i],	// Associated command allocator
																nullptr,					// Initial PipelineStateObject
																IID_PPV_ARGS(&m_arrCommandList[i])),
																E_FAIL);

		/*__________________________________________________________________________________________________________
		Start off in a closed state.  
		This is because the first time we refer to the command list we will Reset it, 
		and it needs to be closed before calling Reset.
		____________________________________________________________________________________________________________*/
		m_arrCommandList[i]->Close();
	}

	m_arrCommandList[CMDID::CMD_MAIN]->SetName(L"COMMANDLIST_MAIN");
	m_arrCommandList[CMDID::CMD_LOADING]->SetName(L"COMMANDLIST_LOADING");

	return S_OK;
}

HRESULT CGraphicDevice::Create_SwapChain(HWND hWnd, WINMODE eMode, const _uint& iWidth, const _uint& iHeight)
{
	/*__________________________________________________________________________________________________________
	[ 4X MSAA 품질 수준 지원 점검 ]
	____________________________________________________________________________________________________________*/
	D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS MSAA_QualityLevels;
	ZeroMemory(&MSAA_QualityLevels, sizeof(D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS));

	MSAA_QualityLevels.Format			= DXGI_FORMAT_R8G8B8A8_UNORM;
	MSAA_QualityLevels.SampleCount		= 4;	//MSAA 4x 다중 샘플링
	MSAA_QualityLevels.Flags			= D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
	MSAA_QualityLevels.NumQualityLevels = 0;

	// 디바이스가 지원하는 다중 샘플의 품질 수준을 확인.
	FAILED_CHECK_RETURN(m_pGraphicDevice->CheckFeatureSupport(D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS,		// 다중 표본화 기능.
															  &MSAA_QualityLevels,												// 기능 지원 정보가 설정될 구조체 포인터.
															  sizeof(D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS)), E_FAIL);	// 구조체의 크기.

	m_uiMSAA4X_QualityLevels = MSAA_QualityLevels.NumQualityLevels;

	//다중 샘플의 품질 수준이 1보다 크면 다중 샘플링을 활성화.
	m_bIsMSAA4X_Enable = (m_uiMSAA4X_QualityLevels > 1) ? true : false;



	/*__________________________________________________________________________________________________________
	[ Release the previous swapchain we will be recreating ]
	____________________________________________________________________________________________________________*/
	DXGI_SWAP_CHAIN_DESC SwapChainDesc;
	SwapChainDesc.BufferDesc.Width						= iWidth;
	SwapChainDesc.BufferDesc.Height						= iHeight;
	SwapChainDesc.BufferDesc.RefreshRate.Numerator		= 60;
	SwapChainDesc.BufferDesc.RefreshRate.Denominator	= 1;
	SwapChainDesc.BufferDesc.Format						= DXGI_FORMAT_R8G8B8A8_UNORM;
	SwapChainDesc.BufferDesc.ScanlineOrdering			= DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	SwapChainDesc.BufferDesc.Scaling					= DXGI_MODE_SCALING_UNSPECIFIED;

	SwapChainDesc.SampleDesc.Count						= m_bIsMSAA4X_Enable ? 4 : 1;
	SwapChainDesc.SampleDesc.Quality					= m_bIsMSAA4X_Enable ? (m_uiMSAA4X_QualityLevels - 1) : 0;

	SwapChainDesc.BufferUsage							= DXGI_USAGE_RENDER_TARGET_OUTPUT;
	SwapChainDesc.BufferCount							= SWAPCHAIN::BUFFER_END;
	SwapChainDesc.OutputWindow							= hWnd;
	SwapChainDesc.Windowed								= _bool(eMode);
	SwapChainDesc.SwapEffect							= DXGI_SWAP_EFFECT_FLIP_DISCARD;
	SwapChainDesc.Flags									= DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	// Note: Swap chain uses queue to perform flush.
	FAILED_CHECK_RETURN(m_pFactory->CreateSwapChain(m_pCommandQueue,
													&SwapChainDesc, 
													&m_pSwapChain), 
													E_FAIL);

	return S_OK;
}

HRESULT CGraphicDevice::Create_RtvAndDsvDescriptorHeaps()
{
	D3D12_DESCRIPTOR_HEAP_DESC RTV_HeapDesc;
	RTV_HeapDesc.NumDescriptors	= SWAPCHAIN::BUFFER_END;
	RTV_HeapDesc.Type			= D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	RTV_HeapDesc.Flags			= D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	RTV_HeapDesc.NodeMask		= 0;
	FAILED_CHECK_RETURN(m_pGraphicDevice->CreateDescriptorHeap(&RTV_HeapDesc, IID_PPV_ARGS(&m_pRTV_Heap)), E_FAIL);

	D3D12_DESCRIPTOR_HEAP_DESC DSV_HeapDesc;
	DSV_HeapDesc.NumDescriptors	= 1;
	DSV_HeapDesc.Type			= D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	DSV_HeapDesc.Flags			= D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	DSV_HeapDesc.NodeMask		= 0;
	FAILED_CHECK_RETURN(m_pGraphicDevice->CreateDescriptorHeap(&DSV_HeapDesc, IID_PPV_ARGS(&m_pDSV_Heap)), E_FAIL);

	return S_OK;
}

HRESULT CGraphicDevice::Create_FenceObject()
{
	/*__________________________________________________________________________________________________________
	- 펜스와 동기화를 위한 이벤트 객체를 생성한다. (이벤트 객체의 초기값을 FALSE이다).
	- 이벤트가 실행되면 (Signal)이벤트의 값을 자동적으로 FALSE가 되도록 생성한다.
	____________________________________________________________________________________________________________*/
	FAILED_CHECK_RETURN(m_pGraphicDevice->CreateFence(0,
													  D3D12_FENCE_FLAG_NONE,
													  IID_PPV_ARGS(&m_pFence)), 
													  E_FAIL);

	return S_OK;
}

HRESULT CGraphicDevice::Create_11On12GraphicDevice()
{
	/*__________________________________________________________________________________________________________
	[ Create 11On12 state to enable D2D rendering on D3D12. ]

	- 첫 번째 단계는 ID3D12Device를 만든 후에 ID3D11On12Device를 만드는 것입니다.
	- 이때 API D3D11On12CreateDevice를 통해 ID3D12Device에 래핑되는 ID3D11Device를 만듭니다.
	- 이 API는 다른 매개 변수 중에서 ID3D12CommandQueue를 사용하므로 11On12 디바이스는 해당 명령을 제출할 수 있습니다.
	- ID3D11Device가 만들어지면 여기에서 ID3D11On12Device 인터페이스를 쿼리할 수 있습니다.
	- 이것은 D2D 설정에 사용할 기본 디바이스 개체입니다.
	____________________________________________________________________________________________________________*/
	IUnknown* pRenderCommandQueue = m_pCommandQueue;

	FAILED_CHECK_RETURN(D3D11On12CreateDevice(m_pGraphicDevice,					// D3D11 interop에 사용할 기존 D3D12 장치.
											  D3D11_CREATE_DEVICE_BGRA_SUPPORT,
											  nullptr,							// NULL을 하면 D3D12장치의 기능 수준.
											  0,								// 기능 레벨 배열의 크기(바이트).
											  &pRenderCommandQueue,				// D3D11on12에서 사용할 고유한 큐 배열.
											  1,								// 명령 큐 배열의 크기(바이트).
											  0,								// D3D12 장치의 노드. 1비트만 설정할 수 있음.								
											  &m_p11Device,						// 11Device
											  &m_p11Context,					// 11DeviceContext
											  nullptr), 
											  E_FAIL);

	FAILED_CHECK_RETURN(m_p11Device->QueryInterface(&m_p11On12Device), E_FAIL);

	/*__________________________________________________________________________________________________________
	[ Create D2D/DWrite components ]

	- D2D 팩토리 만들기.
	- 이제 11On12 디바이스가 있으므로, 이 디바이스를 사용하여 D3D11을 사용할 때처럼 D2D 팩터리 및 디바이스를 만듭니다.
	____________________________________________________________________________________________________________*/
	FAILED_CHECK_RETURN(DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, 
						__uuidof(m_pDWriteFactory), 
						(IUnknown**)&m_pDWriteFactory),
						E_FAIL);

	FAILED_CHECK_RETURN(D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED,
						IID_PPV_ARGS(&m_pD2D_Factory)),
						E_FAIL);
	
	
	D2D1_DEVICE_CONTEXT_OPTIONS DeviceOptions	= D2D1_DEVICE_CONTEXT_OPTIONS_NONE;
	IDXGIDevice*				pDxgiDevice		= nullptr;;

	FAILED_CHECK_RETURN(m_p11On12Device->QueryInterface(&pDxgiDevice), E_FAIL);
	
	FAILED_CHECK_RETURN(m_pD2D_Factory->CreateDevice(pDxgiDevice, &m_pD2D_Device), E_FAIL);
	Safe_Release(pDxgiDevice);
	
	FAILED_CHECK_RETURN(m_pD2D_Device->CreateDeviceContext(DeviceOptions, &m_pD2D_Context), E_FAIL);
	
	/*__________________________________________________________________________________________________________
	[ Cretae D2D RenderTarget ]

	- D3D12는 스왑 체인을 소유하므로, 11On12 디바이스(D2D 콘텐츠)를 사용하여 백 버퍼로 렌더링하려면,
	ID3D12Resource 형식의 백 버퍼에서 ID3D11Resource 형식의 래핑된 리소스를 만들어야 합니다.
	- 이렇게 하면 11On12 디바이스에 사용할 수 있도록 ID3D12Resource가 D3D11 기반 인터페이스에 연결됩니다.
	- 래핑된 리소스를 준비한 후에는 마찬가지로 LoadAssets 메서드에서 D2D가 렌더링될 렌더링 대상 화면을 만들 수 있습니다.
	____________________________________________________________________________________________________________*/

	/*__________________________________________________________________________________________________________
	- Query the desktop's dpi settings, which will be used to create D2D's render targets.
	____________________________________________________________________________________________________________*/
	_float DPIx = 0.0f;
	_float DPIy = 0.0f;
	m_pD2D_Factory->GetDesktopDpi(&DPIx, &DPIy);

	D2D1_BITMAP_PROPERTIES1 bitmapProperties = D2D1::BitmapProperties1(D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW,
																	   D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED),
																	   DPIx, DPIy);
	/*__________________________________________________________________________________________________________
	- For each buffer in the swapchain, we need to create a wrapped resource, 
	and create a D2D render target object to enable D2D rendering for the UI.

	- D3D12는 스왑 체인을 소유하므로, 11On12 디바이스(D2D 콘텐츠)를 사용하여 백 버퍼로 렌더링하려면 
	ID3D12Resource 형식의 백 버퍼에서 ID3D11Resource 형식의 래핑된 리소스를 만들어야 합니다. 
	- 이렇게 하면 11On12 디바이스에 사용할 수 있도록 ID3D12Resource가 D3D11 기반 인터페이스에 연결됩니다.
	- 래핑된 리소스를 준비한 후에는 마찬가지로 LoadAssets 메서드에서 D2D가 렌더링될 렌더링 대상 화면을 만들 수 있습니다.
	____________________________________________________________________________________________________________*/
	for (_uint i = 0; i < SWAPCHAIN::BUFFER_END; ++i)
	{
		D3D11_RESOURCE_FLAGS d3d11Flags = { D3D11_BIND_RENDER_TARGET };

		FAILED_CHECK_RETURN(m_p11On12Device->CreateWrappedResource(m_arrSwapChainBuffer[i],
																   &d3d11Flags,
																   D3D12_RESOURCE_STATE_RENDER_TARGET,
																   D3D12_RESOURCE_STATE_PRESENT,
																   IID_PPV_ARGS(&m_arrWrappedBackBuffers[i])),
																   E_FAIL);

		IDXGISurface* pSurface;
		FAILED_CHECK_RETURN(m_arrWrappedBackBuffers[i]->QueryInterface(&pSurface), E_FAIL);

		m_pD2D_Context->CreateBitmapFromDxgiSurface(pSurface, &bitmapProperties, &m_arrD2DRenderTargets[i]);
		Safe_Release(pSurface);
	}

	return S_OK;
}

HRESULT CGraphicDevice::Create_RenderTargetAndDepthStencilBuffer(const _uint& iWidth, const _uint& iHeight)
{
	assert(m_pGraphicDevice);
	assert(m_pSwapChain);
	assert(m_arrCommandAllocator.data());

	/*__________________________________________________________________________________________________________
	[ Flush before changing any resources ]
	____________________________________________________________________________________________________________*/
	Wait_ForGpuComplete();

	FAILED_CHECK_RETURN(m_arrCommandList[CMD_MAIN]->Reset(m_arrCommandAllocator[CMD_MAIN], nullptr), E_FAIL);

	/*__________________________________________________________________________________________________________
	[ Release the previous resources we will be recreating ]
	____________________________________________________________________________________________________________*/
	for (_int i = 0; i < SWAPCHAIN::BUFFER_END; ++i)
		m_arrSwapChainBuffer[i]	= nullptr;

	m_pDepthStencilBuffer		= nullptr;

	/*__________________________________________________________________________________________________________
	[ Resize the swap chain ]
	____________________________________________________________________________________________________________*/
	FAILED_CHECK_RETURN(m_pSwapChain->ResizeBuffers(SWAPCHAIN::BUFFER_END,
													iWidth, 
													iHeight,
													DXGI_FORMAT_R8G8B8A8_UNORM,
													DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH),
													E_FAIL);


	m_iCurrBackBuffer = 0;

	CD3DX12_CPU_DESCRIPTOR_HANDLE RTV_HeapHandle(m_pRTV_Heap->GetCPUDescriptorHandleForHeapStart());
	for (_uint i = 0; i < SWAPCHAIN::BUFFER_END; i++)
	{
		FAILED_CHECK_RETURN(m_pSwapChain->GetBuffer(i, IID_PPV_ARGS(&m_arrSwapChainBuffer[i])), E_FAIL);

		m_pGraphicDevice->CreateRenderTargetView(m_arrSwapChainBuffer[i], 
												 nullptr, 
												 RTV_HeapHandle);

		RTV_HeapHandle.Offset(1, m_uiRTV_DescriptorSize);
	}

	// RenderTarget ResourceBarrier
	m_RenterTargetResourceBarrier.Type                    = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	m_RenterTargetResourceBarrier.Flags                   = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	m_RenterTargetResourceBarrier.Transition.Subresource  = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;


	/*__________________________________________________________________________________________________________
	[ Create the depth/stencil buffer and view ]
	____________________________________________________________________________________________________________*/
	D3D12_HEAP_PROPERTIES Default_HeapProperties;
	Default_HeapProperties.Type                 = D3D12_HEAP_TYPE_DEFAULT;
	Default_HeapProperties.CPUPageProperty      = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	Default_HeapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	Default_HeapProperties.CreationNodeMask     = 1;
	Default_HeapProperties.VisibleNodeMask      = 1;
	
	D3D12_RESOURCE_DESC DepthStencilDesc;
	DepthStencilDesc.Dimension			= D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	DepthStencilDesc.Alignment			= 0;
	DepthStencilDesc.Width				= iWidth;
	DepthStencilDesc.Height				= iHeight;
	DepthStencilDesc.DepthOrArraySize	= 1;
	DepthStencilDesc.MipLevels			= 1;
	DepthStencilDesc.Format				= DXGI_FORMAT_D24_UNORM_S8_UINT;
	DepthStencilDesc.SampleDesc.Count	= m_bIsMSAA4X_Enable ? 4 : 1;
	DepthStencilDesc.SampleDesc.Quality = m_bIsMSAA4X_Enable ? (m_uiMSAA4X_QualityLevels - 1) : 0;
	DepthStencilDesc.Layout				= D3D12_TEXTURE_LAYOUT_UNKNOWN;
	DepthStencilDesc.Flags				= D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	D3D12_CLEAR_VALUE optClear;
	optClear.Format						= DXGI_FORMAT_D24_UNORM_S8_UINT;
	optClear.DepthStencil.Depth			= 1.0f;
	optClear.DepthStencil.Stencil		= 0;
	FAILED_CHECK_RETURN(m_pGraphicDevice->CreateCommittedResource(&Default_HeapProperties,
																  D3D12_HEAP_FLAG_NONE,
																  &DepthStencilDesc,
																  D3D12_RESOURCE_STATE_COMMON,
																  &optClear,
																  IID_PPV_ARGS(&m_pDepthStencilBuffer)), 
																  E_FAIL);

	/*__________________________________________________________________________________________________________
	[ Create descriptor to mip level 0 of entire resource using the format of the resource ]
	____________________________________________________________________________________________________________*/
	m_pGraphicDevice->CreateDepthStencilView(m_pDepthStencilBuffer,
											 nullptr, 
											 m_pDSV_Heap->GetCPUDescriptorHandleForHeapStart());

	/*__________________________________________________________________________________________________________
	[ Transition the resource from its initial state to be used as a depth buffer ]
	____________________________________________________________________________________________________________*/
	D3D12_RESOURCE_BARRIER DepthStencil_ResourceBarrier;
	DepthStencil_ResourceBarrier.Type                    = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	DepthStencil_ResourceBarrier.Flags                   = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	DepthStencil_ResourceBarrier.Transition.Subresource  = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	DepthStencil_ResourceBarrier.Transition.pResource    = m_pDepthStencilBuffer;
	DepthStencil_ResourceBarrier.Transition.StateBefore  = D3D12_RESOURCE_STATE_COMMON;
	DepthStencil_ResourceBarrier.Transition.StateAfter   = D3D12_RESOURCE_STATE_DEPTH_WRITE;
	
	m_arrCommandList[CMD_MAIN]->ResourceBarrier(1, &DepthStencil_ResourceBarrier);

	/*__________________________________________________________________________________________________________
	[ Execute the resize commands ]
	____________________________________________________________________________________________________________*/
	FAILED_CHECK_RETURN(m_arrCommandList[CMD_MAIN]->Close(), E_FAIL);
	ID3D12CommandList* cmdsLists[] = { *m_arrCommandList.data() };
	m_pCommandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

	/*__________________________________________________________________________________________________________
	[ Wait until resize is complete ]
	____________________________________________________________________________________________________________*/
	Wait_ForGpuComplete();

	/*__________________________________________________________________________________________________________
	[ Update the viewport transform to cover the client area ]
	____________________________________________________________________________________________________________*/
	m_Viewport.TopLeftX = 0;
	m_Viewport.TopLeftY = 0;
	m_Viewport.Width	= static_cast<_float>(iWidth);
	m_Viewport.Height	= static_cast<_float>(iHeight);
	m_Viewport.MinDepth = 0.0f;
	m_Viewport.MaxDepth = 1.0f;

	m_ScissorRect		= { 0, 0, (LONG)iWidth, (LONG)iHeight };

	return S_OK;
}

void CGraphicDevice::Log_Adapters()
{
	_uint					i			= 0;
	IDXGIAdapter*			pAdapter	= nullptr;
	vector<IDXGIAdapter*>	AdapterList;

	while (DXGI_ERROR_NOT_FOUND != m_pFactory->EnumAdapters(i, &pAdapter))
	{
		DXGI_ADAPTER_DESC AdapterDesc;
		pAdapter->GetDesc(&AdapterDesc);

		wstring text = L"***Adapter: ";
		text += AdapterDesc.Description;
		text += L"\n";

		OutputDebugString(text.c_str());

		AdapterList.emplace_back(pAdapter);

		++i;
	}

	for (size_t i = 0; i < AdapterList.size(); ++i)
	{
		Log_AdapterOutputs(AdapterList[i]);
		Safe_Release(AdapterList[i]);
	}
}

void CGraphicDevice::Log_AdapterOutputs(IDXGIAdapter* pAdapter)
{
	_uint			i		= 0;
	IDXGIOutput*	pOutput	= nullptr;

	while (DXGI_ERROR_NOT_FOUND != pAdapter->EnumOutputs(i, &pOutput))
	{
		DXGI_OUTPUT_DESC OutputDesc;
		pOutput->GetDesc(&OutputDesc);

		wstring text = L"***Output: ";
		text += OutputDesc.DeviceName;
		text += L"\n";
		OutputDebugString(text.c_str());

		Log_OutputDisplayModes(pOutput, DXGI_FORMAT_R8G8B8A8_UNORM);

		Safe_Release(pOutput);

		++i;
	}
}

void CGraphicDevice::Log_OutputDisplayModes(IDXGIOutput* output, DXGI_FORMAT format)
{
	_uint count = 0;
	_uint flags = 0;

	// Call with nullptr to get list count.
	output->GetDisplayModeList(format, flags, &count, nullptr);

	vector<DXGI_MODE_DESC> modeList(count);
	output->GetDisplayModeList(format, flags, &count, &modeList[0]);

	for (auto& x : modeList)
	{
		_uint n = x.RefreshRate.Numerator;
		_uint d = x.RefreshRate.Denominator;
		wstring text  =
			L"Width   = " + to_wstring(x.Width) + L" " +
			L"Height  = " + to_wstring(x.Height) + L" " +
			L"Refresh = " + to_wstring(n) + L"/" + to_wstring(d) +
			L"\n";

		OutputDebugString(text.c_str());
	}
}

void CGraphicDevice::Get_HardwareAdapter(IDXGIFactory2* pFactory, IDXGIAdapter1** ppOutAdapter)
{
	// 1. 가장 큰 비디오카드 메모리를 찾는다.
	UINT64	uiMaxVideoMemory = 0;
	_int	iIdx             = 0;

	IDXGIAdapter1* pAdapter;
	for (_int i = 0; DXGI_ERROR_NOT_FOUND != pFactory->EnumAdapters1(i, &pAdapter); ++i)
	{
		DXGI_ADAPTER_DESC1 AdapterDesc;
		pAdapter->GetDesc1(&AdapterDesc);

		if (uiMaxVideoMemory <= AdapterDesc.DedicatedVideoMemory)
		{
			uiMaxVideoMemory = AdapterDesc.DedicatedVideoMemory;
			iIdx = i;
		}

	}

	// 2. Adapter 반환.
	pFactory->EnumAdapters1(iIdx, &pAdapter);

	*ppOutAdapter = nullptr;
	*ppOutAdapter = pAdapter;

}


void CGraphicDevice::Free()
{
#ifdef ENGINE_LOG
	COUT_STR("Destroy GrpahicDevice");
#endif

	for (_int i = 0; i < SWAPCHAIN::BUFFER_END; i++)
	{
		Safe_Release(m_arrSwapChainBuffer[i]);
		Safe_Release(m_arrWrappedBackBuffers[i]);
		Safe_Release(m_arrD2DRenderTargets[i]);
	}

	Safe_Release(m_pDepthStencilBuffer);
	Safe_Release(m_pRTV_Heap);
	Safe_Release(m_pDSV_Heap);

	for (_int i = 0; i < CMDID::CMD_END; ++i)
	{
		Safe_Release(m_arrCommandAllocator[i]);
		Safe_Release(m_arrCommandList[i]);
	}

	Safe_Release(m_pCommandQueue);

	Safe_Release(m_pFence);

	m_pSwapChain->SetFullscreenState(FALSE, nullptr);
	Safe_Release(m_pSwapChain);

	Safe_Release(m_pGraphicDevice);
	Safe_Release(m_pFactory);

	// DirectX 11
	Safe_Release(m_p11Device);
	Safe_Release(m_p11On12Device);
	Safe_Release(m_pD2D_Device);
	Safe_Release(m_pD2D_Context);

	Safe_Release(m_pD2D_Factory);
	Safe_Release(m_pDWriteFactory);

	if (m_p11Context)
	{
		m_p11Context->ClearState();
		m_p11Context->Flush();
		Safe_Release(m_p11Context);
	}
}
