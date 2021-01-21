#pragma once
#include "Base.h"

BEGIN(Engine)

class CLight;

class ENGINE_DLL CLightMgr final : public CBase
{
	DECLARE_SINGLETON(CLightMgr)

private:
	explicit CLightMgr();
	virtual ~CLightMgr() = default;

public:
	// Get
	D3DLIGHT&			Get_LightInfo(const LIGHTTYPE & eLightID, const _int& iIdx = 0);
	CLight*				Get_Light(const LIGHTTYPE & eLightID, const _int & iIdx = 0);
	vector<CLight*>&	Get_VecLightInfo(const LIGHTTYPE & eLightID) { return m_vecLight[eLightID]; }
	SHADOW_DESC&		Get_ShadowDesc()		{ return m_tShadowDesc; }
	const _vec3&		Get_ShadowLightEye()	{ return m_vShadowLightEye; }
	const _vec3&		Get_ShadowLightAt()		{ return m_vShadowLightAt; }


	// Set
	HRESULT		Set_LightInfo(const LIGHTTYPE& eLightID, const _int & iIdx, const D3DLIGHT & tLightInfo);
	HRESULT		Set_IsLightOn(const LIGHTTYPE& eLightID, const _int & iIdx, const _bool & bIsLightOn);
	void		Set_ShadowLightHeight(const _float& fHeight)	{ m_fShadowLightHeight = fHeight; }
	void		Set_ShadowLightAt(const _vec3& vAt)				{ m_vShadowLightAt = vAt; }
	void		Set_ShadowLightFovY(const _float& fFovY)		{ m_fShadowLightFovY = fFovY; }
	void		Set_ShadowLightFar(const _float& fFar)			{ m_fShadowLightFar = fFar; }

	// Method
	HRESULT		Add_Light(ID3D12Device* pGraphicDevice, 
						  ID3D12GraphicsCommandList* pCommandList, 
						  const LIGHTTYPE& eLightID,
						  const D3DLIGHT& tLightInfo);
	void		Update_Light();
	void		Render_Light(vector<ComPtr<ID3D12Resource>> pvecTargetTexture);

private:
	/*__________________________________________________________________________________________________________
	[ Light ]
	____________________________________________________________________________________________________________*/
	vector<CLight*>	m_vecLight[LIGHTTYPE::D3DLIGHT_END];

	/*__________________________________________________________________________________________________________
	[ Shadow Light ]
	____________________________________________________________________________________________________________*/
	SHADOW_DESC	m_tShadowDesc{ };

	// _matrix		m_matLightView			= INIT_MATRIX;
	_vec3		m_vShadowLightEye		= _vec3(0.0f, 0.0f, 0.0f);
	_vec3		m_vShadowLightAt		= _vec3(128.0f, 0.0f, 128.0f);
	_float		m_fShadowLightHeight	= 500.0f;

	// _matrix		m_matLightProj			= INIT_MATRIX;
	_float		m_fShadowLightFovY		= 45.0f;
	_float		m_fShadowLightNear		= 1.0f;
	_float		m_fShadowLightFar		= 10'000.0f;

private:
	virtual void Free();
};

END