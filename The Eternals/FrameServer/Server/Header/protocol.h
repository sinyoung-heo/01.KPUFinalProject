#pragma once

constexpr char SERVER_IP[16] = "127.0.0.1";
constexpr int SERVER_PORT = 5689;

constexpr int MAX_ID_LEN = 32;
constexpr int MAX_PW_LEN = 32;

constexpr int MAX_STR_LEN = 100;
constexpr int MIDDLE_STR_LEN = 64;

constexpr int MAX_USER = 10000;
constexpr int MAX_NPC = 100;
constexpr int MAX_MONSTER = 100;

constexpr int NPC_NUM_START = 1000;
constexpr int MON_NUM_START = 5000;

constexpr int WORLD_WIDTH = 1000;
constexpr int WORLD_HEIGHT = 1000;
constexpr int SECTOR_SIZE = 40;

constexpr int VIEW_LIMIT = 20;				

#pragma pack (push, 1)

constexpr char SC_PACKET_LOGIN_OK = 0;
constexpr char SC_PACKET_MOVE = 1;
constexpr char SC_PACKET_MOVE_STOP = 2;
constexpr char SC_PACKET_ENTER = 3;
constexpr char SC_PACKET_LEAVE = 4;
constexpr char SC_PACKET_CHAT = 5;
constexpr char SC_PACKET_LOGIN_FAIL = 6;
constexpr char SC_PACKET_STAT_CHANGE = 7;
constexpr char SC_PACKET_NPC_MOVE = 8;
constexpr char SC_PACKET_NPC_ENTER = 9;
constexpr char SC_PACKET_MONSTER_ENTER = 10;
constexpr char SC_PACKET_MONSTER_MOVE = 11;
constexpr char SC_PACKET_MONSTER_ATTACK = 12;

constexpr char CS_LOGIN = 0;
constexpr char CS_MOVE = 1;
constexpr char CS_MOVE_STOP = 2;
constexpr char CS_ATTACK = 3;
constexpr char CS_CHAT = 4;
constexpr char CS_LOGOUT = 5;
constexpr char CS_COLLIDE = 6;

struct sc_packet_login_ok 
{
	unsigned char size;
	char type;
	int  id;

	int hp;
	int maxHp;
	int mp;
	int maxMp;
	int exp;
	int maxExp;
	int level;
	int att;
	float spd;
	float posX, posY, posZ;
};

constexpr char MV_FRONT = 0;
constexpr char MV_BACK = 1;
constexpr char MV_RIGHT = 2;
constexpr char MV_RIGHT_UP = 3;
constexpr char MV_RIGHT_DOWN = 4;
constexpr char MV_LEFT = 5;
constexpr char MV_LEFT_UP = 6;
constexpr char MV_LEFT_DOWN = 7;

struct sc_packet_move 
{
	unsigned char size;
	char type;
	int id;

	int move_time;
	float posX, posY, posZ;
	float dirX, dirY, dirZ;
};

/* CHARACTER TYPE */
constexpr char TYPE_PLAYER = 0;
constexpr char TYPE_NPC = 1;
constexpr char TYPE_MONSTER = 2;

/*____________________________________________________________________
NPC NUMBER
______________________________________________________________________*/
constexpr char NPC_NORMAL = 0;
constexpr char NPC_MERCHANT = 1;
constexpr char NPC_QUEST = 2;
constexpr char NPC_BG = 3;

/*____________________________________________________________________
MONSTER NUMBER
______________________________________________________________________*/
constexpr char MON_NORMAL = 0;
constexpr char MON_CHASE = 1;
constexpr char MON_BOSS = 2;

struct sc_packet_enter 
{
	unsigned char size;
	char type;
	int  id;

	char name[MAX_ID_LEN];
	char o_type;
	float posX, posY, posZ;
	float dirX, dirY, dirZ;
};

struct sc_packet_npc_enter
{
	unsigned char size;
	char type;
	int  id;

	char name[MAX_ID_LEN];
	char naviType[MIDDLE_STR_LEN];
	char o_type;
	char npc_num;
	float posX, posY, posZ;
	float angleX, angleY, angleZ;
};

struct sc_packet_monster_enter
{
	unsigned char size;
	char type;
	int  id;

	char name[MAX_ID_LEN];
	char naviType[MIDDLE_STR_LEN];
	char mon_num;
	int Hp, maxHp;
	float posX, posY, posZ;
	float angleX, angleY, angleZ;
};

struct sc_packet_leave 
{
	unsigned char size;
	char type;
	int  id;
};

struct sc_packet_chat 
{
	unsigned char  size;
	char  type;
	int	  id;			// teller
	char  message[MAX_STR_LEN];
};

struct sc_packet_login_fail 
{
	unsigned char  size;
	char  type;
	int	  id;
	char  message[MAX_STR_LEN];
};

struct sc_packet_stat_change 
{
	unsigned char size;
	char type;

	int		id;
	int		hp;
	int		mp;
	int		exp;
};

struct sc_packet_monster_attack
{
	unsigned char size;
	char type;

	int		id;
};

struct cs_packet_login 
{
	unsigned char  size;
	char  type;
	char  name[MAX_ID_LEN];
	char  password[MAX_PW_LEN];
};

struct cs_packet_move 
{
	unsigned char  size;
	char  type;

	int	  move_time; 
	float dirX, dirY, dirZ;
	float posX, posY, posZ;
};

struct cs_packet_move_stop
{
	unsigned char  size;
	char  type;

	float posX, posY, posZ;
	float dirX, dirY, dirZ;
};

struct cs_packet_attack 
{
	unsigned char	size;
	char	type;
};

struct cs_packet_player_collision
{
	unsigned char	size;
	char	type;

	int col_id;
};

struct cs_packet_chat 
{
	unsigned char	size;
	char	type;
	char	message[MAX_STR_LEN];
};

struct cs_packet_logout 
{
	unsigned char	size;
	char	type;
};

struct cs_packet_teleport 
{
	unsigned char size;
	char type;
	short x, y;
};

#pragma pack (pop)