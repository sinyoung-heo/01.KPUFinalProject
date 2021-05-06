#include "stdafx.h"
#include "PacketMgr.h"
#include "ObjectMgr.h"
#include "Management.h"
#include "Renderer.h"
#include "InstancePoolMgr.h"
#include "TestPlayer.h"
#include "TestOthers.h"
#include "NPC_Walker.h"
#include "NPC_Assistant.h"
#include "NPC_Stander.h"
#include "NPC_Merchant.h"
#include "NPC_Quest.h"
#include "NPC_Children.h"
#include "Crab.h"
#include "Monkey.h"
#include "CloderA.h"
#include "DrownedSailor.h"
#include "GiantBeetle.h"
#include "GiantMonkey.h"
#include "CraftyArachne.h"
#include "DynamicCamera.h"
#include "TestColPlayer.h"
#include "TestColMonster.h"
#include "PCGladiator.h"
#include "PCOthersGladiator.h"
#include "FadeInOut.h"

IMPLEMENT_SINGLETON(CPacketMgr)

CPacketMgr::CPacketMgr()
	: m_pObjectMgr(Engine::CObjectMgr::Get_Instance())
	, m_pManagement(Engine::CManagement::Get_Instance())
	, m_pRenderer(Engine::CRenderer::Get_Instance())
	, m_pInstancePoolMgr(CInstancePoolMgr::Get_Instance())
	, m_eCurKey(MVKEY::K_END), m_ePreKey(MVKEY::K_END)
{
	memset(m_recv_buf, 0, sizeof(m_recv_buf));
	m_packet_start = m_recv_buf;
	m_recv_start = m_recv_buf;
	m_next_recv_ptr = m_recv_buf;
}

HRESULT CPacketMgr::Ready_Server(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList)
{
	m_pGraphicDevice = pGraphicDevice;
	m_pCommandList = pCommandList;
	m_eCurKey = MVKEY::K_END;
	m_ePreKey = MVKEY::K_END;

	// Initialize Windows Socket
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);

	// Create Windows Socket
	g_hSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	/* Non-Blocking Socket으로 전환 */
	unsigned long ul = 1;
	ioctlsocket(g_hSocket, FIONBIO, (unsigned long*)&ul);

	return S_OK;
}

HRESULT CPacketMgr::Connect_Server()
{
	/* Socket Address Structure */
	SOCKADDR_IN sockAddr;
	memset(&sockAddr, 0, sizeof(sockAddr));

	sockAddr.sin_family = AF_INET;
	sockAddr.sin_port = htons(SERVER_PORT);
	sockAddr.sin_addr.s_addr = inet_addr(SERVER_IP);

	if (connect(g_hSocket, (SOCKADDR*)&sockAddr, sizeof(sockAddr)) == SOCKET_ERROR)
	{
		int err_no = WSAGetLastError();
		if (err_no != WSAEWOULDBLOCK)
		{
			error_display("connect : ", err_no);
			closesocket(g_hSocket);
			return E_FAIL;
		}
	}

#ifdef ERR_CHECK
	cout << "서버에 접속을 요청하였습니다. 잠시만 기다려주세요." << endl;
#endif 

	CPacketMgr::Get_Instance()->send_login();

#ifdef ERR_CHECK
	cout << "Login packet 전송완료" << endl;
#endif 

	return S_OK;
}

void CPacketMgr::recv_packet()
{
	// Server Data Receive Version - Ring Buffer
	int iLen = MAX_BUF_SIZE - static_cast<int>(m_next_recv_ptr - m_recv_buf);
	int retval = recv(g_hSocket, reinterpret_cast<CHAR*>(m_recv_start), iLen, 0);

#ifdef ERR_CHECK
	if (retval == SOCKET_ERROR)
	{
		int state = WSAGetLastError();
		if (state != WSAEWOULDBLOCK)
		{
			error_display("recv Error : ", state);
			closesocket(g_hSocket);
		}
	}
#endif 

	// 데이터 수신 성공
	if (retval > 0)
	{
		// 패킷 재조립(Version - Ring Buffer)
		Process_recv_reassembly(static_cast<size_t>(retval));
	}
}

