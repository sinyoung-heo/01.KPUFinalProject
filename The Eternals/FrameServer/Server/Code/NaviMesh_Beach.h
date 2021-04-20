#pragma once
#include "Cell.h"

class CNaviMesh_Beach
{
	DECLARE_SINGLETON(CNaviMesh_Beach)

private:
	explicit CNaviMesh_Beach();
	virtual ~CNaviMesh_Beach() = default;

public:
	const vector<CCell*>& Get_CellList() { return m_vecCell; }
	_vec3	Get_CellCenterPos(const _ulong& dwIndex);
	int		Get_CurrentPositionCellIndex(const _vec3& vPos);
	void	Set_CurrentCellIndex(const _ulong& dwIdx) { m_dwCurrentIdx = dwIdx; }

public:
	HRESULT	Ready_NaviMesh(wstring wstrFilePath);

	_vec3	Move_OnNaviMesh(const _vec3* pTargetPos,
							const _vec3* pTargetDir,
							const float& fSpeed = 0.0f,
							const bool& bIsJump = false);

private:
	void	Link_Cell();

public:
	void	Release();

private:
	vector<CCell*>	m_vecCell;
	unsigned long	m_dwCurrentIdx;
};

