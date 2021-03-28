#pragma once
#include "Include.h"
#include "Base.h"

#define ERR_CHECK

namespace Engine
{
	class CObjectMgr;
	class CManagement;
	class CRenderer;
}

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
	void	ProcessData(unsigned char* net_buf, size_t io_byte);
	void	ProcessPacket(char* ptr);

	void Attack_Monster(sc_packet_monster_attack* packet);

	void Move_Monster(sc_packet_move* packet);

	void	send_login();
	void	send_move(const _vec3& vDir, const _vec3& vPos, const _int& iAniIdx);
	void	send_move_stop(const _vec3& vPos, const _vec3& vDir, const _int& iAniIdx);
	void	send_attack(const _vec3& vDir, const _vec3& vPos);
	void    send_attack_stop(const _vec3& vDir, const _vec3& vPos);
	void    send_attackByMonster(int objID);
	void    send_attackToMonster(int objID);

public:
	bool	change_MoveKey(MVKEY eKey);
	void	Enter_Monster(sc_packet_monster_enter* packet);


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

	MVKEY m_eCurKey;
	MVKEY m_ePreKey;
};

