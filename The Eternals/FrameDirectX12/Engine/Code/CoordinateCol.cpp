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
		// X��
		VTXCOL(_vec3(0.0f, 0.0f, 0.0f), _rgba(1.0f, 0.5f, 0.0f, 1.0f)),	// 0
		VTXCOL(_vec3(1.0f, 0.0f, 0.0f), _rgba(1.0f, 0.5f, 0.0f, 1.0f)),	// 1

		// Y��
		VTXCOL(_vec3(0.0f, 0.0f, 0.0f), _rgba(0.0f, 1.f, 0.0f, 1.0f)),	// 2
		VTXCOL(_vec3(0.0f, 1.0f, 0.0f), _rgba(0.0f, 1.f, 0.0f, 1.0f)),	// 3

		// Z��
		VTXCOL(_vec3(0.0f, 0.0f, 0.0f), _rgba(0.0f, 1.f, 1.0f, 1.0f)),	// 4
		VTXCOL(_vec3(0.0f, 0.0f, 1.0f), _rgba(0.0f, 1.f, 1.0f, 1.0f))	// 5
	};

	const _int uiVB_ByteSize = (_uint)vertices.size() * sizeof(VTXCOL);

	/*__________________________________________________________________________________________________________
	[ Vertex & Index Buffer ���� ]
	____________________________________________________________________________________________________________*/
	FAILED_CHECK_RETURN(D3DCreateBlob(uiVB_ByteSize, &m_pVB_CPU), E_FAIL);
	CopyMemory(m_pVB_CPU->GetBufferPointer(), vertices.data(), uiVB_ByteSize);


	/*__________________________________________________________________________________________________________
	[ Vertex & Index Buffer ���� ]
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
	[ �޽��� ���� ���� ��� �ε��� ���� �並 ���� ]
	____________________________________________________________________________________________________________*/
	m_pCommandList->IASetVertexBuffers(0, 						 // ���� ����. (�Է� ������ �� 16��)
									   1, 						 // �Է� ���Ե鿡 ���� ���� ���� ����.
									   &Get_VertexBufferView()); // ���� ���� ���� ù ���Ҹ� ����Ű�� ������.

	/*__________________________________________________________________________________________________________
	[ �޽��� ������Ƽ�� ������ ���� ]
	____________________________________________________________________________________________________________*/
	m_pCommandList->IASetPrimitiveTopology(m_PrimitiveTopology);
}

void CCoordinateCol::Render_Buffer()
{
	/*__________________________________________________________________________________________________________
	[ �������� ������������ �Է� ������ �ܰ�� ���� ]
	____________________________________________________________________________________________________________*/
	m_pCommandList->DrawInstanced(6,	// ������ ����.
								  1,	// �׸� �ν��Ͻ� ����.
								  0,	// ù ��° ������ �ε���.
								  0);	// ���� ���ۿ��� �ν��Ͻ� �� �����͸� �б� ���� �� �ε����� �߰� �� ���Դϴ�.
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
