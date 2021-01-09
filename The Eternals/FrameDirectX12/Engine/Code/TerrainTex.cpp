#include "TerrainTex.h"
#include "GraphicDevice.h"

USING(Engine)

CTerrainTex::CTerrainTex(ID3D12Device * pGraphicDevice, ID3D12GraphicsCommandList * pCommandList)
	: CVIBuffer(pGraphicDevice, pCommandList)
{
}

CTerrainTex::CTerrainTex(const CTerrainTex & rhs)
	: CVIBuffer(rhs)
	, m_iNumVerticesX(rhs.m_iNumVerticesX)
	, m_iNumVerticesZ(rhs.m_iNumVerticesZ)
	, m_fInterval(rhs.m_fInterval)
{
}


HRESULT CTerrainTex::Ready_Buffer(const _uint & iNumVerticesX,
								  const _uint & iNumVerticesZ,
								  const _float & fInterval)
{
	FAILED_CHECK_RETURN(CVIBuffer::Ready_Buffer(), E_FAIL);

	m_iNumVerticesX = iNumVerticesX;
	m_iNumVerticesZ = iNumVerticesZ;
	m_fInterval		= fInterval;

	_uint uiVertexCnt	= m_iNumVerticesX * m_iNumVerticesZ;
	_uint uiFaceCnt		= (m_iNumVerticesX - 1) * (m_iNumVerticesZ - 1) * 2;

	/*__________________________________________________________________________________________________________
	[ Vertex Buffer ]
	____________________________________________________________________________________________________________*/
	vector<VTXTEX> vecVertices;
	vecVertices.resize(uiVertexCnt);

	for (_uint i = 0; i < m_iNumVerticesZ; ++i)
	{
		for (_uint j = 0; j < m_iNumVerticesX; ++j)
		{
			_int iIdx = i * iNumVerticesX + j;

			vecVertices[iIdx].vPos		= _vec3(j * m_fInterval, 0.f, i * m_fInterval);
			vecVertices[iIdx].vNormal	= _vec3(0.f, 0.f, 0.f);
			vecVertices[iIdx].vTexUV	= _vec2((j / (iNumVerticesX - 1.f)) * 5.f, (i / (iNumVerticesZ - 1.f)) * 5.f);
		}
	}

	/*__________________________________________________________________________________________________________
	[ Index Buffer ]
	____________________________________________________________________________________________________________*/
	vector<_uint> vecIndices;
	vecIndices.resize(uiFaceCnt * 3);

	_uint k = 0;

	for (_uint i = 0; i < m_iNumVerticesZ - 1; ++i)
	{
		for (_uint j = 0; j < m_iNumVerticesX - 1; ++j)
		{
			size_t iIndex = i * iNumVerticesX + j;

			vecIndices[k]		= (_uint)iIndex + iNumVerticesX;
			vecIndices[k + 1]	= (_uint)iIndex + iNumVerticesX + 1;
			vecIndices[k + 2]	= (_uint)iIndex + 1;

			_vec3 vSour, vDest;
			_vec3 vNormal;

			vSour = vecVertices[vecIndices[k + 1]].vPos - vecVertices[vecIndices[k]].vPos;
			vDest = vecVertices[vecIndices[k + 2]].vPos - vecVertices[vecIndices[k + 1]].vPos;

			vNormal = vSour.Cross_InputDst(vDest);
			vNormal.Normalize();

			vecVertices[vecIndices[k]].vNormal		+= vNormal;
			vecVertices[vecIndices[k + 1]].vNormal	+= vNormal;
			vecVertices[vecIndices[k + 2]].vNormal	+= vNormal;

			k += 3;

			vecIndices[k]		= (_uint)iIndex + iNumVerticesX;
			vecIndices[k + 1]	= (_uint)iIndex + 1;
			vecIndices[k + 2]	= (_uint)iIndex;

			vSour = vecVertices[vecIndices[k + 1]].vPos - vecVertices[vecIndices[k]].vPos;
			vDest = vecVertices[vecIndices[k + 2]].vPos - vecVertices[vecIndices[k + 1]].vPos;

			vNormal = vSour.Cross_InputDst(vDest);
			vNormal.Normalize();

			vecVertices[vecIndices[k]].vNormal		+= vNormal;
			vecVertices[vecIndices[k + 1]].vNormal	+= vNormal;
			vecVertices[vecIndices[k + 2]].vNormal	+= vNormal;

			k += 3; // next quad
		}
	}


	const _uint vbByteSize = (_uint)vecVertices.size() * sizeof(VTXTEX);
	const _uint ibByteSize = (_uint)vecIndices.size() * sizeof(_uint);

	/*__________________________________________________________________________________________________________
	[ Vertex & Index Buffer 복사 ]
	____________________________________________________________________________________________________________*/
	FAILED_CHECK_RETURN(D3DCreateBlob(vbByteSize, &m_pVB_CPU), E_FAIL);
	CopyMemory(m_pVB_CPU->GetBufferPointer(), vecVertices.data(), vbByteSize);

	FAILED_CHECK_RETURN(D3DCreateBlob(ibByteSize, &m_pIB_CPU), E_FAIL);
	CopyMemory(m_pIB_CPU->GetBufferPointer(), vecIndices.data(), ibByteSize);

	/*__________________________________________________________________________________________________________
	[ Vertex & Index Buffer 생성 ]
	____________________________________________________________________________________________________________*/
	m_pVB_GPU = Create_DefaultBuffer(vecVertices.data(), vbByteSize, m_pVB_Uploader);
	NULL_CHECK_RETURN(m_pVB_GPU, E_FAIL);

	m_pIB_GPU = Create_DefaultBuffer(vecIndices.data(), ibByteSize, m_pIB_Uploader);
	NULL_CHECK_RETURN(m_pIB_GPU, E_FAIL);


	m_uiVertexByteStride	= sizeof(VTXTEX);
	m_uiVB_ByteSize			= vbByteSize;
	m_uiIB_ByteSize			= ibByteSize;
	m_IndexFormat			= DXGI_FORMAT_R32_UINT;

	m_tSubMeshGeometry.uiIndexCount			= (_uint)vecIndices.size();
	m_tSubMeshGeometry.uiStartIndexLocation = 0;
	m_tSubMeshGeometry.iBaseVertexLocation	= 0;

	return S_OK;
}

void CTerrainTex::Begin_Buffer()
{
	CVIBuffer::Begin_Buffer();
}


void CTerrainTex::Render_Buffer()
{
	CVIBuffer::Render_Buffer();
}

CComponent * CTerrainTex::Clone()
{
	return new CTerrainTex(*this);
}

CTerrainTex * CTerrainTex::Create(ID3D12Device * pGraphicDevice,
										ID3D12GraphicsCommandList * pCommandList, 
										const _uint & iNumVerticesX, 
										const _uint & iNumVerticesZ, 
										const _float & fInterval)
{
	CTerrainTex* pInstance = new CTerrainTex(pGraphicDevice, pCommandList);

	if (FAILED(pInstance->Ready_Buffer(iNumVerticesX, iNumVerticesZ, fInterval)))
		Engine::Safe_Release(pInstance);

	return pInstance;
}

void CTerrainTex::Free()
{
	CVIBuffer::Free();
}
