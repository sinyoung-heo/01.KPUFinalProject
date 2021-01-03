#pragma once
#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CRcCol final : public CVIBuffer
{
private:
	explicit CRcCol(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList);
	explicit CRcCol(const CRcCol& rhs);
	virtual ~CRcCol() = default;

public:
	HRESULT	Ready_Buffer();
	void	Begin_Buffer();
	void	Render_Buffer();

public:
	virtual CComponent*	Clone();
	static	CRcCol*		Create(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList);
private:
	virtual void		Free();
};

END