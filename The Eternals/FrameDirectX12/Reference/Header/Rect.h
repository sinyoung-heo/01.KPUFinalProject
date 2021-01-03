#pragma once
#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CRect : public CVIBuffer
{
private:
	explicit CRect(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList);
	explicit CRect(const CRect& rhs);
	virtual ~CRect() = default;

public:
	HRESULT	Ready_Buffer();
	void	Begin_Buffer();
	void	End_Buffer();
	void	Render_Buffer();

public:
	virtual CComponent* Clone();
	static	CRect*		Create(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList);
private:
	virtual void		Free();
};

END