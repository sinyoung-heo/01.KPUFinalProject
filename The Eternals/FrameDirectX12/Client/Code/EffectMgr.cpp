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
#include "ParticleEffect.h"
#include "PublicSphere.h"
#include "PublicPlane.h"
#include "MagicCircleGlow.h"
#include "GridShieldEffect.h"
#include "EffectShield.h"
#include "SwordEffect_s.h"
#include "IceStorm_m.h"
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

void CEffectMgr::Effect_SwordEffect_s(_vec3 vecPos, _vec3 vecDir)
{
	_vec3 upVec = _vec3(0, 1, 0);
	_vec3 crossVec = upVec.Cross_InputV1(vecDir);

	for (int i = -6; i <= 6; i+=3)
	{
		_vec3 newPos = vecPos - crossVec * (1.5f) * i;
		newPos -= (vecDir * (18 - abs( i)));
		pGameObj = CSwordEffect_s::Create(m_pGraphicDevice, m_pCommandList,
			L"publicSword",
			_vec3(0.f),
			_vec3(0.f, 0.f, 0.f),
			newPos//Pos
			, vecDir, 0
		);
		static_cast<CSwordEffect_s*>(pGameObj)->Set_isScaleAnim(true);
		Engine::FAILED_CHECK_RETURN(m_pObjectMgr->Add_GameObject(L"Layer_GameObject", L"publicSword", pGameObj), E_FAIL);
	}
}

void CEffectMgr::Effect_SwordTrail(_vec3 vecPos, _vec3 vecDir)
{
	_matrix rotationY=XMMatrixRotationY(XMConvertToRadians(20.f));
	XMVECTOR temp= XMVector3TransformCoord(vecDir.Get_XMVECTOR(), rotationY);
	
	_vec3 DirectVec;
	XMStoreFloat3(&DirectVec, temp);
	DirectVec.Normalize();
	for (int i = 0; i < 13; i++)
	{
		_vec3 PosOffSet = vecDir * 2.f + (DirectVec *(1+ i)*(1+powf(i/13,2)));
		pGameObj = CIceStorm_m::Create(m_pGraphicDevice, m_pCommandList,
			L"publicSkill3",
			_vec3(0.f),
			_vec3(rand() % 20 - 10,m_pObjectMgr->Get_GameObject(L"Layer_GameObject", L"ThisPlayer")->Get_Transform()->m_vAngle.y
				,rand()%20 - 10),
			vecPos+ PosOffSet
			, 0.05f + (0.02f * i)
		);
		Engine::FAILED_CHECK_RETURN(m_pObjectMgr->Add_GameObject(L"Layer_GameObject", L"publicSkill3", pGameObj), E_FAIL);
	}
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

	pGameObj = CFireRing::Create(m_pGraphicDevice, m_pCommandList,
		L"HalfMoon0",
		_vec3(0.25f) ,
		_vec3(-0.f, RotY-90.f, 0.f),
		vecPos+vecDir//Pos
		, FRAME(1, 1, 1)
	);
	Engine::FAILED_CHECK_RETURN(m_pObjectMgr->Add_GameObject(L"Layer_GameObject", L"PublicCone00", pGameObj), E_FAIL);

	for (int i = 0; i < 5; i++)
	{
		_vec3 newPos = vecPos;
		newPos.y = 0.75 * i;
		pGameObj = CFrameMesh::Create(m_pGraphicDevice, m_pCommandList,
			L"PublicCylinder02",
			_vec3(0.015f, 20.5, 0.015),
			_vec3(-0.f, RotY + 60 * i , -0.f),
			newPos//Pos
			, FRAME(8, 8, 128)
		);
		Engine::FAILED_CHECK_RETURN(m_pObjectMgr->Add_GameObject(L"Layer_GameObject", L"PublicCylinder01", pGameObj), E_FAIL);
	}
}