void CPacketMgr::Process_recv_reassembly(size_t iosize)
{
	// m_packet_start		: 처리할 데이터 버퍼 및 위치 (= iocp_buffer)
	// m_packet_start[0]	: 처리할 패킷의 크기
	unsigned char p_size = m_packet_start[0];

	/* 다음 패킷을 받을 링버퍼 위치 설정 */
	// m_recv_start			: 수신한 패킷의 시작 위치
	m_next_recv_ptr = m_recv_start + iosize;

	// 처리할 패킷 크기보다 남아있는 공간이 적당할 경우 패킷 처리 시작
	while (p_size <= m_next_recv_ptr - m_packet_start)
	{
		// 패킷 처리
		Process_packet();

		// 패킷 처리 후 유저의 데이터 처리 버퍼 시작 위치 변경
		m_packet_start += p_size;

		// recv한 데이터를 처리했는데 아직 처리하지 못한 데이터를 가지고 있을 수도 있다.
		// 처리해야 할 패킷 크기 갱신
		if (m_packet_start < m_next_recv_ptr)
			p_size = m_packet_start[0];
		else break;
	}

	// 처리하지 못한(남아있는) 데이터의 시작 위치
	long long left_data = m_next_recv_ptr - m_packet_start;

	// recv 처리 종료 -> 다시 recv를 해야 한다.
	// 다시 recv를 하기 전에 링 버퍼의 여유 공간을 확인 후 다 사용했을 경우 초기화한다.
	// 남아있는 공간이 MIN_BUFFER 보다 작으면 남은 데이터를 링 버퍼 맨 앞으로 옮겨준다.
	
	if ((MAX_BUF_SIZE - (int)(m_next_recv_ptr - m_recv_buf)) < MIN_BUF_SIZE)
	{
		// 남은 데이터 버퍼를 recv_buf에 복사한다.
		memcpy(m_recv_buf, m_packet_start, left_data);
		// 처리할 데이터 버퍼 갱신한다.
		m_packet_start = m_recv_buf;
		// 다음 패킷을 받을 버퍼의 시작 위치를 갱신한다.
		m_next_recv_ptr = m_packet_start + left_data;
	}

	/* 다음 recv 준비 */
	m_recv_start = m_next_recv_ptr;
}

void CPacketMgr::Process_packet()
{
	// 패킷 타입
	char p_type = m_packet_start[1];

	switch (p_type)
	{
	case SC_PACKET_LOGIN_OK:
	{
		sc_packet_login_ok* packet = reinterpret_cast<sc_packet_login_ok*>(m_packet_start);
		Login_Player(packet);
	}
	break;

	case SC_PACKET_LOGIN_FAIL:
		break;

	case SC_PACKET_ENTER:
	{
		sc_packet_enter* packet = reinterpret_cast<sc_packet_enter*>(m_packet_start);

		int retflag;
		Enter_Others(packet, retflag);
		if (retflag == 2) break;
	}
	break;

	case SC_PACKET_MOVE:
	{
		sc_packet_move* packet = reinterpret_cast<sc_packet_move*>(m_packet_start);

		Move_User(packet);
	}
	break;

	case SC_PACKET_MOVE_STOP:
	{
		sc_packet_move* packet = reinterpret_cast<sc_packet_move*>(m_packet_start);

		MoveStop_User(packet);
	}
	break;

	case SC_PACKET_STAT_CHANGE:
	{
		sc_packet_stat_change* packet = reinterpret_cast<sc_packet_stat_change*>(m_packet_start);

		ChangeStat_User(packet);
	}
	break;

	case SC_PACKET_ATTACK:
	{
		sc_packet_attack* packet = reinterpret_cast<sc_packet_attack*>(m_packet_start);

		Attack_User(packet);
	}
	break;

	case SC_PACKET_ATTACK_STOP:
	{
		sc_packet_attack* packet = reinterpret_cast<sc_packet_attack*>(m_packet_start);

		AttackStop_User(packet);
	}
	break;

	case SC_PACKET_LEAVE:
	{
		sc_packet_leave* packet = reinterpret_cast<sc_packet_leave*>(m_packet_start);

		int retflag;
		Leave_Object(packet, retflag);
		if (retflag == 2) break;
	}
	break;

	case SC_PACKET_STANCE_CHANGE:
	{
		sc_packet_stance_change* packet = reinterpret_cast<sc_packet_stance_change*>(m_packet_start);
		int retflag;
		Change_Stance_Others(packet, retflag);
		if (retflag == 2) break;
	}
	break;

	case SC_PACKET_NPC_ENTER:
	{
		sc_packet_npc_enter* packet = reinterpret_cast<sc_packet_npc_enter*>(m_packet_start);
		Enter_NPC(packet);
	}
	break;

	case SC_PACKET_NPC_MOVE:
	{
		sc_packet_move* packet = reinterpret_cast<sc_packet_move*>(m_packet_start);
		Move_NPC(packet);
	}
	break;

	case SC_PACKET_MONSTER_ENTER:
	{
		sc_packet_monster_enter* packet = reinterpret_cast<sc_packet_monster_enter*>(m_packet_start);
		Enter_Monster(packet);
	}
	break;

	case SC_PACKET_MONSTER_MOVE:
	{
		sc_packet_move* packet = reinterpret_cast<sc_packet_move*>(m_packet_start);
		Move_Monster(packet);
	}
	break;

	case SC_PACKET_MONSTER_ATTACK:
	{
		sc_packet_monster_attack* packet = reinterpret_cast<sc_packet_monster_attack*>(m_packet_start);
		Attack_Monster(packet);
	}
	break;

	case SC_PACKET_MONSTER_RUSH:
	{
		sc_packet_monster_rushAttack* packet = reinterpret_cast<sc_packet_monster_rushAttack*>(m_packet_start);
		Rush_Monster(packet);
	}
	break;

	case SC_PACKET_MONSTER_STAT:
	{
		sc_packet_stat_change* packet = reinterpret_cast<sc_packet_stat_change*>(m_packet_start);
		Change_Monster_Stat(packet);
	}
	break;

	case SC_PACKET_MONSTER_DEAD:
	{
		sc_packet_animationIndex* packet = reinterpret_cast<sc_packet_animationIndex*>(m_packet_start);
		Dead_Monster(packet);
	}
	break;

	case SC_PACKET_STAGE_CHANGE:
	{
		sc_packet_stage_change* packet = reinterpret_cast<sc_packet_stage_change*>(m_packet_start);
		Stage_Change(packet);
	}
	break;

	case SC_PACKET_ANIM_INDEX:
	{
		sc_packet_animationIndex* packet = reinterpret_cast<sc_packet_animationIndex*>(m_packet_start);

		bool retflag;
		Change_Animation(packet, retflag);
		if (retflag) return;
	}
	break;

	default:
#ifdef ERR_CHECK
		printf("Unknown PACKET type [%d]\n", m_packet_start[1]);
#endif 
		break;
	}
}

