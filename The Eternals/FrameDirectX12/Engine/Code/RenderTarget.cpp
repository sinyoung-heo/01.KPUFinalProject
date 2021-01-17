#include "RenderTarget.h"

#include "GraphicDevice.h"
#include "ComponentMgr.h"
#include "Renderer.h"
#include "ScreenTex.h"
#include "ShaderTexture.h"

USING(Engine)

CRenderTarget::CRenderTarget(ID3D12Device * pGraphicDevice, ID3D12GraphicsCommandList * pCommandList)
	: m_pGraphicDevice(pGraphicDevice), m_pCommandList(pCommandList)
{
}

void CRenderTarget::Set_TargetClearColor(const _uint& iIdx, const _rgba& vColor, const DXGI_FORMAT& TargetFormat)
{
	if (iIdx >= m_vecClearColor.size())
		return;

	m_vecClearColor[iIdx].Color[0]	= vColor.x;
	m_vecClearColor[iIdx].Color[1]	= vColor.y;
	m_vecClearColor[iIdx].Color[2]	= vColor.z;
	m_vecClearColor[iIdx].Color[3]	= vColor.w;

	m_vecTargetFormat[iIdx]			= TargetFormat;
}

void CRenderTarget::Set_TargetTextureSize(const _uint & iIdx, const _float & fWidth, const _float & fHeight)
{
	if (iIdx >= m_vecViewPort.size())
		return;

	m_vecViewPort[iIdx] = { 0.0f, 0.0f, fWidth, fWidth, 0.0f, 1.0f };
}

HRESULT CRenderTarget::Ready_RenderTarget(const _uint& uiTargetCnt)
{
	// RenderTarget 개수.
	m_uiTargetCnt = uiTargetCnt;

	// Target Texture
	for (_uint i = 0; i < m_uiTargetCnt; ++i)
	{
		ComPtr<ID3D12Resource> pTargetTexture = nullptr;
		m_vecTargetTexture.emplace_back(pTargetTexture);
		m_vecDepthStencilBuffer.emplace_back(pTargetTexture);
		m_vecTargetFormat.emplace_back();
		m_vecClearColor.emplace_back();

		D3D12_VIEWPORT viewport = CGraphicDevice::Get_Instance()->Get_Viewport();
		m_vecViewPort.emplace_back(viewport);
	}

	// Target Rendering
	m_pBufferCom = static_cast<CScreenTex*>(CComponentMgr::Get_Instance()->Clone_Component(L"ScreenTex", COMPONENTID::ID_STATIC));
	NULL_CHECK_RETURN(m_pBufferCom, E_FAIL);

	m_pShaderCom = static_cast<CShaderTexture*>(CComponentMgr::Get_Instance()->Clone_Component(L"ShaderTexture", COMPONENTID::ID_STATIC));
	NULL_CHECK_RETURN(m_pShaderCom, E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_PipelineStatePass(3), E_FAIL);

	// Render RenderTarget
	m_vPos		= _vec3(160.0f, 90.0f, 1.0f);
	m_vScale	= _vec3(160.0f, 90.0f, 1.0f);

	return S_OK;
}