void CEffectMgr::Effect_Test(_vec3 vecPos)
{
	for(int i=0; i<5; i++)
	{
		vecPos.y = 1.f;
		pGameObj = CEffectShield::Create(m_pGraphicDevice, m_pCommandList,
			L"publicShield",
			_vec3(0.00f),
			_vec3(0.f, 0.f, 0.f),
			vecPos, (360/5) *i);
		static_cast<CEffectShield*>(pGameObj)->Set_isScaleAnim(true);
		Engine::FAILED_CHECK_RETURN(m_pObjectMgr->Add_GameObject(L"Layer_GameObject", L"publicShield", pGameObj), E_FAIL);
	}

	vecPos.y = rand()%50 *0.01f;
	vecPos.z += 0.2f;
	pGameObj = CMagicCircle::Create(m_pGraphicDevice, m_pCommandList,
		L"PublicPlane00",
		_vec3(0.00f),
		_vec3(0.f, 0.0f, 0.0f), vecPos);
	Engine::FAILED_CHECK_RETURN(m_pObjectMgr->Add_GameObject(L"Layer_GameObject", L"PublicPlane00", pGameObj), E_FAIL);
	static_cast<CMagicCircle*>(pGameObj)->Set_TexIDX(20, 20, 2);
	static_cast<CMagicCircle*>(pGameObj)->Set_isScaleAnim(true);
	static_cast<CMagicCircle*>(pGameObj)->Set_isRotate(false);


	//vecPos.y = 2.5f;
	//pGameObj = CMagicCircle::Create(m_pGraphicDevice, m_pCommandList,
	//	L"PublicPlane00",
	//	_vec3(0.02f),
	//	_vec3(0.f, 0.0f, 0.0f), vecPos);
	//Engine::FAILED_CHECK_RETURN(m_pObjectMgr->Add_GameObject(L"Layer_GameObject", L"PublicPlane00", pGameObj), E_FAIL);
	//static_cast<CMagicCircle*>(pGameObj)->Set_TexIDX(15, 15, 2);
	//static_cast<CMagicCircle*>(pGameObj)->Set_isScaleAnim(false);
	//static_cast<CMagicCircle*>(pGameObj)->Set_isRotate(false);
	
	
	//for (int i = 0; i < 5; i++)
	//{
	//	_vec3 newPos = vecPos;
	//	newPos.y = 1.25 * i;
	//	pGameObj = CFrameMesh::Create(m_pGraphicDevice, m_pCommandList,
	//		L"PublicCylinder02",
	//		_vec3(0.015f, 20.5, 0.015),
	//		_vec3(-0.f,  60 * i, 80.f),
	//		newPos//Pos
	//		, FRAME(8, 8, 128)
	//	);
	//	Engine::FAILED_CHECK_RETURN(m_pObjectMgr->Add_GameObject(L"Layer_GameObject", L"PublicCylinder01", pGameObj), E_FAIL);
	//}
	//pGameObj = CParticleEffect::Create(m_pGraphicDevice, m_pCommandList,
	//	L"Fire2",						// TextureTag
	//	_vec3(0.1f),		// Scale
	//	_vec3(0.0f, 0.0f, 0.0f),		// Angle
	//	vecPos,	// Pos
	//	FRAME(10, 1, 10.0f));			// Sprite Image Frame
	//Engine::FAILED_CHECK_RETURN(m_pObjectMgr->Add_GameObject(L"Layer_GameObject", L"Fire2", pGameObj), E_FAIL);
}

void CEffectMgr::Effect_GridShieldEffect(_vec3 vecPos,int type)
{
	int Pipeidx = 0;
	vecPos.y = 1.f + rand()%10 * 0.01;
	type == 0 ? Pipeidx = 2 : Pipeidx=10;
	pGameObj = CGridShieldEffect::Create(m_pGraphicDevice, m_pCommandList,
		L"PublicSphere00",
		_vec3(0.00f),
		_vec3(0.f, 0.f, 0.f),
		vecPos, Pipeidx
	);
	Engine::FAILED_CHECK_RETURN(m_pObjectMgr->Add_GameObject(L"Layer_GameObject", L"PublicSphere00", pGameObj), E_FAIL);
	static_cast<CGridShieldEffect*>(pGameObj)->Set_isScaleAnim(true);
	if (type == 0)
		static_cast<CGridShieldEffect*>(pGameObj)->Set_TexIDX(0, 2, 16);
	else
		static_cast<CGridShieldEffect*>(pGameObj)->Set_TexIDX(0, 25, 16);

	
	pGameObj = CMagicCircle::Create(m_pGraphicDevice, m_pCommandList,
		L"PublicPlane00",
		_vec3(0.00f),
		_vec3(0.f, 0.0f, 0.0f), vecPos, 0);
	Engine::FAILED_CHECK_RETURN(m_pObjectMgr->Add_GameObject(L"Layer_GameObject", L"PublicPlane00", pGameObj), E_FAIL);
	if (type == 0)
		static_cast<CMagicCircle*>(pGameObj)->Set_TexIDX(18, 18, 2);
	else
		static_cast<CMagicCircle*>(pGameObj)->Set_TexIDX(19, 19, 2);

	static_cast<CMagicCircle*>(pGameObj)->Set_isScaleAnim(true);
	static_cast<CMagicCircle*>(pGameObj)->Set_isRotate(true);
}

void CEffectMgr::Effect_Shield(_vec3 vecPos)
{
}


void CEffectMgr::Free(void)
{
}
