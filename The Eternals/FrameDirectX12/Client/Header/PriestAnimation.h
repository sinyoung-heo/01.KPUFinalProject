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

	enum STANCE		{ STANCE_NONEATTACK, STANCE_ATTACK, STANCE_END };

	constexpr _float OTHERS_OFFSET	= 0.85f;

	constexpr _float MIN_SPEED       = 0.0f;
	constexpr _float MAX_SPEED       = 5.0f;
	constexpr _float MOVE_STOP_SPEED = 3.0f;

	constexpr _uint HASTE_COLLISIONTICK_START = 32;

}