HRESULT CRenderTarget::SetUp_DefaultSetting(const TARGETID& eID)
{
	/*__________________________________________________________________________________________________________
	[ Descripotr ]
	____________________________________________________________________________________________________________*/
	// Descriptor Size
	m_uiRTV_DescriptorSize = CGraphicDevice::Get_Instance()->Get_RTV_DescriptorSize();
	m_uiDSV_DescriptorSize = CGraphicDevice::Get_Instance()->Get_DSV_DescriptorSize();

	// Descriptor Heap
	D3D12_DESCRIPTOR_HEAP_DESC RTV_HeapDesc;
	RTV_HeapDesc.NumDescriptors = m_uiTargetCnt;
	RTV_HeapDesc.Type			= D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	RTV_HeapDesc.Flags			= D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	RTV_HeapDesc.NodeMask		= 0;

	// MultiRenderTarget에 대한 RTV_Heap을 만든다.
	FAILED_CHECK_RETURN(m_pGraphicDevice->CreateDescriptorHeap(&RTV_HeapDesc, IID_PPV_ARGS(&m_pRTV_Heap)), E_FAIL);

	/*__________________________________________________________________________________________________________
	[ Target Texture Resource ]
	____________________________________________________________________________________________________________*/
	for (_uint i = 0; i < m_uiTargetCnt; ++i)
	{
		D3D12_RESOURCE_DESC TexTarget_Desc;
		ZeroMemory(&TexTarget_Desc, sizeof(D3D12_RESOURCE_DESC));
		TexTarget_Desc.Dimension			= D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		TexTarget_Desc.Alignment			= 0;
		TexTarget_Desc.Width				= (_uint)m_vecViewPort[i].Width;
		TexTarget_Desc.Height				= (_uint)m_vecViewPort[i].Height;
		TexTarget_Desc.DepthOrArraySize		= 1;
		TexTarget_Desc.MipLevels			= 1;
		TexTarget_Desc.SampleDesc.Count		= 1;
		TexTarget_Desc.SampleDesc.Quality	= 0;
		TexTarget_Desc.Layout				= D3D12_TEXTURE_LAYOUT_UNKNOWN;
		TexTarget_Desc.Flags				= D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
		TexTarget_Desc.Format				= m_vecTargetFormat[i];

		m_vecClearColor[i].Format			= m_vecTargetFormat[i];

		FAILED_CHECK_RETURN(m_pGraphicDevice->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
																	  D3D12_HEAP_FLAG_NONE,
																	  &TexTarget_Desc,
																	  D3D12_RESOURCE_STATE_RENDER_TARGET,	// 렌더타겟용 리소스.
																	  &m_vecClearColor[i],
																	  IID_PPV_ARGS(&m_vecTargetTexture[i])), 
																	  E_FAIL);
	}
	
	/*__________________________________________________________________________________________________________
	[ RenderTarget View ]
	____________________________________________________________________________________________________________*/
	for (_uint i = 0; i < m_uiTargetCnt; ++i)
	{
		D3D12_RENDER_TARGET_VIEW_DESC RTV_Desc;
		ZeroMemory(&RTV_Desc, sizeof(RTV_Desc));
		RTV_Desc.Texture2D.MipSlice		= 0;
		RTV_Desc.Texture2D.PlaneSlice	= 0;
		RTV_Desc.ViewDimension			= D3D12_RTV_DIMENSION_TEXTURE2D;
		RTV_Desc.Format					= m_vecTargetFormat[i];
		
		m_pGraphicDevice->CreateRenderTargetView(m_vecTargetTexture[i].Get(),
												 &RTV_Desc,
												 CD3DX12_CPU_DESCRIPTOR_HANDLE(m_pRTV_Heap->GetCPUDescriptorHandleForHeapStart(), 
												 							   i, 
												 							   m_uiRTV_DescriptorSize));
	}

	// Depth/Stencil Buffer View 생성.
	if (TYPE_SHADOWDEPTH == eID)
	{
		// DSV_Heap
		D3D12_DESCRIPTOR_HEAP_DESC DSV_HeapDesc;
		DSV_HeapDesc.NumDescriptors	= m_uiTargetCnt;
		DSV_HeapDesc.Type			= D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
		DSV_HeapDesc.Flags			= D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		DSV_HeapDesc.NodeMask		= 0;
		FAILED_CHECK_RETURN(m_pGraphicDevice->CreateDescriptorHeap(&DSV_HeapDesc, IID_PPV_ARGS(&m_pDSV_Heap)), E_FAIL);

		for (_uint i = 0; i < m_uiTargetCnt; ++i)
		{
			// DepthStencil
			D3D12_RESOURCE_DESC DepthStencilDesc;
			DepthStencilDesc.Dimension			= D3D12_RESOURCE_DIMENSION_TEXTURE2D;
			DepthStencilDesc.Alignment			= 0;
			DepthStencilDesc.Width				= (_uint)m_vecViewPort[i].Width;
			DepthStencilDesc.Height				= (_uint)m_vecViewPort[i].Height;
			DepthStencilDesc.DepthOrArraySize	= 1;
			DepthStencilDesc.MipLevels			= 1;
			DepthStencilDesc.Format				= DXGI_FORMAT_D24_UNORM_S8_UINT;
			DepthStencilDesc.SampleDesc.Count	= 1;
			DepthStencilDesc.SampleDesc.Quality = 0;
			DepthStencilDesc.Layout				= D3D12_TEXTURE_LAYOUT_UNKNOWN;
			DepthStencilDesc.Flags				= D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

			D3D12_CLEAR_VALUE optClear;
			optClear.Format						= DXGI_FORMAT_D24_UNORM_S8_UINT;
			optClear.DepthStencil.Depth			= 1.0f;
			optClear.DepthStencil.Stencil		= 0;
			FAILED_CHECK_RETURN(m_pGraphicDevice->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
																		  D3D12_HEAP_FLAG_NONE,
																		  &DepthStencilDesc,
																		  D3D12_RESOURCE_STATE_COMMON,
																		  &optClear,
																		  IID_PPV_ARGS(&m_vecDepthStencilBuffer[i])),
																		  E_FAIL);

		}

		for (_uint i = 0; i < m_uiTargetCnt; ++i)
		{
			// DepthStencil View
			m_pGraphicDevice->CreateDepthStencilView(m_vecDepthStencilBuffer[i].Get(),
													 nullptr, 
													 m_pDSV_Heap->GetCPUDescriptorHandleForHeapStart());
		}

	}


	FAILED_CHECK_RETURN(Create_TextureDescriptorHeap(), E_FAIL);
	m_pShaderCom->SetUp_ShaderConstantBuffer((_uint)(m_vecTargetTexture.size()));


	return S_OK;
}

