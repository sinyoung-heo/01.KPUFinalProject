#include "stdafx.h"
#include "PacketMgr.h"
#include <random>
#include "ObjectMgr.h"
#include "Management.h"
#include "Renderer.h"
#include "InstancePoolMgr.h"
#include "PartySystemMgr.h"
#include "DynamicCamera.h"
#include "FadeInOut.h"
#include "ChattingMgr.h"
#include "InventoryEquipmentMgr.h"
#include "EquipmentItemSlot.h"
#include "StoreMgr.h"
#include "LightMgr.h"
#include "DmgFont.h"
#include "DragonEffect.h"
#include "SkyBox.h"
#include "SoundMgr.h"

/* USER */
#include "PCGladiator.h"
#include "PCOthersGladiator.h"
#include "PCArcher.h"
#include "PCOthersArcher.h"
#include "PCPriest.h"
#include "PCOthersPriest.h"
/* NPC */
#include "NPC_Walker.h"
#include "NPC_Assistant.h"
#include "NPC_Stander.h"
#include "NPC_Merchant.h"
#include "NPC_Quest.h"
#include "NPC_Children.h"
/* MONSTER */
#include "Crab.h"
#include "Monkey.h"
#include "CloderA.h"
#include "DrownedSailor.h"
#include "GiantBeetle.h"
#include "GiantMonkey.h"
#include "CraftyArachne.h"
#include "Vergos.h"

IMPLEMENT_SINGLETON(CPacketMgr)

CPacketMgr::CPacketMgr()
	: m_pObjectMgr(Engine::CObjectMgr::Get_Instance())
	, m_pManagement(Engine::CManagement::Get_Instance())
	, m_pRenderer(Engine::CRenderer::Get_Instance())
	, m_pInstancePoolMgr(CInstancePoolMgr::Get_Instance())
	, m_pPartySystemMgr(CPartySystemMgr::Get_Instance())
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
	sockAddr.sin_addr.s_addr = inet_addr(SERVER_DHIP);

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
	Sleep(1000);
	send_login();

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

	if (retval == SOCKET_ERROR)
	{
		int state = WSAGetLastError();
		if (state != WSAEWOULDBLOCK)
		{
			error_display("recv Error : ", state);
			closesocket(g_hSocket);
		}
	}

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

	case SC_PACKET_CHAT:
	{
		sc_packet_chat* packet = reinterpret_cast<sc_packet_chat*>(m_packet_start);
		Recv_Chat(packet);
	}
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
		sc_packet_update_party* packet = reinterpret_cast<sc_packet_update_party*>(m_packet_start);
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

	case SC_PACKET_MONSTER_NUCKBACK:
	{
		sc_packet_monster_knockback* packet = reinterpret_cast<sc_packet_monster_knockback*>(m_packet_start);

		bool retflag;
		Knockback_Monster(packet, retflag);
		if (retflag) return;
	}
	break;

	case SC_PACKET_SUGGEST_PARTY:
	{
		// 파티초대 제안을 받음.
		sc_packet_suggest_party* packet = reinterpret_cast<sc_packet_suggest_party*>(m_packet_start);
		Suggest_Party(packet);
	}
	break;

	case SC_PACKET_ENTER_PARTY:
	{
		// 파티초대 제안을 수락.
		sc_packet_enter_party* packet = reinterpret_cast<sc_packet_enter_party*>(m_packet_start);

		bool retflag;
		Enter_PartyMember(packet, retflag);
		if (retflag) return;
	}
	break;

	case SC_PACKET_REJECT_PARTY:
	{
		// 파티초대 or 파티가입요청 거절.
		sc_packet_chat* packet = reinterpret_cast<sc_packet_chat*>(m_packet_start);
		Reject_Party();
	}
	break;

	case SC_PACKET_JOIN_PARTY:
	{
		// 파티가입요청 받음.
		sc_packet_suggest_party* packet = reinterpret_cast<sc_packet_suggest_party*>(m_packet_start);
		Join_Party(packet);
	}
	break;

	case SC_PACKET_LEAVE_PARTY:
	{
		sc_packet_suggest_party* packet = reinterpret_cast<sc_packet_suggest_party*>(m_packet_start);

		bool retflag;
		Leave_Party(packet, retflag);
		if (retflag) return;
	}
	break;

	case SC_PACKET_UPDATE_PARTY:
	{
		sc_packet_update_party* packet = reinterpret_cast<sc_packet_update_party*>(m_packet_start);
		Update_Party(packet);
	}
	break;

	case SC_PACKET_UPDATE_INVENTORY:
	{
		sc_packet_update_inventory* packet = reinterpret_cast<sc_packet_update_inventory*>(m_packet_start);
		update_inventory(packet);
	}
	break;

	case SC_PACKET_UPDATE_EQUIPMENT:
	{
		sc_packet_update_equipment* packet = reinterpret_cast<sc_packet_update_equipment*>(m_packet_start);
		Update_Equipment(packet);
	}
	break;

	case SC_PACKET_LOAD_EQUIPMENT:
	{
		sc_packet_load_equipment* packet = reinterpret_cast<sc_packet_load_equipment*>(m_packet_start);
		LoginToLoadEqipment(packet);
	}
	break;

	case SC_PACKET_UPDATE_MONEY:
	{
		sc_packet_leave* packet = reinterpret_cast<sc_packet_leave*>(m_packet_start);
		Update_UserMoney(packet);
	}
	break;

	case SC_PACKET_BUFF:
	{
		sc_packet_buff* packet = reinterpret_cast<sc_packet_buff*>(m_packet_start);
		BuffToUpgrade(packet);
	}
	break;

	case SC_PACKET_CONSUME_POINT:
	{
		sc_packet_update_party* packet = reinterpret_cast<sc_packet_update_party*>(m_packet_start);
		Consume_Point(packet);
	}
	break;

	case SC_PACKET_DRINK_POTION:
	{
		sc_packet_potion* packet = reinterpret_cast<sc_packet_potion*>(m_packet_start);
		Drink_Potion(packet);			
	}
	break;

	case SC_PACKET_MONSTER_EFFECT:
	{
		sc_packet_monster_effect* packet = reinterpret_cast<sc_packet_monster_effect*>(m_packet_start);	
		Create_Monster_Effect(packet);
	}
	break;

	default:
