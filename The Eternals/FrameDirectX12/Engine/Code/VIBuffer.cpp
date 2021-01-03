#include "VIBuffer.h"
#include "GraphicDevice.h"

USING(Engine)

CVIBuffer::CVIBuffer(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList)
	: CComponent(pGraphicDevice, pCommandList)
{
	ZeroMemory(&m_tSubMeshGeometry, sizeof(SUBMESH_GEOMETRY));
}

CVIBuffer::CVIBuffer(const CVIBuffer & rhs)
	: CComponent(rhs)
	, m_pVB_CPU(rhs.m_pVB_CPU)
	, m_pIB_CPU(rhs.m_pIB_CPU)
	, m_pVB_GPU(rhs.m_pVB_GPU)
	, m_pIB_GPU(rhs.m_pIB_GPU)
	, m_uiVertexByteStride(rhs.m_uiVertexByteStride)
	, m_uiVB_ByteSize(rhs.m_uiVB_ByteSize)
	, m_uiIB_ByteSize(rhs.m_uiIB_ByteSize)
	, m_IndexFormat(rhs.m_IndexFormat)

	, m_tSubMeshGeometry(rhs.m_tSubMeshGeometry)
	, m_PrimitiveTopology(rhs.m_PrimitiveTopology)
{
	if (m_pVB_CPU != nullptr)
		m_pVB_CPU->AddRef();

	if (m_pIB_CPU != nullptr)
		m_pIB_CPU->AddRef();

	if (m_pVB_GPU != nullptr)
		m_pVB_GPU->AddRef();

	if (m_pIB_GPU != nullptr)
		m_pIB_GPU->AddRef();
}


D3D12_VERTEX_BUFFER_VIEW CVIBuffer::Get_VertexBufferView() const
{
	D3D12_VERTEX_BUFFER_VIEW VertexBufferView;
	VertexBufferView.BufferLocation	= m_pVB_GPU->GetGPUVirtualAddress();
	VertexBufferView.StrideInBytes	= m_uiVertexByteStride;
	VertexBufferView.SizeInBytes	= m_uiVB_ByteSize;

	return VertexBufferView;
}

D3D12_INDEX_BUFFER_VIEW CVIBuffer::Get_IndexBufferView() const
{
	D3D12_INDEX_BUFFER_VIEW IndexBufferView;
	IndexBufferView.BufferLocation	= m_pIB_GPU->GetGPUVirtualAddress();
	IndexBufferView.SizeInBytes		= m_uiIB_ByteSize;
	IndexBufferView.Format			= m_IndexFormat;

	return IndexBufferView;
}

HRESULT CVIBuffer::Ready_Buffer()
{
	NULL_CHECK_RETURN(m_pGraphicDevice, E_FAIL);
	NULL_CHECK_RETURN(m_pCommandList, E_FAIL);

	return S_OK;
}

void CVIBuffer::Begin_Buffer()
{
	/*__________________________________________________________________________________________________________
	[ 메쉬의 정점 버퍼 뷰와 인덱스 버퍼 뷰를 설정 ]
	____________________________________________________________________________________________________________*/
	m_pCommandList->IASetVertexBuffers(0, 						 // 시작 슬롯. (입력 슬롯은 총 16개)
									   1, 						 // 입력 슬롯들에 묶을 정점 버퍼 개수.
									   &Get_VertexBufferView()); // 정점 버퍼 뷰의 첫 원소를 가리키는 포인터.

	m_pCommandList->IASetIndexBuffer(&Get_IndexBufferView());

	/*__________________________________________________________________________________________________________
	[ 메쉬의 프리미티브 유형을 설정 ]
	____________________________________________________________________________________________________________*/
	m_pCommandList->IASetPrimitiveTopology(m_PrimitiveTopology);
}

void CVIBuffer::Render_Buffer()
{
	/*__________________________________________________________________________________________________________
	[ 정점들이 파이프라인의 입력 조립기 단계로 공급 ]
	____________________________________________________________________________________________________________*/
	m_pCommandList->DrawIndexedInstanced(m_tSubMeshGeometry.uiIndexCount,	// 그리기에 사용할 인덱스들의 개수. (인스턴스 당)
										 1,									// 그릴 인스턴스 개수.
										 0,									// 인덱스 버퍼의 첫 index
										 0, 								// 그리기 호출에 쓰이는 인덱스들에 더할 정수 값.
										 0);								// 인스턴싱 
}

void CVIBuffer::Release_UploadBuffer()
{
	Engine::Safe_Release(m_pVB_Uploader);
	Engine::Safe_Release(m_pIB_Uploader);
}


