#pragma once
#include "ComponentMgr.h"
#include "Renderer.h"
#include "FrustumMgr.h"

BEGIN(Engine)

class CObjectMgr;


class ENGINE_DLL CGameObject : public CBase
{
protected:
	explicit CGameObject(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList);
	explicit CGameObject(const CGameObject& rhs);
	virtual ~CGameObject() = default;

public:
	// Get
	CComponent*		Get_Component(wstring wstrComponentTag, COMPONENTID eID);
	CTransform*		Get_Transform()			{ return m_pTransCom; }
	CInfo*			Get_Info()				{ return m_pInfoCom; }
	_float			Get_DepthOfView()		{ return m_fViewZ; }
	_long			Get_UIDepth()			{ return m_UIDepth; }

	// Set
	void			Set_DeadGameObject()	{ m_bIsDead = true; }

	// CGameObject을(를) 통해 상속됨
	virtual HRESULT Ready_GameObjectPrototype();
	virtual HRESULT	Ready_GameObject(const _bool& bIsCreate_Transform = true, const _bool& bIsCreate_Info = false);
	virtual HRESULT	LateInit_GameObject();
	virtual _int	Update_GameObject(const _float& fTimeDelta);
	virtual _int	LateUpdate_GameObject(const _float& fTimeDelta);
	virtual void	Render_GameObject(const _float& fTimeDelta);
	virtual void	Render_ShadowDepth(const _float & fTimeDelta);

	// 2020.06.11 MultiThreadRendering
	virtual void	Render_GameObject(ID3D12GraphicsCommandList* pCommandList, 
									  const _int& iContextIdx);
	virtual void	Render_ShadowDepth(ID3D12GraphicsCommandList* pCommandList, 
									   const _int& iContextIdx);


protected:
	virtual HRESULT Add_Component();
	void			SetUp_BillboardMatrix();
	void			SetUp_ShadowDepth(_vec3& vLightEye, _vec3& vLightAt, _vec3& vLightDir);
	void			Compute_ViewZ(_vec4& vPosInWorld);

private:
	CComponent*		Find_Component(wstring wstrComponentTag, const COMPONENTID& eID);

protected:
	/*__________________________________________________________________________________________________________
	[ GraphicDevice & Mgr ]
	____________________________________________________________________________________________________________*/
	ID3D12Device*				m_pGraphicDevice	= nullptr;
	ID3D12GraphicsCommandList*	m_pCommandList		= nullptr;
	CRenderer*					m_pRenderer			= nullptr;
	CObjectMgr*					m_pObjectMgr		= nullptr;
	CComponentMgr*				m_pComponentMgr		= nullptr;
	CFrustumMgr*				m_pFrustumMgr		= nullptr;

	/*__________________________________________________________________________________________________________
	[ Component ]
	____________________________________________________________________________________________________________*/
	map<wstring, CComponent*>	m_mapComponent[COMPONENTID::ID_END];
	CTransform*							m_pTransCom	= nullptr;
	CInfo*								m_pInfoCom	= nullptr;

	/*__________________________________________________________________________________________________________
	[ Value ]
	____________________________________________________________________________________________________________*/
	_bool			m_bIsDead		= false;
	_bool			m_bIsLateInit	= false;
	_float			m_fViewZ		= 0.0f;
	_long			m_UIDepth		= 0;

	/*__________________________________________________________________________________________________________
	[ ShadowDepth ]
	____________________________________________________________________________________________________________*/
	SHADOW_DESC		m_tShadowInfo	{ };

public:
	virtual CGameObject* Clone_GameObject();
protected:
	virtual void Free();
};

END