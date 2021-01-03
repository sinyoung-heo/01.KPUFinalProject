#pragma once
#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CCubeBuffer : public CVIBuffer
{
private:
	explicit CCubeBuffer(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList);
	explicit CCubeBuffer(const CCubeBuffer& rhs);
	virtual ~CCubeBuffer() = default;

public:
	HRESULT	Ready_Buffer();
	void	Begin_Buffer();
	void	End_Buffer();
	void	Render_Buffer();

public:
	virtual CComponent* Clone();
	static	CCubeBuffer*	Create(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList);
private:
	virtual void		Free();
};

END