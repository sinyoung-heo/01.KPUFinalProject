#pragma once
#include "Line.h"

class CCell
{
public:
	enum POINT { POINT_A, POINT_B, POINT_C, POINT_END };
	enum NEIGHBOR { NEIGHBOR_AB, NEIGHBOR_BC, NEIGHBOR_CA, NEIGHBOR_END };
	enum LINE { LINE_AB, LINE_BC, LINE_CA, LINE_END };
	enum COMPARE { COMPARE_MOVE, COMPARE_SLIDING, COMPARE_STOP };

private:
	explicit CCell();
	virtual ~CCell() = default;

public:
	// Get
	CLine&			Get_Line(const int& iIndex) { return *m_pLine[iIndex]; }
	const _vec3*	Get_Point(const POINT& eType)		const { return &m_vPoint[eType]; }
	CCell*			Get_Neighbor(const NEIGHBOR& eType) const { return m_pNeighbor[eType]; }
	const unsigned long* Get_Index() { return &m_dwCurrentIdx; }

	// Set
	void			Set_Neighbor(const NEIGHBOR& eType, CCell* pNeighbor) { m_pNeighbor[eType] = pNeighbor; }

	// Method
	HRESULT			Ready_Cell(const unsigned long& dwIndex,
							   const _vec3& vPointA,
							   const _vec3& vPointB,
							   const _vec3& vPointC,
							   const int& iOption);

	bool			Compare_Point(const _vec3* pPointF, const _vec3* pPointS, CCell* pCell);
	COMPARE			Compare(const _vec3* pEndPos,
							unsigned long* pIndex,
							const float& fTargetSpeed,
							_vec3* pTargetPos = nullptr,
							_vec3* pTargetDir = nullptr,
							_vec3* pSlidingDir = nullptr);

	COMPARE			Compare(_vec3* pEndPos, unsigned long* pIndex, int* iLineIndex);
	float			Position_On_Height(const _vec3* vPos);

	void			Release();

public:
	static CCell* Create(
		const unsigned long& dwIndex,
		const _vec3& vPointA,
		const _vec3& vPointB,
		const _vec3& vPointC,
		const int& iOption);

public:
	/*__________________________________________________________________________________________________________
	[ Value ]
	____________________________________________________________________________________________________________*/
	_vec3			m_vPoint[POINT_END];
	CCell*			m_pNeighbor[NEIGHBOR_END];
	CLine*			m_pLine[LINE_END];
	unsigned long	m_dwCurrentIdx = 0;
	int				m_iOption = 0;

	_plane			m_tPlane;
};

