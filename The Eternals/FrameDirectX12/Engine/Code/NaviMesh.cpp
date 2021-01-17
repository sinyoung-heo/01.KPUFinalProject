#include "NaviMesh.h"
#include "Renderer.h"

USING(Engine)

CNaviMesh::CNaviMesh(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList)
	: CComponent(pGraphicDevice, pCommandList)
	, m_dwCurrentIdx(0)
{

}

CNaviMesh::CNaviMesh(const CNaviMesh& rhs)
	: CComponent(rhs)
	, m_vecCell(rhs.m_vecCell)
	, m_dwCurrentIdx(rhs.m_dwCurrentIdx)
{

}

_float CNaviMesh::Get_NaviMeshHeight(const _vec3* pTargetPos)
{
	return m_vecCell[m_dwCurrentIdx]->Position_On_Height(pTargetPos);
}

_vec3 CNaviMesh::Get_CellCenterPos(const _ulong& dwIndex)
{
	_vec3 vResult = (*m_vecCell[dwIndex]->Get_Point(CCell::POINT_A) + 
					 *m_vecCell[dwIndex]->Get_Point(CCell::POINT_B) +
					 *m_vecCell[dwIndex]->Get_Point(CCell::POINT_C)) / 3.0f;

	return vResult;
}

_int CNaviMesh::Get_CurrentPositionCellIndex(const _vec3& vPos)
{
	for (auto& pCell : m_vecCell)
	{
		_vec3 vPosA = *pCell->Get_Point(CCell::POINT_A);
		_vec3 vPosB = *pCell->Get_Point(CCell::POINT_A);
		_vec3 vPosC = *pCell->Get_Point(CCell::POINT_A);

		_vec3 vDirA = vPosB - vPosA;
		_vec3 vDirB = vPosC - vPosB;
		_vec3 vDirC = vPosA - vPosC;

		vDirA = _vec3{ -vDirA.z, vDirA.y, vDirA.x };
		vDirB = _vec3{ -vDirB.z, vDirB.y, vDirB.x };
		vDirC = _vec3{ -vDirC.z, vDirC.y, vDirC.x };

		vDirA.Normalize();
		vDirB.Normalize();
		vDirC.Normalize();

		_vec3 vCellDir[3] =
		{
			vPos - vPosA,
			vPos - vPosB,
			vPos - vPosC
		};

		for (auto& cell_dir : vCellDir)
			cell_dir.Normalize();

		float A = vCellDir[0].Dot(vDirA);
		float B = vCellDir[1].Dot(vDirB);
		float C = vCellDir[2].Dot(vDirC);

		if (A < 0 && B < 0 && C < 0)
			return *pCell->Get_Index();
	}

	return -1;
}

void CNaviMesh::Update_Component(const _float& fTimeDelta)
{
	/*__________________________________________________________________________________________________________
	[ Renderer - Add RenderGroup ]
	____________________________________________________________________________________________________________*/
	if (CRenderer::Get_Instance()->Get_RenderOnOff(L"Collider"))
	{
		for (auto& pCell : m_vecCell)
			pCell->Update_Component(fTimeDelta);

		if (E_FAIL == CRenderer::Get_Instance()->Add_Renderer(this))
			return;
	}
}

void CNaviMesh::Render_Component(const _float& fTimeDelta)
{
	/*__________________________________________________________________________________________________________
	[ Renderer - Add RenderGroup ]
	____________________________________________________________________________________________________________*/
	if (CRenderer::Get_Instance()->Get_RenderOnOff(L"Collider"))
	{
		for (auto& pCell : m_vecCell)
			pCell->Render_Component(fTimeDelta);
	}
}

_vec3 CNaviMesh::Move_OnNaviMesh(const _vec3* pTargetPos,
								 const _vec3* pTargetDir, 
								 const _float& fSpeed, 
								 const _bool& bIsJump)
{
	_vec3 vEndPos		= *pTargetPos + *pTargetDir * fSpeed;
	_vec3 vSliding		= _vec3(0.f);
	_vec3 vTargetDir	= *pTargetDir;
	_vec3 vTargetPos	= *pTargetPos;

	CCell::COMPARE eResult = m_vecCell[m_dwCurrentIdx]->Compare(&vEndPos,
																&m_dwCurrentIdx,
																fSpeed,
																&vTargetPos,
																&vTargetDir,
																&vSliding);

	/*____________________________________________________________________
	MOVE
	______________________________________________________________________*/
	if (CCell::COMPARE_MOVE == eResult)
	{
		//if(!bIsJump)
		//vEndPos.y = GetNaviHeight(&vTargetPos); // test
		return vEndPos;
	}
	
	/*____________________________________________________________________
	Sliding Vector
	______________________________________________________________________*/
	else if (CCell::COMPARE_SLIDING == eResult)
	{
		vEndPos = *pTargetPos + vSliding * fSpeed;
		//if (!bIsJump)
		//vEndPos.y = GetNaviHeight(&vTargetPos); // test

		return vEndPos;
	}

	/*____________________________________________________________________
	STOP
	______________________________________________________________________*/
	else if (CCell::COMPARE_STOP == eResult)
	{
		vEndPos = *pTargetPos + vTargetDir * fSpeed;
		eResult = m_vecCell[m_dwCurrentIdx]->Compare(&vEndPos,
													 &m_dwCurrentIdx,
													 fSpeed,
													 &vTargetPos,
													 &vTargetDir,
													 &vSliding);

		if (CCell::COMPARE_MOVE == eResult)
		{
			//if (!bIsJump)
			//vEndPos.y = GetNaviHeight(&vTargetPos); // test

			return vEndPos;
		}

		vEndPos = *pTargetPos + vSliding * fSpeed;
		eResult = m_vecCell[m_dwCurrentIdx]->Compare(&vEndPos,
													 &m_dwCurrentIdx,
													 fSpeed,
													 &vTargetPos,
													 &vTargetDir,
													 &vSliding);

		if (CCell::COMPARE_MOVE == eResult)
		{
			//if (!bIsJump)
			//vEndPos.y = GetNaviHeight(&vTargetPos); // test

			return vEndPos;
		}

		_vec3 vA = *m_vecCell[m_dwCurrentIdx]->Get_Point(CCell::POINT_A);
		_vec3 vB = *m_vecCell[m_dwCurrentIdx]->Get_Point(CCell::POINT_B);
		_vec3 vC = *m_vecCell[m_dwCurrentIdx]->Get_Point(CCell::POINT_C);

		_vec3 src = (vA + vB + vC) / 3.f;
		_vec3 dir = src - *pTargetPos;
		dir.Normalize();

		src = *pTargetPos + dir * fSpeed;
		//m_dwCurrentIdx = m_dstIdx;

		return src;
	}

	return vEndPos;
}

