#pragma once

constexpr char SERVER_IP[16]			= "127.0.0.1";
constexpr char SERVER_DHIP[16]			= "192.168.0.19";
constexpr int SERVER_PORT				= 5689;

constexpr int MAX_ID_LEN				= 32;
constexpr int MAX_PW_LEN				= 32;

constexpr int MAX_STR_LEN				= 100;
constexpr int MIDDLE_STR_LEN			= 64;

constexpr int MAX_USER					= 10000;
constexpr int MAX_NPC					= 50;
constexpr int MAX_MONSTER				= 50;
constexpr int MAX_AI					= 10;

constexpr int NPC_NUM_START				= 1000;
constexpr int MON_NUM_START				= 5000;
constexpr int AI_NUM_START				= 9000;

constexpr int WORLD_WIDTH				= 1000;
constexpr int WORLD_HEIGHT				= 1000;
constexpr int SECTOR_SIZE				= 40;

constexpr int VIEW_LIMIT				= 20;				

#pragma pack (push, 1)

constexpr char SC_PACKET_LOGIN_OK			= 0;
constexpr char SC_PACKET_MOVE				= 1;
constexpr char SC_PACKET_MOVE_STOP			= 2;
constexpr char SC_PACKET_ENTER				= 3;
constexpr char SC_PACKET_LEAVE				= 4;
constexpr char SC_PACKET_CHAT				= 5;
constexpr char SC_PACKET_LOGIN_FAIL			= 6;
constexpr char SC_PACKET_STAT_CHANGE		= 7;
constexpr char SC_PACKET_ATTACK				= 8;
constexpr char SC_PACKET_ATTACK_STOP		= 9;
constexpr char SC_PACKET_STANCE_CHANGE		= 10;
constexpr char SC_PACKET_NPC_MOVE			= 11;
constexpr char SC_PACKET_NPC_ENTER			= 12;
constexpr char SC_PACKET_MONSTER_ENTER		= 13;
constexpr char SC_PACKET_MONSTER_MOVE		= 14;
constexpr char SC_PACKET_MONSTER_ATTACK		= 15;
constexpr char SC_PACKET_MONSTER_RUSH		= 16;
constexpr char SC_PACKET_MONSTER_STAT		= 17;
constexpr char SC_PACKET_MONSTER_DEAD		= 18;
constexpr char SC_PACKET_STAGE_CHANGE		= 19;
constexpr char SC_PACKET_ANIM_INDEX			= 20;
constexpr char SC_PACKET_MONSTER_NUCKBACK	= 21;
constexpr char SC_PACKET_SUGGEST_PARTY		= 22;
constexpr char SC_PACKET_ENTER_PARTY		= 23;
constexpr char SC_PACKET_REJECT_PARTY		= 24;
constexpr char SC_PACKET_JOIN_PARTY			= 25;
constexpr char SC_PACKET_LEAVE_PARTY		= 26;
constexpr char SC_PACKET_UPDATE_PARTY		= 27;
constexpr char SC_PACKET_UPDATE_INVENTORY	= 28;
constexpr char SC_PACKET_UPDATE_EQUIPMENT	= 29;
constexpr char SC_PACKET_LOAD_EQUIPMENT		= 30;
constexpr char SC_PACKET_UPDATE_MONEY		= 31;
constexpr char SC_PACKET_BUFF				= 32;
constexpr char SC_PACKET_DRINK_POTION		= 33;
constexpr char SC_PACKET_CONSUME_POINT		= 34;

