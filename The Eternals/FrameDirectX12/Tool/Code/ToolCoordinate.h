#pragma once
#include "GameObject.h"

namespace Engine
{
	class CCoordinateCol;
	class CShaderColor;
}

class CToolCoordinate : public Engine::CGameObject
{
private:
	explicit CToolCoordinate(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList);
	explicit CToolCoordinate(const CToolCoordinate& rhs);
	virtual ~CToolCoordinate() = default;

public:
	// CGameObject을(를) 통해 상속됨
	virtual HRESULT	Ready_GameObject(const _vec3 & vScale,
									 const _vec3 & vAngle,
									 const _vec3 & vPos);
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
	Engine::CCoordinateCol*	m_pBufferCom = nullptr;
	Engine::CShaderColor*	m_pShaderCom = nullptr;

public:
	static Engine::CGameObject* Create(ID3D12Device* pGraphicDevice, 
									   ID3D12GraphicsCommandList* pCommandList,
									   const _vec3 & vScale,
									   const _vec3 & vAngle,
									   const _vec3 & vPos);
private:
	virtual void Free();
};