void CPacketMgr::Dead_Monster(sc_packet_animationIndex* packet)
{
	int s_num = packet->id;
	Engine::CGameObject* pObj = m_pObjectMgr->Get_ServerObject(L"Layer_GameObject", L"MONSTER", s_num);
	if (pObj == nullptr) return;

	pObj->Set_State(packet->aniIdx);
}

void CPacketMgr::Rush_Monster(sc_packet_monster_rushAttack* packet)
{
	int s_num = packet->id;

	Engine::CGameObject* pObj = m_pObjectMgr->Get_ServerObject(L"Layer_GameObject", L"MONSTER", s_num);
	if (pObj == nullptr) return;

	pObj->Set_State(packet->animIdx);
	pObj->Get_Transform()->m_vPos = _vec3(packet->posX, packet->posY, packet->posZ);
	pObj->Ready_AngleInterpolationValue(pObj->Set_Other_Angle(_vec3(packet->dirX, packet->dirY, packet->dirZ)));
}

void CPacketMgr::Change_Monster_Stat(sc_packet_stat_change* packet)
{
	int s_num = packet->id;
	Engine::CGameObject* pObj = m_pObjectMgr->Get_ServerObject(L"Layer_GameObject", L"MONSTER", s_num);
	if (pObj == nullptr) return;

	pObj->Get_Info()->m_iHp = packet->hp;
	pObj->Get_Info()->m_iMp = packet->mp;
	pObj->Get_Info()->m_iExp = packet->exp;
}

void CPacketMgr::Move_NPC(sc_packet_move* packet)
{
	int s_num = packet->id;

	Engine::CGameObject* pObj = m_pObjectMgr->Get_ServerObject(L"Layer_GameObject", L"NPC", s_num);
	if (pObj == nullptr) return;

	pObj->Set_MoveStop(false);
	pObj->Set_State(packet->animIdx);
	pObj->Ready_PositionInterpolationValue(_vec3(packet->posX, packet->posY, packet->posZ), packet->spd);
	pObj->Ready_AngleInterpolationValue(pObj->Set_Other_Angle(_vec3(packet->dirX, packet->dirY, packet->dirZ)));
}

void CPacketMgr::Stage_Change(sc_packet_stage_change* packet)
{
	int s_num = packet->id;

	/* 현재 클라이언트가 공격한 경우 */
	if (s_num == g_iSNum)
	{
		Engine::CGameObject* pThisPlayer = m_pObjectMgr->Get_GameObject(L"Layer_GameObject", L"ThisPlayer");

		// Set Player
		if (nullptr != pThisPlayer)
		{
			pThisPlayer->Get_Transform()->m_vPos = _vec3(packet->posX, packet->posY, packet->posZ);
			pThisPlayer->Set_CurrentStageID(packet->stage_id);
		}

		// Static Object
		if (STAGE_VELIKA == packet->stage_id)
			Engine::CObjectMgr::Get_Instance()->Set_CurrentStage(Engine::STAGEID::STAGE_VELIKA);
		else if (STAGE_BEACH == packet->stage_id)
			Engine::CObjectMgr::Get_Instance()->Set_CurrentStage(Engine::STAGEID::STAGE_BEACH);
		else if (STAGE_WINTER == packet->stage_id)
			Engine::CObjectMgr::Get_Instance()->Set_CurrentStage(Engine::STAGEID::STAGE_WINTER);

		// Set FadeInOut
		Engine::CGameObject* pFadeInOut = *(--(m_pObjectMgr->Get_OBJLIST(L"Layer_UI", L"StageChange_FadeInOut")->end()));

		if (nullptr != pFadeInOut)
			static_cast<CFadeInOut*>(pFadeInOut)->Set_IsReceivePacket(true);
	}
	else
	{
		m_pObjectMgr->Delete_ServerObject(L"Layer_GameObject", L"Others", s_num);
	}
}

