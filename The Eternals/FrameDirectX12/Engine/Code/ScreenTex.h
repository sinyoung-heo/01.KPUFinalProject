#pragma once
#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CScreenTex final : public CVIBuffer
{
private:
	explicit CScreenTex(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList);
	explicit CScreenTex(const CScreenTex& rhs);
	virtual ~CScreenTex() = default;

public:
	HRESULT	Ready_Buffer();
	void	Begin_Buffer();
	void	Render_Buffer();

public:
	virtual CComponent*	Clone();
	static	CScreenTex*	Create(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList);
private:
	virtual void		Free();
};

END