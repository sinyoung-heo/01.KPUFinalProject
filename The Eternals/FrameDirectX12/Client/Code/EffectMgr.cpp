#include "stdafx.h"
#include "EffectMgr.h"
#include "IceStorm.h"
#include "IceDecal.h"
#include "FireDecal.h"
#include "SwordEffect.h"
#include "ObjectMgr.h"
#include "GraphicDevice.h"
#include "FrameMesh.h"
IMPLEMENT_SINGLETON(CEffectMgr)

CEffectMgr::CEffectMgr()
{
	m_pGraphicDevice=Engine::CGraphicDevice::Get_Instance()->Get_GraphicDevice();
	m_pCommandList=Engine::CGraphicDevice::Get_Instance()->Get_CommandList(Engine::CMDID::CMD_MAIN);
	m_pObjectMgr= Engine::CObjectMgr::Get_Instance();
}

void CEffectMgr::Effect_IceStorm(_vec3 vecPos , int Cnt , float Radius )
{
	Engine::CGameObject* pGameObj = nullptr;
	for (int i = 0; i < 36; i+=(36/Cnt))
	{
		
			pGameObj = CIceStorm::Create(m_pGraphicDevice, m_pCommandList,
				L"IceStorm1",
				_vec3(0.f),
				_vec3(0.f, 0.0f, 0.0f),
				vecPos,//Pos
				Radius, XMConvertToRadians(i * 10.f));
			Engine::FAILED_CHECK_RETURN(m_pObjectMgr->Add_GameObject(L"Layer_GameObject", L"IceStorm1", pGameObj), E_FAIL);
		
	}
}

void CEffectMgr::Effect_SwordEffect(_vec3 vecPos,_vec3 vecDir)
{
	
	Engine::CGameObject* pGameObj = nullptr;
	pGameObj = CSwordEffect::Create(m_pGraphicDevice, m_pCommandList,
		L"Twohand19_A_SM",
		_vec3(0.f),
		_vec3(0.f,0.f,180.f),
		vecPos+ vecDir*10//Pos
	);
	Engine::FAILED_CHECK_RETURN(m_pObjectMgr->Add_GameObject(L"Layer_GameObject", L"SwordEffect", pGameObj), E_FAIL);

	vecPos.y = 0.f;
	//pGameObj = CFrameMesh::Create(m_pGraphicDevice, m_pCommandList,
	//	L"PublicCylinder00",
	//	_vec3(0.1f),
	//	_vec3(90.f, 0.f, 0.f),
	//	vecPos + vecDir * 10//Pos
	//	, FRAME(1, 1, 1)
	//);
	//Engine::FAILED_CHECK_RETURN(m_pObjectMgr->Add_GameObject(L"Layer_GameObject", L"PublicCylinder00", pGameObj), E_FAIL);
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
void CEffectMgr::Effect_IceDecal(_vec3 vecPos)
{
	Engine::CGameObject* pGameObj = nullptr;
	pGameObj = CIceDecal::Create(m_pGraphicDevice, m_pCommandList,
		L"PublicPlane00",
		_vec3(0.01f),
		_vec3(0.f, 0.0f, 0.0f),
		vecPos);
	Engine::FAILED_CHECK_RETURN(m_pObjectMgr->Add_GameObject(L"Layer_GameObject", L"IceDecal", pGameObj), E_FAIL);
}


void CEffectMgr::Free(void)
{
}
