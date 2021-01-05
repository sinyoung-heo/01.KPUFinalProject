#pragma once
#include "Engine_Include.h"
#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CComponent : public CBase
{
protected:
	explicit CComponent();
	explicit CComponent(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList = nullptr);
	explicit CComponent(const CComponent& rhs);
	virtual ~CComponent() = default;

public:
	virtual void	Update_Component(const _float& fTimeDelta);
	virtual void	Render_Component(const _float& fTimeDelta);
	virtual void	Release_UploadBuffer();

protected:
	ID3D12Device*				m_pGraphicDevice	= nullptr;
	ID3D12GraphicsCommandList*	m_pCommandList		= nullptr;

	_bool						m_bIsClone			= false;

public:
	virtual CComponent* Clone() { return nullptr; }
protected:
	virtual void		Free();
};

END