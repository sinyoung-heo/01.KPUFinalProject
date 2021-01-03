#include "CubeTex.h"

USING(Engine)

CCubeTex::CCubeTex(ID3D12Device * pGraphicDevice, ID3D12GraphicsCommandList * pCommandList)
	: CVIBuffer(pGraphicDevice, pCommandList)
{
}

CCubeTex::CCubeTex(const CCubeTex & rhs)
	: CVIBuffer(rhs)
{
}

HRESULT CCubeTex::Ready_Buffer()
{
	FAILED_CHECK_RETURN(CVIBuffer::Ready_Buffer(), E_FAIL);

	m_PrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	/*__________________________________________________________________________________________________________
	[ Vertex Buffer ]
	____________________________________________________________________________________________________________*/
	array<VTXCUBE, 8> vertices =
	{
		VTXCUBE(_vec3(-0.5f, 0.5f, -0.5f), _vec3(-0.5f, 0.5f, -0.5f)),
		VTXCUBE(_vec3(0.5f, 0.5f, -0.5f), _vec3(0.5f, 0.5f, -0.5f)),
		VTXCUBE(_vec3(0.5f, -0.5f, -0.5f), _vec3(0.5f, -0.5f, -0.5f)),
		VTXCUBE(_vec3(-0.5f, -0.5f, -0.5f), _vec3(-0.5f, -0.5f, -0.5f)),

		VTXCUBE(_vec3(-0.5f, 0.5f, 0.5f), _vec3(-0.5f, 0.5f, 0.5f)),
		VTXCUBE(_vec3(0.5f, 0.5f, 0.5f), _vec3(0.5f, 0.5f, 0.5f)),
		VTXCUBE(_vec3(0.5f, -0.5f, 0.5f), _vec3(0.5f, -0.5f, 0.5f)),
		VTXCUBE(_vec3(-0.5f, -0.5f, 0.5f), _vec3(-0.5f, -0.5f, 0.5f))
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

	const _int uiVB_ByteSize = (_uint)vertices.size() * sizeof(VTXCUBE);
	const _int uiIB_ByteSize = (_uint)indices.size() * sizeof(_uint);

	/*__________________________________________________________________________________________________________
	[ Vertex & Index Buffer ���� ]
	____________________________________________________________________________________________________________*/
	FAILED_CHECK_RETURN(D3DCreateBlob(uiVB_ByteSize, &m_pVB_CPU), E_FAIL);
	CopyMemory(m_pVB_CPU->GetBufferPointer(), vertices.data(), uiVB_ByteSize);

	FAILED_CHECK_RETURN(D3DCreateBlob(uiIB_ByteSize, &m_pIB_CPU), E_FAIL);
	CopyMemory(m_pIB_CPU->GetBufferPointer(), indices.data(), uiIB_ByteSize);

	/*__________________________________________________________________________________________________________
	[ Vertex & Index Buffer ���� ]
	____________________________________________________________________________________________________________*/
	m_pVB_GPU = Create_DefaultBuffer(vertices.data(), uiVB_ByteSize, m_pVB_Uploader);
	NULL_CHECK_RETURN(m_pVB_GPU, E_FAIL);

	m_pIB_GPU = Create_DefaultBuffer(indices.data(), uiIB_ByteSize, m_pIB_Uploader);
	NULL_CHECK_RETURN(m_pIB_GPU, E_FAIL);


	m_uiVertexByteStride	= sizeof(VTXCUBE);
	m_uiVB_ByteSize			= uiVB_ByteSize;
	m_uiIB_ByteSize			= uiIB_ByteSize;
	m_IndexFormat			= DXGI_FORMAT_R32_UINT;

	m_tSubMeshGeometry.uiIndexCount			= (_uint)indices.size();
	m_tSubMeshGeometry.uiStartIndexLocation = 0;
	m_tSubMeshGeometry.iBaseVertexLocation	= 0;

	return S_OK;
}

void CCubeTex::Begin_Buffer()
{
	CVIBuffer::Begin_Buffer();
}

void CCubeTex::Render_Buffer()
{
	CVIBuffer::Render_Buffer();
}

CComponent * CCubeTex::Clone()
{
	return new CCubeTex(*this);
}

CCubeTex * CCubeTex::Create(ID3D12Device * pGraphicDevice, ID3D12GraphicsCommandList * pCommandList)
{
	CCubeTex* pInstance = new CCubeTex(pGraphicDevice, pCommandList);

	if (FAILED(pInstance->Ready_Buffer()))
		Safe_Release(pInstance);

	return pInstance;
}

void CCubeTex::Free()
{
	CVIBuffer::Free();
}
