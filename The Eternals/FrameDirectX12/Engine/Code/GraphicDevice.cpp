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
	- ��� �Ҵ��ڿ� ��� ����Ʈ �缳��.
	- �缳���� GPU�� ���� ��� ��ϵ��� ��� ó���� �Ŀ� �Ͼ��.
	____________________________________________________________________________________________________________*/
	FAILED_CHECK_RETURN(m_arrCommandAllocator[CMDID::CMD_MAIN]->Reset(), E_FAIL);
	FAILED_CHECK_RETURN(m_arrCommandList[CMDID::CMD_MAIN]->Reset(m_arrCommandAllocator[CMDID::CMD_MAIN], m_pPipelineState), E_FAIL);


	/*__________________________________________________________________________________________________________
	- ����Ʈ�� ���� �簢���� �����Ѵ�. 
	- ��� ���(CommandList)�� �缳���� ������ �̵鵵 �缳���ؾ� �Ѵ�.
	____________________________________________________________________________________________________________*/
	m_arrCommandList[CMDID::CMD_MAIN]->RSSetViewports(1, &m_Viewport);
	m_arrCommandList[CMDID::CMD_MAIN]->RSSetScissorRects(1, &m_ScissorRect);


	/*__________________________________________________________________________________________________________
	Indicate a state transition on the resource usage.
	- �ڿ� �뵵�� ���õ� �������̸� Direct3D�� �����Ѵ�.
	- ���� ���� Ÿ�ٿ� ���� ������Ʈ�� �����⸦ ��ٸ���. 
	- ������Ʈ�� ������ ���� Ÿ�� ������ ���´� 
	  ������Ʈ ����(D3D12_RESOURCE_STATE_PRESENT)���� ���� Ÿ�� ����(D3D12_RESOURCE_STATE_RENDER_TARGET)�� �ٲ� ���̴�.
	____________________________________________________________________________________________________________*/
	m_RenterTargetResourceBarrier.Transition.pResource   = m_arrSwapChainBuffer[m_iCurrBackBuffer];
	m_RenterTargetResourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
	m_RenterTargetResourceBarrier.Transition.StateAfter  = D3D12_RESOURCE_STATE_RENDER_TARGET;
	m_arrCommandList[CMDID::CMD_MAIN]->ResourceBarrier(1, &m_RenterTargetResourceBarrier);

	/*__________________________________________________________________________________________________________
	 Clear the back buffer and depth buffer.
	- ������ ���� Ÿ�ٿ� �ش��ϴ� �������� CPU �ּ�(�ڵ�)�� ���.
	- ���ϴ� �������� ���� Ÿ��(��)�� �����.
	____________________________________________________________________________________________________________*/
	_float arrClearColor[4] = { vRGBA.x, vRGBA.y, vRGBA.z, vRGBA.w };
	m_arrCommandList[CMDID::CMD_MAIN]->ClearRenderTargetView(CD3DX12_CPU_DESCRIPTOR_HANDLE(m_pRTV_Heap->GetCPUDescriptorHandleForHeapStart(),
																						   m_iCurrBackBuffer,
																						   m_uiRTV_DescriptorSize),
															 arrClearColor,
															 0,
															 nullptr);

	/*__________________________________________________________________________________________________________
	- ����-���ٽ� �������� CPU �ּҸ� ���.
	- ���ϴ� ������ ����-���ٽ�(��)�� �����.
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
	- ������ ����� ��ϵ� ���� Ÿ�� ���۵��� �����Ѵ�.
	- RTV(������)�� DSV(������)�� ���-���� �ܰ�(OM)�� �����Ѵ�.
	- OMSetRenderTargets : �������� ����� ���� Ÿ�ٰ� ����,���ٽ� ���۸� ���������ο� ���´�.

	��, ���� ������ ����Ÿ�� �ؽ��Ŀ� ���� ���̸� �׸��� ����.
	____________________________________________________________________________________________________________*/
	m_arrCommandList[CMDID::CMD_MAIN]->OMSetRenderTargets(1,
														  &CD3DX12_CPU_DESCRIPTOR_HANDLE(m_pRTV_Heap->GetCPUDescriptorHandleForHeapStart(),
														  								 m_iCurrBackBuffer,
														  								 m_uiRTV_DescriptorSize), 
														  true,
														  &m_pDSV_Heap->GetCPUDescriptorHandleForHeapStart());



	/* ------------------------------------ ������ �ڵ�� ���⿡ �߰��� ���̴�. ----------------------------------- */



	return S_OK;
}

