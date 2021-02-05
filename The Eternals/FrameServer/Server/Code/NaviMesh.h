#pragma once
#include "Cell.h"

class CNaviMesh
{
	DECLARE_SINGLETON(CNaviMesh)

private:
	explicit CNaviMesh();
	virtual ~CNaviMesh() = default;

public:
	_vec3*	Get_CellCenterPos(const unsigned long& dwIndex);
	int		Get_CurrentPositionCellIndex(const _vec3& vPos);
	void	Set_CurrentCellIndex(const unsigned long& dwIdx) { m_dwCurrentIdx = dwIdx; }

	_vec3*	Move_OnNaviMesh(const _vec3* pTargetPos,
							const _vec3* pTargetDir,
							const float& fSpeed = 0.0f,
							const bool& bIsJump = false);

public:
	HRESULT	Ready_NaviMesh(wstring wstrFilePath);

private:
	void	Link_Cell();

public:
	void	Release();

private:
	vector<CCell*>	m_vecCell;
	unsigned long	m_dwCurrentIdx;
};

