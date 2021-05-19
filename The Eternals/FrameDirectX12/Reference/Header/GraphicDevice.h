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
	wstring						Get_GraphicDeviceName()							  { return m_wstrGraphicDeviceName; }

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
	_bool m_bIsLateInit = false;

	/*__________________________________________________________________________________________________________
	[ DirectX 12 GraphicDevice ]
	____________________________________________________________________________________________________________*/
	IDXGIFactory4*		m_pFactory				= nullptr;
	ID3D12Device*		m_pGraphicDevice		= nullptr;
	IDXGISwapChain*		m_pSwapChain			= nullptr;


	/*__________________________________________________________________________________________________________
	[ DirectX 11 GraphicDevice ]
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
	[ CommandList / CommandAllocator / CommandQueue ]
	____________________________________________________________________________________________________________*/
	ID3D12PipelineState*								m_pPipelineState		= nullptr;

	ID3D12CommandQueue*									m_pCommandQueue			= nullptr;
	array<ID3D12CommandAllocator*, CMDID::CMD_END>		m_arrCommandAllocator	{ nullptr };
	array<ID3D12GraphicsCommandList*, CMDID::CMD_END>	m_arrCommandList		{ nullptr };

	/*__________________________________________________________________________________________________________
	[ ¼­¼úÀÚ Èü (Descriptor Heap) ]
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

	ID3D12Resource*									m_pDepthStencilBuffer			= nullptr;


	/*__________________________________________________________________________________________________________
	[ FENCE ]
	____________________________________________________________________________________________________________*/
	ID3D12Fence*	m_pFence            = nullptr;
	UINT64			m_uiCurrentFence	= 0;

	/*__________________________________________________________________________________________________________
	[ View/Projection/Ortho Matrix ]
	____________________________________________________________________________________________________________*/
	array<_matrix*, MATRIXID::MATRIX_END> m_arrTransform{ nullptr };

	wstring m_wstrGraphicDeviceName = L"";

private:
	virtual void Free();
};

END