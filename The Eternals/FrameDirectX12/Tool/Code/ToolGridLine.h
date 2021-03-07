#pragma once
#include "GameObject.h"

namespace Engine
{
	class CRcCol;
	class CShaderColor;
}

class CToolGridLine : public Engine::CGameObject
{
private:
	explicit CToolGridLine(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList);
	virtual ~CToolGridLine() = default;

public:
	// CGameObject을(를) 통해 상속됨
	virtual HRESULT	Ready_GameObject(const _vec3 & vScale,
									 const _vec3 & vPos,
									 const _long& iUIDepth);
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
	Engine::CRcCol*			m_pBufferCom = nullptr;
	Engine::CShaderColor*	m_pShaderCom = nullptr;

private:
	_vec3 m_vConvert = _vec3(0.0f);

public:
	static Engine::CGameObject* Create(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList,
									   const _vec3 & vScale,
									   const _vec3 & vPos,
									   const _long& iUIDepth = 1000);
private:
	virtual void Free();
};