ID3D12Resource * CVIBuffer::Create_DefaultBuffer(const void * InitData,
												 UINT64 uiByteSize,
												 ID3D12Resource *& pUploadBuffer)
{
	Engine::CGraphicDevice::Get_Instance()->Begin_ResetCmdList(CMDID::CMD_MAIN);

	ID3D12Resource* pDefaultBuffer = nullptr;

	/*__________________________________________________________________________________________________________
	[ 실제 기본 버퍼 자원을 생성 ]
	____________________________________________________________________________________________________________*/
	D3D12_HEAP_PROPERTIES Default_HeapProperties;
	Default_HeapProperties.Type                 = D3D12_HEAP_TYPE_DEFAULT;
	Default_HeapProperties.CPUPageProperty      = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	Default_HeapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	Default_HeapProperties.CreationNodeMask     = 1;
	Default_HeapProperties.VisibleNodeMask      = 1;

	D3D12_RESOURCE_DESC Default_ResourceDesc;
	Default_ResourceDesc.Dimension				= D3D12_RESOURCE_DIMENSION_BUFFER;
	Default_ResourceDesc.Alignment				= 0;
	Default_ResourceDesc.Width					= uiByteSize;
	Default_ResourceDesc.Height					= 1;
	Default_ResourceDesc.DepthOrArraySize		= 1;
	Default_ResourceDesc.MipLevels				= 1;
	Default_ResourceDesc.Format					= DXGI_FORMAT_UNKNOWN;
	Default_ResourceDesc.SampleDesc.Count		= 1;
	Default_ResourceDesc.SampleDesc.Quality		= 0;
	Default_ResourceDesc.Layout					= D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	Default_ResourceDesc.Flags					= D3D12_RESOURCE_FLAG_NONE;

	FAILED_CHECK_RETURN(m_pGraphicDevice->CreateCommittedResource(&Default_HeapProperties,
																  D3D12_HEAP_FLAG_NONE,
																  &Default_ResourceDesc,
																  D3D12_RESOURCE_STATE_COMMON,
																  nullptr,
																  IID_PPV_ARGS(&pDefaultBuffer)), 
																  NULL);
 
	/*__________________________________________________________________________________________________________
	- CPU 메모리의 자료를 기본 버퍼에 복사하려면, 임시 업로드 힙을 만들어야 한다.
	____________________________________________________________________________________________________________*/
	D3D12_HEAP_PROPERTIES Upload_HeapProperties;
	Upload_HeapProperties.Type                 = D3D12_HEAP_TYPE_UPLOAD;
	Upload_HeapProperties.CPUPageProperty      = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	Upload_HeapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	Upload_HeapProperties.CreationNodeMask     = 1;
	Upload_HeapProperties.VisibleNodeMask      = 1;

	D3D12_RESOURCE_DESC Upload_ResourceDest;
	Upload_ResourceDest.Dimension				= D3D12_RESOURCE_DIMENSION_BUFFER;
	Upload_ResourceDest.Alignment				= 0;
	Upload_ResourceDest.Width					= uiByteSize;
	Upload_ResourceDest.Height					= 1;
	Upload_ResourceDest.DepthOrArraySize		= 1;
	Upload_ResourceDest.MipLevels				= 1;
	Upload_ResourceDest.Format					= DXGI_FORMAT_UNKNOWN;
	Upload_ResourceDest.SampleDesc.Count		= 1;
	Upload_ResourceDest.SampleDesc.Quality		= 0;
	Upload_ResourceDest.Layout					= D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	Upload_ResourceDest.Flags					= D3D12_RESOURCE_FLAG_NONE;

	FAILED_CHECK_RETURN(m_pGraphicDevice->CreateCommittedResource(&Upload_HeapProperties,
																  D3D12_HEAP_FLAG_NONE,
																  &Upload_ResourceDest,
																  D3D12_RESOURCE_STATE_GENERIC_READ,
																  nullptr,
																  IID_PPV_ARGS(&pUploadBuffer)), 
																  NULL);

	
	/*__________________________________________________________________________________________________________
	[ 기본 버퍼에 복사할 자료를 서술 ]
	____________________________________________________________________________________________________________*/
	D3D12_SUBRESOURCE_DATA subResourceData = {};
	subResourceData.pData		= InitData;
	subResourceData.RowPitch	= uiByteSize;
	subResourceData.SlicePitch	= subResourceData.RowPitch;

	/*__________________________________________________________________________________________________________
	[ 기본 버퍼 자원으로의 자료 복사를 요청 ]
	- 계략적으로 말하자면, 보조 함수 UpdateSubresources는 CPU 메모리를 임시 업로드 힙에 복사하고,
	ID3D12CommandList::CopySubresourceRegion을 이용해서 임시 업로드 힙의 자료를 mBuffer에 복사한다.
	____________________________________________________________________________________________________________*/
	D3D12_RESOURCE_BARRIER ResourceBarrier;
	ResourceBarrier.Type                    = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	ResourceBarrier.Flags                   = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	ResourceBarrier.Transition.Subresource  = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	ResourceBarrier.Transition.pResource    = pDefaultBuffer;
	ResourceBarrier.Transition.StateBefore  = D3D12_RESOURCE_STATE_COMMON;
	ResourceBarrier.Transition.StateAfter   = D3D12_RESOURCE_STATE_COPY_DEST;

	m_pCommandList->ResourceBarrier(1, &ResourceBarrier);

	UpdateSubresources<1>(m_pCommandList, 
						  pDefaultBuffer, 
						  pUploadBuffer, 
						  0, 
						  0, 
						  1,
						  &subResourceData);

	ResourceBarrier.Transition.pResource    = pDefaultBuffer;
	ResourceBarrier.Transition.StateBefore  = D3D12_RESOURCE_STATE_COPY_DEST;
	ResourceBarrier.Transition.StateAfter   = D3D12_RESOURCE_STATE_GENERIC_READ;

	m_pCommandList->ResourceBarrier(1, &ResourceBarrier);

	/*__________________________________________________________________________________________________________
	[ 주의 ]
	- 위의 함수 호출 이후에도, UploadBuffer를 계속 유지해야 한다.
	- 실제로 복사를 수행하는 명령 목록이 아직 실행되지 않았기 때문이다.
	- 복사가 완료되었음이 확실해진 후에 호출자가 UploadBuffer를 해제하면 된다.
	____________________________________________________________________________________________________________*/

	Engine::CGraphicDevice::Get_Instance()->End_ResetCmdList(CMDID::CMD_MAIN);

	return pDefaultBuffer;
}

void CVIBuffer::Free()
{
	CComponent::Free();

	Engine::Safe_Release(m_pVB_CPU);
	Engine::Safe_Release(m_pIB_CPU);
	Engine::Safe_Release(m_pVB_GPU);
	Engine::Safe_Release(m_pIB_GPU);

}
