#pragma once
namespace Gladiator
{
	constexpr int NONE_ATTACK_IDLE  = 0;
	constexpr int NONE_ATTACK_WALK  = 1;
	constexpr int ATTACK_WAIT       = 2;
	constexpr int IN_WEAPON         = 3;
	constexpr int OUT_WEAPON        = 4;
	constexpr int ATTACK_RUN        = 5;
	constexpr int COMBO1            = 6;
	constexpr int COMBO1R           = 7;
	constexpr int COMBO2            = 8;
	constexpr int COMBO2R           = 9;
	constexpr int COMBO3            = 10;
	constexpr int COMBO3R           = 11;
	constexpr int COMBO4            = 12;
	constexpr int STINGER_BLADE     = 13;
	constexpr int CUTTING_SLASH     = 14;
	constexpr int JAW_BREAKER       = 15;
	constexpr int CUT_HEAD          = 16;
	constexpr int WIND_CUTTER1      = 17;
	constexpr int WIND_CUTTER1R     = 18;
	constexpr int WIND_CUTTER2      = 19;
	constexpr int WIND_CUTTER2R     = 20;
	constexpr int WIND_CUTTER3      = 21;
	constexpr int GAIA_CRUSH1       = 22;
	constexpr int GAIA_CRUSH2       = 23;
	constexpr int GAIA_CRUSH3       = 24;
	constexpr int DRAW_SWORD_CHARGE = 25;
	constexpr int DRAW_SWORD_LOOP   = 26;
	constexpr int DRAW_SWORD        = 27;
	constexpr int DRAW_SWORD_END    = 28;
	constexpr int TUMBLING          = 29;

	enum STANCE		{ STANCE_NONEATTACK, STANCE_ATTACK, STANCE_END };
	enum COMBOCNT	{ COMBOCNT_0, COMBOCNT_1, COMBOCNT_2, COMBOCNT_3, COMBOCNT_4 };

	constexpr _float OTHERS_OFFSET	= 0.9f;

	constexpr _float MIN_SPEED       = 0.0f;
	constexpr _float MAX_SPEED       = 5.0f;
	constexpr _float MOVE_STOP_SPEED = 3.0f;

	// COMOBO ATTACK
	constexpr _uint COMBO1_MOVE_STOP = 16;
	constexpr _uint COMBO2_MOVE_STOP = 20;
	constexpr _uint COMBO3_MOVE_STOP = 15;
	constexpr _uint COMBO4_MOVE_STOP = 30;

	constexpr _uint COMBO1_TRAIL_START = 12;
	constexpr _uint COMBO1_TRAIL_STOP  = 28;
	constexpr _uint COMBO2_TRAIL_START = 13;
	constexpr _uint COMBO2_TRAIL_STOP  = 28;
	constexpr _uint COMBO3_TRAIL_START = 10;
	constexpr _uint COMBO3_TRAIL_STOP  = 26;
	constexpr _uint COMBO4_TRAIL_START = 20;
	constexpr _uint COMBO4_TRAIL_STOP  = 56;

	constexpr _uint COMBO1_COLLISIONTICK_START = 18;
	constexpr _uint COMBO2_COLLISIONTICK_START = 18;
	constexpr _uint COMBO3_COLLISIONTICK_START = 16;
	constexpr _uint COMBO4_COLLISIONTICK_START = 25;

	// SKILL ATTACK
	constexpr _uint STINGER_BLADE_MOVE_START = 20;
	constexpr _uint STINGER_BLADE_MOVE_STOP  = 30;
	constexpr _uint CUTTING_SLASH_MOVE_START = 0;
	constexpr _uint CUTTING_SLASH_MOVE_STOP  = 18;
	constexpr _uint JAW_BREAKER_MOVE_START   = 0;
	constexpr _uint JAW_BREAKER_MOVE_STOP    = 10;
	constexpr _uint CUT_HEAD_MOVE_START      = 34;
	constexpr _uint CUT_HEAD_MOVE_END        = 50;
	constexpr _uint WIND_CUTTER1_MOVE_START  = 0;
	constexpr _uint WIND_CUTTER1_MOVE_STOP   = 10;
	constexpr _uint WIND_CUTTER2_MOVE_START  = 0;
	constexpr _uint WIND_CUTTER2_MOVE_STOP   = 8;
	constexpr _uint WIND_CUTTER3_MOVE_START  = 0;
	constexpr _uint WIND_CUTTER3_MOVE_STOP   = 15;
	constexpr _uint GAIA_CRUSH1_MOVE_START   = 4;
	constexpr _uint GAIA_CRUSH1_MOVE_STOP    = 19;
	constexpr _uint GAIA_CRUSH2_MOVE_START   = 0;
	constexpr _uint GAIA_CRUSH2_MOVE_STOP    = 12;
	constexpr _uint DRAW_SWORD_MOVE_START    = 4;
	constexpr _uint DRAW_SWORD_MOVE_STOP     = 18;
	constexpr _uint TUMBLING_MOVE_START      = 0;
	constexpr _uint TUMBLING_MOVE_STOP       = 25;

	constexpr _uint STINGER_BLADE_TRAIL_START  = 15;
	constexpr _uint STINGER_BLADE_TRAIL_STOP   = 70;
	constexpr _uint CUTTING_SLASH_TRAIL_START  = 30;
	constexpr _uint CUTTING_SLASH_TRAIL_STOP   = 42;
	constexpr _uint JAW_BREAKER_TRAIL_START    = 8;
	constexpr _uint JAW_BREAKER_TRAIL_STOP     = 27;
	constexpr _uint CUT_HEAD_TRAIL_START       = 25;
	constexpr _uint CUT_HEAD_TRAIL_STOP        = 70;
	constexpr _uint WIND_CUTTER1_TRAIL_START   = 6;
	constexpr _uint WIND_CUTTER1_TRAIL_STOP    = 16;
	constexpr _uint WIND_CUTTER2_TRAIL_START   = 4;
	constexpr _uint WIND_CUTTER2_TRAIL_STOP    = 16;
	constexpr _uint WIND_CUTTER3_TRAIL_START   = 6;
	constexpr _uint WIND_CUTTER3_TRAIL_STOP    = 36;
	constexpr _uint GAIA_CRUSH1_TRAIL_START    = 16;
	constexpr _uint GAIA_CRUSH1_TRAIL_STOP     = 999;
	constexpr _uint GAIA_CRUSH2_TRAIL_START    = 0;
	constexpr _uint GAIA_CRUSH2_TRAIL_STOP     = 40;
	constexpr _uint DRAW_SWORD_TRAIL_START     = 8;
	constexpr _uint DRAW_SWORD_TRAIL_STOP      = 999;
	constexpr _uint DRAW_SWORD_END_TRAIL_START = 0;
	constexpr _uint DRAW_SWORD_END_TRAIL_STOP  = 50;

	constexpr _uint STINGER_BLADE_COLLISIONTICK_START = 32;
	constexpr _uint CUTTING_SLASH_COLLISIONTICK_START = 38;
	constexpr _uint JAW_BREAKER_COLLISIONTICK_START   = 12;
	constexpr _uint WIND_CUTTER1_COLLISIONTICK_START  = 9;
	constexpr _uint WIND_CUTTER2_COLLISIONTICK_START  = 6;
	constexpr _uint WIND_CUTTER3_COLLISIONTICK_START  = 12;
	constexpr _uint GAIA_CRUSH2_COLLISIONTICK_START   = 24;
	constexpr _uint DRAW_SWORD_COLLISIONTICK_START    = 16;
}