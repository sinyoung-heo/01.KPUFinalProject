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
#include "EffectAxe.h"
#include "SwordEffect_s.h"
#include "IceStorm_m.h"
#include "IceStorm_s.h"
#include "DistTrail.h"
#include "CollisionTick.h"
#include "PCGladiator.h"

#include "MeshParticleEffect.h"
#include "DragonEffect.h"
#include "TrailTexEffect.h"
#include "WarningGround.h"
#include "RectDecal.h"
#include "DirParticleEffect.h"
#include "BossDecal.h"
#include "TonadoEffect.h"
IMPLEMENT_SINGLETON(CEffectMgr)

CEffectMgr::CEffectMgr()
{
	m_pGraphicDevice=Engine::CGraphicDevice::Get_Instance()->Get_GraphicDevice();
	m_pCommandList=Engine::CGraphicDevice::Get_Instance()->Get_CommandList(Engine::CMDID::CMD_MAIN);
	m_pObjectMgr= Engine::CObjectMgr::Get_Instance();
}

void CEffectMgr::Effect_Dust(_vec3 vecPos, float Radius, float Scale,float FPS)
{
	_vec3 newPos;
	for (int i = 0; i < 18; i++)
	{
		newPos.y = 1.f;
		newPos.x = vecPos.x + Radius * cos(XMConvertToRadians(i * 20.f));
		newPos.z = vecPos.z + Radius * sin(XMConvertToRadians(i * 20.f));
		Effect_TextureEffect(L"Dust", _vec3(Scale, Scale,0.f), _vec3(0.f), newPos, FRAME(12, 7, FPS), false, false);
	
	}
}

void CEffectMgr::Effect_IceStorm(_vec3 vecPos , int Cnt , float Radius )
{
	Effect_MeshParticle(L"publicStone"+to_wstring(rand()%4), _vec3(0.01f), _vec3(0.f), vecPos,false,false, 5, 20
		, 0, 0, 0, _vec2(28, 7),0,true);
	Effect_MeshParticle(L"publicStone" + to_wstring(rand() % 4), _vec3(0.01f), _vec3(0.f), vecPos,false,false, 5, 20
		, 0, 0, 0, _vec2(28, 7), 0, true);
	for (int i = 0; i < 36; i+=(36/Cnt))
	{
		pGameObj = Pop_Instance(CInstancePoolMgr::Get_Instance()->Get_Effect_IceStormEffect());
		if (nullptr != pGameObj)
		{
			static_cast<CIceStorm*>(pGameObj)->Set_CreateInfo(_vec3(0.f), _vec3(0.f), vecPos, Radius, XMConvertToRadians(i * 10.f));
			Engine::FAILED_CHECK_RETURN(m_pObjectMgr->Add_GameObject(L"Layer_GameObject", L"IceStorm1", pGameObj), E_FAIL);
		}

	}
}

void CEffectMgr::Effect_IceStorm_s(_vec3 vecPos, float Radius)
{
	pGameObj = Pop_Instance(CInstancePoolMgr::Get_Instance()->Get_Effect_IceStorm_s_Effect());
	if (nullptr != pGameObj)
	{
		static_cast<CIceStorm_s*>(pGameObj)->Set_CreateInfo(_vec3(0.f), _vec3(0.f), vecPos, Radius, XMConvertToRadians(rand() % 36 * 10.f));
		Engine::FAILED_CHECK_RETURN(m_pObjectMgr->Add_GameObject(L"Layer_GameObject", L"IceStorm1", pGameObj), E_FAIL);
	}
}

void CEffectMgr::Effect_SwordEffect(_vec3 vecPos,_vec3 vecDir)
{
	
	vecPos.y += 100.f;
	pGameObj = Pop_Instance(CInstancePoolMgr::Get_Instance()->Get_Effect_SwordEffect());
	if (nullptr != pGameObj)
	{
		static_cast<CSwordEffect*>(pGameObj)->Set_CreateInfo(_vec3(0.f), _vec3(0.f, 0.f, 180.f), vecPos + vecDir * 5);
		Engine::FAILED_CHECK_RETURN(m_pObjectMgr->Add_GameObject(L"Layer_GameObject", L"SwordEffect", pGameObj), E_FAIL);
	}
}