HRESULT CNaviMesh::Ready_NaviMesh(wstring wstrFilePath)
{
	HANDLE hFile = CreateFile(wstrFilePath.c_str(),
							  GENERIC_READ,
							  0,
							  0,
							  OPEN_EXISTING,
							  FILE_ATTRIBUTE_NORMAL,
							  nullptr);

	if (INVALID_HANDLE_VALUE == hFile)
		return E_FAIL;

	_vec3	vPointA;
	_vec3	vPointB;
	_vec3	vPointC;
	CCell*	pCell = nullptr;

	_ulong dwByte = 0;
	while (true)
	{
		ReadFile(hFile, &vPointA, sizeof(_vec3), &dwByte, nullptr);
		ReadFile(hFile, &vPointB, sizeof(_vec3), &dwByte, nullptr);
		ReadFile(hFile, &vPointC, sizeof(_vec3), &dwByte, nullptr);

		if (dwByte == 0)
			break;

		pCell = CCell::Create(m_pGraphicDevice,
							  m_pCommandList,
							  m_vecCell.size(),
							  vPointA,
							  vPointB,
							  vPointC);
		NULL_CHECK_RETURN(pCell, E_FAIL);
		m_vecCell.push_back(pCell);
	}

	CloseHandle(hFile);

	Link_Cell();

	return S_OK;
}

void CNaviMesh::Link_Cell()
{
	for (_ulong i = 0; i < m_vecCell.size(); ++i)
	{
		for (_ulong j = 0; j < m_vecCell.size(); ++j)
		{
			if (i == j)
				continue;

			if (nullptr == m_vecCell[i]->Get_Neighbor(CCell::NEIGHBOR_AB) &&
				m_vecCell[j]->Compare_Point(m_vecCell[i]->Get_Point(CCell::POINT_A),
											m_vecCell[i]->Get_Point(CCell::POINT_B),
											m_vecCell[i]))
			{
				m_vecCell[i]->Set_Neighbor(CCell::NEIGHBOR_AB, m_vecCell[j]);
				continue;
			}

			if (nullptr == m_vecCell[i]->Get_Neighbor(CCell::NEIGHBOR_BC) &&
				m_vecCell[j]->Compare_Point(m_vecCell[i]->Get_Point(CCell::POINT_B),
											m_vecCell[i]->Get_Point(CCell::POINT_C),
											m_vecCell[i]))
			{
				m_vecCell[i]->Set_Neighbor(CCell::NEIGHBOR_BC, m_vecCell[j]);
				continue;
			}

			if (nullptr == m_vecCell[i]->Get_Neighbor(CCell::NEIGHBOR_CA) &&
				m_vecCell[j]->Compare_Point(m_vecCell[i]->Get_Point(CCell::POINT_C),
											m_vecCell[i]->Get_Point(CCell::POINT_A),
											m_vecCell[i]))
			{
				m_vecCell[i]->Set_Neighbor(CCell::NEIGHBOR_CA, m_vecCell[j]);
				continue;
			}
		}
	}

}

CComponent* CNaviMesh::Clone()
{
	return new CNaviMesh(*this);
}

CNaviMesh* CNaviMesh::Create(ID3D12Device* pGraphicDevice,
							 ID3D12GraphicsCommandList* pCommandList,
							 wstring& wstrFilePath)
{
	CNaviMesh* pInstance = new CNaviMesh(pGraphicDevice, pCommandList);

	if (FAILED(pInstance->Ready_NaviMesh(wstrFilePath)))
		Safe_Release(pInstance);

	return pInstance;
}

void CNaviMesh::Free()
{
	for_each(m_vecCell.begin(), m_vecCell.end(), CDeleteObj());
	m_vecCell.clear();
	
	CComponent::Free();
}
