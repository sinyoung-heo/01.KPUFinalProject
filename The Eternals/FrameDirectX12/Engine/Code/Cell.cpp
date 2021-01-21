#include "Cell.h"
#include "GraphicDevice.h"
#include "ComponentMgr.h"

USING(Engine)

CCell::CCell(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList)
	: CVIBuffer(pGraphicDevice, pCommandList)
	, m_dwCurrentIdx(0)
{
	ZeroMemory(m_pNeighbor, sizeof(CCell*) * NEIGHBOR_END);

	for (_int i = 0; i < POINT_END; ++i)
	{
		m_pColliderCom[i]	= nullptr;
		m_matWorld[i]		= INIT_MATRIX;
	}

}

HRESULT CCell::Ready_Cell(const _ulong& dwIndex, 
						  const _vec3& vPointA,
						  const _vec3& vPointB, 
						  const _vec3& vPointC)
{
	m_dwCurrentIdx = dwIndex;

	m_vPoint[POINT_A] = vPointA;
	m_vPoint[POINT_B] = vPointB;
	m_vPoint[POINT_C] = vPointC;

	m_pLine[LINE_AB] = CLine::Create(&_vec2(m_vPoint[POINT_A].x, m_vPoint[POINT_A].z),
									 &_vec2(m_vPoint[POINT_B].x, m_vPoint[POINT_B].z));

	m_pLine[LINE_BC] = CLine::Create(&_vec2(m_vPoint[POINT_B].x, m_vPoint[POINT_B].z),
									 &_vec2(m_vPoint[POINT_C].x, m_vPoint[POINT_C].z));

	m_pLine[LINE_CA] = CLine::Create(&_vec2(m_vPoint[POINT_C].x, m_vPoint[POINT_C].z),
									 &_vec2(m_vPoint[POINT_A].x, m_vPoint[POINT_A].z));

	// Height 산출용 평면 구조체.
	m_tPlane.Plane_FromPoints(m_vPoint[POINT_A], m_vPoint[POINT_B], m_vPoint[POINT_C]);

	FAILED_CHECK_RETURN(Ready_Component(), E_FAIL);

	return S_OK;
}