void CEffectMgr::Effect_SwordEffect_s(_vec3 vecPos, _vec3 vecDir)
{
	vecPos.y = 0.f;
	_vec3 upVec = _vec3(0, 1, 0);
	_vec3 crossVec = upVec.Cross_InputV1(vecDir);

	for (int i = -6; i <= 6; i+=3)
	{
		_vec3 newPos = vecPos - crossVec * (1.5f) * i;
		newPos -= (vecDir * (18 - abs( i)));
		pGameObj = Pop_Instance(CInstancePoolMgr::Get_Instance()->Get_Effect_Sword_s_Effect());
		if (nullptr != pGameObj)
		{	
			static_cast<CSwordEffect_s*>(pGameObj)->Set_CreateInfo(_vec3(0.f), _vec3(0.f), newPos,vecDir);
			static_cast<CSwordEffect_s*>(pGameObj)->Set_isScaleAnim(true);
			Engine::FAILED_CHECK_RETURN(m_pObjectMgr->Add_GameObject(L"Layer_GameObject", L"publicSword", pGameObj), E_FAIL);
		}
		
	}
}

void CEffectMgr::Effect_Straight_IceStorm(_vec3 vecPos, _vec3 vecDir, const _bool& bIsCollider)
{
	
	Engine::CGameObject* pThisPlayer = m_pObjectMgr->Get_GameObject(L"Layer_GameObject", L"ThisPlayer");

	_matrix rotationY=XMMatrixRotationY(XMConvertToRadians(20.f));
	XMVECTOR temp= XMVector3TransformCoord(vecDir.Get_XMVECTOR(), rotationY);
	_vec3 DirectVec;
	XMStoreFloat3(&DirectVec, temp);
	DirectVec.Normalize();
	float fPlayerAngleY = m_pObjectMgr->Get_GameObject(L"Layer_GameObject", L"ThisPlayer")->Get_Transform()->m_vAngle.y;
	for (int i = 0; i < 13; i++)
	{
		_vec3 PosOffSet = vecDir * 2.f + (DirectVec *(1.f+ (float)i)*(1.f+powf((float)i/13.f,2.f)));
		_vec3 newAngle = _vec3(float(rand() % 20 - 10), fPlayerAngleY, float(rand() % 20 - 10));
		pGameObj = Pop_Instance(CInstancePoolMgr::Get_Instance()->Get_Effect_IceStorm_m_Effect());
		if (nullptr != pGameObj)
		{
			static_cast<CIceStorm_m*>(pGameObj)->Set_CreateInfo(_vec3(0.f), newAngle, vecPos + PosOffSet, 0.05f + (0.02f * i));
			Engine::FAILED_CHECK_RETURN(m_pObjectMgr->Add_GameObject(L"Layer_GameObject", L"publicSkill3", pGameObj), E_FAIL);
		}
		CEffectMgr::Get_Instance()->Effect_Particle(vecPos + PosOffSet, 5, L"Lighting0", _vec3(0.3f));

		if (bIsCollider)
		{
			CCollisionTick* pCollisionTick = static_cast<CCollisionTick*>(Pop_Instance(CInstancePoolMgr::Get_Instance()->Get_CollisionTickPool()));
			if (nullptr != pCollisionTick)
			{
				pCollisionTick->Get_BoundingSphere()->Get_BoundingInfo().Radius = 0.5f;
				pCollisionTick->Set_CollisionTag(L"CollisionTick_ThisPlayer");
				pCollisionTick->Set_Damage(pThisPlayer->Get_Info()->Get_RandomDamage());
				pCollisionTick->Set_LifeTime(0.4f);
				pCollisionTick->Get_Transform()->m_vScale = _vec3(2.5f);
				pCollisionTick->Get_Transform()->m_vPos = vecPos + PosOffSet;
				pCollisionTick->Get_BoundingSphere()->Set_Radius(pCollisionTick->Get_Transform()->m_vScale);
				m_pObjectMgr->Add_GameObject(L"Layer_GameObject", L"CollisionTick_ThisPlayer", pCollisionTick);
			}
		}
	}
}
void CEffectMgr::Effect_BossIceStorm(_vec3 vecPos, _vec3 vecDir, const _uint& uiSNum, const _uint& uiDamage)
{
	for (int i = 0; i < 13; i++)
	{
		_vec3 PosOffSet = vecDir * 2.f + (vecDir * (1.f + (float)i) * (1.f + powf((float)i / 13.f, 2.f)*2.f));
		pGameObj = Pop_Instance(CInstancePoolMgr::Get_Instance()->Get_Effect_IceStorm_m_Effect());
		if (nullptr != pGameObj)
		{
			static_cast<CIceStorm_m*>(pGameObj)->Set_CreateInfo(_vec3(0.f), _vec3(0.f,-180.f,0.f), vecPos + PosOffSet, 0.1f + (0.06f * i)
			,2.f);
			Engine::FAILED_CHECK_RETURN(m_pObjectMgr->Add_GameObject(L"Layer_GameObject", L"publicSkill3", pGameObj), E_FAIL);
		}

		// CollisionTick
		CCollisionTick* pCollisionTick = static_cast<CCollisionTick*>(Pop_Instance(CInstancePoolMgr::Get_Instance()->Get_CollisionTickPool()));
		if (nullptr != pCollisionTick)
		{
			pCollisionTick->Get_BoundingSphere()->Get_BoundingInfo().Radius = 0.5f;
			pCollisionTick->Set_CollisionTag(L"CollisionTick_Monster");
			pCollisionTick->Set_Damage(uiDamage);
			pCollisionTick->Set_LifeTime(0.3f);
			pCollisionTick->Get_Transform()->m_vScale = _vec3(9.0f);
			pCollisionTick->Get_Transform()->m_vPos   = vecPos + PosOffSet;
			pCollisionTick->Get_BoundingSphere()->Set_Radius(pCollisionTick->Get_Transform()->m_vScale);
			pCollisionTick->Set_ServerNumber(uiSNum);
			m_pObjectMgr->Add_GameObject(L"Layer_GameObject", L"CollisionTick_Monster", pCollisionTick);
		}
	}
}
void CEffectMgr::Effect_FireDecal(_vec3 vecPos)
{
	
	pGameObj = Pop_Instance(CInstancePoolMgr::Get_Instance()->Get_Effect_FireDecal_Effect());
	if (nullptr != pGameObj)
	{
		static_cast<CFireDecal*>(pGameObj)->Set_CreateInfo(_vec3(0.05f,0.f,0.05f), _vec3(0.f, 0.0f, 0.0f), InterPolated_YOffset(vecPos));
		Engine::FAILED_CHECK_RETURN(m_pObjectMgr->Add_GameObject(L"Layer_GameObject", L"FireDecal", pGameObj), E_FAIL);
	}
}
void CEffectMgr::Effect_IceDecal(_vec3 vecPos)
{
	pGameObj = Pop_Instance(CInstancePoolMgr::Get_Instance()->Get_Effect_IceDecal_Effect());
	if (nullptr != pGameObj)
	{
		static_cast<CIceDecal*>(pGameObj)->Set_CreateInfo(_vec3(0.01f),_vec3(0.f, 0.0f, 0.0f), InterPolated_YOffset(vecPos));
		Engine::FAILED_CHECK_RETURN(m_pObjectMgr->Add_GameObject(L"Layer_GameObject", L"IceDecal", pGameObj), E_FAIL);
	}
}

