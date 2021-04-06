#pragma once
/* Main Loop */
enum OPMODE
{
	OP_MODE_RECV = 0,
	OP_MODE_SEND,
	OP_MODE_ACCEPT,
	OP_RANDOM_MOVE_NPC,
	OP_MODE_CHASE_MONSTER,
	OP_MODE_END
};

/* OBJECT STATUS */
enum STATUS { ST_ACTIVE = 0, ST_NONACTIVE, ST_CHASE, ST_ATTACK, ST_DEAD, ST_END };