void CPacketMgr::Enter_NPC(sc_packet_npc_enter* packet)
{
	/*__________________________________________________________________________________________________________
	[ GameLogic Object(NPC) 생성 ]
	____________________________________________________________________________________________________________*/
	// Engine::CGameObject* pGameObj = nullptr;
	Engine::CGameObject* pInstance = nullptr;

	wstring wstrNaviMeshTag;
	if (packet->naviType == STAGE_VELIKA)
		wstrNaviMeshTag = L"StageVelika_NaviMesh";
	
	/* NPC - Walker */
	if (packet->npcNum == NPC_CHICKEN || packet->npcNum == NPC_CAT || packet->npcNum == NPC_AMAN_BOY)
	{
		wstring wstrMeshTag;

		if (packet->npcNum == NPC_CHICKEN)
		{
			wstrMeshTag = L"Chicken";
			pInstance = Pop_Instance(m_pInstancePoolMgr->Get_NPCChickenPool());
		}
		else if (packet->npcNum == NPC_CAT)
		{
			wstrMeshTag = L"Cat";
			pInstance = Pop_Instance(m_pInstancePoolMgr->Get_NPCCatPool());
		}
		else if (packet->npcNum == NPC_AMAN_BOY)
		{
			wstrMeshTag = L"Aman_boy";
			pInstance = Pop_Instance(m_pInstancePoolMgr->Get_NPCAmanBoyPool());
		}
	}
	/* NPC - Assistant */
	else if (packet->npcNum == NPC_POPORI_BOY)
	{
		pInstance = Pop_Instance(m_pInstancePoolMgr->Get_NPCAssistantPool());
	}
	/* NPC - Stander */
	else if (packet->npcNum == NPC_VILLAGERS || packet->npcNum == NPC_BARAKA_EXTRACTOR)
	{
		wstring wstrMeshTag;
		if (packet->npcNum == NPC_VILLAGERS)
		{
			wstrMeshTag = L"NPC_Villagers";
			pInstance = Pop_Instance(m_pInstancePoolMgr->Get_NPCStanderVillagersPool());
		}
		else if (packet->npcNum == NPC_BARAKA_EXTRACTOR)
		{
			wstrMeshTag = L"Baraka_M_Extractor";
			pInstance = Pop_Instance(m_pInstancePoolMgr->Get_NPCStanderBarakaPool());
		}
	}
	/* NPC - Merchant */
	else if (packet->npcNum == NPC_POPORI_MERCHANT || packet->npcNum == NPC_BARAKA_MERCHANT || packet->npcNum == NPC_BARAKA_MYSTELLIUM)
	{
		wstring wstrMeshTag;
		if (packet->npcNum == NPC_POPORI_MERCHANT)
		{
			wstrMeshTag = L"Popori_M_Merchant";
			pInstance = Pop_Instance(m_pInstancePoolMgr->Get_NPCMerchant_Popori_M_MerchantPool());
		}
		else if (packet->npcNum == NPC_BARAKA_MERCHANT)
		{
			wstrMeshTag = L"Baraka_M_Merchant";
			pInstance = Pop_Instance(m_pInstancePoolMgr->Get_NPCMerchant_Baraka_M_MerchantPool());
		}
		else if (packet->npcNum == NPC_BARAKA_MYSTELLIUM)
		{
			wstrMeshTag = L"Baraka_M_Mystellium";
			pInstance = Pop_Instance(m_pInstancePoolMgr->Get_NPCMerchant_Baraka_M_MystelliumPool());
		}

		static_cast<CNPC_Merchant*>(pInstance)->Set_NPCNumber(packet->npcNum);
	}
	/* NPC - Quest */
	else if (packet->npcNum == NPC_CASTANIC_LSMITH)
	{
		pInstance = Pop_Instance(m_pInstancePoolMgr->Get_NPCQuestPool());
		static_cast<CNPC_Quest*>(pInstance)->Set_NPCNumber(packet->npcNum);
	}
	/* NPC - Children */
	else if (packet->npcNum == NPC_HUMAN_BOY || packet->npcNum == NPC_HUMAN_GIRL || packet->npcNum == NPC_HIGHELF_GIRL)
	{
		wstring wstrMeshTag;
		if (packet->npcNum == NPC_HUMAN_BOY)
		{
			wstrMeshTag = L"Human_boy";
			pInstance = Pop_Instance(m_pInstancePoolMgr->Get_NPCChildren_HumanBoyPool());
		}
		else if (packet->npcNum == NPC_HUMAN_GIRL)
		{
			wstrMeshTag = L"Human_girl";
			pInstance = Pop_Instance(m_pInstancePoolMgr->Get_NPCChildren_HumanGirlPool());
		}
		else if (packet->npcNum == NPC_HIGHELF_GIRL)
		{
			wstrMeshTag = L"Highelf_girl";
			pInstance = Pop_Instance(m_pInstancePoolMgr->Get_NPCChildren_HighelfGirlPool());
		}
	}

	if (nullptr != pInstance)
	{
		pInstance->Get_Transform()->m_vAngle = _vec3(packet->angleX, packet->angleY, packet->angleZ);
		pInstance->Get_Transform()->m_vPos   = _vec3(packet->posX, packet->posY, packet->posZ);
		pInstance->Set_ServerNumber(packet->id);
		pInstance->Set_OType(packet->o_type);

		m_pObjectMgr->Add_GameObject(L"Layer_GameObject", L"NPC", pInstance);
	}
}

