#pragma once
#include "Include.h"
#include "GameObject.h"

class CCollisionTick : public Engine::CGameObject
{
private:
	explicit CCollisionTick(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList);
	virtual ~CCollisionTick() = default;

public:
	// CGameObject을(를) 통해 상속됨
	virtual HRESULT	Ready_GameObject(wstring wstrCollisionTag,
									 const _vec3& vScale,
									 const _vec3& vPos,
									 const _uint& uiDamage,
									 const _float& fLifeTime);
	virtual HRESULT	LateInit_GameObject();
	virtual _int	Update_GameObject(const _float& fTimeDelta);
	virtual _int	LateUpdate_GameObject(const _float& fTimeDelta);
	virtual void	Process_Collision();
private:
	/*__________________________________________________________________________________________________________
	[ Value ]
	____________________________________________________________________________________________________________*/
	_uint	m_uiDamage   = 0;
	_float	m_fTimeDelta = 0.0f;
	_float	m_fLifeTime  = 1.0f;

public:
	static Engine::CGameObject* Create(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList,
									   wstring wstrCollisionTag,
									   const _vec3& vScale,
									   const _vec3& vPos,
									   const _uint& uiDamage,
									   const _float& fLifeTime);
private:
	virtual void Free();
};

