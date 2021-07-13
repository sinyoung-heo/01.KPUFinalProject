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
	Engine::CGameObject* Get_CenterLakan()			{ return m_pCenterLakan; }
	Engine::CGameObject* Get_PrionBerserkerBoss()	{ return m_pPrionBerserkerBoss; }
	Engine::CGameObject* Get_CinemaVergos()			{ return m_pVergos; }
	void Set_IsUpdateVergos(const _bool& bIsUpdate);

	HRESULT Ready_CinemaCharacter();
	void	Update_Animation(const _float& fTimeDelta);
public:
	void	Spawn_Vergos();
	void	Scream_PrionBerserkerBoss();
	void	Scream_PrionBerserkers();
	void	Command_PrionBerserkerBoss();
	void	Rush_Prion();
	void	Rush_Lakan();
	void	Reset_LakanPosition();
	void	Reset_Vergos();
	void	Reset_PrionBerserkerPosition();
	void	Set_IsMoveStopPrionBerserker(const _bool& bIsMoveStop);
private:
	vector<Engine::CGameObject*> m_vecAnimationLakan;
	vector<Engine::CGameObject*> m_vecAnimationPrionBerserker;
	_float m_fLakanAnimationSpeed          = 1.0f;
	_float m_fPrionBerserkerAnimationSpeed = 1.0f;

	vector<Engine::CGameObject*> m_vecLakan;
	vector<Engine::CGameObject*> m_vecPrionBerserker;

	Engine::CObjectMgr*		m_pObjectMgr          = nullptr;
	Engine::CGameObject*	m_pVergos             = nullptr;
	Engine::CGameObject*	m_pCenterLakan        = nullptr;
	Engine::CGameObject*	m_pPrionBerserkerBoss = nullptr;
	Engine::CGameObject*	arrLakan[LINEUP_Z][LINEUP_X];
	Engine::CGameObject*	arrPrionBerserker[LINEUP_Z][LINEUP_X];
private:
	virtual void Free();

};