void CPacketMgr::Change_Stance_Others(sc_packet_stance_change* packet, int& retflag)
{
	retflag = 1;
	int s_num = packet->id;

	if (s_num == g_iSNum) { retflag = 2; return; };

	Engine::CGameObject* pObj = m_pObjectMgr->Get_ServerObject(L"Layer_GameObject", L"Others", s_num);

	if (PC_GLADIATOR == packet->o_type)
	{
		static_cast<CPCOthersGladiator*>(pObj)->Set_StanceChange(packet->animIdx, packet->is_stance_attack);
	}
	else if (PC_ARCHER == packet->o_type)
	{

	}
	else if (PC_PRIEST == packet->o_type)
	{

	}
	else
	{
		static_cast<CPCOthersGladiator*>(pObj)->Set_StanceChange(packet->animIdx, packet->is_stance_attack);
	}
}

void CPacketMgr::Leave_Object(sc_packet_leave* packet, int& retflag)
{
	retflag = 1;

	if (packet->id == g_iSNum) { retflag = 2; return; };

	if (packet->id >= NPC_NUM_START && packet->id < MON_NUM_START)
		m_pObjectMgr->Delete_ServerObject(L"Layer_GameObject", L"NPC", packet->id);
	else if (packet->id >= MON_NUM_START)
		m_pObjectMgr->Delete_ServerObject(L"Layer_GameObject", L"MONSTER", packet->id);
	else
		m_pObjectMgr->Delete_ServerObject(L"Layer_GameObject", L"Others", packet->id);
}

void CPacketMgr::AttackStop_User(sc_packet_attack* packet)
{
	int s_num = packet->id;

	/* 현재 클라이언트가 공격을 멈춘 경우 */
	if (s_num == g_iSNum)
	{
		Engine::CGameObject* pObj = m_pObjectMgr->Get_GameObject(L"Layer_GameObject", L"ThisPlayer", 0);
		pObj->Get_Transform()->m_vPos = _vec3(packet->posX, packet->posY, packet->posZ);
	}
	/* 다른 클라이언트가 공격을 멈춘 경우 */
	else
	{
		Engine::CGameObject* pObj = m_pObjectMgr->Get_ServerObject(L"Layer_GameObject", L"Others", s_num);

		if (PC_GLADIATOR == packet->o_type)
		{
			static_cast<CPCOthersGladiator*>(pObj)->Set_AnimationIdx(packet->animIdx);
		}
		else if (PC_ARCHER == packet->o_type)
		{
		}
		else if (PC_PRIEST == packet->o_type)
		{
		}
		else
			static_cast<CPCOthersGladiator*>(pObj)->Set_AnimationIdx(packet->animIdx);

		pObj->Set_IsStartPosInterpolation(true);
		pObj->Set_LinearPos(pObj->Get_Transform()->m_vPos, _vec3(packet->posX, packet->posY, packet->posZ));
	
		pObj->Set_Other_direction(_vec3(packet->dirX, packet->dirY, packet->dirZ));
		pObj->Set_Attack(false);
		pObj->Set_MoveStop(true);
	}
}

void CPacketMgr::Attack_User(sc_packet_attack* packet)
{
	int s_num = packet->id;

	/* 현재 클라이언트가 공격한 경우 */
	if (s_num == g_iSNum)
	{
		Engine::CGameObject* pObj = m_pObjectMgr->Get_GameObject(L"Layer_GameObject", L"ThisPlayer", 0);
		pObj->Set_DeadReckoning(_vec3(packet->posX, packet->posY, packet->posZ));
	}
	/* 다른 클라이언트가 공격한 경우 */
	else
	{
		Engine::CGameObject* pObj = m_pObjectMgr->Get_ServerObject(L"Layer_GameObject", L"Others", s_num);

		if (PC_GLADIATOR == packet->o_type)
		{
			static_cast<CPCOthersGladiator*>(pObj)->Set_AnimationIdx(packet->animIdx);
			static_cast<CPCOthersGladiator*>(pObj)->Reset_AttackMoveInterpolationRatio();
		}
		else if (PC_ARCHER == packet->o_type)
		{
		}
		else if (PC_PRIEST == packet->o_type)
		{
		}
		else
			static_cast<CPCOthersGladiator*>(pObj)->Set_AnimationIdx(packet->animIdx);

		pObj->Set_DeadReckoning(_vec3(packet->posX, packet->posY, packet->posZ));
		pObj->Set_Other_direction(_vec3(packet->dirX, packet->dirY, packet->dirZ));
		if (-1 != packet->end_angleY)
		{
			pObj->Set_IsStartAngleInterpolation(true);
			pObj->Set_LinearAngle(pObj->Get_Transform()->m_vAngle.y, packet->end_angleY);
		}
		pObj->Set_Attack(true);
	}
}

void CPacketMgr::ChangeStat_User(sc_packet_stat_change* packet)
{
	int s_num = packet->id;

	Engine::CGameObject* pObj = nullptr;
	/* 현재 클라이언트의 스탯이 갱신된 경우 */
	if (s_num == g_iSNum)
		pObj = m_pObjectMgr->Get_GameObject(L"Layer_GameObject", L"ThisPlayer", 0);
	/* 다른 클라이언트의 스탯이 갱신된 경우 */
	else
		pObj = m_pObjectMgr->Get_ServerObject(L"Layer_GameObject", L"Others", s_num);

	pObj->Get_Info()->m_iHp = packet->hp;
	pObj->Get_Info()->m_iMp = packet->mp;
	pObj->Get_Info()->m_iExp = packet->exp;
}