constexpr char CS_LOGIN					= 0;
constexpr char CS_MOVE					= 1;
constexpr char CS_MOVE_STOP				= 2;
constexpr char CS_ATTACK				= 3;
constexpr char CS_ATTACK_STOP			= 4;
constexpr char CS_CHAT					= 5;
constexpr char CS_LOGOUT				= 6;
constexpr char CS_COLLIDE				= 7;	// Player가 다른 Object에게 충돌당했을 경우
constexpr char CS_COLLIDE_MONSTER		= 8;	// Player가 Monster 공격
constexpr char CS_STANCE_CHANGE         = 9;
constexpr char CS_STAGE_CHANGE			= 10;
constexpr char CS_SUGGEST_PARTY         = 11;	// 파티 초대
constexpr char CS_RESPOND_PARTY			= 12;	// 파티 초대에 대한 응답
constexpr char CS_JOIN_PARTY			= 13;	// 파티 가입 신청
constexpr char CS_DECIDE_PARTY			= 14;	// 파티 가입 신청에 대한 응답
constexpr char CS_LEAVE_PARTY			= 15;	// 파티 탈퇴
constexpr char CS_ADD_ITEM				= 16;	// 아이템 획득
constexpr char CS_DELETE_ITEM			= 17;	// 아이템 제거
constexpr char CS_EQUIP_ITEM			= 18;	// 장비 장착
constexpr char CS_UNEQUIP_ITEM			= 19;	// 장비 해체
constexpr char CS_SHOP					= 20;	// 상점에서 아이템 거래
constexpr char CS_BUFF					= 21;	// 버프 스킬 사용
constexpr char CS_DRINK_POTION			= 22;	// 포션 사용

// Stage ID
constexpr char STAGE_VELIKA				= 0;
constexpr char STAGE_BEACH				= 1;
constexpr char STAGE_WINTER				= 2;

// Stage StartPos
constexpr float STAGE_VELIKA_X			= 130.0f;
constexpr float STAGE_VELIKA_Z			= 70.0f;
constexpr float STAGE_BEACH_X			= 80.0f + 256.0f;
constexpr float STAGE_BEACH_Z			= 80.0f + 0.0f;
constexpr float STAGE_WINTER_X			= 380.0f;
constexpr float STAGE_WINTER_Z			= 330.0f;

/* CHARACTER TYPE */   // 추후 삭제 예정
constexpr char TYPE_PLAYER				= 0;

/* PLAYER CHARACTER TYPE */
constexpr char PC_GLADIATOR				= 0;
constexpr char PC_ARCHER				= 1;
constexpr char PC_PRIEST				= 2;
constexpr char PC_ALL					= 3;

/* MONSTER TYPE */
constexpr char MON_NORMAL				= 0;
constexpr char MON_EPIC					= 1;
constexpr char MON_BOSS					= 2;

/* MONSTER NUMBER */
constexpr char MON_CRAB					= 1;
constexpr char MON_MONKEY				= 2;
constexpr char MON_CLODER				= 3;
constexpr char MON_ARACHNE				= 4;
constexpr char MON_SAILOR				= 5;
constexpr char MON_GBEETLE				= 6;
constexpr char MON_GMONKEY				= 7;

/* NPC TYPE */   
constexpr char NPC_MOVE					= 0;
constexpr char NPC_NONMOVE				= 1;
constexpr char NPC_MERCHANT				= 2;
constexpr char NPC_QUEST				= 3;

/* NPC NUMBER */
constexpr char NPC_CAT					= 0;
constexpr char NPC_CHICKEN				= 1;
constexpr char NPC_HUMAN_BOY			= 2;
constexpr char NPC_HUMAN_GIRL			= 3;
constexpr char NPC_HIGHELF_GIRL			= 4;
constexpr char NPC_AMAN_BOY				= 5;

constexpr char NPC_VILLAGERS			= 6;
constexpr char NPC_POPORI_BOY			= 7;
constexpr char NPC_BARAKA_EXTRACTOR		= 8;

constexpr char NPC_POPORI_MERCHANT		= 9;
constexpr char NPC_BARAKA_MERCHANT		= 10;
constexpr char NPC_BARAKA_MYSTELLIUM	= 11;

constexpr char NPC_CASTANIC_LSMITH		= 12;

/* PLAYER SKILL AFFECT */
constexpr char AFFECT_FINCH				= 0;
constexpr char AFFECT_GROGGY			= 1;
constexpr char AFFECT_KNOCKBACK			= 2;

/* EQUIPMENT TYPE */
constexpr char EQUIP_WEAPON				= 0;
constexpr char EQUIP_ARMOR				= 1;
constexpr char EQUIP_HELMET				= 2;
constexpr char EQUIP_SHOES				= 3;
constexpr char EQUIP_END				= 4;

