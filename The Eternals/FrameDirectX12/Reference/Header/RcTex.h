#pragma once
#include "VIBuffer.h"


BEGIN(Engine)

class ENGINE_DLL CRcTex final : public CVIBuffer
{
private:
	explicit CRcTex(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList);
	explicit CRcTex(const CRcTex& rhs);
	virtual ~CRcTex() = default;

public:
	HRESULT	Ready_Buffer();
	void	Begin_Buffer();
	void	Render_Buffer();

public:
	virtual CComponent*	Clone();
	static	CRcTex*		Create(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList);
private:
	virtual void		Free();
};

END