HRESULT CRenderTarget::SetUp_OnGraphicDevice(const TARGETID& eID)
{
	/*__________________________________________________________________________________________________________
	- 현재의 BackBuffer RenderTarget을 RENDER_TARGET상태에서 PRESENT 상태로 잠시 내려놓는다.
	____________________________________________________________________________________________________________*/
	CGraphicDevice::Get_Instance()->Begin_BackBufferSetting();

	/*__________________________________________________________________________________________________________
	- 셰이더에서 그린 RenderTarget 텍스쳐를 그리기 위해 TargetTexture를 READ상태에서 RENDER_TARGET상태로 올린다.
	____________________________________________________________________________________________________________*/
	for (_uint i = 0; i < m_uiTargetCnt; ++i)
	{
		m_pCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_vecTargetTexture[i].Get(),
																				 D3D12_RESOURCE_STATE_GENERIC_READ,
																				 D3D12_RESOURCE_STATE_RENDER_TARGET));
	}

	for (_uint i = 0; i < m_uiTargetCnt; ++i)
	{
		m_pCommandList->ClearRenderTargetView(CD3DX12_CPU_DESCRIPTOR_HANDLE(m_pRTV_Heap->GetCPUDescriptorHandleForHeapStart(), 
																			i, 
																			m_uiRTV_DescriptorSize), 
											 m_vecClearColor[i].Color,
											 0, 
											 nullptr);
	}

	// Depth/Stencil Buffer	O
	if (TYPE_DEFAULT == eID)
	{
		/*__________________________________________________________________________________________________________
		- TargetTexture에 셰이더에서 그려놓은 Texture들을 Set 해준다.
		____________________________________________________________________________________________________________*/
		m_pCommandList->OMSetRenderTargets(m_uiTargetCnt,
										   &CD3DX12_CPU_DESCRIPTOR_HANDLE(m_pRTV_Heap->GetCPUDescriptorHandleForHeapStart()), 
										   true,
										   &CD3DX12_CPU_DESCRIPTOR_HANDLE(CGraphicDevice::Get_Instance()->Get_DSV_Heap()->GetCPUDescriptorHandleForHeapStart()));
	}

	// Depth/Stencil Buffer	X
	else if (TYPE_LIGHTING == eID)
	{
		/*__________________________________________________________________________________________________________
		- TargetTexture에 셰이더에서 그려놓은 Texture들을 Set 해준다.
		____________________________________________________________________________________________________________*/
		m_pCommandList->OMSetRenderTargets(m_uiTargetCnt,
										  &CD3DX12_CPU_DESCRIPTOR_HANDLE(m_pRTV_Heap->GetCPUDescriptorHandleForHeapStart()), 
										  true,
										  nullptr);	// Depth Stencil은 nullptr
	}

	// DSV Heap O
	else if (TYPE_SHADOWDEPTH == eID)
	{
		/*__________________________________________________________________________________________________________
		- Depth/Stencil Buffer를 Clear해준다.
		____________________________________________________________________________________________________________*/
		for (_uint i = 0; i < m_uiTargetCnt; ++i)
		{
			m_pCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_vecDepthStencilBuffer[i].Get(),
																					 D3D12_RESOURCE_STATE_COMMON,
																					 D3D12_RESOURCE_STATE_DEPTH_WRITE));
		}

		for (_uint i = 0; i < m_uiTargetCnt; ++i)
		{
			m_pCommandList->ClearDepthStencilView(CD3DX12_CPU_DESCRIPTOR_HANDLE(m_pDSV_Heap->GetCPUDescriptorHandleForHeapStart(),
																				i, 
																				m_uiDSV_DescriptorSize),
												  D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 
												  1.0f,
												  0,
												  0, 
												  nullptr);
		}

		/*__________________________________________________________________________________________________________
		- TargetTexture에 셰이더에서 그려놓은 Texture들을 Set 해준다.
		____________________________________________________________________________________________________________*/
		D3D12_RECT ScissorRect = { 0, 0, (LONG)m_vecViewPort[0].Width, (LONG)m_vecViewPort[0].Height };
		m_pCommandList->RSSetViewports(1, &m_vecViewPort[0]);
		m_pCommandList->RSSetScissorRects(1, &ScissorRect);

		m_pCommandList->OMSetRenderTargets(m_uiTargetCnt,
										   &CD3DX12_CPU_DESCRIPTOR_HANDLE(m_pRTV_Heap->GetCPUDescriptorHandleForHeapStart()), 
										   true,
										   &CD3DX12_CPU_DESCRIPTOR_HANDLE(m_pDSV_Heap->GetCPUDescriptorHandleForHeapStart()));
	}


	return S_OK;
}