HRESULT CGraphicDevice::Render_ExcuteCmdList()
{
	/*__________________________________________________________________________________________________________
	 Indicate a state transition on the resource usage.
	- �ڿ� �뵵�� ���õ� ���� ���̸� Direct3D�� �����Ѵ�.
	- ���� ���� Ÿ�ٿ� ���� �������� �����⸦ ��ٸ���. 
	- GPU�� ���� Ÿ��(����)�� �� �̻� ������� ������ ���� Ÿ���� ���´� 
	  ������Ʈ ����(D3D12_RESOURCE_STATE_PRESENT)�� �ٲ� ���̴�.
	____________________________________________________________________________________________________________*/
	if (false == CRenderer::Get_Instance()->Get_RenderOnOff(L"Font"))
	{
		m_RenterTargetResourceBarrier.Transition.pResource   = m_arrSwapChainBuffer[m_iCurrBackBuffer];
		m_RenterTargetResourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
		m_RenterTargetResourceBarrier.Transition.StateAfter  = D3D12_RESOURCE_STATE_PRESENT;
		m_arrCommandList[CMDID::CMD_MAIN]->ResourceBarrier(1, &m_RenterTargetResourceBarrier);
	}

	/*__________________________________________________________________________________________________________
	- ��ɵ��� ����� ��ģ��.
	- ExecuteCommandLists�� ��� ����� �����ϱ� ���� 
	  �ݵ�� �� �޼��带 �̿��ؼ� ��� ����� �ݾƾ� �Ѵ�.
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
	- ��� ����Ʈ�� GPUť�� �����Ѵ�.
	____________________________________________________________________________________________________________*/
	ID3D12CommandList* ppCommandLists[] = { m_arrCommandList[CMDID::CMD_MAIN] };
	m_pCommandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

	return S_OK;
}

HRESULT CGraphicDevice::Render_TextBegin()
{
	/*__________________________________________________________________________________________________________
	- ���� ��� D3D12 ��� ����� �����Ͽ� 3D ����� �������� ����, �� ���� UI�� �������Ѵ�.
	�ٸ� ���ÿ����� ��� ����� �ݱ� ���� �Ϲ������� ���ҽ� �庮�� �����Ͽ� ������ ��� ���¿��� ���� ���·� �� ���۸� ��ȯ�մϴ�. 
	�׷��� �� ���ÿ����� D2D�� ����Ͽ� �� ���۷� ��� �������ؾ� �ϹǷ� �ش� ���ҽ� �庮�� �����մϴ�. 

	- �� ������ ���ε� ���ҽ��� ���� �� ������ ��� ���¸� "IN" ���·� �����ϰ�, ���� ���¸� "OUT" ���·� ����.
	(m_arrWrappedBackBuffers�� PRESENT ���·� �����Ͽ� ResourceBarrier - PRESENT ��ü.)
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
	- ����������, 11On12 ����̽����� ����� ��� ����� ���� ID3D12CommandQueue�� �����ϱ� ���� 
	ID3D11DeviceContext���� Flush�� ȣ���ؾ� �մϴ�.
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
	- ����ü���� ������Ʈ�Ѵ�. -> �ĸ� ���ۿ� ���� ���۸� ��ȯ.
	- ������Ʈ�� �ϸ� ���� ���� Ÿ��(�ĸ����)�� ������ ������۷� �Ű�����, ���� Ÿ�� �ε����� �ٲ� ���̴�.
	____________________________________________________________________________________________________________*/
	FAILED_CHECK_RETURN(m_pSwapChain->Present(0, 0), E_FAIL);
	m_iCurrBackBuffer = (m_iCurrBackBuffer + 1) % SWAPCHAIN::BUFFER_END;

	return S_OK;
}