void CEffectMgr::Effect_ArrowHitted(_vec3 vecPos,_float maxScale)
{	
	Effect_TextureEffect(L"Lighting3", _vec3(0.f), _vec3(0.f), vecPos, FRAME(4, 4, 16.0f), true, true, maxScale,0,
		_vec4(0.f, 0.3f, 0.6f, 1.f));
	
}

void CEffectMgr::Effect_FireCone(_vec3 vecPos, float RotY , _vec3 vecDir)
{
	/*pGameObj = CDragonEffect::Create(m_pGraphicDevice, m_pCommandList, L"DragonEffect", _vec3(0.012f), _vec3(0.f), vecPos);
	Engine::FAILED_CHECK_RETURN(m_pObjectMgr->Add_GameObject(L"Layer_GameObject", L"EffectDragon", pGameObj), E_FAIL);*/

	vecPos.y = 0.5f;

	pGameObj = Pop_Instance(CInstancePoolMgr::Get_Instance()->Get_Effect_FireRing_Effect());
	if (nullptr != pGameObj)
	{
		static_cast<CFireRing*>(pGameObj)->Set_CreateInfo(_vec3(0.01f), _vec3(0.f, 0.0f, 0.0f), vecPos);
		Engine::FAILED_CHECK_RETURN(m_pObjectMgr->Add_GameObject(L"Layer_GameObject", L"HalfMoon0", pGameObj), E_FAIL);
	}

	for (int i = 0; i < 5; i++)
	{
		pGameObj = Pop_Instance(CInstancePoolMgr::Get_Instance()->Get_Effect_FrameMesh_Effect());
		if (nullptr != pGameObj)
		{
			static_cast<CFrameMesh*>(pGameObj)->Set_CreateInfo(_vec3(0.015f, 20.5f, 0.015f), _vec3(-0.f, RotY + 60.f * i, -0.f)
				, vecPos, FRAME(8, 8, 128));
			Engine::FAILED_CHECK_RETURN(m_pObjectMgr->Add_GameObject(L"Layer_GameObject", L"PublicCylinder02", pGameObj), E_FAIL);
		}
	}
}

