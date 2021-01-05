#include "Line.h"

USING(Engine)


HRESULT CLine::Ready_Line(const _vec2* pPointA, const _vec2* pPointB)
{
	m_vPoint[POINT_START]	= *pPointA;
	m_vPoint[POINT_FINISH]	= *pPointB;

	m_vDirection	= m_vPoint[POINT_FINISH] - m_vPoint[POINT_START];
	m_vNormal		= _vec2(m_vDirection.y * -1.0f, m_vDirection.x);
	m_vNormal.Normalize();

	return S_OK;
}

CLine::COMPARE CLine::Compare(const _vec2* pEndPos)
{
	_vec2 vDest = *pEndPos - m_vPoint[POINT_START];
	vDest.Normalize();

	_float fResult = vDest.Dot(m_vNormal);

	if (0 <= fResult)
		return COMPARE_LEFT;
	else
		return COMPARE_RIGHT;
}

CLine* CLine::Create(const _vec2* pPointA, const _vec2* pPointB)
{
	CLine* pInstance = new CLine;

	if (FAILED(pInstance->Ready_Line(pPointA, pPointB)))
		Safe_Release(pInstance);

	return pInstance;
}

void CLine::Free()
{

}
