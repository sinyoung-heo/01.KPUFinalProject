#include "TerrainTex.h"
#include "GraphicDevice.h"
#include "ShaderBumpTerrain.h"

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
	, vecVertices(rhs.vecVertices)
	, vecIndices(rhs.vecIndices)
{
}


HRESULT CTerrainTex::Ready_Buffer(const _uint & iNumVerticesX,
								  const _uint & iNumVerticesZ,
								  const _float & fInterval,
								  const _float & fDetail)
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
	vecVertices.resize(uiVertexCnt);

	for (_uint i = 0; i < m_iNumVerticesZ; ++i)
	{
		for (_uint j = 0; j < m_iNumVerticesX; ++j)
		{
			_int iIdx = i * iNumVerticesX + j;

			vecVertices[iIdx].vPos		= _vec3(j * m_fInterval, 0.f, i * m_fInterval);
			vecVertices[iIdx].vTexUV	= _vec2((j / (iNumVerticesX - 1.f)) * fDetail, (i / (iNumVerticesZ - 1.f)) * fDetail);
		}
	}

	/*__________________________________________________________________________________________________________
	[ Index Buffer ]
	____________________________________________________________________________________________________________*/
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

			vSour = vecVertices[vecIndices[k + 1]].vPos - vecVertices[vecIndices[k]].vPos;
			vDest = vecVertices[vecIndices[k + 2]].vPos - vecVertices[vecIndices[k + 1]].vPos;

			k += 3;

			vecIndices[k]		= (_uint)iIndex + iNumVerticesX;
			vecIndices[k + 1]	= (_uint)iIndex + 1;
			vecIndices[k + 2]	= (_uint)iIndex;

			vSour = vecVertices[vecIndices[k + 1]].vPos - vecVertices[vecIndices[k]].vPos;
			vDest = vecVertices[vecIndices[k + 2]].vPos - vecVertices[vecIndices[k + 1]].vPos;

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

void CTerrainTex::Render_BumpTerrain(ID3D12GraphicsCommandList* pCommandList,
									 const _int& iContextIdx, 
									 CShader* pShader)
{
	static_cast<CShaderBumpTerrain*>(pShader)->Begin_Shader(pCommandList, iContextIdx);
	Begin_Buffer(pCommandList);
	Render_Buffer(pCommandList);
}

void CTerrainTex::Begin_Buffer(ID3D12GraphicsCommandList* pCommandList)
{
	pCommandList->IASetVertexBuffers(0, 						// 시작 슬롯. (입력 슬롯은 총 16개)
									 1, 						// 입력 슬롯들에 묶을 정점 버퍼 개수.
									 &Get_VertexBufferView());	// 정점 버퍼 뷰의 첫 원소를 가리키는 포인터.
	pCommandList->IASetIndexBuffer(&Get_IndexBufferView());
	pCommandList->IASetPrimitiveTopology(m_PrimitiveTopology);
}

void CTerrainTex::Render_Buffer(ID3D12GraphicsCommandList* pCommandList)
{
	pCommandList->DrawIndexedInstanced(m_tSubMeshGeometry.uiIndexCount,	// 그리기에 사용할 인덱스들의 개수. (인스턴스 당)
									   1,								// 그릴 인스턴스 개수.
									   0,								// 인덱스 버퍼의 첫 index
									   0, 								// 그리기 호출에 쓰이는 인덱스들에 더할 정수 값.
									   0);								// 인스턴싱
}

CComponent * CTerrainTex::Clone()
{
	return new CTerrainTex(*this);
}

CTerrainTex * CTerrainTex::Create(ID3D12Device * pGraphicDevice,
										ID3D12GraphicsCommandList * pCommandList, 
										const _uint & iNumVerticesX, 
										const _uint & iNumVerticesZ, 
										const _float & fInterval,
										const _float& fDetail)
{
	CTerrainTex* pInstance = new CTerrainTex(pGraphicDevice, pCommandList);

	if (FAILED(pInstance->Ready_Buffer(iNumVerticesX, iNumVerticesZ, fInterval, fDetail)))
		Engine::Safe_Release(pInstance);

	return pInstance;
}

void CTerrainTex::Free()
{
	CVIBuffer::Free();
}
