#pragma once

constexpr char SERVER_IP[16]			= "127.0.0.1";
constexpr int SERVER_PORT				= 5689;

constexpr int MAX_ID_LEN				= 32;
constexpr int MAX_PW_LEN				= 32;

constexpr int MAX_STR_LEN				= 100;
constexpr int MIDDLE_STR_LEN			= 64;

constexpr int MAX_USER					= 10000;
constexpr int MAX_NPC					= 100;
constexpr int MAX_MONSTER				= 100;

constexpr int NPC_NUM_START				= 1000;
constexpr int MON_NUM_START				= 5000;

constexpr int WORLD_WIDTH				= 1000;
constexpr int WORLD_HEIGHT				= 1000;
constexpr int SECTOR_SIZE				= 40;

constexpr int VIEW_LIMIT				= 20;				

#pragma pack (push, 1)

constexpr char SC_PACKET_LOGIN_OK		= 0;
constexpr char SC_PACKET_MOVE			= 1;
constexpr char SC_PACKET_MOVE_STOP		= 2;
constexpr char SC_PACKET_ENTER			= 3;
constexpr char SC_PACKET_LEAVE			= 4;
constexpr char SC_PACKET_CHAT			= 5;
constexpr char SC_PACKET_LOGIN_FAIL		= 6;
constexpr char SC_PACKET_STAT_CHANGE	= 7;
constexpr char SC_PACKET_ATTACK			= 8;
constexpr char SC_PACKET_STANCE_CHANGE  = 9;
constexpr char SC_PACKET_NPC_MOVE		= 10;
constexpr char SC_PACKET_NPC_ENTER		= 11;
constexpr char SC_PACKET_MONSTER_ENTER	= 12;
constexpr char SC_PACKET_MONSTER_MOVE	= 13;
constexpr char SC_PACKET_MONSTER_ATTACK = 14;
constexpr char SC_PACKET_MONSTER_STAT	= 15;


constexpr char CS_LOGIN					= 0;
constexpr char CS_MOVE					= 1;
constexpr char CS_MOVE_STOP				= 2;
constexpr char CS_ATTACK				= 3;
constexpr char CS_ATTACK_STOP			= 4;
constexpr char CS_CHAT					= 5;
constexpr char CS_LOGOUT				= 6;
constexpr char CS_COLLIDE				= 7;   // Player가 다른 Object에게 충돌당했을 경우
constexpr char CS_COLLIDE_MONSTER		= 8;   // Player가 Monster 공격
constexpr char CS_STANCE_CHANGE         = 9;


/* CHARACTER TYPE */   // 추후 삭제 예정
constexpr char TYPE_PLAYER				= 0;
constexpr char TYPE_NPC					= 1;
constexpr char TYPE_MONSTER				= 2;

/* PLAYER CHARACTER TYPE */
constexpr char PC_GLADIATOR				= 0;
constexpr char PC_ARCHER				= 1;
constexpr char PC_PRIEST				= 2;

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

/* NPC NUMBER */   // 추후 NPC NUMBER 삭제 후 NPC TYPE으로 사용 할 예정
constexpr char NPC_NORMAL				= 0;
constexpr char NPC_MERCHANT				= 1;
constexpr char NPC_QUEST				= 2;
constexpr char NPC_BG					= 3;

/* ___________________________________________________________________________________________________________________*/
/*													SERVER -> CLIENT												  */
/* ___________________________________________________________________________________________________________________*/
struct sc_packet_login_ok 
{
	unsigned char	size;
	char			type;
	int				id;

	char			o_type;
	int				hp;
	int				maxHp;
	int				mp;
	int				maxMp;
	int				exp;
	int				maxExp;
	int				level;
	int				att;
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
};

struct sc_packet_enter 
{
	unsigned char	size;
	char			type;
	int				id;

	char			name[MAX_ID_LEN];
	char			o_type;
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
	int				mp;
	int				exp;
};

struct sc_packet_chat 
{
	unsigned char	size;
	char			type;
	int				id;						// teller

	char			message[MAX_STR_LEN];
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

	char			name[MAX_ID_LEN];
	char			naviType[MIDDLE_STR_LEN];
	char			o_type;
	char			npc_num;
	float			posX, posY, posZ;
	float			angleX, angleY, angleZ;
};

struct sc_packet_monster_enter
{
	unsigned char	size;
	char			type;
	int				id;

	char			name[MAX_ID_LEN];
	char			naviType[MIDDLE_STR_LEN];
	char			mon_num;
	int				Hp, maxHp;
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

/* ___________________________________________________________________________________________________________________*/
/*													CLIENT -> SERVER												  */
/* ___________________________________________________________________________________________________________________*/
struct cs_packet_login 
{
	unsigned char	size;
	char			type;

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
};

struct cs_packet_player_collision
{
	unsigned char	size;
	char			type;

	int				col_id;
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
};

struct cs_packet_teleport 
{
	unsigned char	size;
	char			type;

	float			posX, posY, posZ;
};

#pragma pack (pop)