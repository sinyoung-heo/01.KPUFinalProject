#pragma once
#include "Include.h"
#include "Base.h"

//#define ERR_CHECK

namespace Engine
{
	class CObjectMgr;
	class CManagement;
	class CRenderer;
}

class CInstancePoolMgr;
class CPartySystemMgr;

class CPacketMgr : public Engine::CBase
{
	DECLARE_SINGLETON(CPacketMgr)

private:
	explicit CPacketMgr();
	virtual ~CPacketMgr() = default;

public:
	HRESULT Ready_Server(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList);
	HRESULT Connect_Server();

public:
	void	recv_packet();

	// Server Data Receive - Version Ring Buffer
	void	Process_recv_reassembly(size_t iosize);
	void	Process_packet();

	void Enter_PartyMember(sc_packet_update_party_new_member* packet, bool& retflag);

	void Suggest_Party(sc_packet_suggest_party* packet);

	void	Change_Animation(sc_packet_animationIndex* packet, bool& retflag);

public:
	void	send_login();
	void	send_move(const _vec3& vDir, const _vec3& vPos, const _int& iAniIdx);
	void	send_move_stop(const _vec3& vPos, const _vec3& vDir, const _int& iAniIdx);
	void	send_stance_change(const _int& iAniIdx, const _bool& bIsStanceAttack);
	void	send_attack(const _int& iAniIdx, const _vec3& vDir, const _vec3& vPos, const _float& fEndAngleY);
	void    send_attack_stop(const _int& iAniIdx, const _vec3& vDir, const _vec3& vPos);
	void    send_attackByMonster(int objID, const _int& iDamage);
	void    send_attackToMonster(int objID, const _int& iDamage, const char& affect = AFFECT_FINCH);
	void	send_stage_change(const char& chStageId);
	void	send_suggest_party(const int& others_id);							// 파티 초대
	void	send_respond_party(const bool& result, const int& suggester_id);	// 파티 초대에 대한 응답
	void	send_join_party(const int& others_id);								// 파티 참여 신청
	void	send_decide_party(const bool& result, const int& joinner_id);		// 파티 참여 신청에 대한 응답
	void	send_logout();

public:
	bool	change_MoveKey(MVKEY eKey);

	void	Login_Player(sc_packet_login_ok* packet);
	void	Enter_Others(sc_packet_enter* packet, int& retflag);
	void	Move_User(sc_packet_move* packet);
	void	MoveStop_User(sc_packet_move* packet);
	void	Attack_User(sc_packet_attack* packet);
	void	AttackStop_User(sc_packet_attack* packet);
	void	ChangeStat_User(sc_packet_stat_change* packet);
	void	Change_Stance_Others(sc_packet_stance_change* packet, int& retflag);
	void	Leave_Object(sc_packet_leave* packet, int& retflag);

	void	Enter_Monster(sc_packet_monster_enter* packet);
	void	Move_Monster(sc_packet_move* packet);
	void	Attack_Monster(sc_packet_monster_attack* packet);
	void	Rush_Monster(sc_packet_monster_rushAttack* packet);
	void	Change_Monster_Stat(sc_packet_stat_change* packet);
	void	Knockback_Monster(sc_packet_monster_knockback* packet, bool& retflag);
	void	Dead_Monster(sc_packet_animationIndex* packet);

	void	Enter_NPC(sc_packet_npc_enter* packet);
	void	Move_NPC(sc_packet_move* packet);

	void	Stage_Change(sc_packet_stage_change* packet);

private:
	void	send_packet(void* packet);

private:
	virtual void Free();

private:
	/*__________________________________________________________________________________________________________
	[ GraphicDevice / Mgr ]
	____________________________________________________________________________________________________________*/
	Engine::CObjectMgr*			m_pObjectMgr	 = nullptr;
	Engine::CManagement*		m_pManagement	 = nullptr;
	Engine::CRenderer*			m_pRenderer		 = nullptr;
	ID3D12Device*				m_pGraphicDevice = nullptr;
	ID3D12GraphicsCommandList*	m_pCommandList   = nullptr;

	CInstancePoolMgr*			m_pInstancePoolMgr = nullptr;
	CPartySystemMgr*			m_pPartySystemMgr = nullptr;

	MVKEY m_eCurKey = MVKEY::K_END;
	MVKEY m_ePreKey = MVKEY::K_END;

	/* 패킷 재조립 - Ring Buffer */
	unsigned char* m_packet_start;
	unsigned char* m_recv_start;
	unsigned char* m_next_recv_ptr;
	unsigned char m_recv_buf[MAX_BUF_SIZE];
};