void CPacketMgr::MoveStop_User(sc_packet_move* packet)
{
	int s_num = packet->id;

	/* 현재 클라이언트가 움직인 경우 */
	if (s_num == g_iSNum)
	{
		Engine::CGameObject* pObj = m_pObjectMgr->Get_GameObject(L"Layer_GameObject", L"ThisPlayer", 0);

		auto d_ms = duration_cast<milliseconds>(high_resolution_clock::now().time_since_epoch()).count() - packet->move_time;

		pObj->Set_DeadReckoning(_vec3(packet->posX, packet->posY, packet->posZ));
	}

	/* 다른 클라이언트가 움직인 경우 */
	else
	{
		Engine::CGameObject* pObj = m_pObjectMgr->Get_ServerObject(L"Layer_GameObject", L"Others", s_num);
		auto d_ms = duration_cast<milliseconds>(high_resolution_clock::now().time_since_epoch()).count() - packet->move_time;

		static_cast<CPCOthersGladiator*>(pObj)->Set_AnimationIdx(packet->animIdx);
		pObj->Set_IsStartPosInterpolation(true);
		pObj->Set_LinearPos(pObj->Get_Transform()->m_vPos, _vec3(packet->posX, packet->posY, packet->posZ));
		//pObj->Get_Transform()->m_vPos = _vec3(packet->posX, packet->posY, packet->posZ);
		pObj->Set_Other_direction(_vec3(packet->dirX, packet->dirY, packet->dirZ));
		pObj->Set_MoveStop(true);
	}
}

void CPacketMgr::Move_User(sc_packet_move* packet)
{
	int s_num = packet->id;

	/* 현재 클라이언트가 움직인 경우 */
	if (s_num == g_iSNum)
	{
		Engine::CGameObject* pObj = m_pObjectMgr->Get_GameObject(L"Layer_GameObject", L"ThisPlayer", 0);

		auto d_ms = duration_cast<milliseconds>(high_resolution_clock::now().time_since_epoch()).count() - packet->move_time;

		pObj->Set_DeadReckoning(_vec3(packet->posX, packet->posY, packet->posZ));

	}
	/* 다른 클라이언트가 움직인 경우 */
	else
	{
		Engine::CGameObject* pObj = m_pObjectMgr->Get_ServerObject(L"Layer_GameObject", L"Others", s_num);

		auto d_ms = duration_cast<milliseconds>(high_resolution_clock::now().time_since_epoch()).count() - packet->move_time;

		static_cast<CPCOthersGladiator*>(pObj)->Set_AnimationIdx(packet->animIdx);
		pObj->Set_DeadReckoning(_vec3(packet->posX, packet->posY, packet->posZ));
		pObj->Set_Other_direction(_vec3(packet->dirX, packet->dirY, packet->dirZ));
		pObj->Set_MoveStop(false);
	}
}

void CPacketMgr::Enter_Others(sc_packet_enter* packet, int& retflag)
{
	retflag = 1;
	if (packet->id == g_iSNum) { retflag = 2; return; };

	/*__________________________________________________________________________________________________________
	[ GameLogic Object(Ohters) 생성 ]
	____________________________________________________________________________________________________________*/
	Engine::CGameObject* pGameObj = nullptr;
	wstring wstrMeshTag = L"";

	if (PC_GLADIATOR == packet->o_type)
	{
		wstrMeshTag = L"PoporiR27Gladiator";
		pGameObj = CPCOthersGladiator::Create(m_pGraphicDevice, m_pCommandList,
			L"PoporiR27Gladiator",							// MeshTag
			L"StageVelika_NaviMesh",							// NaviMeshTag
			_vec3(0.05f, 0.05f, 0.05f),						// Scale
			_vec3(0.0f, 0.0f, 0.0f),							// Angle
			_vec3(packet->posX, packet->posY, packet->posZ),	// Pos
			Twohand19_A_SM);									// WeaponType

	}
	else if (PC_ARCHER == packet->o_type)
	{

	}
	else if (PC_PRIEST == packet->o_type)
	{

	}
	else
	{
		wstrMeshTag = L"PoporiR27Gladiator";

		pGameObj = CPCOthersGladiator::Create(m_pGraphicDevice, m_pCommandList,
			wstrMeshTag,							// MeshTag
			L"StageVelika_NaviMesh",							// NaviMeshTag
			_vec3(0.05f, 0.05f, 0.05f),						// Scale
			_vec3(0.0f, 0.0f, 0.0f),							// Angle
			_vec3(packet->posX, packet->posY, packet->posZ),	// Pos
			Twohand19_A_SM);									// WeaponType
	}

	pGameObj->Set_CurrentStageID(packet->stageID);
	pGameObj->Set_OType(packet->o_type);
	pGameObj->Set_ServerNumber(packet->id);

	Engine::FAILED_CHECK_RETURN(m_pObjectMgr->Add_GameObject(L"Layer_GameObject", L"Others", pGameObj), E_FAIL);

#ifdef ERR_CHECK
	cout << "Others 등장!" << endl;
#endif
}

