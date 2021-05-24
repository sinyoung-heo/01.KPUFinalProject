#pragma once
namespace Archer
{
	const int NONE_ATTACK_IDLE   = 0;
	const int NONE_ATTACK_WALK   = 1;
	const int ATTACK_WAIT        = 2;
	const int ATTACK_RUN         = 3;
	const int IN_WEAPON          = 4;
	const int OUT_WEAPON         = 5;
	const int BACK_DASH          = 6;
	const int ATTACK_ARROW       = 7;
	const int RAPID_SHOT1        = 8;
	const int RAPID_SHOT2        = 9;
	const int ESCAPING_BOMB      = 10;
	const int ARROW_SHOWER_START = 11;
	const int ARROW_SHOWER_LOOP  = 12;
	const int ARROW_SHOWER_SHOT  = 13;
	const int ARROW_FALL_START   = 14;
	const int ARROW_FALL_LOOP    = 15;
	const int ARROW_FALL_SHOT    = 16;
	const int CHARGE_ARROW_START = 17;
	const int CHARGE_ARROW_LOOP  = 18;
	const int CHARGE_ARROW_SHOT  = 19;

	enum STANCE	{ STANCE_NONEATTACK, STANCE_ATTACK, STANCE_END };

	const _float OTHERS_OFFSET	= 0.85f;

	const _float MIN_SPEED       = 0.0f;
	const _float MAX_SPEED       = 5.0f;
	const _float MAX_ATTACK_SPEED = MAX_SPEED * 1.35f;
	const _float MOVE_STOP_SPEED = 3.0f;
}