#ifdef ERR_CHECK
		printf("Unknown PACKET type [%d]\n", m_packet_start[1]);
#endif 
		break;
	}
}

void CPacketMgr::Create_Monster_Effect(sc_packet_monster_effect* packet)
{
	for (int i = 0; i < 3; ++i)
	{
		_vec3 vPos = _vec3(packet->posX[i], 0.1f, packet->posZ[i]);
		CEffectMgr::Get_Instance()->Effect_WarningGround(vPos, 0.03f, true);
		/*Engine::CGameObject* pGameObj = CDragonEffect::Create(m_pGraphicDevice, m_pCommandList, 
			L"DragonEffect", _vec3(0.012f), _vec3(-180.f), vPos);

		m_pObjectMgr->Add_GameObject(L"Layer_GameObject", L"EffectDragon", pGameObj);*/
	}
}

void CPacketMgr::LoginToLoadEqipment(sc_packet_load_equipment* packet)
{
	Engine::CGameObject* pThisPlayer = m_pObjectMgr->Get_GameObject(L"Layer_GameObject", L"ThisPlayer");
	if (nullptr != pThisPlayer)
	{
		switch (pThisPlayer->Get_OType())
		{
		case PC_GLADIATOR:
			packet->equipType[0] = ItemType_WeaponTwoHand;
			break;
		case PC_ARCHER:
			packet->equipType[0] = ItemType_WeaponBow;
			break;
		case PC_PRIEST:
			packet->equipType[0] = ItemType_WeaponRod;
			break;

		}
		pThisPlayer->Set_ThisPlayerLoginEquipment(packet->equipType[0], packet->itemName[0]);
		pThisPlayer->Set_ThisPlayerLoginEquipment(ItemType_Armor, packet->itemName[1]);
		pThisPlayer->Set_ThisPlayerLoginEquipment(ItemType_Helmet, packet->itemName[2]);
		pThisPlayer->Set_ThisPlayerLoginEquipment(ItemType_Shoes, packet->itemName[3]);
	}
}

void CPacketMgr::Drink_Potion(sc_packet_potion* packet)
{
	Engine::CGameObject* pThisPlayer = m_pObjectMgr->Get_GameObject(L"Layer_GameObject", L"ThisPlayer");

	/* 인벤토리 아이템 제거 */
	CInventoryEquipmentMgr::Get_Instance()->Pop_ItemInventory(packet->itemType, packet->itemName);

	if (packet->itemName == Potion_HP)
		pThisPlayer->Set_PotionAbility(packet->ability, true);
	else
		pThisPlayer->Set_PotionAbility(packet->ability, false);
}

void CPacketMgr::Consume_Point(sc_packet_update_party* packet)
{
	Engine::CGameObject* pThisPlayer = m_pObjectMgr->Get_GameObject(L"Layer_GameObject", L"ThisPlayer");
	pThisPlayer->Set_Buff(packet->hp, packet->maxHp, packet->mp, packet->maxMp);
}

void CPacketMgr::BuffToUpgrade(sc_packet_buff* packet)
{
	Engine::CGameObject* pThisPlayer = m_pObjectMgr->Get_GameObject(L"Layer_GameObject", L"ThisPlayer");

	if (packet->priest_id != g_iSNum)
	{
		pThisPlayer->Set_Buff(packet->hp, packet->maxHp, packet->mp, packet->maxMp);
		pThisPlayer->Update_PartyMember(packet->priest_id, packet->priest_hp, packet->priest_maxHp, packet->priest_mp, packet->priest_maxMp);
	}
	
	pThisPlayer->Buff_AllPartyMemeber(packet->animIdx);
	m_pPartySystemMgr->Update_ThisPlayerPartyList();
}

void CPacketMgr::Update_UserMoney(sc_packet_leave* packet)
{
	Engine::CGameObject* pThisPlayer = m_pObjectMgr->Get_GameObject(L"Layer_GameObject", L"ThisPlayer");
	pThisPlayer->Set_Money(packet->id);
}

void CPacketMgr::Update_Equipment(sc_packet_update_equipment* packet)
{
	Engine::CGameObject* pThisPlayer = m_pObjectMgr->Get_GameObject(L"Layer_GameObject", L"ThisPlayer");

	pThisPlayer->Set_Info(pThisPlayer->Get_Info()->m_iLev,
						  packet->hp, packet->maxHp,
						  packet->mp, packet->maxMp,
						  pThisPlayer->Get_Info()->m_iExp, pThisPlayer->Get_Info()->m_iMaxExp,
						  packet->minAtt, packet->maxAtt,
						  pThisPlayer->Get_Info()->m_fSpeed,
						  pThisPlayer->Get_Info()->m_iMoney);
}

