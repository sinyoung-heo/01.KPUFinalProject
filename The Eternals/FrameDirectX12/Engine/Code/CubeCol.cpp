#include "CubeCol.h"
#include "ShaderColorInstancing.h"

USING(Engine)

CCubeCol::CCubeCol(ID3D12Device * pGraphicDevice, ID3D12GraphicsCommandList * pCommandList)
	: CVIBuffer(pGraphicDevice, pCommandList)
{
}

CCubeCol::CCubeCol(const CCubeCol & rhs)
	: CVIBuffer(rhs)

{
}


HRESULT CCubeCol::Ready_Buffer()
{
	FAILED_CHECK_RETURN(CVIBuffer::Ready_Buffer(), E_FAIL);

	m_PrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	/*__________________________________________________________________________________________________________
	[ Vertex Buffer ]
	____________________________________________________________________________________________________________*/
	array<VTXCOL, 8> vertices =
	{
		VTXCOL(_vec3(-0.5f, 0.5f, -0.5f), _rgba(RANDOM_COLOR)),
		VTXCOL(_vec3(0.5f, 0.5f, -0.5f), _rgba(RANDOM_COLOR)),
		VTXCOL(_vec3(0.5f, -0.5f, -0.5f), _rgba(RANDOM_COLOR)),
		VTXCOL(_vec3(-0.5f, -0.5f, -0.5f), _rgba(RANDOM_COLOR)),

		VTXCOL(_vec3(-0.5f, 0.5f, 0.5f), _rgba(RANDOM_COLOR)),
		VTXCOL(_vec3(0.5f, 0.5f, 0.5f), _rgba(RANDOM_COLOR)),
		VTXCOL(_vec3(0.5f, -0.5f, 0.5f), _rgba(RANDOM_COLOR)),
		VTXCOL(_vec3(-0.5f, -0.5f, 0.5f), _rgba(RANDOM_COLOR))
	};

	/*__________________________________________________________________________________________________________
	[ Index Buffer ]
	____________________________________________________________________________________________________________*/
	array<_uint, 36> indices =
	{
		// + X
		1, 5, 6,	
		1, 6, 2,

		// - X
		4, 0, 3,
		4, 3, 7,

		// + Y
		4, 5, 1,
		4, 1, 0,

		// - Y
		3, 2, 6,
		3, 6, 7,

		// + Z
		7, 6, 5,
		7, 5, 4,

		// - Z
		0, 1, 2,
		0, 2, 3
	};

	const _int uiVB_ByteSize = (_uint)vertices.size() * sizeof(VTXCOL);
	const _int uiIB_ByteSize = (_uint)indices.size() * sizeof(_uint);

	/*__________________________________________________________________________________________________________
	[ Vertex & Index Buffer 복사 ]
	____________________________________________________________________________________________________________*/
	FAILED_CHECK_RETURN(D3DCreateBlob(uiVB_ByteSize, &m_pVB_CPU), E_FAIL);
	CopyMemory(m_pVB_CPU->GetBufferPointer(), vertices.data(), uiVB_ByteSize);

	FAILED_CHECK_RETURN(D3DCreateBlob(uiIB_ByteSize, &m_pIB_CPU), E_FAIL);
	CopyMemory(m_pIB_CPU->GetBufferPointer(), indices.data(), uiIB_ByteSize);

	/*__________________________________________________________________________________________________________
	[ Vertex & Index Buffer 생성 ]
	____________________________________________________________________________________________________________*/
	m_pVB_GPU = Create_DefaultBuffer(vertices.data(), uiVB_ByteSize, m_pVB_Uploader);
	NULL_CHECK_RETURN(m_pVB_GPU, E_FAIL);

	m_pIB_GPU = Create_DefaultBuffer(indices.data(), uiIB_ByteSize, m_pIB_Uploader);
	NULL_CHECK_RETURN(m_pIB_GPU, E_FAIL);


	m_uiVertexByteStride	= sizeof(VTXCOL);
	m_uiVB_ByteSize			= uiVB_ByteSize;
	m_uiIB_ByteSize			= uiIB_ByteSize;
	m_IndexFormat			= DXGI_FORMAT_R32_UINT;

	m_tSubMeshGeometry.uiIndexCount			= (_uint)indices.size();
	m_tSubMeshGeometry.uiStartIndexLocation = 0;
	m_tSubMeshGeometry.iBaseVertexLocation	= 0;

	return S_OK;
}

void CCubeCol::Begin_Buffer()
{
	CVIBuffer::Begin_Buffer();
}

void CCubeCol::Render_Buffer()
{
	CVIBuffer::Render_Buffer();
}

CComponent * CCubeCol::Clone()
{
	CShaderColorInstancing::Get_Instance()->Add_TotalInstancCount(COLOR_BUFFER::BUFFER_CUBE);

	return new CCubeCol(*this);
}

CCubeCol * CCubeCol::Create(ID3D12Device * pGraphicDevice, ID3D12GraphicsCommandList * pCommandList)
{
	CCubeCol* pInstance = new CCubeCol(pGraphicDevice, pCommandList);

	if (FAILED(pInstance->Ready_Buffer()))
		Engine::Safe_Release(pInstance);

	return pInstance;
}

void CCubeCol::Free()
{
	CVIBuffer::Free();
	CShaderColorInstancing::Get_Instance()->Erase_TotalInstanceCount(COLOR_BUFFER::BUFFER_CUBE);

}
