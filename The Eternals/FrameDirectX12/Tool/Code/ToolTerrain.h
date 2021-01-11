#pragma once
#include "GameObject.h"

namespace Engine
{
	class CTerrainTex;
	class CTexture;
	class CShaderTexture;
}

class CToolTerrain : public Engine::CGameObject
{
private:
	explicit CToolTerrain(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList);
	explicit CToolTerrain(const CToolTerrain& rhs);
	virtual ~CToolTerrain() = default;

public:
	// CGameObject을(를) 통해 상속됨
	virtual HRESULT	Ready_GameObject(wstring wstrTerrainTag);
	virtual HRESULT	LateInit_GameObject();
	virtual _int	Update_GameObject(const _float& fTimeDelta);
	virtual _int	LateUpdate_GameObject(const _float& fTimeDelta);
	virtual void	Render_GameObject(const _float& fTimeDelta);
private:
	virtual HRESULT Add_Component(wstring wstrTerrainTag);
	void			Set_ConstantTable();

public:
	/*__________________________________________________________________________________________________________
	[ Component ]
	____________________________________________________________________________________________________________*/
	Engine::CTerrainTex*	m_pBufferCom	= nullptr;
	Engine::CTexture*		m_pTextureCom	= nullptr;
	Engine::CShaderTexture*	m_pShaderCom	= nullptr;

	/*__________________________________________________________________________________________________________
	[ Value ]
	____________________________________________________________________________________________________________*/
	_uint m_uiTexIdx		= 7;
	_uint m_uiShaderPass	= 1;
	_bool m_bIsUpdate		= false;


public:
	static CToolTerrain* Create(ID3D12Device* pGraphicDevice, 
								ID3D12GraphicsCommandList* pCommandList,
								wstring wstrTerrainTag);
private:
	virtual void Free();
};

