#include "stdafx.h"
#include "PartySystemMgr.h"
#include "ObjectMgr.h"

IMPLEMENT_SINGLETON(CPartySystemMgr)

CPartySystemMgr::CPartySystemMgr()
{
}

void CPartySystemMgr::SetUp_ThisPlayerPartyList()
{
	// 파티가입 or 파티탈퇴 UI 활성화 재설정.
	Engine::CGameObject* pThisPlayer = Engine::CObjectMgr::Get_Instance()->Get_GameObject(L"Layer_GameObject", L"ThisPlayer");
	if (nullptr != pThisPlayer)
	{
		for (auto& pPartyInfoCanvas : m_vecPartyInfoListCanvas)
		{
			pPartyInfoCanvas->Set_IsActive(false);
			pPartyInfoCanvas->Set_IsChildActive(false);
		}

		map<_int, Engine::PARTYMEMBER> mapPartyList = pThisPlayer->Get_PartyList();
		if (m_vecPartyInfoListCanvas.size() >= mapPartyList.size())
		{
			for (_int i = 0; i < mapPartyList.size(); ++i)
			{
				m_vecPartyInfoListCanvas[i]->Set_IsActive(true);
				m_vecPartyInfoListCanvas[i]->Set_IsChildActive(true);
			}
		}
	}
}

void CPartySystemMgr::Reset_UIPartyList()
{
	Engine::CGameObject* pThisPlayer = Engine::CObjectMgr::Get_Instance()->Get_GameObject(L"Layer_GameObject", L"ThisPlayer");
	if (nullptr != pThisPlayer)
	{
		pThisPlayer->Clear_PartyMember();

		for (auto& pPartyInfoCanvas : m_vecPartyInfoListCanvas)
		{
			pPartyInfoCanvas->Set_IsActive(false);
			pPartyInfoCanvas->Set_IsChildActive(false);
		}

		map<_int, Engine::PARTYMEMBER> mapPartyList = pThisPlayer->Get_PartyList();
		if (m_vecPartyInfoListCanvas.size() >= mapPartyList.size())
		{
			for (_int i = 0; i < mapPartyList.size(); ++i)
			{
				m_vecPartyInfoListCanvas[i]->Set_IsActive(true);
				m_vecPartyInfoListCanvas[i]->Set_IsChildActive(true);
			}
		}
	}
}

void CPartySystemMgr::Update_ThisPlayerPartyList()
{
	Engine::CGameObject* pThisPlayer = Engine::CObjectMgr::Get_Instance()->Get_GameObject(L"Layer_GameObject", L"ThisPlayer");
	if (nullptr != pThisPlayer)
	{
		map<_int, Engine::PARTYMEMBER> mapPartyList = pThisPlayer->Get_PartyList();

		if (m_vecPartyInfoListCanvas.size() >= mapPartyList.size())
		{
			_int i = 0;
			for (auto& patyInfo : mapPartyList)
			{
				_float fHpPercent = (_float)patyInfo.second.iHp / (_float)patyInfo.second.iMaxHp;
				_float fMpPercent = (_float)patyInfo.second.iMp / (_float)patyInfo.second.iMaxMp;

				m_vecPartyInfoListCanvas[i]->Set_UserName(patyInfo.second.cName);
				m_vecPartyInfoListCanvas[i]->Get_PartyUIClassInfoClass()->Set_CurrentClassTag(patyInfo.second.cJob);
				m_vecPartyInfoListCanvas[i]->Get_PartyUIHpGaugeClass()->Set_Percent(fHpPercent);
				m_vecPartyInfoListCanvas[i]->Get_PartyUIMpGaugeClass()->Set_Percent(fMpPercent);

				++i;
			}
		}
	}
}

void CPartySystemMgr::Free()
{
	m_pSelectPlayer               = nullptr;
	m_pPartySuggestCanvas         = nullptr;
	m_pPartySuggestResponseCanvas = nullptr;
	m_pPartyLeaveCanvas           = nullptr;
	m_pPartySystemMessageCanvas   = nullptr;

	m_vecPartyInfoListCanvas.clear();
	m_vecPartyInfoListCanvas.shrink_to_fit();
}
