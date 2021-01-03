#pragma once
#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CCube : public CVIBuffer
{
private:
	explicit CCube(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList);
	explicit CCube(const CCube& rhs);
	virtual ~CCube() = default;

public:
	HRESULT	Ready_Buffer();
	void	Begin_Buffer();
	void	End_Buffer();
	void	Render_Buffer();

public:
	virtual CComponent* Clone();
	static	CCube*	Create(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList);
private:
	virtual void		Free();
};

END