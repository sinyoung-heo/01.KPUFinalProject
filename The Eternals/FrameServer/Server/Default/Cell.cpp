#include "stdafx.h"
#include "Cell.h"

CCell::CCell()
	: m_dwCurrentIdx(0)
{
	ZeroMemory(m_pNeighbor, sizeof(CCell*) * NEIGHBOR_END);
}

HRESULT CCell::Ready_Cell(const unsigned long& dwIndex, const _vec3& vPointA, const _vec3& vPointB, const _vec3& vPointC, const int& iOption)
{
	m_dwCurrentIdx = dwIndex;
	m_iOption = iOption;

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
	//m_tPlane.Plane_FromPoints(m_vPoint[POINT_A], m_vPoint[POINT_B], m_vPoint[POINT_C]);

	return S_OK;
}

bool CCell::Compare_Point(const _vec3* pPointF, const _vec3* pPointS, CCell* pCell)
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

CCell::COMPARE CCell::Compare(const _vec3* pEndPos, unsigned long* pIndex, const float& fTargetSpeed, _vec3* pTargetPos, _vec3* pTargetDir, _vec3* pSlidingDir)
{
	for (unsigned long i = 0; i < LINE_END; ++i)
	{
		if (CLine::COMPARE_LEFT == m_pLine[i]->Compare(&_vec2(pEndPos->x, pEndPos->z)))
		{
			if (nullptr == m_pNeighbor[i])
			{
				/*____________________________________________________________________
				선분의 법선벡터
				______________________________________________________________________*/
				_vec3 vNormal(_vec3(m_pLine[i]->Get_Normal().x, 0.f, m_pLine[i]->Get_Normal().y));

				/*____________________________________________________________________
				Slideing Vector 공식.
				S = P - n ( Dot(P,n) )
				______________________________________________________________________*/
				*pSlidingDir = *pTargetDir - vNormal * pTargetDir->Dot(vNormal);

				_vec3 doubleNormal(*pTargetDir - (vNormal * 1.4f) * pTargetDir->Dot(vNormal * 1.4f));
				_vec3 TripleNormal(*pTargetDir - (vNormal * 2.f) * pTargetDir->Dot(vNormal * 2.f));

				_vec3 vEndPos(*pTargetPos + *pSlidingDir * fTargetSpeed * 5.f);
				_vec3 vDoublePos(*pTargetPos + doubleNormal * fTargetSpeed * 5.f);
				_vec3 vTriplePos(*pTargetPos + TripleNormal * fTargetSpeed * 5.f);

				/*____________________________________________________________________
				Start & Finish점 비교.
				꼭지점을 뚫고가지 않도록 예외처리.
				______________________________________________________________________*/
				float fMaxX = max(m_pLine[i]->Get_Point(CLine::POINT_START).x, m_pLine[i]->Get_Point(CLine::POINT_FINISH).x);
				float fMinX = min(m_pLine[i]->Get_Point(CLine::POINT_START).x, m_pLine[i]->Get_Point(CLine::POINT_FINISH).x);

				float fMaxZ = max(m_pLine[i]->Get_Point(CLine::POINT_START).y, m_pLine[i]->Get_Point(CLine::POINT_FINISH).y);
				float fMinZ = min(m_pLine[i]->Get_Point(CLine::POINT_START).y, m_pLine[i]->Get_Point(CLine::POINT_FINISH).y);

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

CCell::COMPARE CCell::Compare(_vec3* pEndPos, unsigned long* pIndex, int* iLineIndex)
{
	for (unsigned long i = 0; i < LINE_END; ++i)
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

void CCell::Release()
{
}

CCell* CCell::Create(const unsigned long& dwIndex, const _vec3& vPointA, const _vec3& vPointB, const _vec3& vPointC, const int& iOption)
{
	CCell* pInstance = new CCell();

	if (FAILED(pInstance->Ready_Cell(dwIndex, vPointA, vPointB, vPointC, iOption)))
	{
		delete pInstance;
		pInstance = nullptr;
	}

	return pInstance;
}