HRESULT CGraphicDevice::Wait_ForGpuComplete()
{
	/*__________________________________________________________________________________________________________
	 [ Advance the fence value to mark commands up to this fence point ]
	- CPU �潺�� ����.
	- ���� Fence ���������� ��ɵ��� ǥ���ϵ��� ��Ÿ�� ���� ������Ų��.
	____________________________________________________________________________________________________________*/
	m_uiCurrentFence++;

	/*__________________________________________________________________________________________________________
	  Add an instruction to the command queue to set a new fence point.
	 Because we are on the GPU timeline, the new fence point won't be set
	 until the GPU finishes processing all the commands prior to this Signal().

	- CPU�� �潺�� ���� �����ϴ� ����� ��� ť�� �߰�.
	- �� Fence ������ �����ϴ� ���(Signal)�� ��� ��⿭�� �߰��Ѵ�. ���� �츮�� GPU �ð��� �� �����Ƿ�,
	�� Fence ������ GPU�� Signal()��ɱ����� ��� ����� ó���ϱ� �������� �������� �ʴ´�.
	____________________________________________________________________________________________________________*/
	FAILED_CHECK_RETURN(m_pCommandQueue->Signal(m_pFence, m_uiCurrentFence), E_FAIL);

	/*__________________________________________________________________________________________________________
	[ Wait until the GPU has completed commands up to this fence point ]
	- GPU�� �� Fence ���������� ��ɵ��� �Ϸ��� �� ���� ��ٸ���.
	____________________________________________________________________________________________________________*/
	if (m_pFence->GetCompletedValue() < m_uiCurrentFence)
	{
		HANDLE eventHandle = CreateEventEx(NULL, 
										   FALSE, // Event Name
										   FALSE, // Flags 
										   EVENT_ALL_ACCESS);

		// GPU�� ���� Fence ������ ���������� �̺�Ʈ�� �ߵ��Ѵ�.
		FAILED_CHECK_RETURN(m_pFence->SetEventOnCompletion(m_uiCurrentFence, eventHandle), E_FAIL);


		// GPU�� ���� ��Ÿ�� ������ ���������� ���ϴ� �̺�Ʈ�� ��ٸ���.
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
	[ DXGI ���丮�� ���� ]
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
	- Ư�� ���� 12.0�� �����ϴ� �ϵ���� ����̽��� ������ �� ������ WARP ����̽��� ����.
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
	[ 4X MSAA ǰ�� ���� ���� ���� ]
	____________________________________________________________________________________________________________*/
	D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS MSAA_QualityLevels;
	ZeroMemory(&MSAA_QualityLevels, sizeof(D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS));

	MSAA_QualityLevels.Format			= DXGI_FORMAT_R8G8B8A8_UNORM;
	MSAA_QualityLevels.SampleCount		= 4;	//MSAA 4x ���� ���ø�
	MSAA_QualityLevels.Flags			= D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
	MSAA_QualityLevels.NumQualityLevels = 0;

	// ����̽��� �����ϴ� ���� ������ ǰ�� ������ Ȯ��.
	FAILED_CHECK_RETURN(m_pGraphicDevice->CheckFeatureSupport(D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS,		// ���� ǥ��ȭ ���.
															  &MSAA_QualityLevels,												// ��� ���� ������ ������ ����ü ������.
															  sizeof(D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS)), E_FAIL);	// ����ü�� ũ��.

	m_uiMSAA4X_QualityLevels = MSAA_QualityLevels.NumQualityLevels;

	//���� ������ ǰ�� ������ 1���� ũ�� ���� ���ø��� Ȱ��ȭ.
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
	- �潺�� ����ȭ�� ���� �̺�Ʈ ��ü�� �����Ѵ�. (�̺�Ʈ ��ü�� �ʱⰪ�� FALSE�̴�).
	- �̺�Ʈ�� ����Ǹ� (Signal)�̺�Ʈ�� ���� �ڵ������� FALSE�� �ǵ��� �����Ѵ�.
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

	- ù ��° �ܰ�� ID3D12Device�� ���� �Ŀ� ID3D11On12Device�� ����� ���Դϴ�.
	- �̶� API D3D11On12CreateDevice�� ���� ID3D12Device�� ���εǴ� ID3D11Device�� ����ϴ�.
	- �� API�� �ٸ� �Ű� ���� �߿��� ID3D12CommandQueue�� ����ϹǷ� 11On12 ����̽��� �ش� ����� ������ �� �ֽ��ϴ�.
	- ID3D11Device�� ��������� ���⿡�� ID3D11On12Device �������̽��� ������ �� �ֽ��ϴ�.
	- �̰��� D2D ������ ����� �⺻ ����̽� ��ü�Դϴ�.
	____________________________________________________________________________________________________________*/
	IUnknown* pRenderCommandQueue = m_pCommandQueue;

	FAILED_CHECK_RETURN(D3D11On12CreateDevice(m_pGraphicDevice,					// D3D11 interop�� ����� ���� D3D12 ��ġ.
											  D3D11_CREATE_DEVICE_BGRA_SUPPORT,
											  nullptr,							// NULL�� �ϸ� D3D12��ġ�� ��� ����.
											  0,								// ��� ���� �迭�� ũ��(����Ʈ).
											  &pRenderCommandQueue,				// D3D11on12���� ����� ������ ť �迭.
											  1,								// ��� ť �迭�� ũ��(����Ʈ).
											  0,								// D3D12 ��ġ�� ���. 1��Ʈ�� ������ �� ����.								
											  &m_p11Device,						// 11Device
											  &m_p11Context,					// 11DeviceContext
											  nullptr), 
											  E_FAIL);

	FAILED_CHECK_RETURN(m_p11Device->QueryInterface(&m_p11On12Device), E_FAIL);

	/*__________________________________________________________________________________________________________
	[ Create D2D/DWrite components ]

	- D2D ���丮 �����.
	- ���� 11On12 ����̽��� �����Ƿ�, �� ����̽��� ����Ͽ� D3D11�� ����� ��ó�� D2D ���͸� �� ����̽��� ����ϴ�.
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

	- D3D12�� ���� ü���� �����ϹǷ�, 11On12 ����̽�(D2D ������)�� ����Ͽ� �� ���۷� �������Ϸ���,
	ID3D12Resource ������ �� ���ۿ��� ID3D11Resource ������ ���ε� ���ҽ��� ������ �մϴ�.
	- �̷��� �ϸ� 11On12 ����̽��� ����� �� �ֵ��� ID3D12Resource�� D3D11 ��� �������̽��� ����˴ϴ�.
	- ���ε� ���ҽ��� �غ��� �Ŀ��� ���������� LoadAssets �޼��忡�� D2D�� �������� ������ ��� ȭ���� ���� �� �ֽ��ϴ�.
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

	- D3D12�� ���� ü���� �����ϹǷ�, 11On12 ����̽�(D2D ������)�� ����Ͽ� �� ���۷� �������Ϸ��� 
	ID3D12Resource ������ �� ���ۿ��� ID3D11Resource ������ ���ε� ���ҽ��� ������ �մϴ�. 
	- �̷��� �ϸ� 11On12 ����̽��� ����� �� �ֵ��� ID3D12Resource�� D3D11 ��� �������̽��� ����˴ϴ�.
	- ���ε� ���ҽ��� �غ��� �Ŀ��� ���������� LoadAssets �޼��忡�� D2D�� �������� ������ ��� ȭ���� ���� �� �ֽ��ϴ�.
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
	// 1. ���� ū ����ī�� �޸𸮸� ã�´�.
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

	// 2. Adapter ��ȯ.
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
