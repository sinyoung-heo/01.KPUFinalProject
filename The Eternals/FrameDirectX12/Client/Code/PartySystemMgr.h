#pragma once
#include "Include.h"
#include "Base.h"

namespace Engine
{
	class CGameObject;
}

class CPartySuggestCanvas;
class CPartySuggestResponseCanvas;

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

	void Set_SelectPlayer(Engine::CGameObject* pPlayer)							{ m_pSelectPlayer = pPlayer; };
	void Set_PartySuggestCanvas(CPartySuggestCanvas* pCanvas)					{ m_pPartySuggestCanvas = pCanvas; }
	void Set_PartySuggestResponseCanvas(CPartySuggestResponseCanvas* pCanvas)	{ m_pPartySuggestResponseCanvas = pCanvas; }
private:
	Engine::CGameObject*			m_pSelectPlayer               = nullptr;
	CPartySuggestCanvas*			m_pPartySuggestCanvas         = nullptr;
	CPartySuggestResponseCanvas*	m_pPartySuggestResponseCanvas = nullptr;

private:
	virtual void Free();
};