void CPacketMgr::update_inventory(sc_packet_update_inventory* packet)
{
	/* 인벤토리 아이템 추가 */
	if (packet->is_pushItem)
	{
		/* 아이템 == 포션 */
		if (packet->itemType == ItemType_Potion)
			CInventoryEquipmentMgr::Get_Instance()->Push_ItemInventory(packet->itemType, packet->itemName, packet->count);
		/* 아이템 != 포션 */
		else
			for (int i = 0; i < packet->count; ++i)
				CInventoryEquipmentMgr::Get_Instance()->Push_ItemInventory(packet->itemType, packet->itemName);
	}
	/* 인벤토리 아이템 제거 */
	else
		CInventoryEquipmentMgr::Get_Instance()->Pop_ItemInventory(packet->itemType, packet->itemName);
}

void CPacketMgr::Reject_Party()
{
	m_pPartySystemMgr->Get_PartySystemMessageCanvas()->Set_IsActive(true);
	m_pPartySystemMgr->Get_PartySystemMessageCanvas()->Set_PartyMessageState(PARTY_SYSTEM_MESSAGE::REJECT_PARTY_REQUEST);
}

void CPacketMgr::Recv_Chat(sc_packet_chat* packet)
{
	_tchar szOut[MAX_STR] = L"";
	/*____________________________________________________________________
	멀티바이트 형식을 유니코드 형식으로 바꿔주는 함수.
	______________________________________________________________________*/
	MultiByteToWideChar(CP_ACP,
						0,
						packet->message,		// 변환 할 문자열.
						packet->messageLen,
						szOut,					// 변환 값 저장 버퍼.
						MAX_STR);

	szOut[packet->messageLen] = '\0';
	CChattingMgr::Get_Instance()->Push_ChattingMessage(packet->name, szOut);
}

void CPacketMgr::Update_Party(sc_packet_update_party* packet)
{
	Engine::CGameObject* pThisPlayer = m_pObjectMgr->Get_GameObject(L"Layer_GameObject", L"ThisPlayer");
	pThisPlayer->Update_PartyMember(packet->id, packet->hp, packet->maxHp, packet->mp, packet->maxMp);

	m_pPartySystemMgr->Update_ThisPlayerPartyList();
}

void CPacketMgr::Join_Party(sc_packet_suggest_party* packet)
{
	Engine::CGameObject* pThisPlayer = m_pObjectMgr->Get_GameObject(L"Layer_GameObject", L"ThisPlayer");
	pThisPlayer->JoinRequest_Party(packet->id);

	m_pPartySystemMgr->Get_PartySuggestResponseCanvas()->Set_PartyRequestState(PARTY_REQUEST_STATE::REQUEST_PARTY_JOIN);
	m_pPartySystemMgr->Get_PartySuggestResponseCanvas()->Set_IsActive(true);
	m_pPartySystemMgr->Get_PartySuggestResponseCanvas()->Set_IsChildActive(true);
}

void CPacketMgr::Leave_Party(sc_packet_suggest_party* packet, bool& retflag)
{
	retflag = true;
	Engine::CGameObject* pThisPlayer = m_pObjectMgr->Get_GameObject(L"Layer_GameObject", L"ThisPlayer");
	pThisPlayer->Leave_PartyMember(packet->id);

	m_pPartySystemMgr->Get_PartySystemMessageCanvas()->Set_IsActive(true);
	m_pPartySystemMgr->Get_PartySystemMessageCanvas()->Set_PartyMessageState(PARTY_SYSTEM_MESSAGE::LEAVE_PARTY_MEMBER);
	m_pPartySystemMgr->SetUp_ThisPlayerPartyList();
	m_pPartySystemMgr->Update_ThisPlayerPartyList();

	// 미니맵 && 외곽선 재설정.
	pThisPlayer->SetUp_OthersIsInMyParty();

	retflag = false;
}

void CPacketMgr::Enter_PartyMember(sc_packet_enter_party* packet, bool& retflag)
{
	retflag = true;
	Engine::CGameObject* pThisPlayer = m_pObjectMgr->Get_GameObject(L"Layer_GameObject", L"ThisPlayer");
	pThisPlayer->Set_PartyState(true);	
	pThisPlayer->Enter_PartyMember(packet->id, Engine::PARTYMEMBER(packet->name, packet->o_type, packet->hp, packet->maxHp, packet->mp, packet->maxMp));

	m_pPartySystemMgr->Get_PartySystemMessageCanvas()->Set_IsActive(true);
	m_pPartySystemMgr->Get_PartySystemMessageCanvas()->Set_PartyMessageState(PARTY_SYSTEM_MESSAGE::ENTER_PARTY_MEMBER,
																			 packet->name, 
																			 packet->o_type);
	m_pPartySystemMgr->SetUp_ThisPlayerPartyList();
	m_pPartySystemMgr->Update_ThisPlayerPartyList();

	// 미니맵 && 외곽선 재설정.
	pThisPlayer->SetUp_OthersIsInMyParty();

	retflag = false;
}

void CPacketMgr::Suggest_Party(sc_packet_suggest_party* packet)
{
	Engine::CGameObject* pThisPlayer = m_pObjectMgr->Get_GameObject(L"Layer_GameObject", L"ThisPlayer");
	pThisPlayer->Request_Party(packet->id);

	m_pPartySystemMgr->Get_PartySuggestResponseCanvas()->Set_PartyRequestState(PARTY_REQUEST_STATE::REQUEST_PARTY_INVITE);
	m_pPartySystemMgr->Get_PartySuggestResponseCanvas()->Set_IsActive(true);
	m_pPartySystemMgr->Get_PartySuggestResponseCanvas()->Set_IsChildActive(true);
}

