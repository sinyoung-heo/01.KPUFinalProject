#pragma once
#include "Include.h"
#include "GameObject.h"

class CMiniMapFrame
{
private:
	explicit CMiniMapFrame(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList);
	virtual ~CMiniMapFrame() = default;
public:
	// CGameObject을(를) 통해 상속됨
	virtual HRESULT	Ready_GameObject(const _vec3& vPos, const _uint& uiMiniMapTexIdx);
	virtual HRESULT	LateInit_GameObject();
	virtual _int	Update_GameObject(const _float& fTimeDelta);
	virtual _int	LateUpdate_GameObject(const _float& fTimeDelta);
	virtual void	Send_PacketToServer();
	// SingleThread Rendering.
	virtual void	Render_MiniMap(const _float& fTimeDelta);
private:
	void			Set_ConstantTable();
private:
	_uint m_uiTexidx = 0;
public:
	static Engine::CGameObject* Create(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList,
									   const _vec3& vPos,
									   const _uint& uiMiniMapTexIdx);
private:
	virtual void Free();
};

