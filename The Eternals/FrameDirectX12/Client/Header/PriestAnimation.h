#pragma once
namespace Priest
{
	constexpr int NONE_ATTACK_IDLE = 0;
	constexpr int NONE_ATTACK_WALK = 1;
	constexpr int ATTACK_WAIT      = 2;
	constexpr int ATTACK_RUN       = 3;
	constexpr int IN_WEAPON        = 4;
	constexpr int OUT_WEAPON       = 5;
	constexpr int HASTE            = 6;
	constexpr int AURA_ON          = 7;
	constexpr int PURIFY           = 8;
	constexpr int HEAL_START       = 9;
	constexpr int HEAL_LOOP        = 10;
	constexpr int HEAL_SHOT        = 11;
	constexpr int MP_CHARGE_START  = 12;
	constexpr int MP_CHARGE_LOOP   = 13;
	constexpr int MP_CHARGE_END    = 14;

	constexpr int NUM_ANIMATION = 15;
	static double duration[NUM_ANIMATION] =
	{
		27520,5280,2720,3840,4320,2400,7200,7200,7200,4800,4800,7200,4800,4800,4800
	};

	enum STANCE		{ STANCE_NONEATTACK, STANCE_ATTACK, STANCE_END };

	constexpr float OTHERS_OFFSET	= 0.85f;

	constexpr float MIN_SPEED		= 0.0f;
	constexpr float MAX_SPEED		= 5.0f;
	constexpr float MOVE_STOP_SPEED	= 3.0f;

	constexpr float	PLUS_HP			= 20;
	constexpr float	PLUS_MP			= 10;

	// 스킬 필요 MP
	constexpr int	AMOUNT_AURA		= 70;
	constexpr int	AMOUNT_PURIFY	= 70;
	constexpr int	AMOUNT_HEAL		= 90;
	constexpr int	AMOUNT_MANA		= 50;

	constexpr unsigned int HASTE_COLLISIONTICK_START = 32;

}