HRESULT CRenderTarget::Release_OnGraphicDevice(const TARGETID& eID)
{
	if (TYPE_DEFAULT == eID || TYPE_LIGHTING == eID)
	{
		/*__________________________________________________________________________________________________________
		- TargetTexture에 Set이 다 끝났다면, TargetTexture들을 다시 RENDER_TARGET상태에서 READ상태로 내린다.
		____________________________________________________________________________________________________________*/
		for (_uint i = 0; i < m_uiTargetCnt; ++i)
		{
			m_pCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_vecTargetTexture[i].Get(),
																					 D3D12_RESOURCE_STATE_RENDER_TARGET,
																					 D3D12_RESOURCE_STATE_GENERIC_READ));
		}

		/*__________________________________________________________________________________________________________
		- TargetTexture에 RenderTarget의 Set이 끝났다면, 
		BackBuffer RenderTarget을 PRESENT상태에서 다시 RENDER_TARGET상태로 올린다.
		____________________________________________________________________________________________________________*/
		CGraphicDevice::Get_Instance()->End_BackBufferSetting();

		return S_OK;
	}

	else if (TYPE_SHADOWDEPTH == eID)
	{
		for (_uint i = 0; i < m_uiTargetCnt; ++i)
		{
			m_pCommandList->ClearDepthStencilView(CD3DX12_CPU_DESCRIPTOR_HANDLE(m_pDSV_Heap->GetCPUDescriptorHandleForHeapStart(),
																				i, 
																				m_uiDSV_DescriptorSize),
												  D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 
												  1.0f,
												  0,
												  0, 
												  nullptr);
		}

		/*__________________________________________________________________________________________________________
		- TargetTexture에 RenderTarget의 Set이 끝났다면,
		BackBuffer RenderTarget을 PRESENT상태에서 다시 RENDER_TARGET상태로 올린다.
		____________________________________________________________________________________________________________*/
		CGraphicDevice::Get_Instance()->End_BackBufferSetting();

		/*__________________________________________________________________________________________________________
		- TargetTexture에 Set이 다 끝났다면, TargetTexture들을 다시 RENDER_TARGET상태에서 READ상태로 내린다.
		____________________________________________________________________________________________________________*/
		for (_uint i = 0; i < m_uiTargetCnt; ++i)
		{
			m_pCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_vecTargetTexture[i].Get(),
																					 D3D12_RESOURCE_STATE_RENDER_TARGET,
																					 D3D12_RESOURCE_STATE_GENERIC_READ));

			m_pCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_vecDepthStencilBuffer[i].Get(),
																					 D3D12_RESOURCE_STATE_DEPTH_WRITE,
																					 D3D12_RESOURCE_STATE_COMMON));

			D3D12_RECT		ScissorRect = { 0, 0, (LONG)WINCX, (LONG)WINCY };
			D3D12_VIEWPORT	Viewport = CGraphicDevice::Get_Instance()->Get_Viewport();
			m_pCommandList->RSSetViewports(1, &Viewport);
			m_pCommandList->RSSetScissorRects(1, &ScissorRect);
		}

		return S_OK;
	}

	return S_OK;
}

