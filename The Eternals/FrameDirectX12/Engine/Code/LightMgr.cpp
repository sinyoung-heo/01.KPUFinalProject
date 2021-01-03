#include "LightMgr.h"
#include "Light.h"

USING(Engine)
IMPLEMENT_SINGLETON(CLightMgr)

CLightMgr::CLightMgr()
{
}

const D3DLIGHT & CLightMgr::Get_LightInfo(const _int & iIdx)
{
	if (iIdx >= m_vecLight.size())
		return D3DLIGHT();

	return m_vecLight[iIdx]->Get_LightInfo();
}

HRESULT CLightMgr::Set_LightInfo(const _int & iIdx, const D3DLIGHT & tLightInfo)
{
	if (iIdx >= m_vecLight.size())
		return E_FAIL;

	m_vecLight[iIdx]->Set_LightInfo(tLightInfo);

	return S_OK;
}

HRESULT CLightMgr::Set_IsLightOn(const _int & iIdx, const _bool & bIsLightOn)
{
	if (iIdx >= m_vecLight.size())
		return E_FAIL;

	m_vecLight[iIdx]->Set_IsLightOn(bIsLightOn);

	return S_OK;
}

HRESULT CLightMgr::Add_Light(ID3D12Device * pGraphicDevice,
							 ID3D12GraphicsCommandList * pCommandList, 
							 const D3DLIGHT & tLightInfo)
{
	CLight* pInstance = CLight::Create(pGraphicDevice, pCommandList, tLightInfo);
	NULL_CHECK_RETURN(pInstance, E_FAIL);

	m_vecLight.emplace_back(pInstance);

	return S_OK;
}

void CLightMgr::Render_Light(vector<ComPtr<ID3D12Resource>> pvecTargetTexture)
{
	for (auto& pLight : m_vecLight)
	{
		if (nullptr != pLight)
			pLight->Render_Light(pvecTargetTexture);
	}
}

void CLightMgr::Free()
{
	for_each(m_vecLight.begin(), m_vecLight.end(), Safe_Release<CLight*>);
	m_vecLight.clear();
	m_vecLight.shrink_to_fit();
}