void CPacketMgr::Login_Player(sc_packet_login_ok* packet)
{
	g_iSNum = packet->id;

	/*__________________________________________________________________________________________________________
	[ GameLogic Object(ThisPlayer) 생성 ]
	____________________________________________________________________________________________________________*/
	Engine::CGameObject* pGameObj = nullptr;
	wstring wstrMeshTag = L"";

	if (PC_GLADIATOR == packet->o_type)
	{
		wstrMeshTag = L"PoporiR27Gladiator";
	}
	else if (PC_ARCHER == packet->o_type)
	{

	}
	else if (PC_PRIEST == packet->o_type)
	{

	}
	else
		wstrMeshTag = L"PoporiR27Gladiator";

	pGameObj = CPCGladiator::Create(m_pGraphicDevice, m_pCommandList,
									wstrMeshTag,										// MeshTag
									L"StageVelika_NaviMesh",							// NaviMeshTag
									_vec3(0.05f, 0.05f, 0.05f),							// Scale
									_vec3(0.0f, 0.0f, 0.0f),							// Angle
									_vec3(packet->posX, packet->posY, packet->posZ),	// Pos
									TwoHand33_B_SM);									// WeaponType

	pGameObj->Set_OType(packet->o_type);
	pGameObj->Set_ServerNumber(g_iSNum);
	pGameObj->Set_Info(packet->level, packet->hp, packet->maxHp, packet->mp, packet->maxMp, packet->exp, packet->maxExp, packet->min_att, packet->max_att, packet->spd);
	Engine::FAILED_CHECK_RETURN(m_pObjectMgr->Add_GameObject(L"Layer_GameObject", L"ThisPlayer", pGameObj), E_FAIL);

#ifdef ERR_CHECK
	cout << "Login OK! 접속완료!" << endl;
#endif
}

void CPacketMgr::Change_Animation(sc_packet_animationIndex* packet, bool& retflag)
{
	retflag = true;
	if (packet->id == g_iSNum)
		return;

	/* NPC */
	if (packet->id >= NPC_NUM_START && packet->id < MON_NUM_START)
	{
		Engine::CGameObject* pObj = m_pObjectMgr->Get_ServerObject(L"Layer_GameObject", L"NPC", packet->id);
		pObj->Set_State(packet->aniIdx);
	}

	/* MONSTER */
	else if (packet->id >= MON_NUM_START)
	{
	}

	/* OTHERS */
	else
	{
	}
	retflag = false;
}

void CPacketMgr::Attack_Monster(sc_packet_monster_attack* packet)
{
	int s_num = packet->id;

	Engine::CGameObject* pObj = m_pObjectMgr->Get_ServerObject(L"Layer_GameObject", L"MONSTER", s_num);
	if (pObj == nullptr) return;

	pObj->Set_State(packet->animIdx);
	pObj->Set_MoveStop(true);
	pObj->Ready_AngleInterpolationValue(pObj->Set_Other_Angle(_vec3(packet->dirX, packet->dirY, packet->dirZ)));
}

void CPacketMgr::Move_Monster(sc_packet_move* packet)
{
	int s_num = packet->id;

	Engine::CGameObject* pObj = m_pObjectMgr->Get_ServerObject(L"Layer_GameObject", L"MONSTER", s_num);
	if (pObj == nullptr) return;

	pObj->Set_State(packet->animIdx);
	pObj->Get_Transform()->m_vPos = _vec3(packet->posX, packet->posY, packet->posZ);
	pObj->Ready_AngleInterpolationValue(pObj->Set_Other_Angle(_vec3(packet->dirX, packet->dirY, packet->dirZ)));
}

void CPacketMgr::Enter_Monster(sc_packet_monster_enter* packet)
{
	Engine::CGameObject* pInstance = nullptr;

	wstring wstrNaviMeshTag;
	if (packet->naviType == STAGE_VELIKA)
		wstrNaviMeshTag = L"StageVelika_NaviMesh";

	if (packet->mon_num == MON_CRAB)
	{
		pInstance = Pop_Instance(m_pInstancePoolMgr->Get_MonsterCrabPool());
	}
	else if (packet->mon_num == MON_MONKEY)
	{
		pInstance = Pop_Instance(m_pInstancePoolMgr->Get_MonsterMonkeyPool());
	}
	else if (packet->mon_num == MON_CLODER)
	{
		pInstance = Pop_Instance(m_pInstancePoolMgr->Get_MonsterCloderAPool());
	}
	else if (packet->mon_num == MON_SAILOR)
	{
		pInstance = Pop_Instance(m_pInstancePoolMgr->Get_MonsterDrownedSailorPool());
	}
	else if (packet->mon_num == MON_GBEETLE)
	{
		pInstance = Pop_Instance(m_pInstancePoolMgr->Get_MonsterGiantBeetlePool());
	}
	else if (packet->mon_num == MON_GMONKEY)
	{
		pInstance = Pop_Instance(m_pInstancePoolMgr->Get_MonsterGiantMonkeyPool());
	}
	else if (packet->mon_num == MON_ARACHNE)
	{
		pInstance = Pop_Instance(m_pInstancePoolMgr->Get_MonsterCraftyArachnePool());
	}
	
	if (nullptr != pInstance)
	{
		pInstance->Get_Transform()->m_vScale = _vec3(0.05f, 0.05f, 0.05f);
		pInstance->Get_Transform()->m_vAngle = _vec3(packet->angleX, packet->angleY, packet->angleZ);
		pInstance->Get_Transform()->m_vPos   = _vec3(packet->posX, packet->posY, packet->posZ);
		pInstance->Set_ServerNumber(packet->id);
		pInstance->Set_Info(1, packet->Hp, packet->maxHp, 0, 0, 0, 0, packet->min_att, packet->max_att, packet->spd);
		m_pObjectMgr->Add_GameObject(L"Layer_GameObject", L"MONSTER", pInstance);
	}
}

