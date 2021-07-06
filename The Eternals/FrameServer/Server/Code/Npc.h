#pragma once
#include "Obj.h"
class CNpc :
    public CObj
{
public:
	CNpc();
	virtual ~CNpc();

public:
	void	Set_AnimDuration(double arr[]);
	void	Set_NumAnimation(const _uint& num) { m_uiNumAniIndex = num; }
	void	Set_AnimationKey(const _uint& uiAniKey);

	void	Set_Start_Move(chrono::seconds t = 7s);
	void	Set_Stop_Move();

public:
	void	Ready_NPC(const _vec3& pos, const _vec3& angle, const char& type, const char& num, const char& naviType);
	int		Update_NPC(const float& fTimeDelta);

	void	active_npc();			// 해당 NPC의 STATUS = ST_ACTIVE
	void	nonActive_npc();		// 해당 NPC의 STATUS = ST_NONACTIVE
	void	wakeUp_npc();

private:
	void	Change_Animation(const float& fTimeDelta);

	/* MOVE NPC - Walker (Aman_Boy, Cat, Chicken) */
	void	Change_Walker_Animation(const float& fTimeDelta);
	void	Move_Walker_NPC(const float& fTimeDelta);

	/* MOVE NPC - Children (Highelf_girl, Human_boy, Human_girl) */
	void	Change_Children_Animation(const float& fTimeDelta);

	/* NON MOVE NPC - Assistant (Popori_boy) */
	void	Change_Assistant_Animation(const float& fTimeDelta);
	void	Idle_Assistant_NPC(const float& fTimeDelta, int animIdx);

	/* MERCHANT NPC - Merchant (Villagers, Baraka_M_Extractor) */
	void	Change_Merchant_Animation(const float& fTimeDelta);
	void	Greet_Merchant_Animation(const float& fTimeDelta);

	/* QUEST NPC - QuestNPC (Castanic_M_Lsmith) */
	void	Change_QuestNPC_Animation(const float& fTimeDelta);
	void	Greet_QuestNPC_Animation(const float& fTimeDelta);

	/* NON MOVE NPC - Stander (Villagers, Baraka_M_Extractor) */
	//void	Change_Stander_Animation(const float& fTimeDelta);

private:
	void	Play_Animation(float fTimeDelta);
	bool	Is_AnimationSetEnd(const float& fTimeDelta);

public:
	void	send_NPC_enter_packet(const int& to_client);				// NPC 등장 패킷
	void	send_NPC_move_packet(const int& to_client, const int& ani);		// NPC 움직임 패킷
	void	send_NPC_animation_packet(const int& to_client, const int& ani);

	virtual DWORD Release();

public:
	char			m_npcNum;
	volatile bool	m_bIsMove				= false;
	bool			m_bIsDirSelect			= false;
	float			m_fSpd					= 0.f;

	/* Animation */
	_uint			m_uiNewAniIndex			= 0;
	_uint			m_uiCurAniIndex			= 0;
	_uint			m_uiNumAniIndex			= 0;
	_uint			m_ui3DMax_NumFrame		= 0;
	_uint			m_ui3DMax_CurFrame		= 0;

	float			m_fAnimationTime		= 0.f;
	float			m_fBlendAnimationTime	= 0.f;
	float			m_fBlendingTime			= 0.f;

	double			m_arrDuration[MAX_ANI]	= { 0 };

	_uint			m_uiAnimIdx = 0;   // Apply Animation Index
};

