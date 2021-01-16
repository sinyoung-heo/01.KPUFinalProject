#include "LightMgr.h"
#include "Light.h"

USING(Engine)
IMPLEMENT_SINGLETON(CLightMgr)

CLightMgr::CLightMgr()
{
}


D3DLIGHT CLightMgr::Get_LightInfo(const LIGHTTYPE& eLightID, const _int& iIdx)
{
	if (iIdx >= m_vecLight[eLightID].size())
		return D3DLIGHT();

	return m_vecLight[eLightID][iIdx]->Get_LightInfo();
}

HRESULT CLightMgr::Set_LightInfo(const LIGHTTYPE& eLightID,
								 const _int& iIdx, 
								 const D3DLIGHT& tLightInfo)
{
	if (iIdx >= m_vecLight[eLightID].size())
		return E_FAIL;

	m_vecLight[eLightID][iIdx]->Set_LightInfo(tLightInfo);

	return S_OK;
}

HRESULT CLightMgr::Set_IsLightOn(const LIGHTTYPE& eLightID, 
								 const _int& iIdx, 
								 const _bool& bIsLightOn)
{
	if (iIdx >= m_vecLight[eLightID].size())
		return E_FAIL;

	m_vecLight[iIdx][eLightID]->Set_IsLightOn(bIsLightOn);

	return S_OK;
}

HRESULT CLightMgr::Add_Light(ID3D12Device * pGraphicDevice,
							 ID3D12GraphicsCommandList * pCommandList, 
							 const LIGHTTYPE& eLightID,
							 const D3DLIGHT & tLightInfo)
{
	CLight* pInstance = CLight::Create(pGraphicDevice, pCommandList, tLightInfo);
	NULL_CHECK_RETURN(pInstance, E_FAIL);

	m_vecLight[eLightID].emplace_back(pInstance);

	return S_OK;
}

void CLightMgr::Update_Light()
{
	/*__________________________________________________________________________________________________________
	[ MFC Tool 에서 사용 ]
	____________________________________________________________________________________________________________*/
	auto iter_begin = m_vecLight[D3DLIGHT_POINT].begin();
	auto iter_end	= m_vecLight[D3DLIGHT_POINT].end();

	for (; iter_begin != iter_end ;)
	{
		_int iEvent = (*iter_begin)->Update_Light();
		if (DEAD_OBJ == iEvent)
		{
			Safe_Release(*iter_begin);
			iter_begin	= m_vecLight[D3DLIGHT_POINT].erase(iter_begin);
			iter_end	= m_vecLight[D3DLIGHT_POINT].end();
		}
		else
			++iter_begin;
	}

	/*__________________________________________________________________________________________________________
	[ Shadow Light View & Proj Update ]
	____________________________________________________________________________________________________________*/
	_vec3 vLightDir = _vec3(Get_LightInfo(Engine::LIGHTTYPE::D3DLIGHT_DIRECTIONAL, 0).Direction);
	vLightDir *= -1.0f;
	
	// LightView
	m_vShadowLightEye	= (vLightDir) * m_fShadowLightHeight;
	_vec3 vAt			= m_vShadowLightAt;
	_vec3 vUp			= _vec3(0.0f, 1.0f, 0.0f);

	m_tShadowDesc.vLightPosition	= _vec4(m_vShadowLightEye, 1.0f);
	m_tShadowDesc.matLightView		= XMMatrixLookAtLH(m_vShadowLightEye.Get_XMVECTOR(),
													   vAt.Get_XMVECTOR(),
													   vUp.Get_XMVECTOR());
	// LightProj
	m_tShadowDesc.fLightPorjFar		= m_fShadowLightFar;
	m_tShadowDesc.matLightProj		= XMMatrixPerspectiveFovLH(XMConvertToRadians(m_fShadowLightFovY),
															   1.0f,
															   m_fShadowLightNear, 
															   m_fShadowLightFar);

}

void CLightMgr::Render_Light(vector<ComPtr<ID3D12Resource>> pvecTargetTexture)
{
	for (auto& vecLight : m_vecLight)
	{
		for (auto& pLight : vecLight)
		{
			if (nullptr != pLight)
				pLight->Render_Light(pvecTargetTexture);
		}

	}
}


void CLightMgr::Free()
{
	for (auto& vecLight : m_vecLight)
	{
		for_each(vecLight.begin(), vecLight.end(), Safe_Release<CLight*>);
		vecLight.clear();
		vecLight.shrink_to_fit();
	}

}
