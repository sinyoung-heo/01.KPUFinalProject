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
	void Effect_IceStorm(_vec3 vecPos,int Cnt=36,float Radius=5.f);
	void Effect_SwordEffect(_vec3 vecPos,_vec3 vecDir);
	void Effect_FireDecal(_vec3 vecPos);
	void Effect_IceDecal(_vec3 vecPos);


public:
public:
	
	ID3D12Device* m_pGraphicDevice = nullptr;
	ID3D12GraphicsCommandList* m_pCommandList = nullptr;
	Engine::CObjectMgr* m_pObjectMgr = nullptr;
	
	// CBase을(를) 통해 상속됨
	virtual void Free(void) override;

};