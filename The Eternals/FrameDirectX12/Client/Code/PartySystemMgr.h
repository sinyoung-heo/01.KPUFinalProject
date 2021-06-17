#pragma once
#include "Include.h"
#include "Base.h"
#include "PartySuggestCanvas.h"
#include "PartySuggestResponseCanvas.h"

namespace Engine
{
	class CGameObject;
}

class CPartySystemMgr : public Engine::CBase
{
	DECLARE_SINGLETON(CPartySystemMgr)

private:
	explicit CPartySystemMgr();
	virtual ~CPartySystemMgr() = default;
public:
	Engine::CGameObject*			Get_SelectPlayer()					{ return m_pSelectPlayer; }
	CPartySuggestCanvas*			Get_PartySuggestCanvas()			{ return m_pPartySuggestCanvas; }
	CPartySuggestResponseCanvas*	Get_PartySuggestResponseCanvas()	{ return m_pPartySuggestResponseCanvas; }
	CGameUIRoot*					Get_PartyLeaveCanvas()				{ return m_pPartyLeaveCanvas; }

	void Set_SelectPlayer(Engine::CGameObject* pPlayer)							{ m_pSelectPlayer = pPlayer; };
	void Set_PartySuggestCanvas(CPartySuggestCanvas* pCanvas)					{ m_pPartySuggestCanvas = pCanvas; }
	void Set_PartySuggestResponseCanvas(CPartySuggestResponseCanvas* pCanvas)	{ m_pPartySuggestResponseCanvas = pCanvas; }
	void Set_PartyLeaveCanvas(CGameUIRoot* pCanvas)								{ m_pPartyLeaveCanvas = pCanvas; }
private:
	Engine::CGameObject*			m_pSelectPlayer               = nullptr;
	CPartySuggestCanvas*			m_pPartySuggestCanvas         = nullptr;
	CPartySuggestResponseCanvas*	m_pPartySuggestResponseCanvas = nullptr;
	CGameUIRoot*					m_pPartyLeaveCanvas           = nullptr;

private:
	virtual void Free();
};