void CEffectMgr::Effect_Test(_vec3 vecPos,_vec3* parent)
{
	/*pGameObj= CTrailTexEffect::Create(m_pGraphicDevice, m_pCommandList, L"Fire3", _vec3(4.2f), _vec3(0.f), vecPos, FRAME(8, 8, 64), 2, 40);
	static_cast<CTrailTexEffect*>(pGameObj)->SetTransform(parent);
	Engine::FAILED_CHECK_RETURN(m_pObjectMgr->Add_GameObject(L"Layer_GameObject", L"Trail", pGameObj), E_FAIL);*/

}

void CEffectMgr::Effect_GridShieldEffect(_vec3 vecPos,int type,Engine::CTransform* parentTransform)
{
	int Pipeidx = 0;
	_vec3 Texidx;
	vecPos.y = 1.f + float(rand()%10) * 0.01f;
	type == 0 ? Pipeidx = 2 : Pipeidx=10;
	type == 0 ? Texidx = _vec3(0,2,16) : Texidx=_vec3(0, 25, 16);

	pGameObj = Pop_Instance(CInstancePoolMgr::Get_Instance()->Get_Effect_GridShieldEffect());
	if (nullptr != pGameObj)
	{
		static_cast<CGridShieldEffect*>(pGameObj)->Set_CreateInfo(_vec3(0.f), _vec3(0.f), vecPos, (_int)Texidx.x, (_int)Texidx.y, (_int)Texidx.z,Pipeidx, true, true, parentTransform);
		Engine::FAILED_CHECK_RETURN(m_pObjectMgr->Add_GameObject(L"Layer_GameObject", L"PublicSphere00", pGameObj), E_FAIL);
	}

	if (type == 0)
		Effect_MagicCircle_Effect(_vec3(0.0f), _vec3(0.0f), vecPos, 18, 18, 2,true, true, parentTransform, true);
	else
		Effect_MagicCircle_Effect(_vec3(0.0f), _vec3(0.0f), vecPos, 19, 19, 2, true, true, parentTransform, true);
}

