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
	D3DLIGHT	Get_LightInfo(const LIGHTTYPE & eLightID, const _int& iIdx = 0);

	// Set
	HRESULT		Set_LightInfo(const LIGHTTYPE& eLightID, const _int & iIdx, const D3DLIGHT & tLightInfo);
	HRESULT		Set_IsLightOn(const LIGHTTYPE& eLightID, const _int & iIdx, const _bool & bIsLightOn);

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


private:
	virtual void Free();
};

END