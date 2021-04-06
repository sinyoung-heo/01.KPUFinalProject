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
	FAILED_CHECK_RETURN(m_pGraphicDevice->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
																  D3D12_HEAP_FLAG_NONE,
																  &CD3DX12_RESOURCE_DESC::Buffer(uiByteSize),
																  D3D12_RESOURCE_STATE_COMMON,
																  nullptr,
																  IID_PPV_ARGS(&pDefaultBuffer)), 
																  NULL);
 
	/*__________________________________________________________________________________________________________
	- CPU 메모리의 자료를 기본 버퍼에 복사하려면, 임시 업로드 힙을 만들어야 한다.
	____________________________________________________________________________________________________________*/
	FAILED_CHECK_RETURN(m_pGraphicDevice->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
																  D3D12_HEAP_FLAG_NONE,
																  &CD3DX12_RESOURCE_DESC::Buffer(uiByteSize),
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

	[ 주의 ]
	- 위의 함수 호출 이후에도, UploadBuffer를 계속 유지해야 한다.
	- 실제로 복사를 수행하는 명령 목록이 아직 실행되지 않았기 때문이다.
	- 복사가 완료되었음이 확실해진 후에 호출자가 UploadBuffer를 해제하면 된다.
	____________________________________________________________________________________________________________*/
	m_pCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(pDefaultBuffer,
																			 D3D12_RESOURCE_STATE_COMMON, 
																			 D3D12_RESOURCE_STATE_COPY_DEST));

	UpdateSubresources<1>(m_pCommandList, pDefaultBuffer, pUploadBuffer, 0, 0, 1, &subResourceData);

	m_pCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(pDefaultBuffer,
																			 D3D12_RESOURCE_STATE_COPY_DEST, 
																			 D3D12_RESOURCE_STATE_GENERIC_READ));

	Engine::CGraphicDevice::Get_Instance()->End_ResetCmdList(CMDID::CMD_MAIN);

	return pDefaultBuffer;
}

ID3D12Resource* CVIBuffer::Create_DynamicBuffer(const void* InitData,
												UINT64 uiByteSize, 
												ID3D12Resource*& pUploadBuffer)
{
	Engine::CGraphicDevice::Get_Instance()->Begin_ResetCmdList(Engine::CMDID::CMD_MAIN);

	ID3D12Resource* pDefaultBuffer = nullptr;

	Engine::FAILED_CHECK_RETURN(m_pGraphicDevice->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
																		  D3D12_HEAP_FLAG_NONE,
																		  &CD3DX12_RESOURCE_DESC::Buffer(uiByteSize),
																		  D3D12_RESOURCE_STATE_GENERIC_READ,
																		  nullptr,
																		  IID_PPV_ARGS(&pDefaultBuffer)),
																		  NULL);

	// Write Vertex Data to pVertexDataBegin dynamically
	pDefaultBuffer->Map(0, nullptr, (void**)&m_pVetexData);
	
	Engine::CGraphicDevice::Get_Instance()->End_ResetCmdList(Engine::CMDID::CMD_MAIN);

	return pDefaultBuffer;
}

CComponent* CVIBuffer::Clone(void)
{
	return nullptr;
}

void CVIBuffer::Free()
{
	CComponent::Free();
	Safe_Release(m_pVB_CPU);
	Safe_Release(m_pIB_CPU);
	Safe_Release(m_pVB_GPU);
	Safe_Release(m_pIB_GPU);

}
