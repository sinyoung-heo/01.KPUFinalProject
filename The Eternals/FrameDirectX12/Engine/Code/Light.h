#pragma once
#include "Base.h"
#include "Engine_Include.h"

BEGIN(Engine)

class CScreenTex;
class CShaderLighting;
class CColliderBox;
class CShaderLightingInstancing;

class ENGINE_DLL CLight final : public CBase
{
private:
	explicit CLight(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList);
	virtual ~CLight() = default;

public:
	// Get
	const D3DLIGHT&	Get_LightInfo() const	{ return m_tLightInfo; }
	D3DLIGHT&		Get_LightInfo()			{ return m_tLightInfo; }
	CColliderBox*	Get_ColliderCom()		{ return m_pColliderCom; }

	// Set
	void			Set_LightInfo(const D3DLIGHT& tLightInfo)	{ m_tLightInfo = tLightInfo; }
	void			Set_IsLightOn(const _bool& bIsLightOn)		{ m_bIsLightOn = bIsLightOn; }
	void			Set_IsDead()								{ m_bIsDead = true; }
	void			Set_ColliderColorSelected();
	void			Set_ColliderColorDiffuse();
	void			Set_ColliderPosition();

	// Method
	HRESULT			Ready_Light(const D3DLIGHT& tLightInfo);
	_int			Update_Light();
	void			Render_Light();
	void			Set_ConstantTable(const _int& iInstanceIdx);
private:
	/*__________________________________________________________________________________________________________
	[ Graphic Device ]
	____________________________________________________________________________________________________________*/
	ID3D12Device*				m_pGraphicDevice = nullptr;
	ID3D12GraphicsCommandList*	m_pCommandList	 = nullptr;

	/*__________________________________________________________________________________________________________
	[ Component ]
	____________________________________________________________________________________________________________*/
	CScreenTex*					m_pBufferCom	    = nullptr;
	CShaderLightingInstancing*	m_pShaderInstancing = nullptr;
	CColliderBox*				m_pColliderCom	    = nullptr;

	/*__________________________________________________________________________________________________________
	[ Value ]
	____________________________________________________________________________________________________________*/
	D3DLIGHT	m_tLightInfo;
	_bool		m_bIsLightOn	= true;
	_bool		m_bIsSetTexture	= false;
	_bool		m_bIsDead		= false;

	_uint		m_iPipelineStatePass = 0;

public:
	static CLight* Create(ID3D12Device* pGraphicDevice,
						  ID3D12GraphicsCommandList* pCommandList,
						  const D3DLIGHT& tLightInfo);
private:
	virtual void	Free();
};

END