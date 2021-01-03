#pragma once
#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CRectBuffer : public CVIBuffer
{
private:
	explicit CRectBuffer(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList);
	explicit CRectBuffer(const CRectBuffer& rhs);
	virtual ~CRectBuffer() = default;

public:
	HRESULT	Ready_Buffer();
	void	Begin_Buffer();
	void	End_Buffer();
	void	Render_Buffer();

public:
	virtual CComponent* Clone();
	static	CRectBuffer*		Create(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList);
private:
	virtual void		Free();
};

END