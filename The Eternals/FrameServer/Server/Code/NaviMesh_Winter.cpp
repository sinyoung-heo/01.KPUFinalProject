#include "stdafx.h"
#include "NaviMesh_Winter.h"

IMPLEMENT_SINGLETON(CNaviMesh_Winter)

CNaviMesh_Winter::CNaviMesh_Winter()
	: m_dwCurrentIdx(0)
{

}

_vec3 CNaviMesh_Winter::Get_CellCenterPos(const _ulong& dwIndex)
{
	_vec3 vResult = (*m_vecCell[dwIndex]->Get_Point(CCell::POINT_A) +
					 *m_vecCell[dwIndex]->Get_Point(CCell::POINT_B) +
					 *m_vecCell[dwIndex]->Get_Point(CCell::POINT_C)) / 3.0f;

	return vResult;
}

int CNaviMesh_Winter::Get_CurrentPositionCellIndex(const _vec3& vPos)
{
	for (auto& pCell : m_vecCell)
	{
		_vec3 vPosA = *pCell->Get_Point(CCell::POINT_A);
		_vec3 vPosB = *pCell->Get_Point(CCell::POINT_B);
		_vec3 vPosC = *pCell->Get_Point(CCell::POINT_C);

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

_vec3 CNaviMesh_Winter::Move_OnNaviMesh(const _vec3* pTargetPos, const _vec3* pTargetDir, const float& fSpeed, const bool& bIsJump)
{
	_vec3 vEndPos = *pTargetPos + *pTargetDir * fSpeed;
	_vec3 vSliding = _vec3(0.f);
	_vec3 vTargetDir = *pTargetDir;
	_vec3 vTargetPos = *pTargetPos;

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
		return vEndPos;
	}

	/*____________________________________________________________________
	Sliding Vector
	______________________________________________________________________*/
	else if (CCell::COMPARE_SLIDING == eResult)
	{
		vEndPos = *pTargetPos + vSliding * fSpeed;
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
			return vEndPos;
		}

		_vec3 vA = *m_vecCell[m_dwCurrentIdx]->Get_Point(CCell::POINT_A);
		_vec3 vB = *m_vecCell[m_dwCurrentIdx]->Get_Point(CCell::POINT_B);
		_vec3 vC = *m_vecCell[m_dwCurrentIdx]->Get_Point(CCell::POINT_C);

		_vec3 src = (vA + vB + vC) / 3.f;
		_vec3 dir = src - *pTargetPos;
		dir.Normalize();

		src = *pTargetPos + dir * fSpeed;

		return src;
	}

	return vEndPos;
}

HRESULT CNaviMesh_Winter::Ready_NaviMesh(wstring wstrFilePath)
{
	wifstream fin{ wstrFilePath };
	if (fin.fail())
		return E_FAIL;

	_vec3 vOffSet = _vec3(STAGE_BEACH_OFFSET_X, 0.0f, STAGE_BEACH_OFFSET_Z);

	while (true)
	{
		CCell*	pCell = nullptr;
		_vec3	vPointA = _vec3(0.0f);
		_vec3	vPointB = _vec3(0.0f);
		_vec3	vPointC = _vec3(0.0f);
		int		iOption = 0;
		_ulong	iIdx = 0;

		// Cell Data 불러오기.
		fin >> vPointA.x		// PointA
			>> vPointA.y
			>> vPointA.z
			>> vPointB.x		// vPointB
			>> vPointB.y
			>> vPointB.z
			>> vPointC.x		// vPointC
			>> vPointC.y
			>> vPointC.z
			>> iOption			// iOption
			>> iIdx;			// Index

		if (fin.eof())
			break;

		pCell = CCell::Create(m_vecCell.size(),
							  vPointA + vOffSet,
							  vPointB + vOffSet,
							  vPointC + vOffSet,
							  iOption);

		if (!pCell) E_FAIL;

		m_vecCell.emplace_back(pCell);
	}

	Link_Cell();

	return S_OK;
}

void CNaviMesh_Winter::Link_Cell()
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

void CNaviMesh_Winter::Release()
{
	for (auto& v : m_vecCell)
	{
		delete v;
		v = nullptr;
	}
	m_vecCell.clear();
}