HRESULT CRenderTarget::Begin_RenderTargetOnContext(ID3D12GraphicsCommandList * pCommandList, const THREADID& eID)
{
	if (THREADID::SHADOW == eID)
	{
		/*__________________________________________________________________________________________________________
		- ViewPort의 크기를 Shadow텍스쳐 크기만큼 늘려준다.
		____________________________________________________________________________________________________________*/
		D3D12_RECT ScissorRect = { 0, 0, (LONG)m_vecViewPort[0].Width, (LONG)m_vecViewPort[0].Height };
		pCommandList->RSSetViewports(1, &m_vecViewPort[0]);
		pCommandList->RSSetScissorRects(1, &ScissorRect);
		
		/*__________________________________________________________________________________________________________
		- 셰이더에서 그린 RenderTarget 텍스쳐를 그리기 위해 TargetTexture를 READ상태에서 RENDER_TARGET상태로 올린다.
		____________________________________________________________________________________________________________*/
		for (_uint i = 0; i < m_uiTargetCnt; ++i)
		{
			pCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_vecTargetTexture[i].Get(),
																				   D3D12_RESOURCE_STATE_GENERIC_READ,
																				   D3D12_RESOURCE_STATE_RENDER_TARGET));

		}

		pCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_vecDepthStencilBuffer[0].Get(),
																			   D3D12_RESOURCE_STATE_COMMON,
																			   D3D12_RESOURCE_STATE_DEPTH_WRITE));

		for (_uint i = 0; i < m_uiTargetCnt; ++i)
		{
			pCommandList->ClearRenderTargetView(CD3DX12_CPU_DESCRIPTOR_HANDLE(m_pRTV_Heap->GetCPUDescriptorHandleForHeapStart(),
				i,
				m_uiRTV_DescriptorSize),
				m_vecClearColor[i].Color,
				0,
				nullptr);
		}
			pCommandList->ClearDepthStencilView(m_pDSV_Heap->GetCPUDescriptorHandleForHeapStart(),
												D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL,
												1.0f,
												0,
												0,
												nullptr);
	}

	else if (THREADID::SCENE == eID)
	{
		/*__________________________________________________________________________________________________________
		- ViewPort의 크기를 Shadow텍스쳐 크기만큼 늘려준다.
		____________________________________________________________________________________________________________*/
		D3D12_RECT		ScissorRect = { 0, 0, (LONG)WINCX, (LONG)WINCY };
		D3D12_VIEWPORT	Viewport = CGraphicDevice::Get_Instance()->Get_Viewport();
		pCommandList->RSSetViewports(1, &Viewport);
		pCommandList->RSSetScissorRects(1, &ScissorRect);

		/*__________________________________________________________________________________________________________
		- 셰이더에서 그린 RenderTarget 텍스쳐를 그리기 위해 TargetTexture를 READ상태에서 RENDER_TARGET상태로 올린다.
		____________________________________________________________________________________________________________*/
		for (_uint i = 0; i < m_uiTargetCnt; ++i)
		{
			pCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_vecTargetTexture[i].Get(),
																				   D3D12_RESOURCE_STATE_GENERIC_READ,
																				   D3D12_RESOURCE_STATE_RENDER_TARGET));
		}

		for (_uint i = 0; i < m_uiTargetCnt; ++i)
		{
			pCommandList->ClearRenderTargetView(CD3DX12_CPU_DESCRIPTOR_HANDLE(m_pRTV_Heap->GetCPUDescriptorHandleForHeapStart(),
																			  i,
																			  m_uiRTV_DescriptorSize),
																			  m_vecClearColor[i].Color,
																			  0,
																			  nullptr);
		}
	}

	return S_OK;
}

