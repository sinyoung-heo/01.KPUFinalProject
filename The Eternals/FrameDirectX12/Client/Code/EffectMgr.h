#pragma once
#include "Include.h"
#include "Base.h"

class CObjectMgr;
class CEffectMgr : public Engine::CBase
{
	DECLARE_SINGLETON(CEffectMgr)

private:
	explicit CEffectMgr();
	virtual ~CEffectMgr() = default;

public:
	void Effect_Dust(_vec3 vecPos, float Radius=2.f);
	void Effect_IceStorm(_vec3 vecPos,int Cnt=36,float Radius=5.f);
	void Effect_IceStorm_s(_vec3 vecPos,float Radius);
	void Effect_SwordEffect(_vec3 vecPos,_vec3 vecDir);
	void Effect_SwordEffect_s(_vec3 vecPos, _vec3 vecDir);
	void Effect_Straight_IceStorm(_vec3 vecPos, _vec3 vecDir);
	void Effect_FireDecal(_vec3 vecPos);
	void Effect_IceDecal(_vec3 vecPos);
	void Effect_ArrowHitted(_vec3 vecPos);
	void Effect_FireCone(_vec3 vecPos, float RotY, _vec3 vecDir=_vec3(0.f));
	void Effect_Test(_vec3 vecPos);
	void Effect_GridShieldEffect(_vec3 vecPos, int type=0, Engine::CTransform* parentTransform=nullptr);
	void Effect_Shield(_vec3 vecPos, Engine::CTransform* parentTransform);
	void Effect_Axe(_vec3 vecPos, Engine::CTransform* parentTransform);
	void Effect_TargetShield(_vec3 vecPos, Engine::CTransform* parentTransform);
	void Effect_TargetAxe(_vec3 vecPos, Engine::CTransform* parentTransform);


	void Effect_DistTrail(_vec3 vecPos,_vec3 Angle, bool isCrossFilter, float SizeOffSet);
	void Effect_Particle(_vec3 vecPos,_int Cnt=20,wstring Tag=L"Snow", _vec3 vecScale=_vec3(0.1f), FRAME Frame=FRAME(1,1,1));
	
	void Effect_TextureEffect(wstring TexTag,_vec3 Scale,_vec3 Angle,_vec3 Pos,FRAME frame
		, bool isLoop = false, bool isScaleAnim = false, _vec4 colorOffset = _vec4(0.f, 0.f, 0.f, 1.f)
	,bool isFollowHand=false, Engine::HIERARCHY_DESC* hierachy=nullptr,Engine::CTransform* parentTransform=nullptr);

	void Effect_MagicCircle_Effect(const _vec3& vScale, const _vec3& vAngle, const _vec3& vPos,_int Diff,_int Norm,_int Spec,
		bool bisRotate = false, bool bisScaleAnim = false
		, const Engine::CTransform* ParentTransform = nullptr, bool bisFollowPlayer = false);
public:
public:
	Engine::CGameObject* pGameObj = nullptr;
	ID3D12Device* m_pGraphicDevice = nullptr;
	ID3D12GraphicsCommandList* m_pCommandList = nullptr;
	Engine::CObjectMgr* m_pObjectMgr = nullptr;
	
	// CBase을(를) 통해 상속됨
	virtual void Free(void) override;

};