#pragma once
#include "VIBuffer.h"

BEGIN(Engine)

class CShader;

class ENGINE_DLL CTerrainTex final : public CVIBuffer
{
private:
	explicit CTerrainTex(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList);
	explicit CTerrainTex(const CTerrainTex& rhs);
	virtual ~CTerrainTex() = default;

public:
	const _uint&	Get_VtxCntX()		{ return m_iNumVerticesX; }
	const _uint&	Get_VtxCntZ()		{ return m_iNumVerticesZ; }
	vector<VTXTEX>& Get_VertexBuffer()	{ return vecVertices; }
	vector<_uint>&	Get_IndexBuffer()	{ return vecIndices; }

public:
	HRESULT	Ready_Buffer(const _uint& iNumVerticesX,
						 const _uint& iNumVerticesZ,
						 const _float& fInterval = 1.f,
						 const _float& fDetail = 5.0f);
	void	Begin_Buffer();
	void	Render_Buffer();

	// BumpTerrain
	void	Render_BumpTerrain(ID3D12GraphicsCommandList* pCommandList, 
							  const _int& iContextIdx,
							  CShader* pShader);
private:
	void	Begin_Buffer(ID3D12GraphicsCommandList* pCommandList);
	void	Render_Buffer(ID3D12GraphicsCommandList* pCommandList);

private:
	_uint   m_iNumVerticesX = 0;
	_uint   m_iNumVerticesZ = 0;
	_float  m_fInterval		= 0.0f;

	vector<VTXTEX>	vecVertices;
	vector<_uint>	vecIndices;

public:
	virtual CComponent*		Clone();
	static	CTerrainTex*	Create(ID3D12Device* pGraphicDevice, 
								   ID3D12GraphicsCommandList* pCommandList, 
								   const _uint& iNumVerticesX, 
								   const _uint& iNumVerticesZ, 
								   const _float& fInterval = 1.f,
								   const _float& fDetail = 5.0f);
private:
	virtual void			Free();
};

END