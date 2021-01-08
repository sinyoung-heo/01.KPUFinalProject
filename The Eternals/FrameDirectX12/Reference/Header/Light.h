#pragma once
#include "Engine_Include.h"
#include "Base.h"

BEGIN(Engine)

class CScreenTex;
class CShaderLighting;
class CColliderBox;

class ENGINE_DLL CLight final : public CBase
{
private:
	explicit CLight(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList);
	virtual ~CLight() = default;

public:
	// Get
	const D3DLIGHT&	Get_LightInfo() const { return m_tLightInfo; }

	// Set
	void			Set_LightInfo(const D3DLIGHT& tLightInfo)	{ m_tLightInfo = tLightInfo; }
	void			Set_IsLightOn(const _bool& bIsLightOn)		{ m_bIsLightOn = bIsLightOn; }

	// Method
	HRESULT			Ready_Light(const D3DLIGHT& tLightInfo);
	void			Render_Light(vector<ComPtr<ID3D12Resource>> pvecTargetTexture);
private:
	void			Set_ConstantTable();

private:
	/*__________________________________________________________________________________________________________
	[ Graphic Device ]
	____________________________________________________________________________________________________________*/
	ID3D12Device*				m_pGraphicDevice	{ nullptr };
	ID3D12GraphicsCommandList*	m_pCommandList		{ nullptr };

	/*__________________________________________________________________________________________________________
	[ Component ]
	____________________________________________________________________________________________________________*/
	CScreenTex*			m_pBufferCom				{ nullptr };
	CShaderLighting*	m_pShaderCom				{ nullptr };
	CColliderBox*		m_pColliderCom				{ nullptr };

	/*__________________________________________________________________________________________________________
	[ Value ]
	____________________________________________________________________________________________________________*/
	D3DLIGHT	m_tLightInfo;
	_bool		m_bIsLightOn						{ true };
	_bool		m_bIsSetTexture						{ false };



public:
	static CLight* Create(ID3D12Device* pGraphicDevice,
						  ID3D12GraphicsCommandList* pCommandList,
						  const D3DLIGHT& tLightInfo);
private:
	virtual void	Free();
};

END