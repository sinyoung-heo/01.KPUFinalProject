#include "ScreenTex.h"

USING(Engine)

CScreenTex::CScreenTex(ID3D12Device * pGraphicDevice, ID3D12GraphicsCommandList * pCommandList)
	: CVIBuffer(pGraphicDevice, pCommandList)
{
}

CScreenTex::CScreenTex(const CScreenTex & rhs)
	: CVIBuffer(rhs)
{
}

HRESULT CScreenTex::Ready_Buffer()
{
	FAILED_CHECK_RETURN(CVIBuffer::Ready_Buffer(), E_FAIL);

	m_PrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	/*__________________________________________________________________________________________________________
	[ Vertex Buffer ]
	____________________________________________________________________________________________________________*/
	array<VTXSCREEN, 4> vertices =
	{
		//		Pos							TexUV
		VTXSCREEN(_vec3(-1.0f, 1.0f, 0.f), _vec2(0.0f, 0.0f)),	// 0
		VTXSCREEN(_vec3(1.0f, 1.0f, 0.f),  _vec2(1.0f, 0.0f)),	// 1
		VTXSCREEN(_vec3(1.0f, -1.0f, 0.f), _vec2(1.0f, 1.0f)),	// 2
		VTXSCREEN(_vec3(-1.0f, -1.0f, 0.f), _vec2(0.0f, 1.0f))	// 3
	};

	/*__________________________________________________________________________________________________________
	[ Index Buffer ]
	____________________________________________________________________________________________________________*/
	array<_uint, 6> indices =
	{
		0, 1, 2,	// 첫 번째 삼각형.
		0, 2, 3,	// 두 번째 삼각형
	};

	const _int uiVB_ByteSize = (_uint)vertices.size() * sizeof(VTXSCREEN);
	const _int uiIB_ByteSize = (_uint)indices.size() * sizeof(_uint);

	/*__________________________________________________________________________________________________________
	[ Vertex & Index Buffer 복사 ]
	____________________________________________________________________________________________________________*/
	FAILED_CHECK_RETURN((D3DCreateBlob(uiVB_ByteSize, &m_pVB_CPU)), E_FAIL);
	CopyMemory(m_pVB_CPU->GetBufferPointer(), vertices.data(), uiVB_ByteSize);

	FAILED_CHECK_RETURN((D3DCreateBlob(uiIB_ByteSize, &m_pIB_CPU)), E_FAIL);
	CopyMemory(m_pIB_CPU->GetBufferPointer(), indices.data(), uiIB_ByteSize);

	/*__________________________________________________________________________________________________________
	[ Vertex & Index Buffer 생성 ]
	____________________________________________________________________________________________________________*/
	m_pVB_GPU = Create_DefaultBuffer(vertices.data(), uiVB_ByteSize, m_pVB_Uploader);
	NULL_CHECK_RETURN(m_pVB_GPU, E_FAIL);

	m_pIB_GPU = Create_DefaultBuffer(indices.data(), uiIB_ByteSize, m_pIB_Uploader);
	NULL_CHECK_RETURN(m_pIB_GPU, E_FAIL);


	m_uiVertexByteStride	= sizeof(VTXSCREEN);
	m_uiVB_ByteSize			= uiVB_ByteSize;
	m_uiIB_ByteSize			= uiIB_ByteSize;
	m_IndexFormat			= DXGI_FORMAT_R32_UINT;

	m_tSubMeshGeometry.uiIndexCount			= (_uint)indices.size();
	m_tSubMeshGeometry.uiStartIndexLocation = 0;
	m_tSubMeshGeometry.iBaseVertexLocation	= 0;

	return S_OK;
}

void CScreenTex::Begin_Buffer()
{
	CVIBuffer::Begin_Buffer();
}

void CScreenTex::Render_Buffer()
{
	CVIBuffer::Render_Buffer();
}

CComponent * CScreenTex::Clone()
{
	return new CScreenTex(*this);
}

CScreenTex * CScreenTex::Create(ID3D12Device * pGraphicDevice, ID3D12GraphicsCommandList * pCommandList)
{
	CScreenTex* pInstance = new CScreenTex(pGraphicDevice, pCommandList);

	if (FAILED(pInstance->Ready_Buffer()))
		Safe_Release(pInstance);

	return pInstance;
}

void CScreenTex::Free()
{
	CVIBuffer::Free();
}
