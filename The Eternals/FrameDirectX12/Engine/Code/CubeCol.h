#pragma once
#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CCubeCol final : public CVIBuffer
{
private:
	explicit CCubeCol(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList);
	explicit CCubeCol(const CCubeCol& rhs);
	virtual ~CCubeCol() = default;

public:
	HRESULT	Ready_Buffer();
	void	Begin_Buffer();
	void	Render_Buffer();

public:
	virtual CComponent*	Clone();
	static	CCubeCol*	Create(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList);
private:
	virtual void		Free();
};

END