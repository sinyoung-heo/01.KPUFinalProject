#pragma once
#include "Engine_Include.h"
#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CGraphicDevice final : public CBase
{
	DECLARE_SINGLETON(CGraphicDevice)

private:
	explicit CGraphicDevice();
	virtual ~CGraphicDevice() = default;

public:
	// Get - DirectX 12
	ID3D12Device*				Get_GraphicDevice()							const { return m_pGraphicDevice; }
	ID3D12GraphicsCommandList*	Get_CommandList(const CMDID& eCmdID)		const { return m_arrCommandList[eCmdID]; }
	ID3D12CommandAllocator*		Get_CommandAllocator(const CMDID& eCmdID)	const { return m_arrCommandAllocator[eCmdID]; }
	ID3D12CommandQueue*			Get_CommandQueue()							const { return m_pCommandQueue; }
	const D3D12_VIEWPORT&		Get_Viewport()								const { return m_Viewport; }
	const _bool&				Get_MSAA4X_Enable()							const { return m_bIsMSAA4X_Enable; }
	const _uint&				Get_MSAA4X_QualityLevels()					const { return m_uiMSAA4X_QualityLevels; }
	const _uint&				Get_RTV_DescriptorSize()					const { return m_uiRTV_DescriptorSize; }
	const _uint&				Get_DSV_DescriptorSize()					const { return m_uiDSV_DescriptorSize; }
	const _uint&				Get_CBV_SRV_UAV_DescriptorSize()			const { return m_uiCBV_SRV_UAV_DescriptorSize; }
	ID3D12DescriptorHeap*		Get_RTV_Heap()								const { return m_pRTV_Heap; }
	ID3D12DescriptorHeap*		Get_DSV_Heap()								const { return m_pDSV_Heap; }
	const _int&					Get_CurrBackBufferIdx()						const { return m_iCurrBackBuffer; }
	_matrix*					Get_Transform(const MATRIXID& eID)			const { return m_arrTransform[eID]; }

	// Get - DirectX 11 
	ID2D1DeviceContext2*		Get_D2DContext()							const { return m_pD2D_Context; }
	IDWriteFactory2*			Get_DWriteFactory()							const { return m_pDWriteFactory; }

	// Set
	void						Set_PipelineState(ID3D12PipelineState* pPipelineState)	{ m_pPipelineState = pPipelineState; }
	void						Set_Transform(const MATRIXID& eID, _matrix* pMatrix)	{ m_arrTransform[eID] = pMatrix; }

	// Method
	HRESULT Ready_GraphicDevice(HWND hWnd, HANDLE hHandle, WINMODE eMode, const _uint& iWidth, const _uint& iHeight);
	HRESULT	Render_Begin(const _rgba& vRGBA);
	HRESULT Render_ExcuteCmdList();
	HRESULT Render_TextBegin();
	HRESULT Render_TextEnd();
	HRESULT	Render_End();

	HRESULT	Wait_ForGpuComplete();
	HRESULT Begin_ResetCmdList(const CMDID& eCmdID);
	HRESULT End_ResetCmdList(const CMDID& eCmdID);
	void	Begin_BackBufferSetting();
	void	End_BackBufferSetting();
	void	Clear_DepthStencilBuffer(const CMDID& eCmdID);

private:
	HRESULT	Create_GraphicDevice(const _uint& iWidth, const _uint& iHeight);
	HRESULT	Create_CommandQueueAndList();
	HRESULT	Create_SwapChain(HWND hWnd, WINMODE eMode, const _uint& iWidth, const _uint& iHeight);
	HRESULT Create_RtvAndDsvDescriptorHeaps();
	HRESULT Create_FenceObject();
	HRESULT	Create_11On12GraphicDevice();
	HRESULT Create_RenderTargetAndDepthStencilBuffer(const _uint& iWidth, const _uint& iHeight);

	void	Log_Adapters();
	void	Log_AdapterOutputs(IDXGIAdapter* adapter);
	void	Log_OutputDisplayModes(IDXGIOutput* output, DXGI_FORMAT format);
	void	Get_HardwareAdapter(IDXGIFactory2* pFactory, IDXGIAdapter1** ppOutAdapter);

private:
	/*__________________________________________________________________________________________________________
	[ DirectX 12 GraphicDevice ]
	IDXGIFactory	: IDXGISwapChain 인터페이스 생성과 디스플레이 어댑터(그래픽 카드) 열거에 쓰임.
	ID3D12Device	: 주로 리소스를 생성하기 위해 필요.
	IDXGISwapChain	: 주로 디스플레이를 제어하기 위해 필요.
	____________________________________________________________________________________________________________*/
	IDXGIFactory4*		m_pFactory				= nullptr;
	ID3D12Device*		m_pGraphicDevice		= nullptr;
	IDXGISwapChain*		m_pSwapChain			= nullptr;


	/*__________________________________________________________________________________________________________
	[ DirectX 11 GraphicDevice ]
	- D2D / TextFont Render.
	____________________________________________________________________________________________________________*/
	ID3D11Device*									m_p11Device				= nullptr;
	ID3D11DeviceContext*							m_p11Context			= nullptr;
	ID3D11On12Device*								m_p11On12Device			= nullptr;

	ID2D1Factory3*									m_pD2D_Factory			= nullptr;
	ID2D1Device2*									m_pD2D_Device			= nullptr;
	ID2D1DeviceContext2*							m_pD2D_Context			= nullptr;
	IDWriteFactory2*								m_pDWriteFactory		= nullptr;

	array<ID3D11Resource*, SWAPCHAIN::BUFFER_END>	m_arrWrappedBackBuffers	{ nullptr };
	array<ID2D1Bitmap1*, SWAPCHAIN::BUFFER_END>		m_arrD2DRenderTargets	{ nullptr };


	/*__________________________________________________________________________________________________________
	[ 명령 대기열과 명령 목록 ]
	ID3D12CommandQueue			: 명령 대기열을 대표하는 인터페이스.
	ID3D12CommandAllocator		: 명령 할당자. 명령 목록에 추가된 명령들은 이 할당자의 메모리에 저장된다.
								  ExecuteCommandLists로 명령 목록을 실행하면, 명령 대기열은 그 할당자에 담긴 명령들을 참조한다.
	ID3D12GraphicsCommandList	: 실제 그래픽 작업을 위한 명령 목록을 대표하는 인터페이스.

	- 여러 쓰레드에서 같은 CommandList를 동시에 사용 불가. (1 쓰레드  : 1 CommandList)
	____________________________________________________________________________________________________________*/
	// 그래픽스 파이프라인 상태 객체에 대한 인터페이스 포인터.
	ID3D12PipelineState*								m_pPipelineState		= nullptr;

	ID3D12CommandQueue*									m_pCommandQueue			= nullptr;
	array<ID3D12CommandAllocator*, CMDID::CMD_END>		m_arrCommandAllocator	{ nullptr };
	array<ID3D12GraphicsCommandList*, CMDID::CMD_END>	m_arrCommandList		{ nullptr };


	/*__________________________________________________________________________________________________________
	[ 서술자 힙 (Descriptor Heap) ]
	- 그리기 명령을 제출하기 전에, 먼저 해당 그리기 호출이 참조할 자원들을 렌더링 파이프라인에 묶어야 한다.
	- 실제로 파이프라인에 묶이는 것은 해당 자원을 참조하는 서술자(Descriptor) 객체이다.
	- 서술자는 자원을 GPU에게 서술해주는 경량의 자료구조라고 할 수 있다.
	- GPU는 자원 서술자를 통해서 자원의 실제 자료에 접근하며, 그 자료를 사용하는 데 필요한 정보 역시 서술자로부터 얻는다.
	- 서술자는 자원 자료를 지정하는 수단일 뿐만 아니라, 자원을 GPU에 서술하는 수단. 
	  (자원을 파이프라인의 어느 단계에 묶어야 하는지를 말해줌.)
	
	1. CBV(상수 버퍼), SRV(셰이더 자원), UAV(순서 없는 접근)을 서술.
	2. 표본 추출기 서술자는 텍스처 적용에 쓰이는 표본 추출기(smapler) 자원을 서술.
	3. RTV 서술자는 렌더 대상(Render Target)자원을 서술.
	4. DSV 서술자는 깊이, 스텐실(Depth/Stencil)자원을 서술.

	- 서술자들은 응용 프로그램의 초기화 시점에서 생성해야 한다.
	____________________________________________________________________________________________________________*/
	ID3D12DescriptorHeap*							m_pRTV_Heap                     = nullptr;
	ID3D12DescriptorHeap*							m_pDSV_Heap                     = nullptr;
	_uint											m_uiRTV_DescriptorSize			= 0;
	_uint											m_uiDSV_DescriptorSize			= 0;
	_uint											m_uiCBV_SRV_UAV_DescriptorSize	= 0;
	
	array<ID3D12Resource*, SWAPCHAIN::BUFFER_END>	m_arrSwapChainBuffer			{ nullptr };
	_int											m_iCurrBackBuffer				= 0;
	_bool											m_bIsMSAA4X_Enable				= false;
	_uint											m_uiMSAA4X_QualityLevels		= 0;
	D3D12_VIEWPORT									m_Viewport						{ };
	D3D12_RECT										m_ScissorRect					{ };
	D3D12_RESOURCE_BARRIER							m_RenterTargetResourceBarrier;

	ID3D12Resource*									m_pDepthStencilBuffer			= nullptr;


	/*__________________________________________________________________________________________________________
	[ CPU/GPU 동기화 ]
	ID3D12Fence	: GPU와 CPU의 동기화를 위한 수단으로 쓰이는 대표 인터페이스.
	FenceValue	: Fence의 값. 시간상의 특정 Fence지점을 식별하는 정수.
	____________________________________________________________________________________________________________*/
	ID3D12Fence*	m_pFence            = nullptr;
	UINT64			m_uiCurrentFence	= 0;

	/*__________________________________________________________________________________________________________
	[ View/Projection/Ortho Matrix ]
	____________________________________________________________________________________________________________*/
	array<_matrix*, MATRIXID::MATRIX_END> m_arrTransform{ nullptr };

private:
	virtual void Free();
};

END