#pragma once
#include "VIBuffer.h"

BEGIN(Engine)

const _uint TRAIL_SIZE = 25;

class ENGINE_DLL CTrailTex final : public CVIBuffer
{
private:
	explicit CTrailTex(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList);
	explicit CTrailTex(const CTrailTex& rhs);
	virtual ~CTrailTex() = default;

public:
	array<Engine::VTXTEX, TRAIL_SIZE>& Get_ArrayVerteices() { return m_arrVertices; }

	HRESULT	Ready_Buffer();
	void	Begin_Buffer();
	void	Render_Buffer();

private:
	array<Engine::VTXTEX, TRAIL_SIZE> m_arrVertices;

public:
	virtual CComponent* Clone();
	static	CTrailTex*	Create(ID3D12Device * pGraphicDevice, ID3D12GraphicsCommandList * pCommandList);
private:
	virtual void Free();
};

END