void CPacketMgr::send_login()
{
	cs_packet_login p;

	p.size   = sizeof(p);
	p.type   = CS_LOGIN;
	// p.o_type = PC_GLADIATOR;

	int t_id = GetCurrentProcessId();
	sprintf_s(p.name, "P%03d", t_id % 1000);
	sprintf_s(p.password, "%03d", t_id % 1000);

	send_packet(&p);
}

void CPacketMgr::send_move(const _vec3& vDir, const _vec3& vPos, const _int& iAniIdx)
{
	cs_packet_move p;

	p.size = sizeof(p);
	p.type = CS_MOVE;
	p.animIdx = iAniIdx;
	p.posX = vPos.x;
	p.posY = vPos.y;
	p.posZ = vPos.z;

	p.dirX = vDir.x;
	p.dirY = vDir.y;
	p.dirZ = vDir.z;

	p.move_time = static_cast<unsigned>(duration_cast<milliseconds>(high_resolution_clock::now().time_since_epoch()).count());

	send_packet(&p);
}

bool CPacketMgr::change_MoveKey(MVKEY eKey)
{
	m_eCurKey = eKey;

	/* 키 입력이 변경되었을 경우 */
	if (m_eCurKey != m_ePreKey)
	{
		m_ePreKey = m_eCurKey;
		return true;
	}

	return false;
}

void CPacketMgr::send_move_stop(const _vec3& vPos, const _vec3& vDir, const _int& iAniIdx)
{
	cs_packet_move_stop p;

	p.size = sizeof(p);
	p.type = CS_MOVE_STOP;
	p.animIdx = iAniIdx;
	p.posX = vPos.x;
	p.posY = vPos.y;
	p.posZ = vPos.z;

	p.dirX = vDir.x;
	p.dirY = vDir.y;
	p.dirZ = vDir.z;

	send_packet(&p);
}

void CPacketMgr::send_stance_change(const _int& iAniIdx, const _bool& bIsStanceAttack)
{
	cs_packet_stance_change p;

	p.size             = sizeof(p);
	p.type             = CS_STANCE_CHANGE;
	
	p.animIdx          = iAniIdx;
	p.is_stance_attack = bIsStanceAttack;

	send_packet(&p);
}

void CPacketMgr::send_attack(const _int& iAniIdx, const _vec3& vDir, const _vec3& vPos, const _float& fEndAngleY)
{
	cs_packet_attack p;

	p.size = sizeof(p);
	p.type = CS_ATTACK;

	p.animIdx = iAniIdx;
	p.posX    = vPos.x;
	p.posY    = vPos.y;
	p.posZ    = vPos.z;
	p.dirX    = vDir.x;
	p.dirY    = vDir.y;
	p.dirZ    = vDir.z;
	p.end_angleY = fEndAngleY;

	send_packet(&p);
}

void CPacketMgr::send_attack_stop(const _int& iAniIdx, const _vec3& vDir, const _vec3& vPos)
{
	cs_packet_attack p;

	p.size = sizeof(p);
	p.type = CS_ATTACK_STOP;

	p.animIdx = iAniIdx;

	p.posX    = vPos.x;
	p.posY    = vPos.y;
	p.posZ    = vPos.z;

	p.dirX    = vDir.x;
	p.dirY    = vDir.y;
	p.dirZ    = vDir.z;

	send_packet(&p);
}

void CPacketMgr::send_attackByMonster(int objID, const _int& iDamage)
{
	cs_packet_player_collision p;

	p.size   = sizeof(p);
	p.type   = CS_COLLIDE;
	p.col_id = objID;
	p.damage = iDamage;

	send_packet(&p);
}

void CPacketMgr::send_attackToMonster(int objID, const _int& iDamage)
{
	cs_packet_player_collision p;

	p.size   = sizeof(p);
	p.type   = CS_COLLIDE_MONSTER;
	p.col_id = objID;
	p.damage = iDamage;

	send_packet(&p);
}

void CPacketMgr::send_stage_change(const char& chStageId)
{
	cs_packet_stage_change p;

	p.size     = sizeof(p);
	p.type     = CS_STAGE_CHANGE;
	p.stage_id = chStageId;

	send_packet(&p);
}

void CPacketMgr::send_packet(void* packet)
{
	char* p = reinterpret_cast<char*>(packet);

	int iSendBytes = send(g_hSocket, p, p[0], 0);

#ifdef ERR_CHECK
	if (iSendBytes == SOCKET_ERROR)
	{
		error_display("send Error : ", WSAGetLastError());
		closesocket(g_hSocket);
	}
#endif 
	
}

void CPacketMgr::Free()
{
}
