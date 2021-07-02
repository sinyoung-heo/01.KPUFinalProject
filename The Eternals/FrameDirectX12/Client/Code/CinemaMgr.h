#pragma once
#include "Include.h"
#include "Base.h"

class CCinemaMgr : public Engine::CBase
{
	DECLARE_SINGLETON(CCinemaMgr)

private:
	explicit CCinemaMgr();
	virtual ~CCinemaMgr() = default;

public:
	HRESULT Ready_CinemaCharacter();

	void	Scream_PrionBerserkerBoss();
	void	Scream_PrionBerserkers();
	void	Command_PrionBerserkerBoss();
	void	Rush_Prion();
	void	Rush_Lakan();

private:
	Engine::CObjectMgr* m_pObjectMgr = nullptr;
	Engine::CGameObject* arrLakan[5][10];
	Engine::CGameObject* arrPrionBerserker[5][10];

private:
	virtual void Free();

};