/* SHOP */
constexpr int  SHOP_SLOT					= 12;

/* ___________________________________________________________________________________________________________________*/
/*													SERVER -> CLIENT												  */
/* ___________________________________________________________________________________________________________________*/
struct sc_packet_login_ok 
{
	unsigned char	size;
	char			type;
	int				id;

	char			o_type;
	char			weaponType;
	char			naviType;

	int				money;
	int				hp;
	int				maxHp;
	int				mp;
	int				maxMp;
	int				exp;
	int				maxExp;
	int				level;
	int				min_att;
	int				max_att;
	float			spd;
	float			posX, posY, posZ;
};

struct sc_packet_move 
{
	unsigned char	size;
	char			type;
	int				id;

	int				animIdx;
	int				move_time;
	float			spd;
	float			posX, posY, posZ;
	float			dirX, dirY, dirZ;
};

struct sc_packet_attack
{
	unsigned char	size;
	char			type;
	int				id;

	char			o_type;
	int				animIdx;
	float			posX, posY, posZ;
	float			dirX, dirY, dirZ;
	float			end_angleY;
};

struct sc_packet_enter 
{
	unsigned char	size;
	char			type;
	int				id;

	char			name[MAX_ID_LEN];
	char			stageID;
	char			o_type;
	char			weaponType;
	bool			is_stance_attack;
	bool			is_party_state;
	float			posX, posY, posZ;
	float			dirX, dirY, dirZ;
};

struct sc_packet_leave 
{
	unsigned char	size;
	char			type;
	int				id;
};

struct sc_packet_stat_change 
{
	unsigned char	size;
	char			type;
	int				id;

	int				hp;
	int				maxHp;
	int				mp;
	int				maxMp;
	int				exp;
	int				maxExp;
	int				lev;
	int				maxAtt, minAtt;
};

struct sc_packet_buff
{
	unsigned char	size;
	char			type;
	int				priest_id;

	int				hp;
	int				maxHp;
	int				mp;
	int				maxMp;

	int				priest_hp;
	int				priest_maxHp;
	int				priest_mp;
	int				priest_maxMp;

	int				animIdx;
};

struct sc_packet_chat 
{
	unsigned char   size;
	char			type;
	int				id;                  // teller

	char			name[MAX_ID_LEN];
	char			message[MAX_STR_LEN];
	int				messageLen;
};

struct sc_packet_login_fail 
{
	unsigned char	size;
	char			type;
	int				id;

	char			message[MAX_STR_LEN];
};

struct sc_packet_stance_change
{
	unsigned char	size;
	char			type;
	int				id;

	char			o_type;
	int				animIdx;
	bool			is_stance_attack;
};

struct sc_packet_npc_enter
{
	unsigned char	size;
	char			type;
	int				id;

	char			naviType;
	char			o_type;
	char			npcNum;
	float			posX, posY, posZ;
	float			angleX, angleY, angleZ;
};

struct sc_packet_monster_enter
{
	unsigned char	size;
	char			type;
	int				id;

	char			naviType;
	char			o_type;
	char			mon_num;
	int				animIdx;
	int				Hp, maxHp, min_att, max_att;
	float			spd;
	float			posX, posY, posZ;
	float			angleX, angleY, angleZ;
};

struct sc_packet_monster_attack
{
	unsigned char	size;
	char			type;
	int				id;

	int				animIdx;
	float			dirX, dirY, dirZ;
};

struct sc_packet_monster_rushAttack
{
	unsigned char	size;
	char			type;
	int				id;

	int				animIdx;
	float			posX, posY, posZ;
	float			dirX, dirY, dirZ;
};

struct sc_packet_monster_knockback
{
	unsigned char	size;
	char			type;
	int				id;

	int				animIdx;
	float			posX, posY, posZ;
};

struct sc_packet_stage_change
{
	unsigned char	size;
	char			type;
	int				id;

	char			stage_id;
	float			posX, posY, posZ;
};

struct sc_packet_animationIndex
{
	unsigned char	size;
	char			type;
	int				id;