void CEffectMgr::Effect_Shield(_vec3 vecPos,Engine::CTransform* parentTransform)
{
	for (int i = 0; i < 5; i++)
	{
		vecPos.y = 1.f;
		pGameObj = Pop_Instance(CInstancePoolMgr::Get_Instance()->Get_Effect_ShieldEffect());
		if (nullptr != pGameObj)
		{
			static_cast<CEffectShield*>(pGameObj)->Set_CreateInfo(_vec3(0.0f), _vec3(0.0f), vecPos,
				5.5f,0.12f,3.f,(360.f / 5.f) * i, parentTransform);
			Engine::FAILED_CHECK_RETURN(m_pObjectMgr->Add_GameObject(L"Layer_GameObject", L"publicShield", pGameObj), E_FAIL);
		}
	}
	

	Effect_MagicCircle_Effect(_vec3(0.0f), _vec3(0.0f), vecPos, 20, 20, 2, true, true, parentTransform, true);
}

void CEffectMgr::Effect_Axe(_vec3 vecPos, Engine::CTransform* parentTransform)
{
	for (int i = 0; i < 5; i++)
	{
		vecPos.y = -1.f;
		pGameObj = Pop_Instance(CInstancePoolMgr::Get_Instance()->Get_Effect_AxeEffect());
		if (nullptr != pGameObj)
		{
			static_cast<CEffectAxe*>(pGameObj)-> Set_CreateInfo(_vec3(0.0f), _vec3(0.0f), vecPos,
				5.5f, 0.12f, 5.f, (360.f / 5.f) * i, parentTransform);
			Engine::FAILED_CHECK_RETURN(m_pObjectMgr->Add_GameObject(L"Layer_GameObject", L"publicAxe", pGameObj), E_FAIL);
		}
	}
	
	Effect_MagicCircle_Effect(_vec3(0.0f), _vec3(0.0f), vecPos, 21, 21, 2, true, true, parentTransform, true);
}
void CEffectMgr::Effect_TargetShield(_vec3 vecPos, Engine::CTransform* parentTransform)
{
	
	for (int i = 0; i < 3; i++)
	{
		vecPos.y = 1.f;
		pGameObj = Pop_Instance(CInstancePoolMgr::Get_Instance()->Get_Effect_ShieldEffect());
		if (nullptr != pGameObj)
		{
			static_cast<CEffectShield*>(pGameObj)->Set_CreateInfo(_vec3(0.0f), _vec3(0.0f), vecPos,
				60.f, 0.06f, 1.5f
				, (360.f / 3.f) * i, parentTransform,true);
			Engine::FAILED_CHECK_RETURN(m_pObjectMgr->Add_GameObject(L"Layer_GameObject", L"publicShield", pGameObj), E_FAIL);
		}
	}
}

void CEffectMgr::Effect_TargetAxe(_vec3 vecPos, Engine::CTransform* parentTransform)
{
	for (int i = 0; i < 3; i++)
	{
		vecPos.y = -1.f;
		pGameObj = Pop_Instance(CInstancePoolMgr::Get_Instance()->Get_Effect_AxeEffect());
		if (nullptr != pGameObj)
		{
			static_cast<CEffectAxe*>(pGameObj)->Set_CreateInfo(_vec3(0.0f), _vec3(0.0f), vecPos,
				60.f, 0.06f, 2.f
				, (360.f / 3.f) * i, parentTransform, true);
			Engine::FAILED_CHECK_RETURN(m_pObjectMgr->Add_GameObject(L"Layer_GameObject", L"publicAxe", pGameObj), E_FAIL);
		}
	}
}

void CEffectMgr::Effect_WarningGround(_vec3 vecPos, _float fLimitScale)
{
	vecPos.y = 0.4f;
	pGameObj = Pop_Instance(CInstancePoolMgr::Get_Instance()->Get_Effect_WarningGround_Effect());
	if (nullptr != pGameObj)
	{
		static_cast<CWarningGround*>(pGameObj)->Set_CreateInfo(_vec3(0.f),_vec3(0.f), vecPos,true,0.03f);
		static_cast<CWarningGround*>(pGameObj)->Set_TexIDX(0, 3, 2);
		Engine::FAILED_CHECK_RETURN(m_pObjectMgr->Add_GameObject(L"Layer_GameObject", L"WarningGround", pGameObj), E_FAIL);
	}
}

