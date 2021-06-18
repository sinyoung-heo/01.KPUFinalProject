#pragma once
#include "Include.h"
#include "Base.h"
#include "PartySuggestCanvas.h"
#include "PartySuggestResponseCanvas.h"
#include "PartySystemMessageCanvas.h"
#include "PartyInfoListCanvas.h"

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
	CPartySystemMessageCanvas*		Get_PartySystemMessageCanvas()		{ return m_pPartySystemMessageCanvas; }

	void Set_SelectPlayer(Engine::CGameObject* pPlayer)							{ m_pSelectPlayer = pPlayer; };
	void Set_PartySuggestCanvas(CPartySuggestCanvas* pCanvas)					{ m_pPartySuggestCanvas = pCanvas; }
	void Set_PartySuggestResponseCanvas(CPartySuggestResponseCanvas* pCanvas)	{ m_pPartySuggestResponseCanvas = pCanvas; }
	void Set_PartyLeaveCanvas(CGameUIRoot* pCanvas)								{ m_pPartyLeaveCanvas = pCanvas; }
	void Set_PartySystemMessageCanvas(CPartySystemMessageCanvas* pCanvas)		{ m_pPartySystemMessageCanvas = pCanvas; }

	void Add_PartyListInfoCanvas(CPartyInfoListCanvas* pCanvas)					{ m_vecPartyInfoListCanvas.emplace_back(pCanvas); }
	void SetUp_ThisPlayerPartyList();
	void Update_ThisPlayerPartyList();
private:
	Engine::CGameObject*			m_pSelectPlayer               = nullptr;
	CPartySuggestCanvas*			m_pPartySuggestCanvas         = nullptr;
	CPartySuggestResponseCanvas*	m_pPartySuggestResponseCanvas = nullptr;
	CGameUIRoot*					m_pPartyLeaveCanvas           = nullptr;
	CPartySystemMessageCanvas*		m_pPartySystemMessageCanvas   = nullptr;

	vector<CPartyInfoListCanvas*>	m_vecPartyInfoListCanvas;
private:
	virtual void Free();
};

