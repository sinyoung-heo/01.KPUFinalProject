#include "CoordinateCol.h"

USING(Engine)

CCoordinateCol::CCoordinateCol(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList)
	: CVIBuffer(pGraphicDevice, pCommandList)
{
}

CCoordinateCol::CCoordinateCol(const CCoordinateCol& rhs)
	: CVIBuffer(rhs)
{
}

HRESULT CCoordinateCol::Ready_Buffer()
{
	FAILED_CHECK_RETURN(CVIBuffer::Ready_Buffer(), E_FAIL);

	m_PrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_LINELIST;

	/*__________________________________________________________________________________________________________
	[ Vertex Buffer ]
	____________________________________________________________________________________________________________*/
	array<VTXCOL, 6> vertices =
	{
		// X축
		VTXCOL(_vec3(0.0f, 0.0f, 0.0f), _rgba(1.0f, 0.5f, 0.0f, 1.0f)),	// 0
		VTXCOL(_vec3(1.0f, 0.0f, 0.0f), _rgba(1.0f, 0.5f, 0.0f, 1.0f)),	// 1

		// Y축
		VTXCOL(_vec3(0.0f, 0.0f, 0.0f), _rgba(0.0f, 1.f, 0.0f, 1.0f)),	// 2
		VTXCOL(_vec3(0.0f, 1.0f, 0.0f), _rgba(0.0f, 1.f, 0.0f, 1.0f)),	// 3

		// Z축
		VTXCOL(_vec3(0.0f, 0.0f, 0.0f), _rgba(0.0f, 1.f, 1.0f, 1.0f)),	// 4
		VTXCOL(_vec3(0.0f, 0.0f, 1.0f), _rgba(0.0f, 1.f, 1.0f, 1.0f))	// 5
	};

	const _int uiVB_ByteSize = (_uint)vertices.size() * sizeof(VTXCOL);

	/*__________________________________________________________________________________________________________
	[ Vertex & Index Buffer 복사 ]
	____________________________________________________________________________________________________________*/
	FAILED_CHECK_RETURN(D3DCreateBlob(uiVB_ByteSize, &m_pVB_CPU), E_FAIL);
	CopyMemory(m_pVB_CPU->GetBufferPointer(), vertices.data(), uiVB_ByteSize);


	/*__________________________________________________________________________________________________________
	[ Vertex & Index Buffer 생성 ]
	____________________________________________________________________________________________________________*/
	m_pVB_GPU = Create_DefaultBuffer(vertices.data(), uiVB_ByteSize, m_pVB_Uploader);
	NULL_CHECK_RETURN(m_pVB_GPU, E_FAIL);


	m_uiVertexByteStride	= sizeof(VTXCOL);
	m_uiVB_ByteSize			= uiVB_ByteSize;

	m_tSubMeshGeometry.uiStartIndexLocation = 0;
	m_tSubMeshGeometry.iBaseVertexLocation	= 0;

	return S_OK;
}

void CCoordinateCol::Begin_Buffer()
{
	/*__________________________________________________________________________________________________________
	[ 메쉬의 정점 버퍼 뷰와 인덱스 버퍼 뷰를 설정 ]
	____________________________________________________________________________________________________________*/
	m_pCommandList->IASetVertexBuffers(0, 						 // 시작 슬롯. (입력 슬롯은 총 16개)
									   1, 						 // 입력 슬롯들에 묶을 정점 버퍼 개수.
									   &Get_VertexBufferView()); // 정점 버퍼 뷰의 첫 원소를 가리키는 포인터.

	/*__________________________________________________________________________________________________________
	[ 메쉬의 프리미티브 유형을 설정 ]
	____________________________________________________________________________________________________________*/
	m_pCommandList->IASetPrimitiveTopology(m_PrimitiveTopology);
}

void CCoordinateCol::Render_Buffer()
{
	/*__________________________________________________________________________________________________________
	[ 정점들이 파이프라인의 입력 조립기 단계로 공급 ]
	____________________________________________________________________________________________________________*/
	m_pCommandList->DrawInstanced(6,	// 정점의 개수.
								  1,	// 그릴 인스턴스 개수.
								  0,	// 첫 번째 정점의 인덱스.
								  0);	// 정점 버퍼에서 인스턴스 별 데이터를 읽기 전에 각 인덱스에 추가 된 값입니다.
}

CComponent* CCoordinateCol::Clone()
{
	return new CCoordinateCol(*this);
}

CCoordinateCol* CCoordinateCol::Create(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList)
{
	CCoordinateCol* pInstance = new CCoordinateCol(pGraphicDevice, pCommandList);

	if (FAILED(pInstance->Ready_Buffer()))
		Engine::Safe_Release(pInstance);

	return pInstance;
}

void CCoordinateCol::Free()
{
	CVIBuffer::Free();
}