void CEffectMgr::Effect_RectDecal(_vec3 vecPos, float RotY)
{
	pGameObj = Pop_Instance(CInstancePoolMgr::Get_Instance()->Get_Effect_RectDecal_Effect());
	if (nullptr != pGameObj)
	{
		static_cast<CRectDecal*>(pGameObj)->Set_CreateInfo(_vec3(15.f,0.00f,2.5f), _vec3(0.f), InterPolated_YOffset(vecPos));
		Engine::FAILED_CHECK_RETURN(m_pObjectMgr->Add_GameObject(L"Layer_GameObject", L"RectDecal", pGameObj), E_FAIL);
	}
}

void CEffectMgr::Effect_DistTrail(_vec3 vecPos, _vec3 Angle,bool isCrossFilter,float SizeOffSet)
{
	pGameObj = Pop_Instance(CInstancePoolMgr::Get_Instance()->Get_Effect_DistTrail_Effect());
	if (nullptr != pGameObj)
	{
		static_cast<CDistTrail*>(pGameObj)->Set_CreateInfo(_vec3(0.01f), Angle, vecPos);
		Engine::FAILED_CHECK_RETURN(m_pObjectMgr->Add_GameObject(L"Layer_GameObject", L"DistTrail", pGameObj), E_FAIL);
		static_cast<CDistTrail*>(pGameObj)->Set_IsCrossFilter(isCrossFilter);
		static_cast<CDistTrail*>(pGameObj)->Set_SizeOffset(SizeOffSet);
		
	}
}

void CEffectMgr::Effect_Particle(_vec3 vecPos, _int Cnt, wstring Tag,_vec3 vecScale,FRAME Frame)
{
	//Snow Lighting1 Lighting0
	Engine::CGameObject *particleobj = Pop_Instance(CInstancePoolMgr::Get_Instance()->Get_Effect_Particle_Effect());
	if (nullptr != particleobj)
	{
		static_cast<CParticleEffect*>(particleobj)->Set_CreateInfo(vecScale, _vec3(0.f), vecPos,Tag, Frame
		,9,Cnt);
		Engine::FAILED_CHECK_RETURN(m_pObjectMgr->Add_GameObject(L"Layer_GameObject",Tag, particleobj), E_FAIL);
	}
}

void CEffectMgr::Effect_MeshParticle(wstring wstrMeshTag,
	const _vec3& vScale,
	const _vec3& vAngle,
	const _vec3& vPos , _bool isTextrail , _bool isParticleTrail ,
	const _int& PipeLine , const _int& ParticleCnt
	, _uint Diff, _uint Norm, _uint Spec, _vec2 SpeedWeight,float YOffset, _bool isPingPong)
{
	Engine::CGameObject* particleobj=nullptr;
	
	if (wstrMeshTag == L"publicStone0")
		particleobj= Pop_Instance(CInstancePoolMgr::Get_Instance()->Get_Effect_MeshParticleRock0_Effect());
	else if (wstrMeshTag == L"publicStone1")
		particleobj = Pop_Instance(CInstancePoolMgr::Get_Instance()->Get_Effect_MeshParticleRock1_Effect());
	else if (wstrMeshTag == L"publicStone2")
		particleobj = Pop_Instance(CInstancePoolMgr::Get_Instance()->Get_Effect_MeshParticleRock2_Effect());
	else if (wstrMeshTag == L"publicStone3")
		particleobj = Pop_Instance(CInstancePoolMgr::Get_Instance()->Get_Effect_MeshParticleRock3_Effect());
	else if (wstrMeshTag == L"publicStone4")
		particleobj = Pop_Instance(CInstancePoolMgr::Get_Instance()->Get_Effect_MeshParticleRock4_Effect());
	

	if (nullptr != particleobj)
	{
		static_cast<CMeshParticleEffect*>(particleobj)->Set_CreateInfo(vScale, vAngle, vPos, SpeedWeight,isTextrail
			,isParticleTrail,PipeLine,
			ParticleCnt, YOffset, isPingPong);
		static_cast<CMeshParticleEffect*>(particleobj)->Set_TexInfo(Diff, Norm, Spec);
		Engine::FAILED_CHECK_RETURN(m_pObjectMgr->Add_GameObject(L"Layer_GameObject", wstrMeshTag, particleobj), E_FAIL);
	}

}

