#pragma once
#include "Include.h"
#include "GameObject.h"

namespace Engine
{
	class CTerrainTex;
	class CTexture;
	class CShaderTexture;
}

class CTerrainObject : public Engine::CGameObject
{
private:
	explicit CTerrainObject(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList);
	virtual ~CTerrainObject() = default;

public:
	// CGameObject을(를) 통해 상속됨
	virtual HRESULT	Ready_GameObject();
	virtual HRESULT	LateInit_GameObject();
	virtual _int	Update_GameObject(const _float& fTimeDelta);
	virtual _int	LateUpdate_GameObject(const _float& fTimeDelta);
	virtual void	Render_GameObject(const _float& fTimeDelta);
private:
	virtual HRESULT Add_Component();
	void			Set_ConstantTable();

private:
	/*__________________________________________________________________________________________________________
	[ Component ]
	____________________________________________________________________________________________________________*/
	Engine::CTerrainTex*	m_pBufferCom	= nullptr;
	Engine::CTexture*		m_pTextureCom	= nullptr;
	Engine::CShaderTexture*	m_pShaderCom	= nullptr;

	/*__________________________________________________________________________________________________________
	[ Value ]
	____________________________________________________________________________________________________________*/

public:
	static Engine::CGameObject* Create(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList);
private:
	virtual void Free();
};

