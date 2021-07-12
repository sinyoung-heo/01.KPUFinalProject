#pragma once
#include "Include.h"
#include "Base.h"

constexpr _uint LINEUP_Z = 5;
constexpr _uint LINEUP_X = 10;



class CCinemaMgr : public Engine::CBase
{
	DECLARE_SINGLETON(CCinemaMgr)

private:
	explicit CCinemaMgr();
	virtual ~CCinemaMgr() = default;

public:
	HRESULT Ready_CinemaCharacter();
	void	Update_Animation(const _float& fTimeDelta);
public:
	void	Spawn_Vergos();
	void	Scream_PrionBerserkerBoss();
	void	Scream_PrionBerserkers();
	void	Command_PrionBerserkerBoss();
	void	Rush_Prion();
	void	Rush_Lakan();
private:
	vector<Engine::CGameObject*> m_vecAnimationLakan;
	vector<Engine::CGameObject*> m_vecAnimationPrionBerserker;
	_float m_fLakanAnimationSpeed          = 1.0f;
	_float m_fPrionBerserkerAnimationSpeed = 1.0f;

	vector<Engine::CGameObject*> m_vecLakan;
	vector<Engine::CGameObject*> m_vecPrionBerserker;

	Engine::CObjectMgr*		m_pObjectMgr = nullptr;
	Engine::CGameObject*	arrLakan[LINEUP_Z][LINEUP_X];
	Engine::CGameObject*	arrPrionBerserker[LINEUP_Z][LINEUP_X];
private:
	virtual void Free();

};

