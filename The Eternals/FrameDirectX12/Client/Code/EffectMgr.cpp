#include "stdafx.h"
#include "EffectMgr.h"
#include "IceStorm.h"
#include "FireDecal.h"
#include "ObjectMgr.h"
#include "GraphicDevice.h"
IMPLEMENT_SINGLETON(CEffectMgr)

CEffectMgr::CEffectMgr()
{
	m_pGraphicDevice=Engine::CGraphicDevice::Get_Instance()->Get_GraphicDevice();
	m_pCommandList=Engine::CGraphicDevice::Get_Instance()->Get_CommandList(Engine::CMDID::CMD_MAIN);
	m_pObjectMgr= Engine::CObjectMgr::Get_Instance();
}

void CEffectMgr::Effect_IceStorm(_vec3 vecPos)
{
	Engine::CGameObject* pGameObj = nullptr;
	for (int i = 0; i < 36; i++)
	{
		pGameObj = CIceStorm::Create(m_pGraphicDevice, m_pCommandList,
			L"IceStorm1",
			_vec3(0.f),
			_vec3(0.f, 0.0f, 0.0f),
			vecPos,//Pos
			 5.f, XMConvertToRadians(i * 10.f));
		Engine::FAILED_CHECK_RETURN(m_pObjectMgr->Add_GameObject(L"Layer_GameObject", L"IceStorm1", pGameObj), E_FAIL);
	}
}

void CEffectMgr::Effect_FireDecal(_vec3 vecPos)
{
	Engine::CGameObject* pGameObj = nullptr;
	pGameObj = CFireDecal::Create(m_pGraphicDevice, m_pCommandList,
		L"PublicPlane00",
		_vec3(0.01f),
		_vec3(0.f, 0.0f, 0.0f),
		vecPos);
	Engine::FAILED_CHECK_RETURN(m_pObjectMgr->Add_GameObject(L"Layer_GameObject", L"FireDecal", pGameObj), E_FAIL);
}

void CEffectMgr::Free(void)
{
}
