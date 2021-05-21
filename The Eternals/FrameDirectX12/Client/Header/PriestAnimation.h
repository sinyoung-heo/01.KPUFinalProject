#pragma once
namespace Priest
{
	const int NONE_ATTACK_IDLE = 0;
	const int NONE_ATTACK_WALK = 1;
	const int ATTACK_WAIT      = 2;
	const int ATTACK_RUN       = 3;
	const int IN_WEAPON        = 4;
	const int OUT_WEAPON       = 5;
	const int HASTE            = 6;
	const int AURA_ON          = 7;
	const int PURIFY           = 8;
	const int HEAL_START       = 9;
	const int HEAL_LOOP        = 10;
	const int HEAL_SHOT        = 11;
	const int MP_CHARGE_START  = 12;
	const int MP_CHARGE_LOOP   = 13;
	const int MP_CHARGE_END    = 14;

	enum STANCE		{ STANCE_NONEATTACK, STANCE_ATTACK, STANCE_END };

	const _float OTHERS_OFFSET	= 0.85f;

	const _float MIN_SPEED       = 0.0f;
	const _float MAX_SPEED       = 5.0f;
	const _float MOVE_STOP_SPEED = 3.0f;

}