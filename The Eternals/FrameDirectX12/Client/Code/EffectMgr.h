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
	void Effect_SwordEffect(_vec3 vecPos,_vec3 vecDir);
	void Effect_SwordEffect_s(_vec3 vecPos, _vec3 vecDir);
	void Effect_Straight_IceStorm(_vec3 vecPos, _vec3 vecDir);
	void Effect_FireDecal(_vec3 vecPos);
	void Effect_IceDecal(_vec3 vecPos);
	void Effect_ArrowHitted(_vec3 vecPos);
	void Effect_FireCone(_vec3 vecPos, float RotY, _vec3 vecDir=_vec3(0.f));
	void Effect_Test(_vec3 vecPos);
	void Effect_GridShieldEffect(_vec3 vecPos, int type=0);
	void Effect_Shield(_vec3 vecPos);

	void Effect_Particle(_vec3 vecPos,_int Cnt=20,wstring Tag=L"Snow", _vec3 vecScale=_vec3(0.1f), FRAME Frame=FRAME(1,1,1));
	
public:
public:
	Engine::CGameObject* pGameObj = nullptr;
	ID3D12Device* m_pGraphicDevice = nullptr;
	ID3D12GraphicsCommandList* m_pCommandList = nullptr;
	Engine::CObjectMgr* m_pObjectMgr = nullptr;
	
	// CBase��(��) ���� ��ӵ�
	virtual void Free(void) override;

};