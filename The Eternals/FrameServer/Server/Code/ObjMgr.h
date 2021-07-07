#pragma once
#include "Obj.h"

typedef map <int,CObj*>					OBJLIST;
typedef OBJLIST::iterator				OBJITER;
typedef lock_guard<recursive_mutex>		objmgr_lock;

typedef unordered_set<int>				PARTYLIST;

class CObjMgr
{
	DECLARE_SINGLETON(CObjMgr)

private:
	explicit	CObjMgr(void);
	virtual		~CObjMgr(void);

public:
	HRESULT		Init_ObjMgr();

public:
	CObj*		Get_GameObject(wstring wstrObjTag, int server_num = 0);
	OBJLIST*	Get_OBJLIST(wstring wstrObjTag);
	PARTYLIST*	Get_PARTYLIST(const int& party_num);

	void		Create_StageVelikaNPC();
	void		Create_StageBeachMonster();
	void		Create_StageWinterMonster();
	void		Create_AiPlayer();

public:
	bool		Is_Player(int server_num = 0);
	bool		Is_NPC(int server_num = NPC_NUM_START);
	bool		Is_Monster(int server_num = MON_NUM_START);
	bool		Is_AI(int server_num = AI_NUM_START);

	bool		Is_Near(const CObj* me, const CObj* other);
	bool		Is_Monster_Target(const CObj* me, const CObj* other);
	bool		Is_Monster_AttackTarget(const CObj* me, const CObj* other, const int& range);

public:
	HRESULT		Add_GameObject(wstring wstrObjTag, CObj* pObj, int server_num);
	HRESULT		Delete_GameObject(wstring wstrObjTag, CObj* pObj);
	HRESULT		Delete_OBJLIST(wstring wstrObjTag);

	HRESULT		Create_Party(int* iPartyNumber, const int& server_num);
	HRESULT		Add_PartyMember(const int& iPartyNumber, int* responderPartyNum, const int& server_num);
	HRESULT		Leave_Party(int* iPartyNumber, const int& server_num);
	void		Print_PartyInfo(const int& iPartyNumber);

public:
	void		Release();

private:
	map<wstring, OBJLIST>			m_mapObjList;
	map<int, unordered_set<int>>	m_mapPartyList; // 파티번호 + 구성원 서버넘버
	recursive_mutex					m_mutex;
};

