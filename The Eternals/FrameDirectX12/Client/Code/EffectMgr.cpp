#include "stdafx.h"
#include "EffectMgr.h"
#include "IceStorm.h"
#include "IceDecal.h"
#include "FireDecal.h"
#include "SwordEffect.h"
#include "ObjectMgr.h"
#include "GraphicDevice.h"
#include "FrameMesh.h"
#include "FireRing.h"
#include "TextureEffect.h"
#include "MagicCircle.h"

IMPLEMENT_SINGLETON(CEffectMgr)

CEffectMgr::CEffectMgr()
{
	m_pGraphicDevice=Engine::CGraphicDevice::Get_Instance()->Get_GraphicDevice();
	m_pCommandList=Engine::CGraphicDevice::Get_Instance()->Get_CommandList(Engine::CMDID::CMD_MAIN);
	m_pObjectMgr= Engine::CObjectMgr::Get_Instance();
}

void CEffectMgr::Effect_Dust(_vec3 vecPos, float Radius)
{
	for (int i = 0; i < 18; i++)
	{
		_vec3 newPos;
		newPos.y = 0.5f;
		newPos.x = vecPos.x + Radius * cos(XMConvertToRadians(i * 20.f));
		newPos.z = vecPos.z + Radius * sin(XMConvertToRadians(i * 20.f));

		pGameObj = CTextureEffect::Create(m_pGraphicDevice, m_pCommandList,
			L"Dust",						// TextureTag
			_vec3(3.f),		// Scale
			_vec3(0.0f, 0.0f, 0.0f),		// Angle
			newPos,//Pos,	// Pos
			FRAME(12, 7, 42.0f));			// Sprite Image Frame
		Engine::FAILED_CHECK_RETURN(m_pObjectMgr->Add_GameObject(L"Layer_GameObject", L"Dust", pGameObj), E_FAIL);
		static_cast<CTextureEffect*>(pGameObj)->Set_IsLoop(false);
		static_cast<CTextureEffect*>(pGameObj)->Set_ScaleAnim(false);
		
	}
}

void CEffectMgr::Effect_IceStorm(_vec3 vecPos , int Cnt , float Radius )
{
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
	vecPos.y += 100.f;
	pGameObj = CSwordEffect::Create(m_pGraphicDevice, m_pCommandList,
		L"Twohand19_A_SM",
		_vec3(0.f),
		_vec3(0.f,0.f,180.f),
		vecPos+ vecDir*10//Pos
	);
	Engine::FAILED_CHECK_RETURN(m_pObjectMgr->Add_GameObject(L"Layer_GameObject", L"SwordEffect", pGameObj), E_FAIL);



}

void CEffectMgr::Effect_FireDecal(_vec3 vecPos)
{
	pGameObj = CFireDecal::Create(m_pGraphicDevice, m_pCommandList,
		L"PublicPlane00",
		_vec3(0.01f),
		_vec3(0.f, 0.0f, 0.0f),
		vecPos);
	Engine::FAILED_CHECK_RETURN(m_pObjectMgr->Add_GameObject(L"Layer_GameObject", L"FireDecal", pGameObj), E_FAIL);
}
void CEffectMgr::Effect_IceDecal(_vec3 vecPos)
{
	
	pGameObj = CIceDecal::Create(m_pGraphicDevice, m_pCommandList,
		L"PublicPlane00",
		_vec3(0.01f),
		_vec3(0.f, 0.0f, 0.0f),
		vecPos);
	Engine::FAILED_CHECK_RETURN(m_pObjectMgr->Add_GameObject(L"Layer_GameObject", L"IceDecal", pGameObj), E_FAIL);
}

void CEffectMgr::Effect_ArrowHitted(_vec3 vecPos)
{	
	pGameObj = CTextureEffect::Create(m_pGraphicDevice, m_pCommandList,
		L"Lighting3",						// TextureTag
		_vec3(0.f),		// Scale
		_vec3(0.0f, 0.0f, 0.0f),		// Angle
		vecPos,//Pos,	// Pos
		FRAME(4, 4, 16.0f));			// Sprite Image Frame
	Engine::FAILED_CHECK_RETURN(m_pObjectMgr->Add_GameObject(L"Layer_GameObject", L"Lighting3", pGameObj), E_FAIL);
	static_cast<CTextureEffect*>(pGameObj)->Set_IsLoop(true);
	static_cast<CTextureEffect*>(pGameObj)->Set_ScaleAnim(true);
	static_cast<CTextureEffect*>(pGameObj)->Set_ColorOffset(_vec4(0.f, 0.3f, 0.6f, 1.f));
}

void CEffectMgr::Effect_FireCone(_vec3 vecPos, float RotY , _vec3 vecDir)
{
	vecPos.y = 0.5f;
	//Effect_FireDecal(vecPos);
	vecPos.y = 1.5f;

	//pGameObj = CMagicCircle::Create(m_pGraphicDevice, m_pCommandList,
	//	L"PublicPlane00",
	//	_vec3(0.000f),
	//	_vec3(0.f, 0.0f, 0.0f), vecPos);
	//Engine::FAILED_CHECK_RETURN(m_pObjectMgr->Add_GameObject(L"Layer_GameObject", L"PublicPlane00", pGameObj), E_FAIL);
	//static_cast<CMagicCircle*>(pGameObj)->Set_TexIDX(1, 1, 2);
	//static_cast<CMagicCircle*>(pGameObj)->Set_isScaleAnim(true);
	//static_cast<CMagicCircle*>(pGameObj)->Set_isRotate(true);

	pGameObj = CFireRing::Create(m_pGraphicDevice, m_pCommandList,
		L"HalfMoon0",
		_vec3(0.25f) ,
		_vec3(-0.f, RotY-90.f, 0.f),
		vecPos+vecDir//Pos
		, FRAME(1, 1, 1)
	);
	Engine::FAILED_CHECK_RETURN(m_pObjectMgr->Add_GameObject(L"Layer_GameObject", L"PublicCone00", pGameObj), E_FAIL);

	pGameObj = CFrameMesh::Create(m_pGraphicDevice, m_pCommandList,
		L"PublicCylinder02",
		_vec3(0.015f, 20.5, 0.015),
		_vec3(-0.f, RotY, -0.f),
		vecPos//Pos
		, FRAME(8, 8, 128)
	);
	Engine::FAILED_CHECK_RETURN(m_pObjectMgr->Add_GameObject(L"Layer_GameObject", L"PublicCylinder01", pGameObj), E_FAIL);

	//pGameObj = CFrameMesh::Create(m_pGraphicDevice, m_pCommandList,
	//	L"PublicCylinder01",
	//	_vec3(0.015f,20.5,0.015),
	//	_vec3(-0.f, RotY, -0.f),
	//	vecPos//Pos
	//	, FRAME(8, 8, 128)
	//);
	//Engine::FAILED_CHECK_RETURN(m_pObjectMgr->Add_GameObject(L"Layer_GameObject", L"PublicCylinder01", pGameObj), E_FAIL);
}


void CEffectMgr::Free(void)
{
}
