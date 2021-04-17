#pragma once
#include "Mesh.h"
#include "Cell.h"

BEGIN(Engine)

class ENGINE_DLL CNaviMesh final : public CComponent
{
private:
	explicit CNaviMesh(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList);
	explicit CNaviMesh(const CNaviMesh& rhs);
	virtual ~CNaviMesh() = default;

public:
	_float	Get_NaviMeshHeight(const _vec3* pTargetPos);
	_vec3	Get_CellCenterPos(const _ulong & dwIndex);
	 _int	Get_CurrentPositionCellIndex(const _vec3& vPos);
	void	Set_CurrentCellIndex(const _ulong& dwIdx) { m_dwCurrentIdx = dwIdx; }

	virtual void	Update_Component(const _float& fTimeDelta);
	_vec3			Move_OnNaviMesh(const _vec3* pTargetPos,
									const _vec3* pTargetDir,
									const _float& fSpeed = 0.0f,
									const _bool& bIsJump = false);
private:
	HRESULT	Ready_NaviMesh(wstring wstrFilePath, const _vec3& vPosOffset);
	void	Link_Cell();

private:
	vector<CCell*>	m_vecCell;
	_ulong			m_dwCurrentIdx;

public:
	virtual CComponent* Clone();
	static CNaviMesh*	Create(ID3D12Device* pGraphicDevice, 
							   ID3D12GraphicsCommandList* pCommandList,
							   wstring& wstrFilePath, 
							   const _vec3& vPosOffset = _vec3(0.0f));
private:
	virtual void		Free();
};

END