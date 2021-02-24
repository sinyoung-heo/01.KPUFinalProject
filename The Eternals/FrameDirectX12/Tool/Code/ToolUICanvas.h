#pragma once
#include "GameObject.h"

namespace Engine
{
	class CRcTex;
	class CTexture;
	class CShaderTexture;
}

class CToolUICanvas : public Engine::CGameObject
{
private:
	explicit CToolUICanvas(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList);
	virtual ~CToolUICanvas() = default;

public:
	// CGameObject을(를) 통해 상속됨
	virtual HRESULT	Ready_GameObject(_vec3 vPos, _vec3 vScale, _long iUIDepth);
	virtual HRESULT	LateInit_GameObject();
	virtual _int	Update_GameObject(const _float& fTimeDelta);
	virtual _int	LateUpdate_GameObject(const _float& fTimeDelta);
	virtual void	Render_GameObject(const _float& fTimeDelta);

private:
	virtual HRESULT Add_Component();
	void			Set_ConstantTable();

public:
	/*__________________________________________________________________________________________________________
	[ Component ]
	____________________________________________________________________________________________________________*/
	Engine::CRcTex*			m_pBufferCom	= nullptr;
	Engine::CShaderTexture*	m_pShaderCom	= nullptr;

	/*__________________________________________________________________________________________________________
	[ Value ]
	____________________________________________________________________________________________________________*/
	ID3D12DescriptorHeap*	m_pTexDescriptorHeap = nullptr;
	_uint					m_uiTexIdx	         = 0;
private:
	_vec3 m_vConvert	= _vec3(0.0f);

public:
	static Engine::CGameObject* Create(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList,
									   _vec3 vPos,
									   _vec3 vScale,
									   _long iUIDepth = 1000);
private:
	virtual void Free();
};