void CEffectMgr::Effect_DirParticle(const _vec3& vScale, const _vec3& vAngle, const _vec3& vPos, wstring TexTag,
	const _vec3& StartPos, const _vec3& DstPos, const FRAME& tFrame, const _int& PipeLine, const _int& ParticleCnt)
{
	Engine::CGameObject* particleobj = Pop_Instance(CInstancePoolMgr::Get_Instance()->Get_Effect_DirParticle_Effect());
	if (nullptr != particleobj)
	{
		static_cast<CDirParticleEffect*>(particleobj)->Set_CreateInfo(vScale,vAngle, vPos, TexTag,StartPos,DstPos,tFrame
			, PipeLine, ParticleCnt);
		Engine::FAILED_CHECK_RETURN(m_pObjectMgr->Add_GameObject(L"Layer_GameObject", TexTag, particleobj), E_FAIL);
	}
}

void CEffectMgr::Effect_TextureEffect(wstring TexTag, _vec3 Scale, _vec3 Angle, _vec3 Pos, FRAME frame, bool isLoop,
	bool isScaleAnim,float maxScale, _int ScaleAnimIdx, _vec4 colorOffset
 , bool isFollowHand , Engine::HIERARCHY_DESC* hierachy, Engine::CTransform* parentTransform)
{
	Engine::CGameObject* textureObj = Pop_Instance(CInstancePoolMgr::Get_Instance()->Get_Effect_TextureEffect());
	if (nullptr != textureObj)
	{
		static_cast<CTextureEffect*>(textureObj)->Set_CreateInfo(TexTag,Scale,Angle,Pos,frame,isLoop,isScaleAnim, maxScale, ScaleAnimIdx,
			colorOffset, isFollowHand, hierachy, parentTransform);
		Engine::FAILED_CHECK_RETURN(m_pObjectMgr->Add_GameObject(L"Layer_GameObject", TexTag, textureObj), E_FAIL);

	}
}

void CEffectMgr::Effect_MagicCircle_Effect(const _vec3& vScale, const _vec3& vAngle, const _vec3& vPos, _int Diff, _int Norm, _int Spec
	, bool bisRotate, bool bisScaleAnim, const Engine::CTransform* ParentTransform, bool bisFollowPlayer
, float LimitScale, float LimitLifeTime)
{
	Engine::CGameObject* MagicCircleObj = Pop_Instance(CInstancePoolMgr::Get_Instance()->Get_Effect_MagicCircleEffect());
	if (nullptr != MagicCircleObj)
	{
		static_cast<CMagicCircle*>(MagicCircleObj)->Set_CreateInfo(vScale, vAngle, InterPolated_YOffset(vPos), bisRotate, bisScaleAnim, ParentTransform,
			bisFollowPlayer);
		static_cast<CMagicCircle*>(MagicCircleObj)->Set_TexIDX(Diff, Norm, Spec);
		static_cast<CMagicCircle*>(MagicCircleObj)->Set_LimitInfo(LimitScale, LimitLifeTime);
		Engine::FAILED_CHECK_RETURN(m_pObjectMgr->Add_GameObject(L"Layer_GameObject",L"MagicCircle", MagicCircleObj), E_FAIL);
	}
}

_vec3 CEffectMgr::InterPolated_YOffset(_vec3 Pos)
{
	_vec3 ReturnVector=Pos;
	m_fY += 0.05f;
	if (m_fY > 0.31f)
		m_fY = 0.05f;
	ReturnVector.y = m_fY;
	return ReturnVector;
}



void CEffectMgr::Free(void)
{
}
