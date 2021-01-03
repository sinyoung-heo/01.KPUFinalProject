#pragma once
#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CTerrainTex final : public CVIBuffer
{
private:
	explicit CTerrainTex(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList);
	explicit CTerrainTex(const CTerrainTex& rhs);
	virtual ~CTerrainTex() = default;

public:
	HRESULT	Ready_Buffer(const _uint& iNumVerticesX,
						 const _uint& iNumVerticesZ,
						 const _float& fInterval = 1.f);
	void	Begin_Buffer();
	void	Render_Buffer();

private:
	_uint   m_iNumVerticesX = 0;
	_uint   m_iNumVerticesZ = 0;
	_float  m_fInterval		= 0.0f;

public:
	virtual CComponent*		Clone();
	static	CTerrainTex*	Create(ID3D12Device* pGraphicDevice, 
								   ID3D12GraphicsCommandList* pCommandList, 
								   const _uint& iNumVerticesX, 
								   const _uint& iNumVerticesZ, 
								   const _float& fInterval = 1.f);
private:
	virtual void			Free();
};

END