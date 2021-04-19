#pragma once
#include "Include.h"
#include "GameObject.h"

namespace Engine
{
	class CRcTex;
	class CTexture;
	class CShaderTexture;
}

class CFadeInOut : public Engine::CGameObject
{
private:
	explicit CFadeInOut(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList);
	virtual ~CFadeInOut() = default;

public:
	// CGameObject��(��) ���� ��ӵ�
	virtual HRESULT	Ready_GameObject(const EVENT_TYPE& eEventType);
	virtual HRESULT	LateInit_GameObject();
	virtual _int	Update_GameObject(const _float& fTimeDelta);
	virtual _int	LateUpdate_GameObject(const _float& fTimeDelta);
	virtual void	Render_GameObject(const _float& fTimeDelta);
private:
	virtual HRESULT Add_Component();
	void Set_ConstantTable();
	void SetUp_FadeInOutEvent(const _float& fTimeDelta);
private:
	/*__________________________________________________________________________________________________________
	[ Component ]
	____________________________________________________________________________________________________________*/
	Engine::CRcTex*			m_pBufferCom	= nullptr;
	Engine::CTexture*		m_pTextureCom	= nullptr;
	Engine::CShaderTexture*	m_pShaderCom	= nullptr;

	/*__________________________________________________________________________________________________________
	[ Value ]
	____________________________________________________________________________________________________________*/
	_uint		m_uiTexIdx		 = 0;
	EVENT_TYPE	m_eEventType     = EVENT_TYPE::EVENT_END;
	_float		m_fAlpha         = 1.0f;

	_vec3	m_vConvert	= _vec3(0.0f, 0.0f, 0.0f);
	_matrix	m_matView	= INIT_MATRIX;
	_matrix	m_matProj	= INIT_MATRIX;

public:
	static Engine::CGameObject* Create(ID3D12Device* pGraphicDevice, 
									   ID3D12GraphicsCommandList* pCommandList,
									   const EVENT_TYPE& eEventType);
private:
	virtual void Free();
};
