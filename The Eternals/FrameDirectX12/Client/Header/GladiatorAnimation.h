#pragma once
namespace Gladiator
{
	const int NONE_ATTACK_IDLE  = 0;
	const int NONE_ATTACK_WALK  = 1;
	const int ATTACK_WAIT       = 2;
	const int IN_WEAPON         = 3;
	const int OUT_WEAPON        = 4;
	const int ATTACK_RUN        = 5;
	const int COMBO1            = 6;
	const int COMBO1R           = 7;
	const int COMBO2            = 8;
	const int COMBO2R           = 9;
	const int COMBO3            = 10;
	const int COMBO3R           = 11;
	const int COMBO4            = 12;
	const int STINGER_BLADE     = 13;
	const int CUTTING_SLASH     = 14;
	const int JAW_BREAKER       = 15;
	const int CUT_HEAD          = 16;
	const int WIND_CUTTER1      = 17;
	const int WIND_CUTTER1R     = 18;
	const int WIND_CUTTER2      = 19;
	const int WIND_CUTTER2R     = 20;
	const int WIND_CUTTER3      = 21;
	const int GAIA_CRUSH1       = 22;
	const int GAIA_CRUSH2       = 23;
	const int GAIA_CRUSH3       = 24;
	const int DRAW_SWORD_CHARGE = 25;
	const int DRAW_SWORD_LOOP   = 26;
	const int DRAW_SWORD        = 27;
	const int DRAW_SWORD_END    = 28;
	const int TUMBLING          = 29;

	enum STANCE		{ STANCE_NONEATTACK, STANCE_ATTACK, STANCE_END };
	enum COMBOCNT	{ COMBOCNT_0, COMBOCNT_1, COMBOCNT_2, COMBOCNT_3, COMBOCNT_4 };

	const _float OTHERS_OFFSET	= 0.9f;

	const _float MIN_SPEED       = 0.0f;
	const _float MAX_SPEED       = 5.0f;
	const _float MOVE_STOP_SPEED = 3.0f;

	// COMOBO ATTACK
	const _uint COMBO1_MOVE_STOP = 16;
	const _uint COMBO2_MOVE_STOP = 20;
	const _uint COMBO3_MOVE_STOP = 15;
	const _uint COMBO4_MOVE_STOP = 30;

	const _uint COMBO1_TRAIL_START = 12;
	const _uint COMBO1_TRAIL_STOP  = 28;
	const _uint COMBO2_TRAIL_START = 13;
	const _uint COMBO2_TRAIL_STOP  = 28;
	const _uint COMBO3_TRAIL_START = 10;
	const _uint COMBO3_TRAIL_STOP  = 26;
	const _uint COMBO4_TRAIL_START = 20;
	const _uint COMBO4_TRAIL_STOP  = 56;

	const _uint COMBO1_COLLISIONTICK_START = 18;
	const _uint COMBO2_COLLISIONTICK_START = 18;
	const _uint COMBO3_COLLISIONTICK_START = 16;
	const _uint COMBO4_COLLISIONTICK_START = 25;

	// SKILL ATTACK
	const _uint STINGER_BLADE_MOVE_START = 20;
	const _uint STINGER_BLADE_MOVE_STOP  = 30;
	const _uint CUTTING_SLASH_MOVE_START = 0;
	const _uint CUTTING_SLASH_MOVE_STOP  = 18;
	const _uint JAW_BREAKER_MOVE_START   = 0;
	const _uint JAW_BREAKER_MOVE_STOP    = 10;
	const _uint CUT_HEAD_MOVE_START      = 34;
	const _uint CUT_HEAD_MOVE_END        = 50;
	const _uint WIND_CUTTER1_MOVE_START  = 0;
	const _uint WIND_CUTTER1_MOVE_STOP   = 10;
	const _uint WIND_CUTTER2_MOVE_START  = 0;
	const _uint WIND_CUTTER2_MOVE_STOP   = 8;
	const _uint WIND_CUTTER3_MOVE_START  = 0;
	const _uint WIND_CUTTER3_MOVE_STOP   = 15;
	const _uint GAIA_CRUSH1_MOVE_START   = 4;
	const _uint GAIA_CRUSH1_MOVE_STOP    = 19;
	const _uint GAIA_CRUSH2_MOVE_START   = 0;
	const _uint GAIA_CRUSH2_MOVE_STOP    = 12;
	const _uint DRAW_SWORD_MOVE_START    = 4;
	const _uint DRAW_SWORD_MOVE_STOP     = 18;
	const _uint TUMBLING_MOVE_START      = 0;
	const _uint TUMBLING_MOVE_STOP       = 25;

	const _uint STINGER_BLADE_TRAIL_START  = 15;
	const _uint STINGER_BLADE_TRAIL_STOP   = 70;
	const _uint CUTTING_SLASH_TRAIL_START  = 30;
	const _uint CUTTING_SLASH_TRAIL_STOP   = 42;
	const _uint JAW_BREAKER_TRAIL_START    = 8;
	const _uint JAW_BREAKER_TRAIL_STOP     = 27;
	const _uint CUT_HEAD_TRAIL_START       = 25;
	const _uint CUT_HEAD_TRAIL_STOP        = 70;
	const _uint WIND_CUTTER1_TRAIL_START   = 6;
	const _uint WIND_CUTTER1_TRAIL_STOP    = 16;
	const _uint WIND_CUTTER2_TRAIL_START   = 4;
	const _uint WIND_CUTTER2_TRAIL_STOP    = 16;
	const _uint WIND_CUTTER3_TRAIL_START   = 6;
	const _uint WIND_CUTTER3_TRAIL_STOP    = 36;
	const _uint GAIA_CRUSH1_TRAIL_START    = 16;
	const _uint GAIA_CRUSH1_TRAIL_STOP     = 999;
	const _uint GAIA_CRUSH2_TRAIL_START    = 0;
	const _uint GAIA_CRUSH2_TRAIL_STOP     = 40;
	const _uint DRAW_SWORD_TRAIL_START     = 8;
	const _uint DRAW_SWORD_TRAIL_STOP      = 999;
	const _uint DRAW_SWORD_END_TRAIL_START = 0;
	const _uint DRAW_SWORD_END_TRAIL_STOP  = 50;

	const _uint STINGER_BLADE_COLLISIONTICK_START = 32;
	const _uint CUTTING_SLASH_COLLISIONTICK_START = 38;
	const _uint JAW_BREAKER_COLLISIONTICK_START   = 12;
	const _uint WIND_CUTTER1_COLLISIONTICK_START  = 9;
	const _uint WIND_CUTTER2_COLLISIONTICK_START  = 6;
	const _uint WIND_CUTTER3_COLLISIONTICK_START  = 12;
	const _uint GAIA_CRUSH2_COLLISIONTICK_START   = 24;
	const _uint DRAW_SWORD_COLLISIONTICK_START    = 16;
}