void CPacketMgr::Knockback_Monster(sc_packet_monster_knockback* packet, bool& retflag)
{
	retflag = true;
	int s_num = packet->id;

	Engine::CGameObject* pObj = m_pObjectMgr->Get_ServerObject(L"Layer_GameObject", L"MONSTER", s_num);
	if (pObj == nullptr) return;

	pObj->Set_State(packet->animIdx);
	pObj->Set_MoveStop(false);
	pObj->Ready_PositionInterpolationValue(_vec3(packet->posX, packet->posY, packet->posZ), 1.3f);
	retflag = false;
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

void CPacketMgr::Change_Monster_Stat(sc_packet_update_party* packet)
{
	int s_num = packet->id;
	Engine::CGameObject* pObj = m_pObjectMgr->Get_ServerObject(L"Layer_GameObject", L"MONSTER", s_num);
	if (pObj == nullptr) return;

	pObj->Get_Info()->m_iHp = packet->hp;
	pObj->Get_Info()->m_iExp = packet->maxHp;

	if (packet->maxMp != g_iSNum)
	{
		// DmgFont
		Engine::CGameObject* pGameObj = nullptr;
		pGameObj = Pop_Instance(CInstancePoolMgr::Get_Instance()->Get_DmgFontPool());
		if (nullptr != pGameObj)
		{
			random_device					rd;
			default_random_engine			dre{ rd() };
			uniform_int_distribution<_int>	uid{ -7, 7 };

			static_cast<CDmgFont*>(pGameObj)->Get_Transform()->m_vPos = pObj->Get_Transform()->m_vPos;
			static_cast<CDmgFont*>(pGameObj)->Get_Transform()->m_vPos.x += (_float)(uid(dre)) * 0.1f;
			static_cast<CDmgFont*>(pGameObj)->Get_Transform()->m_vPos.y += 3.0f;
			static_cast<CDmgFont*>(pGameObj)->Get_Transform()->m_vPos.z += (_float)(uid(dre)) * 0.1f;
			static_cast<CDmgFont*>(pGameObj)->Set_DamageList(packet->mp);
			static_cast<CDmgFont*>(pGameObj)->Set_DamageType(DMG_TYPE::DMG_OTHERS);
			static_cast<CDmgFont*>(pGameObj)->Set_RandomDir();
			m_pObjectMgr->Add_GameObject(L"Layer_GameObject", L"DmgFont", pGameObj);
		}
	}
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

		// Static Object && Ambient Light
		Engine::D3DLIGHT tDirLight = Engine::CLightMgr::Get_Instance()->Get_LightInfo(Engine::LIGHTTYPE::D3DLIGHT_DIRECTIONAL);
		
		// SkyBox
		CSkyBox* pSkyBox = static_cast<CSkyBox*>(m_pObjectMgr->Get_GameObject(L"Layer_Environment", L"SkyBox"));

		// Sound
		Engine::CSoundMgr::Get_Instance()->Stop_Sound(SOUNDID::SOUND_BGM);

		if (STAGE_VELIKA == packet->stage_id)
		{
			Engine::CObjectMgr::Get_Instance()->Set_CurrentStage(Engine::STAGEID::STAGE_VELIKA);
			tDirLight.Ambient.x = 0.55f;
			tDirLight.Ambient.y = 0.55f;
			tDirLight.Ambient.z = 0.55f;
			tDirLight.Ambient.w = 1.0f;
			Engine::CLightMgr::Get_Instance()->Set_LightInfo(Engine::LIGHTTYPE::D3DLIGHT_DIRECTIONAL, 0, tDirLight);

			if (nullptr != pSkyBox)
				pSkyBox->Set_TextureIdx(1);

			// Sound
			Engine::CSoundMgr::Get_Instance()->Play_BGM(L"StageVelka_Village.gpk_000008.wav");
		}
		else if (STAGE_BEACH == packet->stage_id)
		{
			Engine::CObjectMgr::Get_Instance()->Set_CurrentStage(Engine::STAGEID::STAGE_BEACH);
			tDirLight.Ambient.x = 0.55f;
			tDirLight.Ambient.y = 0.55f;
			tDirLight.Ambient.z = 0.55f;
			tDirLight.Ambient.w = 1.0f;
			Engine::CLightMgr::Get_Instance()->Set_LightInfo(Engine::LIGHTTYPE::D3DLIGHT_DIRECTIONAL, 0, tDirLight);

			if (nullptr != pSkyBox)
				pSkyBox->Set_TextureIdx(1);

			// Sound
			Engine::CSoundMgr::Get_Instance()->Play_BGM(L"StageBeach_Village.gpk_000021.wav");
		}
		else if (STAGE_WINTER == packet->stage_id)
		{
			Engine::CObjectMgr::Get_Instance()->Set_CurrentStage(Engine::STAGEID::STAGE_WINTER);
			tDirLight.Ambient.x = 0.3f;
			tDirLight.Ambient.y = 0.3f;
			tDirLight.Ambient.z = 0.3f;
			tDirLight.Ambient.w = 1.0f;
			Engine::CLightMgr::Get_Instance()->Set_LightInfo(Engine::LIGHTTYPE::D3DLIGHT_DIRECTIONAL, 0, tDirLight);

			if (nullptr != pSkyBox)
				pSkyBox->Set_TextureIdx(6);

			// Sound
			Engine::CSoundMgr::Get_Instance()->Play_BGM(L"StageWendigoCinematic_Music_DungeonEvent.gpk_000010.wav");
		}

		// Set FadeInOut
		Engine::CGameObject* pFadeInOut = *(--(m_pObjectMgr->Get_OBJLIST(L"Layer_UI", L"StageChange_FadeInOut")->end()));

		if (nullptr != pFadeInOut)
			static_cast<CFadeInOut*>(pFadeInOut)->Set_IsReceivePacket(true);
	}
	else
	{
		m_pObjectMgr->Delete_ServerObject(L"Layer_GameObject", L"Others", s_num, false);
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
	else if (packet->naviType == STAGE_BEACH)
		wstrNaviMeshTag = L"StageBeach_NaviMesh";
	
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
			wstrMeshTag = L"Popori_M_Merchant";		// 무기상인
			pInstance = Pop_Instance(m_pInstancePoolMgr->Get_NPCMerchant_Popori_M_MerchantPool());
		}
		else if (packet->npcNum == NPC_BARAKA_MERCHANT)
		{
			wstrMeshTag = L"Baraka_M_Merchant";		// 물약상인
			pInstance = Pop_Instance(m_pInstancePoolMgr->Get_NPCMerchant_Baraka_M_MerchantPool());
		}
		else if (packet->npcNum == NPC_BARAKA_MYSTELLIUM)
		{
			wstrMeshTag = L"Baraka_M_Mystellium";	// 방어구상인
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
	if (pObj == nullptr) return;

	pObj->Set_StanceChange(packet->animIdx, packet->is_stance_attack);
}

void CPacketMgr::Leave_Object(sc_packet_leave* packet, int& retflag)
{
	retflag = 1;

	if (packet->id == g_iSNum) { retflag = 2; return; };

	if (packet->id >= NPC_NUM_START && packet->id < MON_NUM_START)
		m_pObjectMgr->Delete_ServerObject(L"Layer_GameObject", L"NPC", packet->id, true);
	else if (packet->id >= MON_NUM_START && packet->id < AI_NUM_START)
		m_pObjectMgr->Delete_ServerObject(L"Layer_GameObject", L"MONSTER", packet->id, true);
	else if (packet->id >= AI_NUM_START)
		m_pObjectMgr->Delete_ServerObject(L"Layer_GameObject", L"Others", packet->id, true);
	else
		m_pObjectMgr->Delete_ServerObject(L"Layer_GameObject", L"Others", packet->id, true);
}

void CPacketMgr::AttackStop_User(sc_packet_attack* packet)
{
	int s_num = packet->id;

	/* 현재 클라이언트가 공격을 멈춘 경우 */
	if (s_num == g_iSNum)
	{
		Engine::CGameObject* pObj = m_pObjectMgr->Get_GameObject(L"Layer_GameObject", L"ThisPlayer", 0);
		if (pObj == nullptr) return;

		pObj->Get_Transform()->m_vPos = _vec3(packet->posX, packet->posY, packet->posZ);
	}
	/* 다른 클라이언트가 공격을 멈춘 경우 */
	else
	{
		Engine::CGameObject* pObj = m_pObjectMgr->Get_ServerObject(L"Layer_GameObject", L"Others", s_num);
		if (pObj == nullptr) return;

		pObj->Set_AnimationIdx(packet->animIdx);
		pObj->Set_IsStartPosInterpolation(true);
		pObj->Set_LinearPos(pObj->Get_Transform()->m_vPos, _vec3(packet->posX, packet->posY, packet->posZ));
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
		if (pObj == nullptr) return;

		pObj->Set_DeadReckoning(_vec3(packet->posX, packet->posY, packet->posZ));
	}
	/* 다른 클라이언트가 공격한 경우 */
	else
	{
		Engine::CGameObject* pObj = m_pObjectMgr->Get_ServerObject(L"Layer_GameObject", L"Others", s_num);
		if (pObj == nullptr) return;

		pObj->Set_AnimationIdx(packet->animIdx);
		pObj->Reset_AttackMoveInterpolationRatio();
		pObj->Set_DeadReckoning(_vec3(packet->posX, packet->posY, packet->posZ));

		if (-1.f != packet->end_angleY)
		{
			pObj->Set_IsStartAngleInterpolation(true);
			pObj->Set_LinearAngle(pObj->Get_Transform()->m_vAngle.y, packet->end_angleY);
		}
		
		pObj->Set_Attack(true);

		if (PC_ARCHER == pObj->Get_OType())
		{
			static_cast<CPCOthersArcher*>(pObj)->Reset_CollisionArrowDesc();
		}
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

	if (pObj == nullptr) return;

	pObj->Get_Info()->m_iHp			= packet->hp;
	pObj->Get_Info()->m_iMaxHp		= packet->maxHp;
	pObj->Get_Info()->m_iMp			= packet->mp;
	pObj->Get_Info()->m_iMaxMp		= packet->maxMp;
	pObj->Get_Info()->m_iExp		= packet->exp;
	pObj->Get_Info()->m_iMaxExp		= packet->maxExp;
	pObj->Get_Info()->m_iLev		= packet->lev;
	pObj->Get_Info()->m_iMaxAttack	= packet->maxAtt;
	pObj->Get_Info()->m_iMinAttack	= packet->minAtt;
}

void CPacketMgr::MoveStop_User(sc_packet_move* packet)
{
	int s_num = packet->id;

	/* 현재 클라이언트가 움직인 경우 */
	if (s_num == g_iSNum)
	{
		Engine::CGameObject* pObj = m_pObjectMgr->Get_GameObject(L"Layer_GameObject", L"ThisPlayer", 0);
		if (pObj == nullptr) return;

		pObj->Set_DeadReckoning(_vec3(packet->posX, packet->posY, packet->posZ));
	}

	/* 다른 클라이언트가 움직인 경우 */
	else
	{
		Engine::CGameObject* pObj = m_pObjectMgr->Get_ServerObject(L"Layer_GameObject", L"Others", s_num);
		if (pObj == nullptr) return;

		pObj->Set_AnimationIdx(packet->animIdx);
		pObj->Set_IsStartPosInterpolation(true);
		pObj->Set_LinearPos(pObj->Get_Transform()->m_vPos, _vec3(packet->posX, packet->posY, packet->posZ));
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
		if (pObj == nullptr) return;

		pObj->Set_DeadReckoning(_vec3(packet->posX, packet->posY, packet->posZ));
	}
	/* 다른 클라이언트가 움직인 경우 */
	else
	{
		Engine::CGameObject* pObj = m_pObjectMgr->Get_ServerObject(L"Layer_GameObject", L"Others", s_num);
		if (pObj == nullptr) return;

		pObj->Set_AnimationIdx(packet->animIdx);
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
	Engine::CGameObject* pInstance = nullptr;

	if (PC_GLADIATOR == packet->o_type)
		pInstance = Pop_Instance(m_pInstancePoolMgr->Get_PCOthersGladiatorPool());

	else if (PC_ARCHER == packet->o_type)
		pInstance = Pop_Instance(m_pInstancePoolMgr->Get_PCOthersArcherPool());

	else if (PC_PRIEST == packet->o_type)
		pInstance = Pop_Instance(m_pInstancePoolMgr->Get_PCOthersPriestPool());

	if (nullptr != pInstance)
	{
		pInstance->Set_OthersStance(packet->is_stance_attack);
		pInstance->Get_Transform()->m_vPos = _vec3(packet->posX, packet->posY, packet->posZ);
		pInstance->Set_WeaponType(packet->weaponType);
		pInstance->Set_CurrentStageID(packet->stageID);
		pInstance->Set_OType(packet->o_type);
		pInstance->Set_ServerNumber(packet->id);
		pInstance->Set_PartyState(packet->is_party_state);

		m_pObjectMgr->Add_GameObject(L"Layer_GameObject", L"Others", pInstance);
	}

	// 미니맵 && 외곽선 재설정.
	Engine::CGameObject* pThisPlayer = m_pObjectMgr->Get_GameObject(L"Layer_GameObject", L"ThisPlayer");
	pThisPlayer->SetUp_OthersIsInMyParty();

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
	wstring wstrNaviMeshTag = L"";

	/* NaviMesh Type */
	if (packet->naviType == STAGE_VELIKA)
		wstrNaviMeshTag = L"StageVelika_NaviMesh";
	else if (packet->naviType == STAGE_BEACH)
		wstrNaviMeshTag = L"StageBeach_NaviMesh";
	else if (packet->naviType == STAGE_WINTER)
		wstrNaviMeshTag = L"StageWinter_NaviMesh";

	/* Character Type */
	if (PC_GLADIATOR == packet->o_type)
	{
		pGameObj = CPCGladiator::Create(m_pGraphicDevice, m_pCommandList,
										L"PoporiR27Gladiator",								// MeshTag
										wstrNaviMeshTag,									// NaviMeshTag
										_vec3(0.05f, 0.05f, 0.05f),							// Scale
										_vec3(0.0f, 0.0f, 0.0f),							// Angle
										_vec3(packet->posX, packet->posY, packet->posZ),	// Pos
										packet->weaponType);
	}
	else if (PC_ARCHER == packet->o_type)
	{
		pGameObj = CPCArcher::Create(m_pGraphicDevice, m_pCommandList,
									 L"HumanPCEvent27Archer",							// MeshTag
									 wstrNaviMeshTag,									// NaviMeshTag
									 _vec3(0.05f, 0.05f, 0.05f),						// Scale
									 _vec3(0.0f, 0.0f, 0.0f),							// Angle
									 _vec3(packet->posX, packet->posY, packet->posZ),	// Pos
									 packet->weaponType);
	}
	else if (PC_PRIEST == packet->o_type)
	{
		pGameObj = CPCPriest::Create(m_pGraphicDevice, m_pCommandList,
									 L"PoporiMR25Priest",								// MeshTag
									 wstrNaviMeshTag,									// NaviMeshTag
									 _vec3(0.05f, 0.05f, 0.05f),						// Scale
									 _vec3(0.0f, 0.0f, 0.0f),							// Angle
									 _vec3(packet->posX, packet->posY, packet->posZ),	// Pos
									 packet->weaponType);
	}


	pGameObj->Set_OType(packet->o_type);
	pGameObj->Set_ServerNumber(g_iSNum);
	pGameObj->Set_Info(packet->level, packet->hp, packet->maxHp, packet->mp, packet->maxMp, packet->exp, packet->maxExp, packet->min_att, packet->max_att, (int)packet->spd, packet->money);
	
	pGameObj->Set_CurrentStageID(packet->naviType);
	// Static Object
	if (STAGE_VELIKA == packet->naviType)
		Engine::CObjectMgr::Get_Instance()->Set_CurrentStage(Engine::STAGEID::STAGE_VELIKA);
	else if (STAGE_BEACH == packet->naviType)
		Engine::CObjectMgr::Get_Instance()->Set_CurrentStage(Engine::STAGEID::STAGE_BEACH);
	else if (STAGE_WINTER == packet->naviType)
		Engine::CObjectMgr::Get_Instance()->Set_CurrentStage(Engine::STAGEID::STAGE_WINTER);

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
		if (pObj == nullptr) return;

		pObj->Set_State(packet->aniIdx);
	}

	/* MONSTER */
	else if (packet->id >= MON_NUM_START)
	{
		Engine::CGameObject* pObj = m_pObjectMgr->Get_ServerObject(L"Layer_GameObject", L"MONSTER", packet->id);
		if (pObj == nullptr) return;

		pObj->Set_State(packet->aniIdx);
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

	pObj->Set_ServerNumber(packet->id);
	pObj->Set_State(packet->animIdx);
	pObj->Get_Transform()->m_vPos = _vec3(packet->posX, packet->posY, packet->posZ);
	pObj->Ready_AngleInterpolationValue(pObj->Set_Other_Angle(_vec3(packet->dirX, packet->dirY, packet->dirZ)));
}

void CPacketMgr::Enter_Monster(sc_packet_monster_enter* packet)
{
	Engine::CGameObject* pInstance = nullptr;

	if (packet->mon_num == MON_CRAB)
	{
		pInstance = Pop_Instance(m_pInstancePoolMgr->Get_MonsterCrabPool());
	}
	else if (packet->mon_num == MON_MONKEY)
	{
		pInstance = Pop_Instance(m_pInstancePoolMgr->Get_MonsterMonkeyPool());
	}	
	else if (packet->mon_num == MON_SAILOR)
	{
		pInstance = Pop_Instance(m_pInstancePoolMgr->Get_MonsterDrownedSailorPool());
	}	
	else if (packet->mon_num == MON_GMONKEY)
	{
		pInstance = Pop_Instance(m_pInstancePoolMgr->Get_MonsterGiantMonkeyPool());
	}
	else if (packet->mon_num == MON_VERGOS)
	{
		pInstance = Pop_Instance(m_pInstancePoolMgr->Get_MonsterVergosPool());

		CDynamicCamera* pDynamicCamera = static_cast<CDynamicCamera*>(m_pObjectMgr->Get_GameObject(L"Layer_Camera", L"DynamicCamera"));
		if (nullptr != pDynamicCamera)
			pDynamicCamera->Set_Vergos(static_cast<CVergos*>(pInstance));
	}
	
	if (nullptr != pInstance)
	{	
		pInstance->Get_Transform()->m_vScale = _vec3(0.05f, 0.05f, 0.05f);
		pInstance->Get_Transform()->m_vAngle = _vec3(packet->angleX, packet->angleY, packet->angleZ);
		pInstance->Get_Transform()->m_vPos   = _vec3(packet->posX, packet->posY, packet->posZ);
		pInstance->Set_ServerNumber(packet->id);
		pInstance->Set_State(packet->animIdx);
		pInstance->Set_Info(1, packet->Hp, packet->maxHp, 0, 0, 0, 0, packet->min_att, packet->max_att, (int)packet->spd, 0);
		m_pObjectMgr->Add_GameObject(L"Layer_GameObject", L"MONSTER", pInstance);
	}
}

void CPacketMgr::send_login()
{
	cs_packet_login p;

	p.size			= sizeof(p);
	p.type			= CS_LOGIN;

	p.isMember		= g_bIsLoginID;
	p.o_type		= g_cJob;
	p.weapon_type	= Bow23_SM;

	if (p.isMember == false)
	{
		int t_id = GetCurrentProcessId();
		sprintf_s(p.name, "P%03d", t_id % 1000);
		sprintf_s(p.password, "%03d", t_id % 1000);
	}
	else
	{
		char name[MAX_STR] = "";
		char password[MAX_STR] = "";

		WideCharToMultiByte(CP_ACP,
							0,
							g_szID,			// 변환 할 문자열.
							lstrlen(g_szID),
							name,			// 변환 값 저장 버퍼.
							MAX_STR,
							NULL,
							NULL);

		WideCharToMultiByte(CP_ACP,
							0,
							g_szPWD,			// 변환 할 문자열.
							lstrlen(g_szPWD),
							password,			// 변환 값 저장 버퍼.
							MAX_STR,
							NULL,
							NULL);

		cout << "ID: " << name << endl;
		cout << "PW: " << password << endl;



		strncpy_s(p.name, name, strlen(name));
		strncpy_s(p.password, password, strlen(password));
	}

	MultiByteToWideChar(CP_ACP,
						0,
						p.name,			// 변환 할 문자열.
						strlen(p.name),
						g_szPlayerName,	// 변환 값 저장 버퍼.
						MAX_STR);

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

void CPacketMgr::send_buff(const _int& iAniIdx, const _vec3& vDir, const _vec3& vPos, const _float& fEndAngleY)
{
	cs_packet_attack p;

	p.size = sizeof(p);
	p.type = CS_BUFF;

	p.animIdx = iAniIdx;
	p.posX = vPos.x;
	p.posY = vPos.y;
	p.posZ = vPos.z;
	p.dirX = vDir.x;
	p.dirY = vDir.y;
	p.dirZ = vDir.z;
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

void CPacketMgr::send_attackToMonster(int objID, const _int& iDamage, const char& affect)
{
	cs_packet_player_collision p;

	p.size   = sizeof(p);
	p.type   = CS_COLLIDE_MONSTER;

	p.col_id = objID;
	p.damage = iDamage;
	p.affect = affect;

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

void CPacketMgr::send_suggest_party(const int& others_id)
{
	cs_packet_suggest_party p;

	p.size		= sizeof(p);
	p.type		= CS_SUGGEST_PARTY;
	p.member_id = others_id;

	send_packet(&p);
}

void CPacketMgr::send_respond_party(const bool& result, const int& suggester_id)
{
	cs_packet_respond_party p;

	p.size			= sizeof(p);
	p.type			= CS_RESPOND_PARTY;

	p.result		= result;
	p.suggester_id	= suggester_id;

	send_packet(&p);
}

void CPacketMgr::send_join_party(const int& others_id)
{
	cs_packet_suggest_party p;

	p.size		= sizeof(p);
	p.type		= CS_JOIN_PARTY;
	p.member_id = others_id;

	send_packet(&p);
}

void CPacketMgr::send_decide_party(const bool& result, const int& joinner_id)
{
	cs_packet_respond_party p;

	p.size			= sizeof(p);
	p.type			= CS_DECIDE_PARTY;

	p.result		= result;
	p.suggester_id	= joinner_id;

	send_packet(&p);
}

void CPacketMgr::send_leave_party(const int& myId)
{
	cs_packet_suggest_party p;

	p.size = sizeof(p);
	p.type = CS_LEAVE_PARTY;
	p.member_id = myId;

	send_packet(&p);
}

void CPacketMgr::send_logout()
{
	Engine::CGameObject* pThisPlayer = m_pObjectMgr->Get_GameObject(L"Layer_GameObject", L"ThisPlayer");

	cs_packet_logout p;

	p.size			= sizeof(p);
	p.type			= CS_LOGOUT;

	p.posX			= pThisPlayer->Get_Transform()->m_vPos.x;
	p.posY			= pThisPlayer->Get_Transform()->m_vPos.y;
	p.posZ			= pThisPlayer->Get_Transform()->m_vPos.z;
	p.level			= pThisPlayer->Get_Info()->m_iLev;
	p.hp			= pThisPlayer->Get_Info()->m_iHp;
	p.maxHp			= pThisPlayer->Get_Info()->m_iMaxHp;
	p.mp			= pThisPlayer->Get_Info()->m_iMp;
	p.maxMp			= pThisPlayer->Get_Info()->m_iMaxMp;
	p.exp			= pThisPlayer->Get_Info()->m_iExp;
	p.maxExp		= pThisPlayer->Get_Info()->m_iMaxExp;
	p.minAtt		= pThisPlayer->Get_Info()->m_iMinAttack;
	p.maxAtt		= pThisPlayer->Get_Info()->m_iMaxAttack;
	p.stageID		= pThisPlayer->Get_CurrentStageID();
	p.weaponType	= pThisPlayer->Get_WeaponType();

	send_packet(&p);
}

void CPacketMgr::send_chat(const wchar_t* message)
{
	cs_packet_chat p;

	p.size = sizeof(p);
	p.type = CS_CHAT;

	WideCharToMultiByte(CP_ACP,
						0,
						message,			// 변환 할 문자열.
						lstrlen(message) * 2, 
						p.message,         // 변환 값 저장 버퍼.
						MAX_STR,
						NULL,
						NULL);

	p.message[lstrlen(message) * 2] = '\0';

	send_packet(&p);
}

void CPacketMgr::send_add_item(const char& chItemType, const char& chName)
{
	cs_packet_manage_inventory p;

	p.size		= sizeof(p);
	p.type		= CS_ADD_ITEM;

	p.itemType	= chItemType;
	p.itemName	= chName;

	send_packet(&p);
}

void CPacketMgr::send_delete_item(const char& chItemType, const char& chName)
{
	cs_packet_manage_inventory p;

	p.size		= sizeof(p);
	p.type		= CS_DELETE_ITEM;

	p.itemType	= chItemType;
	p.itemName	= chName;

	send_packet(&p);
}

void CPacketMgr::send_equip_item(const char& chSlotType, const char& chItemType, const char& chName)
{
	cs_packet_manage_inventory p;

	p.size			= sizeof(p);
	p.type			= CS_EQUIP_ITEM;

	p.itemSlotType	= chSlotType;
	p.itemType		= chItemType;
	p.itemName		= chName;

	send_packet(&p);
}

void CPacketMgr::send_unequip_item(const char& chSlotType, const char& chItemType, const char& chName)
{
	cs_packet_manage_inventory p;

	p.size			= sizeof(p);
	p.type			= CS_UNEQUIP_ITEM;

	p.itemSlotType	= chSlotType;
	p.itemType		= chItemType;
	p.itemName		= chName;

	send_packet(&p);
}

void CPacketMgr::send_deal_shop(vector<CStoreBuyListSlot*>& buyList, vector<CStoreSellListSlot*>& sellList)
{
	cs_packet_shop p;

	p.size = sizeof(p);
	p.type = CS_SHOP;

	// 리스트에서 대입 후 전송
	for (_int i = 0; i < SHOP_SLOT; ++i)
	{
		p.buyItemType[i]  = buyList[i]->Get_CurItemInfo().chItemType;
		p.buyItemName[i]  = buyList[i]->Get_CurItemInfo().chItemName;
		p.buyItemCount[i] = buyList[i]->Get_CurItemCnt();

		p.sellItemType[i]  = sellList[i]->Get_CurItemInfo().chItemType;
		p.sellItemName[i]  = sellList[i]->Get_CurItemInfo().chItemName;
		p.sellItemCount[i] = sellList[i]->Get_CurItemCnt();
	}

	send_packet(&p);
}

void CPacketMgr::send_use_potion(const bool& bIsPotionHP)
{
	cs_packet_potion p;

	p.size = sizeof(p);
	p.type = CS_DRINK_POTION;

	p.bIsPotionHP = bIsPotionHP;

	send_packet(&p);
}

void CPacketMgr::send_end_cinema()
{
	cs_packet_potion p;

	p.size = sizeof(p);
	p.type = CS_END_CINEMA;

	p.bIsPotionHP = true;

	send_packet(&p);
}

void CPacketMgr::send_packet(void* packet)
{
	char* p = reinterpret_cast<char*>(packet);

	int iSendBytes = send(g_hSocket, p, p[0], 0);

	if (iSendBytes == SOCKET_ERROR)
	{
		error_display("send Error : ", WSAGetLastError());
		closesocket(g_hSocket);
	}
}

void CPacketMgr::Free()
{
}
