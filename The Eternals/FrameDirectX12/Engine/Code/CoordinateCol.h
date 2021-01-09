#pragma once
#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CCoordinateCol : public CVIBuffer
{
private:
	explicit CCoordinateCol(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList);
	explicit CCoordinateCol(const CCoordinateCol& rhs);
	virtual ~CCoordinateCol() = default;

public:
	HRESULT	Ready_Buffer();
	void	Begin_Buffer();
	void	Render_Buffer();

public:
	virtual CComponent*		Clone();
	static	CCoordinateCol* Create(ID3D12Device * pGraphicDevice, ID3D12GraphicsCommandList * pCommandList);
private:
	virtual void Free();
};

END