HRESULT CRenderTarget::Bind_RenderTargetOnContext(ID3D12GraphicsCommandList * pCommandList, const THREADID& eID)
{
	if (THREADID::SHADOW == eID)
	{
		pCommandList->OMSetRenderTargets(m_uiTargetCnt,
										 &m_pRTV_Heap->GetCPUDescriptorHandleForHeapStart(),
										 true,
										 &m_pDSV_Heap->GetCPUDescriptorHandleForHeapStart());

		/*__________________________________________________________________________________________________________
		- TargetTexture에 셰이더에서 그려놓은 Texture들을 Set 해준다.
		____________________________________________________________________________________________________________*/
		D3D12_RECT ScissorRect = { 0, 0, (LONG)m_vecViewPort[0].Width, (LONG)m_vecViewPort[0].Height };
		pCommandList->RSSetViewports(1, &m_vecViewPort[0]);
		pCommandList->RSSetScissorRects(1, &ScissorRect);
	}

	else if (THREADID::SCENE == eID)
	{
		pCommandList->OMSetRenderTargets(m_uiTargetCnt,
										 &m_pRTV_Heap->GetCPUDescriptorHandleForHeapStart(),
										 true,
										 &CGraphicDevice::Get_Instance()->Get_DSV_Heap()->GetCPUDescriptorHandleForHeapStart());

		D3D12_RECT		ScissorRect = { 0, 0, (LONG)WINCX, (LONG)WINCY };
		D3D12_VIEWPORT	Viewport	= CGraphicDevice::Get_Instance()->Get_Viewport();
		pCommandList->RSSetViewports(1, &Viewport);
		pCommandList->RSSetScissorRects(1, &ScissorRect);
	}

	return S_OK;
}

HRESULT CRenderTarget::End_RenderTargetOnContext(ID3D12GraphicsCommandList * pCommandList, const THREADID& eID)
{
	if (THREADID::SHADOW == eID)
	{
		/*__________________________________________________________________________________________________________
		- 리소스의 상태를 다시 처음 상태로 되돌린다.
		____________________________________________________________________________________________________________*/
		for (_uint i = 0; i < m_uiTargetCnt; ++i)
		{
			pCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_vecTargetTexture[i].Get(),
																				   D3D12_RESOURCE_STATE_RENDER_TARGET,
																				   D3D12_RESOURCE_STATE_GENERIC_READ));
		}

		pCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_vecDepthStencilBuffer[0].Get(),
																			   D3D12_RESOURCE_STATE_DEPTH_WRITE,
																			   D3D12_RESOURCE_STATE_COMMON));

		/*__________________________________________________________________________________________________________
		- 원래 윈도우 크기만큼 ViewPort를 다시 돌려놓는다.
		____________________________________________________________________________________________________________*/
		D3D12_RECT		ScissorRect = { 0, 0, (LONG)WINCX, (LONG)WINCY };
		D3D12_VIEWPORT	Viewport = CGraphicDevice::Get_Instance()->Get_Viewport();
		pCommandList->RSSetViewports(1, &Viewport);
		pCommandList->RSSetScissorRects(1, &ScissorRect);
	}

	else if (THREADID::SCENE == eID)
	{

		/*__________________________________________________________________________________________________________
		- 리소스의 상태를 다시 처음 상태로 되돌린다.
		____________________________________________________________________________________________________________*/
		for (_uint i = 0; i < m_uiTargetCnt; ++i)
		{
			pCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_vecTargetTexture[i].Get(),
																				   D3D12_RESOURCE_STATE_RENDER_TARGET,
																				   D3D12_RESOURCE_STATE_GENERIC_READ));
		}

		/*__________________________________________________________________________________________________________
		- 원래 윈도우 크기만큼 ViewPort를 다시 돌려놓는다.
		____________________________________________________________________________________________________________*/
		D3D12_RECT		ScissorRect = { 0, 0, (LONG)WINCX, (LONG)WINCY };
		D3D12_VIEWPORT	Viewport = CGraphicDevice::Get_Instance()->Get_Viewport();
		pCommandList->RSSetViewports(1, &Viewport);
		pCommandList->RSSetScissorRects(1, &ScissorRect);
	}

	return S_OK;
}

