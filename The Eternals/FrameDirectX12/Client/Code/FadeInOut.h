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
	void Set_FadeInOutEventType(const EVENT_TYPE& eEventType);
	void Set_IsSendPacket(const _bool& bIsSendPacket)		{ m_bIsSendPacket = bIsSendPacket; }
	void Set_IsReceivePacket(const _bool& bIsReceivePacket) { m_bIsReceivePacket = bIsReceivePacket; }

	// CGameObject을(를) 통해 상속됨
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

	CPacketMgr* m_pPacketMgr = nullptr;

	/*__________________________________________________________________________________________________________
	[ Value ]
	____________________________________________________________________________________________________________*/
	_uint		m_uiTexIdx		     = 0;
	EVENT_TYPE	m_eEventType         = EVENT_TYPE::EVENT_END;
	_float		m_fAlpha             = 1.0f;
	_bool		m_bIsSendPacket      = false;
	_bool		m_bIsReceivePacket   = false;
	_bool		m_bIsSetCinematic    = false;
	_bool		m_bIsCinematicEnding = false;

	_vec3	m_vConvert	= _vec3(0.0f, 0.0f, 0.0f);
	_matrix	m_matView	= INIT_MATRIX;
	_matrix	m_matProj	= INIT_MATRIX;

public:
	static Engine::CGameObject* Create(ID3D12Device* pGraphicDevice, 
									   ID3D12GraphicsCommandList* pCommandList,
									   const EVENT_TYPE& eEventType);

	static CFadeInOut** Create_InstancePool(ID3D12Device* pGraphicDevice,
											ID3D12GraphicsCommandList* pCommandList,
											const _uint& uiInstanceCnt);
private:
	virtual void Free();
};