	char			aniIdx;
};

struct sc_packet_suggest_party
{
	unsigned char	size;
	char			type;
	int				id;
};

struct sc_packet_enter_party
{
	unsigned char	size;
	char			type;
	int				id;

	char			name[MAX_ID_LEN];
	char			o_type;
	int				hp, maxHp;
	int				mp, maxMp;
};

struct sc_packet_update_party
{
	unsigned char	size;
	char			type;
	int				id;

	int				hp, maxHp;
	int				mp, maxMp;
};

struct sc_packet_update_inventory
{
	unsigned char	size;
	char			type;

	char			itemType;
	char			itemName;
	bool			is_pushItem;
	int				count;
};

struct sc_packet_update_equipment
{
	unsigned char	size;
	char			type;

	char			equipType;
	char			itemName;
	bool			is_pushItem;
	int				minAtt, maxAtt;
	int				hp, maxHp;
	int				mp, maxMp;
};

struct sc_packet_load_equipment
{
	unsigned char	size;
	char			type;

	char			equipType[EQUIP_END];
	char			itemName[EQUIP_END];
};

struct sc_packet_potion
{
	unsigned char	size;
	char			type;

	char			itemType;
	char			itemName;
	bool			is_pushItem;
	int				count;
	int				ability;
};

/* ___________________________________________________________________________________________________________________*/
/*													CLIENT -> SERVER												  */
/* ___________________________________________________________________________________________________________________*/
struct cs_packet_login 
{
	unsigned char	size;
	char			type;

	bool			isMember;
	char			weapon_type;
	char			o_type;
	char			name[MAX_ID_LEN];
	char			password[MAX_PW_LEN];
};

struct cs_packet_move 
{
	unsigned char	size;
	char			type;

	int				animIdx;
	int				move_time; 
	float			dirX, dirY, dirZ;
	float			posX, posY, posZ;
};

struct cs_packet_move_stop
{
	unsigned char	size;
	char			type;

	int				animIdx;
	float			posX, posY, posZ;
	float			dirX, dirY, dirZ;
};

struct cs_packet_stance_change
{
	unsigned char	size;
	char			type;
	
	int				animIdx;
	bool			is_stance_attack;
};

struct cs_packet_attack 
{
	unsigned char	size;
	char			type;

	int				animIdx;
	float			posX, posY, posZ;
	float			dirX, dirY, dirZ;
	float			end_angleY;
};

struct cs_packet_player_collision
{
	unsigned char	size;
	char			type;

	char			affect;
	int				col_id;
	int				damage;
};

struct cs_packet_chat 
{
	unsigned char	size;
	char			type;

	char			message[MAX_STR_LEN];
};

struct cs_packet_logout 
{
	unsigned char	size;
	char			type;

	char			stageID;
	char			weaponType;
	int				level;
	int				hp, maxHp;
	int				mp, maxMp;
	int				exp, maxExp;
	int				minAtt, maxAtt;
	float			posX, posY, posZ;
};

struct cs_packet_teleport 
{
	unsigned char	size;
	char			type;

	float			posX, posY, posZ;
};

struct cs_packet_stage_change
{
	unsigned char	size;
	char			type;

	char			stage_id;
};

struct cs_packet_suggest_party
{
	unsigned char	size;
	char			type;

	int				member_id;
};

struct cs_packet_respond_party
{
	unsigned char	size;
	char			type;

	bool			result;
	int				suggester_id;
};

struct cs_packet_manage_inventory
{
	unsigned char	size;
	char			type;

	char			itemSlotType;
	char			itemType;
	char			itemName;
};

struct cs_packet_shop
{
	unsigned char	size;
	char			type;

	char			buyItemType[SHOP_SLOT];
	char			buyItemName[SHOP_SLOT];
	char			buyItemCount[SHOP_SLOT];

	char			sellItemType[SHOP_SLOT];
	char			sellItemName[SHOP_SLOT];
	char			sellItemCount[SHOP_SLOT];
};

struct cs_packet_potion
{
	unsigned char	size;
	char			type;

	bool			bIsPotionHP;
};

#pragma pack (pop)