void CRenderTarget::Render_RenderTarget()
{
	for (_uint i = 0; i < m_uiTargetCnt; ++i)

	{
		Set_ConstantTable(i);

		m_pShaderCom->Begin_Shader(m_pTexDescriptorHeap, i, i);
		m_pBufferCom->Begin_Buffer();

		m_pBufferCom->Render_Buffer();
	}
}

void CRenderTarget::Set_ConstantTable(const _uint & iIdx)
{
	_matrix matView = INIT_MATRIX;
	_matrix matProj = XMMatrixOrthographicLH((_float)WINCX, (_float)WINCY, 0.0f, 1.0f);

	_vec3 vPos	= _vec3(m_vPos.x, m_vPos.y + (_float)iIdx * m_vScale.y * 2.0f, 1.0f);
	vPos		= vPos.Convert_2DWindowToDescartes(WINCX, WINCY);

	_matrix matScale = XMMatrixScaling(m_vScale.x, m_vScale.y, m_vScale.z);
	_matrix matTrans = XMMatrixTranslation(vPos.x, vPos.y, vPos.z);
	_matrix matWorld = matScale * matTrans;

	CB_MATRIX_DESC	tCB_MatrixDesc;
	ZeroMemory(&tCB_MatrixDesc, sizeof(CB_MATRIX_DESC));
	XMStoreFloat4x4(&tCB_MatrixDesc.matWVP, XMMatrixTranspose(matWorld * matView * matProj));

	m_pShaderCom->Get_UploadBuffer_MatrixDesc()->CopyData(iIdx, tCB_MatrixDesc);
}

HRESULT CRenderTarget::Create_TextureDescriptorHeap()
{
	D3D12_DESCRIPTOR_HEAP_DESC SRV_HeapDesc = {};
	SRV_HeapDesc.NumDescriptors             = m_uiTargetCnt;	// 텍스처의 개수 만큼 설정.
	SRV_HeapDesc.Type                       = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	SRV_HeapDesc.Flags                      = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

	FAILED_CHECK_RETURN(m_pGraphicDevice->CreateDescriptorHeap(&SRV_HeapDesc,
															   IID_PPV_ARGS(&m_pTexDescriptorHeap)),
															   E_FAIL);

	// 힙의 시작을 가리키는 포인터를 얻는다.
	CD3DX12_CPU_DESCRIPTOR_HANDLE SRV_DescriptorHandle(m_pTexDescriptorHeap->GetCPUDescriptorHandleForHeapStart());

	for (_uint i = 0; i < m_uiTargetCnt; ++i)
	{
		D3D12_SHADER_RESOURCE_VIEW_DESC SRV_Desc = {};
		SRV_Desc.Format                        = m_vecTargetTexture[i]->GetDesc().Format;
		SRV_Desc.ViewDimension                 = D3D12_SRV_DIMENSION_TEXTURE2D;
		SRV_Desc.Shader4ComponentMapping       = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		SRV_Desc.Texture2D.MostDetailedMip     = 0;
		SRV_Desc.Texture2D.MipLevels           = m_vecTargetTexture[i]->GetDesc().MipLevels;
		SRV_Desc.Texture2D.ResourceMinLODClamp = 0.0f;

		m_pGraphicDevice->CreateShaderResourceView(m_vecTargetTexture[i].Get(), &SRV_Desc, SRV_DescriptorHandle);

		// 힙의 다음 서술자로 넘어간다.
		SRV_DescriptorHandle.Offset(1, CGraphicDevice::Get_Instance()->Get_CBV_SRV_UAV_DescriptorSize());
	}


	return S_OK;
}



CRenderTarget * CRenderTarget::Create(ID3D12Device * pGraphicDevice, 
									  ID3D12GraphicsCommandList * pCommandList,
									  const _uint& uiTargetCnt)
{
	CRenderTarget* pInstance = new CRenderTarget(pGraphicDevice, pCommandList);

	if (FAILED(pInstance->Ready_RenderTarget(uiTargetCnt)))
		Safe_Release(pInstance);

	return pInstance;
}

void CRenderTarget::Free()
{
	Safe_Release(m_pBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRTV_Heap);
	Safe_Release(m_pDSV_Heap);
	Safe_Release(m_pTexDescriptorHeap);
}
