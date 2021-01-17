#pragma once
#include "VIBuffer.h"
#include "Line.h"

BEGIN(Engine)

class CShaderColor;
class CColliderSphere;

class ENGINE_DLL CCell final : public CVIBuffer
{
public:
	enum POINT		{ POINT_A, POINT_B, POINT_C, POINT_END };
	enum NEIGHBOR	{ NEIGHBOR_AB, NEIGHBOR_BC, NEIGHBOR_CA, NEIGHBOR_END };
	enum LINE		{ LINE_AB, LINE_BC, LINE_CA, LINE_END };
	enum COMPARE	{ COMPARE_MOVE, COMPARE_SLIDING, COMPARE_STOP };

private:
	explicit CCell(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList);
	virtual ~CCell() = default;

public:
	// Get
	CLine&			Get_Line(const int& iIndex)					{ return *m_pLine[iIndex]; }
	const _vec3*	Get_Point(const POINT& eType)		const	{ return &m_vPoint[eType]; }
	CCell*			Get_Neighbor(const NEIGHBOR& eType) const	{ return m_pNeighbor[eType]; }
	const _ulong*	Get_Index()									{ return &m_dwCurrentIdx; }
	
	// Set
	void			Set_Neighbor(const NEIGHBOR& eType, CCell* pNeighbor) { m_pNeighbor[eType] = pNeighbor; }

	// Method
	HRESULT			Ready_Cell(const _ulong& dwIndex,
							   const _vec3& vPointA,
							   const _vec3& vPointB,
							   const _vec3& vPointC);
	HRESULT			Ready_Component();
	virtual void	Update_Component(const _float& fTimeDelta);
	virtual void	Render_Component(const _float& fTimeDelta);
	void			Set_ConstantTable();

	_bool			Compare_Point(const _vec3* pPointF, const _vec3* pPointS, CCell* pCell);
	COMPARE			Compare(const _vec3* pEndPos,
							_ulong* pIndex,
							const _float& fTargetSpeed,
							_vec3* pTargetPos = nullptr,
							_vec3* pTargetDir = nullptr,
							_vec3* pSlidingDir = nullptr);

	COMPARE			Compare(_vec3* pEndPos, _ulong* pIndex, _int* iLineIndex);
	float			Position_On_Height(const _vec3* vPos);

public:
	/*__________________________________________________________________________________________________________
	[ Component ]
	____________________________________________________________________________________________________________*/
	CShaderColor*		m_pShaderCom = nullptr;
	CColliderSphere*	m_pColliderCom[POINT_END];

	/*__________________________________________________________________________________________________________
	[ Value ]
	____________________________________________________________________________________________________________*/
	_matrix m_matWorld[POINT_END];

	_vec3	m_vPoint[POINT_END];
	CCell*	m_pNeighbor[NEIGHBOR_END];
	CLine*	m_pLine[LINE_END];
	_ulong	m_dwCurrentIdx = 0;
	
	_plane	m_tPlane;

public:
	static CCell* Create(ID3D12Device* pGraphicDevice, 
						 ID3D12GraphicsCommandList* pCommandList,
						 const _ulong& dwIndex,
						 const _vec3& vPointA,
						 const _vec3& vPointB,
						 const _vec3& vPointC);
private:
	virtual void Free();
};

END