HRESULT CCell::Ready_Component()
{
	// Shader
	m_pShaderCom = static_cast<CShaderColor*>(CComponentMgr::Get_Instance()->Clone_Component(L"ShaderColor", COMPONENTID::ID_STATIC));
	NULL_CHECK_RETURN(m_pShaderCom, E_FAIL);
	m_pShaderCom->Set_PipelineStatePass(1);

	// Collider
	for (_int i = 0; i < POINT_END; ++i)
	{
		m_pColliderCom[i] = static_cast<CColliderSphere*>(CComponentMgr::Get_Instance()->Clone_Component(L"ColliderSphere", Engine::COMPONENTID::ID_DYNAMIC));
		NULL_CHECK_RETURN(m_pColliderCom[i], E_FAIL);

		m_matWorld[i] = XMMatrixTranslation(m_vPoint[i].x, m_vPoint[i].y, m_vPoint[i].z);
		m_pColliderCom[i]->Set_ParentMatrix(&m_matWorld[i]);	// Parent Matrix
		m_pColliderCom[i]->Set_Scale(_vec3(0.5f, 0.5f, 0.5f));		// Collider Scale
		m_pColliderCom[i]->Set_Radius(_vec3(1.f, 1.f, 1.f));	// Collider Radius


	}

	// Buffer
	FAILED_CHECK_RETURN(CVIBuffer::Ready_Buffer(), E_FAIL);
	m_PrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	/*__________________________________________________________________________________________________________
	[ Vertex Buffer ]
	____________________________________________________________________________________________________________*/
	array<VTXCOL, 3> vertices =
	{
		VTXCOL(m_vPoint[POINT_A], _rgba(RANDOM_COLOR)),	// 0
		VTXCOL(m_vPoint[POINT_B], _rgba(RANDOM_COLOR)),	// 1
		VTXCOL(m_vPoint[POINT_C], _rgba(RANDOM_COLOR)),	// 2
	};

	/*__________________________________________________________________________________________________________
	[ Index Buffer ]
	____________________________________________________________________________________________________________*/
	array<_uint, 3> indices = { 0, 1, 2	}; // 첫 번째 삼각형.

	const _int uiVB_ByteSize = (_uint)vertices.size() * sizeof(VTXCOL);
	const _int uiIB_ByteSize = (_uint)indices.size() * sizeof(_uint);

	FAILED_CHECK_RETURN(D3DCreateBlob(uiVB_ByteSize, &m_pVB_CPU), E_FAIL);
	CopyMemory(m_pVB_CPU->GetBufferPointer(), vertices.data(), uiVB_ByteSize);

	FAILED_CHECK_RETURN(D3DCreateBlob(uiIB_ByteSize, &m_pIB_CPU), E_FAIL);
	CopyMemory(m_pIB_CPU->GetBufferPointer(), indices.data(), uiIB_ByteSize);

	// VB 생성.
	m_pVB_GPU = Create_DefaultBuffer(vertices.data(), uiVB_ByteSize, m_pVB_Uploader);
	NULL_CHECK_RETURN(m_pVB_GPU, E_FAIL);

	// IB 생성.
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

void CCell::Update_Component(const _float& fTimeDelta)
{
	for (_int i = 0; i < POINT_END; ++i)
	{
		m_matWorld[i] = XMMatrixTranslation(m_vPoint[i].x, m_vPoint[i].y, m_vPoint[i].z);
		m_pColliderCom[i]->Set_ParentMatrix(&m_matWorld[i]);

		m_pColliderCom[i]->Update_Component(fTimeDelta);
	}

}

void CCell::Render_Component(const _float& fTimeDelta)
{
	Set_ConstantTable();

	m_pShaderCom->Begin_Shader();
	CVIBuffer::Begin_Buffer();

	CVIBuffer::Render_Buffer();
}

void CCell::Set_ConstantTable()
{
	/*__________________________________________________________________________________________________________
	[ Set ConstantBuffer Data ]
	____________________________________________________________________________________________________________*/
	CB_SHADER_COLOR tCB_ShaderColor;
	ZeroMemory(&tCB_ShaderColor, sizeof(CB_SHADER_COLOR));
	tCB_ShaderColor.matWorld	= CShader::Compute_MatrixTranspose(INIT_MATRIX);
	tCB_ShaderColor.vColor		= _rgba(1.f, 0.f, 0.f, 1.f);

	m_pShaderCom->Get_UploadBuffer_ShaderColor()->CopyData(0, tCB_ShaderColor);
}

_bool CCell::Compare_Point(const _vec3* pPointF, const _vec3* pPointS, CCell* pCell)
{
	if (m_vPoint[POINT_A] == *pPointF)
	{
		if (m_vPoint[POINT_B] == *pPointS)
		{
			m_pNeighbor[NEIGHBOR_AB] = pCell;
			return true;
		}
		if (m_vPoint[POINT_C] == *pPointS)
		{
			m_pNeighbor[NEIGHBOR_CA] = pCell;
			return true;
		}
	}

	if (m_vPoint[POINT_B] == *pPointF)
	{
		if (m_vPoint[POINT_A] == *pPointS)
		{
			m_pNeighbor[NEIGHBOR_AB] = pCell;
			return true;
		}
		if (m_vPoint[POINT_C] == *pPointS)
		{
			m_pNeighbor[NEIGHBOR_BC] = pCell;
			return true;
		}
	}

	if (m_vPoint[POINT_C] == *pPointF)
	{
		if (m_vPoint[POINT_A] == *pPointS)
		{
			m_pNeighbor[NEIGHBOR_CA] = pCell;
			return true;
		}
		if (m_vPoint[POINT_B] == *pPointS)
		{
			m_pNeighbor[NEIGHBOR_BC] = pCell;
			return true;
		}
	}

	return false;
}

CCell::COMPARE CCell::Compare(const _vec3* pEndPos,
							  _ulong* pIndex,
							  const _float& fTargetSpeed, 
							  _vec3* pTargetPos, 
							  _vec3* pTargetDir,
							  _vec3* pSlidingDir)
{
	for (_ulong i = 0; i < LINE_END; ++i)
	{
		if (CLine::COMPARE_LEFT == m_pLine[i]->Compare(&_vec2(pEndPos->x, pEndPos->z)))
		{
			if (nullptr == m_pNeighbor[i])
			{
				/*____________________________________________________________________
				선분의 법선벡터
				______________________________________________________________________*/
				_vec3 vNormal	= _vec3(m_pLine[i]->Get_Normal().x, 0.f, m_pLine[i]->Get_Normal().y);

				/*____________________________________________________________________
				Slideing Vector 공식.
				S = P - n ( Dot(P,n) )
				______________________________________________________________________*/
				*pSlidingDir = *pTargetDir - vNormal * pTargetDir->Dot(vNormal);

				_vec3 doubleNormal	= *pTargetDir - (vNormal * 1.4f) * pTargetDir->Dot(vNormal * 1.4f);
				_vec3 TripleNormal	= *pTargetDir - (vNormal * 2.f) * pTargetDir->Dot(vNormal * 2.f);

				_vec3 vEndPos		= *pTargetPos + *pSlidingDir * fTargetSpeed * 5.f;
				_vec3 vDoublePos	= *pTargetPos + doubleNormal * fTargetSpeed * 5.f;
				_vec3 vTriplePos	= *pTargetPos + TripleNormal * fTargetSpeed * 5.f;

				/*____________________________________________________________________
				Start & Finish점 비교.
				꼭지점을 뚫고가지 않도록 예외처리.
				______________________________________________________________________*/
				_float fMaxX	= max(m_pLine[i]->Get_Point(CLine::POINT_START).x, m_pLine[i]->Get_Point(CLine::POINT_FINISH).x);
				_float fMinX	= min(m_pLine[i]->Get_Point(CLine::POINT_START).x, m_pLine[i]->Get_Point(CLine::POINT_FINISH).x);

				_float fMaxZ	= max(m_pLine[i]->Get_Point(CLine::POINT_START).y, m_pLine[i]->Get_Point(CLine::POINT_FINISH).y);
				_float fMinZ	= min(m_pLine[i]->Get_Point(CLine::POINT_START).y, m_pLine[i]->Get_Point(CLine::POINT_FINISH).y);

				if ((vEndPos.x > fMinX) && (vEndPos.x < fMaxX) &&
					(vEndPos.z > fMinZ) && (vEndPos.z < fMaxZ))
				{
					return COMPARE_SLIDING;
				}
				else
				{
					*pTargetDir = doubleNormal;
					*pSlidingDir = TripleNormal;
					return COMPARE_STOP;
				}
			}
			else
			{
				*pIndex = *m_pNeighbor[i]->Get_Index();

				return COMPARE_MOVE;
			}
		}
	}

	return COMPARE_MOVE;
}

CCell::COMPARE CCell::Compare(_vec3* pEndPos, 
							  _ulong* pIndex, 
							  _int* iLineIndex)
{
	for (_ulong i = 0; i < LINE_END; ++i)
	{
		if (CLine::COMPARE_LEFT == m_pLine[i]->Compare(&_vec2(pEndPos->x, pEndPos->z)))
		{
			if (nullptr == m_pNeighbor[i])
			{
				*iLineIndex = i;
				return COMPARE_STOP;
			}
			else
			{
				*pIndex = *m_pNeighbor[i]->Get_Index();
				return COMPARE_MOVE;
			}
		}
	}

	return COMPARE();
}

float CCell::Position_On_Height(const _vec3* vPos)
{
	float fHegiht = ((m_tPlane.x * vPos->x + m_tPlane.z * vPos->z) + m_tPlane.w) / -m_tPlane.y;

	return fHegiht;
}

CCell* CCell::Create(ID3D12Device* pGraphicDevice, 
					 ID3D12GraphicsCommandList* pCommandList,
					 const _ulong& dwIndex,
					 const _vec3& vPointA,
					 const _vec3& vPointB,
					 const _vec3& vPointC)
{
	CCell* pInstance = new CCell(pGraphicDevice, pCommandList);

	if (FAILED(pInstance->Ready_Cell(dwIndex, vPointA, vPointB, vPointC)))
		Safe_Release(pInstance);

	return pInstance;
}

void CCell::Free()
{
	for (_uint i = 0; i < LINE_END; ++i)
		Safe_Release(m_pLine[i]);

	for (auto& pCollider : m_pColliderCom)
		Safe_Release(pCollider);

	Safe_Release(m_pShaderCom);

	CVIBuffer::Free();
}
