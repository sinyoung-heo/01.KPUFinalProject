#pragma once
namespace Archer
{
	constexpr int NONE_ATTACK_IDLE   = 0;
	constexpr int NONE_ATTACK_WALK   = 1;
	constexpr int ATTACK_WAIT        = 2;
	constexpr int ATTACK_RUN         = 3;
	constexpr int IN_WEAPON          = 4;
	constexpr int OUT_WEAPON         = 5;
	constexpr int BACK_DASH          = 6;
	constexpr int ATTACK_ARROW       = 7;
	constexpr int RAPID_SHOT1        = 8;
	constexpr int RAPID_SHOT2        = 9;
	constexpr int ESCAPING_BOMB      = 10;
	constexpr int ARROW_SHOWER_START = 11;
	constexpr int ARROW_SHOWER_LOOP  = 12;
	constexpr int ARROW_SHOWER_SHOT  = 13;
	constexpr int ARROW_FALL_START   = 14;
	constexpr int ARROW_FALL_LOOP    = 15;
	constexpr int ARROW_FALL_SHOT    = 16;
	constexpr int CHARGE_ARROW_START = 17;
	constexpr int CHARGE_ARROW_LOOP  = 18;
	constexpr int CHARGE_ARROW_SHOT  = 19;

	constexpr int NUM_ANIMATION = 20;
	static double duration[NUM_ANIMATION] =
	{
		13440,4800,6400,3840,6400,6400,3840,2880,2400,6240,8640,4800,3840,4800,7200,3840,14400,9600,3840,8640
	};

	enum STANCE	{ STANCE_NONEATTACK, STANCE_ATTACK, STANCE_END };

	constexpr float OTHERS_OFFSET	= 0.9f;

	constexpr float MIN_SPEED        = 0.0f;
	constexpr float MAX_SPEED        = 5.0f;
	constexpr float MAX_ATTACK_SPEED = MAX_SPEED * 1.35f;
	constexpr float MOVE_STOP_SPEED  = 3.0f;

	constexpr _uint BACK_DASH_MOVE_START     = 2;
	constexpr _uint BACK_DASH_MOVE_STOP      = 19;
	constexpr _uint ESCAPING_BOMB_MOVE_START = 4;
	constexpr _uint ESCAPING_BOMB_MOVE_STOP  = 28;
	constexpr _uint CHARGE_ARROW_MOVE_START  = 2;
	constexpr _uint CHARGE_ARROW_MOVE_STOP   = 10;

	constexpr _uint ATTACK_ARROW_COLLISIONARROW_START = 7;
	constexpr _uint RAPID_SHOT_COLLISIONARROW_START   = 4;
	constexpr _uint ARROW_SHOWER_COLLISIONARROW_START = 21;
	constexpr _uint ARROW_FALL_COLLISIONARROW_START   = 0;
	constexpr _uint CHARGE_ARROW_COLLISIONARROW_START = 3;

	constexpr _uint	ARROW_SHOWER_CNT = 8;
	constexpr float	ARROW_FALL_DIST  = 8.0f;

	// 스킬 필요 MP
	constexpr int	AMOUNT_RAPID_SHOT1			= 30;
	constexpr int	AMOUNT_ESCAPING_BOMB		= 50;
	constexpr int	AMOUNT_ARROW_SHOWER_START	= 70;
	constexpr int	AMOUNT_ARROW_FALL_START		= 90;
	constexpr int	AMOUNT_CHARGE_ARROW_START	= 100;
}