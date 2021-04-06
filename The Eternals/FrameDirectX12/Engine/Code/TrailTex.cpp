#include "TrailTex.h"

USING(Engine)

CTrailTex::CTrailTex(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList)
	: CVIBuffer(pGraphicDevice, pCommandList)
{
}

CTrailTex::CTrailTex(const CTrailTex& rhs)
	: CVIBuffer(rhs)
	, m_arrVertices(rhs.m_arrVertices)
{
	const _int uiVB_ByteSize = (_uint)m_arrVertices.size() * sizeof(Engine::VTXTEX);
	m_pVB_GPU = Create_DynamicBuffer(m_arrVertices.data(), uiVB_ByteSize, m_pVB_Uploader);
}

HRESULT CTrailTex::Ready_Buffer()
{
	FAILED_CHECK_RETURN(CVIBuffer::Ready_Buffer(), E_FAIL);

	m_PrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	/*__________________________________________________________________________________________________________
	[ Vertex Buffer ]
	____________________________________________________________________________________________________________*/
	for (_uint i = 0; i < m_arrVertices.size(); ++i)
	{
		m_arrVertices[i].vPos   = _vec3(0.0f);
		m_arrVertices[i].vTexUV = _vec2(0.0f);
	}

	/*__________________________________________________________________________________________________________
	[ Index Buffer ]
	____________________________________________________________________________________________________________*/
	array<_uint, (TRAIL_SIZE - 2) * 3> indices;

	for (_uint i = 0, j = 0; i < (TRAIL_SIZE - 3) * 3; i += 3, ++j)
	{
		indices[i]		= 0;
		indices[i + 1]	= j + 1;
		indices[i + 2]	= j + 2;
	}

	const _int uiVB_ByteSize = (_uint)m_arrVertices.size() * sizeof(Engine::VTXTEX);
	const _int uiIB_ByteSize = (_uint)indices.size() * sizeof(_uint);

	Engine::FAILED_CHECK_RETURN(D3DCreateBlob(uiVB_ByteSize, &m_pVB_CPU), E_FAIL);
	CopyMemory(m_pVB_CPU->GetBufferPointer(), m_arrVertices.data(), uiVB_ByteSize);

	Engine::FAILED_CHECK_RETURN(D3DCreateBlob(uiIB_ByteSize, &m_pIB_CPU), E_FAIL);
	CopyMemory(m_pIB_CPU->GetBufferPointer(), indices.data(), uiIB_ByteSize);

	//// VB 积己.
	//m_pVB_GPU = Create_DynamicBuffer(m_arrVertices.data(), uiVB_ByteSize, m_pVB_Uploader);
	//Engine::NULL_CHECK_RETURN(m_pVB_GPU, E_FAIL);
	// 
	// IB 积己.
	m_pIB_GPU = Create_DefaultBuffer(indices.data(), uiIB_ByteSize, m_pIB_Uploader);
	Engine::NULL_CHECK_RETURN(m_pIB_GPU, E_FAIL);

	m_uiVertexByteStride = sizeof(Engine::VTXTEX);
	m_uiVB_ByteSize		 = uiVB_ByteSize;
	m_uiIB_ByteSize		 = uiIB_ByteSize;
	m_IndexFormat		 = DXGI_FORMAT_R32_UINT;

	m_tSubMeshGeometry.uiIndexCount			= (_uint)indices.size();
	m_tSubMeshGeometry.uiStartIndexLocation = 0;
	m_tSubMeshGeometry.iBaseVertexLocation	= 0;

	return S_OK;
}

void CTrailTex::Begin_Buffer()
{
	// Vertex滚欺 沥焊 CopyData.
	memcpy(m_pVetexData, m_arrVertices.data(), m_uiVB_ByteSize);

	CVIBuffer::Begin_Buffer();
}

void CTrailTex::Render_Buffer()
{
	CVIBuffer::Render_Buffer();
}

CComponent* CTrailTex::Clone()
{
	return new CTrailTex(*this);
}

CTrailTex* CTrailTex::Create(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList)
{
	CTrailTex* pInstance = new CTrailTex(pGraphicDevice, pCommandList);

	if (FAILED(pInstance->Ready_Buffer()))
		Safe_Release(pInstance);

	return pInstance;
}

void CTrailTex::Free()
{
	CVIBuffer::Free();
}
