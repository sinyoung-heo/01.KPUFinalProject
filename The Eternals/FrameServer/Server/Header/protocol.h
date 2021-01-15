#pragma once

constexpr int SERVER_PORT = 5689;
constexpr int MAX_ID_LEN = 10;
constexpr int MAX_PW_LEN = 16;
constexpr int MAX_USER = 10000;
constexpr int WORLD_WIDTH = 2000;
constexpr int WORLD_HEIGHT = 2000;
constexpr int MAX_STR_LEN = 100;
constexpr int VIEW_LIMIT = 200;				// 시야 반지름, 상대방과 사이에 6개의 타일이 있어도 보여야 함.

constexpr int NUM_NPC = 5000;

#pragma pack (push, 1)

constexpr char SC_PACKET_LOGIN_OK = 0;
constexpr char SC_PACKET_MOVE = 1;
constexpr char SC_PACKET_ENTER = 2;
constexpr char SC_PACKET_LEAVE = 3;
constexpr char SC_PACKET_CHAT = 4;
constexpr char SC_PACKET_LOGIN_FAIL = 5;
constexpr char SC_PACKET_STAT_CHANGE = 6;

constexpr char CS_LOGIN = 0;
constexpr char CS_MOVE = 1;
constexpr char CS_ATTACK = 2;
constexpr char CS_CHAT = 3;
constexpr char CS_LOGOUT = 4;
constexpr char CS_TELEPORT = 5;				// 부하 테스트용 동접 테스트를 위해 텔러포트로 Hot Spot 해소


struct sc_packet_login_ok 
{
	char size;
	char type;
	int  id;

	int hp;
	int maxHp;
	int exp;
	int maxExp;
	int level;
	int att;
	float spd;
	float posX, posY, posZ;
	float dirX, dirY, dirZ;
};

struct sc_packet_move 
{
	char size;
	char type;
	int id;

	int move_time;
	float posX, posY, posZ;
	float dirX, dirY, dirZ;
};

/* PROTOCOL 추가 확장 */
constexpr char TYPE_PLAYER = 0;
constexpr char TYPE_ORC = 1;				// AGRO + ROAMING MOVE
constexpr char TYPE_ELF = 2;				// PEACE + NO MOVE
constexpr char TYPE_BOSS = 3;				// AGRO + ROAMING MOVE
constexpr char TYPE_NORMAL = 4;				// PEACE + ROAMING MOVE

struct sc_packet_enter 
{
	char size;
	char type;
	int  id;

	char name[MAX_ID_LEN];
	char o_type;
	float posX, posY, posZ;
	float dirX, dirY, dirZ;
};

struct sc_packet_leave 
{
	char size;
	char type;
	int  id;
};

struct sc_packet_chat 
{
	char  size;
	char  type;
	int	  id;			// teller
	char  message[MAX_STR_LEN];
};

struct sc_packet_login_fail 
{
	char  size;
	char  type;
	int	  id;
	char  message[MAX_STR_LEN];
};

struct sc_packet_stat_change 
{
	char size;
	char type;
	int  id;
	short hp;
	short level;
	int   exp;
};


struct cs_packet_login 
{
	char  size;
	char  type;
	char  name[MAX_ID_LEN];
	char  password[MAX_PW_LEN];
};

constexpr char MV_UP = 0;
constexpr char MV_DOWN = 1;
constexpr char MV_LEFT = 2;
constexpr char MV_RIGHT = 3;

struct cs_packet_move 
{
	char  size;
	char  type;

	int	  move_time;
	float posX, posY, posZ;
	float dirX, dirY, dirZ;
};

struct cs_packet_attack 
{
	char	size;
	char	type;
};

struct cs_packet_chat 
{
	char	size;
	char	type;
	char	message[MAX_STR_LEN];
};

struct cs_packet_logout 
{
	char	size;
	char	type;
};

struct cs_packet_teleport 
{
	char size;
	char type;
	short x, y;
};

#pragma pack (pop)