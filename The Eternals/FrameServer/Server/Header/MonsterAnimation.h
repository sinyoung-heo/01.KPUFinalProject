#pragma once
/* NORMAL MONSTER */
namespace Monster_Normal
{
	constexpr int WAIT		= 0;
	constexpr int WALK		= 1;
	constexpr int RUN		= 2;
	constexpr int ATTACK	= 3;

	constexpr double TPS	= 4800.0;
}
namespace Crab
{
	constexpr int DEATH		= 4;
	
	constexpr int NUM_ANIMATION = 5;

	enum ANIM_CRAB { A_WAIT, A_WALK, A_RUN, A_ATTACK, A_DEATH, A_END };

	static double duration[NUM_ANIMATION] =
	{
		8000, 4800, 2240, 10400, 10720
	};

	constexpr int ATTACK_START_TICK = 43;
}

namespace Monkey
{
	constexpr int ATTACK_THROW	= 4;
	constexpr int DEATH			= 5;
	constexpr int FINCH			= 6;
	constexpr int GROGGY		= 7;
	constexpr int DOWN			= 8;

	constexpr int NUM_ANIMATION = 9;

	enum ANIM_MONKEY { 
		A_WAIT, A_WALK, A_RUN,
		A_ATTACK, A_ATTACK_THROW, A_DEATH,
		A_FINCH, A_GROGGY, A_DOWN, A_END
	};

	static double duration[NUM_ANIMATION] =
	{
		4800, 6400, 4480, 9600, 12800, 17600, 4800, 8480, 13440
	};

	constexpr int ATTACK_START_TICK       = 25;
	constexpr int ATTACK_THROW_START_TICK = 45;
	constexpr int A_DOWN_START_TICK		  = 1;
}

/* EPIC MONSTER */
namespace Cloder
{
	constexpr int ATTACK_POKE	= 4;
	constexpr int ATTACK_SWING	= 5;
	constexpr int DEATH			= 6;

	constexpr int NUM_ANIMATION = 7;

	enum ANIM_CLODER { A_WAIT, A_WALK, A_RUN, A_ATTACK, A_ATTACK_POKE, A_ATTACK_SWING, A_DEATH, A_END };

	static double duration[NUM_ANIMATION] =
	{
		4800, 6400, 2240, 9600, 14400, 14400, 10720
	};

	constexpr int ATTACK_START_TICK       = 24;
	constexpr int ATTACK_POKE_START_TICK  = 58;
	constexpr int ATTACK_SWING_START_TICK = 7;
}

namespace DrownedSailor
{
	constexpr int ATTACK_SPIN	= 3;
	constexpr int ATTACK_NORMAL = 4;
	constexpr int ATTACK_STRONG = 5; 
	constexpr int ATTACK_RUSH	= 6; // 343~423 (383~404)
	constexpr int ATTACK_HOOK	= 7;
	constexpr int DEATH			= 8;
	constexpr int FINCH			= 9;
	constexpr int GROGGY		= 10;
	constexpr int DOWN			= 11;

	constexpr int NUM_ANIMATION = 12;
	
	enum ANIM_SAILOR {
		A_WAIT, A_WALK, A_RUN,
		A_ATTACK_SPIN, A_ATTACK, A_ATTACK_STRONG,
		A_ATTACK_RUSH, A_ATTACK_HOOK, A_DEATH, 
		A_FINCH, A_GROGGY, A_DOWN, A_END
	};

	static double duration[NUM_ANIMATION] =
	{
		10080, 6400, 5440, 9600, 9600, 12800, 12800, 7200, 18880, 4800, 7680, 22080
	};

	constexpr int ATTACK_SPIN_START_TICK   = 16;
	constexpr int ATTACK_NORMAL_START_TICK = 36;
	constexpr int ATTACK_STRONG_START_TICK = 49;
	constexpr int ATTACK_RUSH_START_TICK   = 49;
	constexpr int ATTACK_HOOK_START_TICK   = 23;
	constexpr int A_DOWN_START_TICK		   = 1;
}

namespace GiantBeetle
{
	constexpr int ATTACK_UP			= 2;
	constexpr int ATTACK_DOWN		= 3;
	constexpr int ATTACK_RUSH		= 4;
	constexpr int ATTACK_FLYSTAMP	= 5;
	constexpr int ROTATE_LEFT		= 6;
	constexpr int ROTATE_RIGHT		= 7;
	constexpr int DEATH				= 8;

	constexpr int NUM_ANIMATION		= 9;

	enum ANIM_GIANTBEETLE {
		A_WAIT, A_WALK, 
		A_ATTACK_UP, A_ATTACK_DOWN, A_ATTACK_RUSH, A_ATTACK_FLYSTAMP,
		A_ROTATE_LEFT, A_ROTATE_RIGHT, A_DEATH, A_END
	};

	static double duration[NUM_ANIMATION] =
	{
		11200, 9600, 16000, 16160, 27360, 28480, 9600, 9600, 17600
	};

	constexpr int A_ATTACK_UP_START_TICK       = 44;
	constexpr int A_ATTACK_DOWN_START_TICK     = 39;
	constexpr int A_ATTACK_RUSH_START_TICK     = 50;
	constexpr int A_ATTACK_FLYSTAMP_START_TICK = 121;
}

namespace GiantMonkey
{
	constexpr int ATTACK_RIGHT		= 2;
	constexpr int ATTACK_LEFT		= 3;
	constexpr int ATTACK_STAMP		= 4;
	constexpr int ATTACK_HOOK		= 5;
	constexpr int ATTACK_JUMPING	= 6;
	constexpr int ATTACK_FLYSTAMP	= 7;
	constexpr int ATTACK_COMBO		= 8;
	constexpr int DEATH				= 9;
	constexpr int FINCH				= 10;
	constexpr int GROGGY			= 11;
	constexpr int DOWN				= 12;

	constexpr int NUM_ANIMATION		= 13;

	enum ANIM_GIANTMONKEY 
	{
		A_WAIT, A_WALK,
		A_ATTACK_RIGHT, A_ATTACK_LEFT, A_ATTACK_STAMP, A_ATTACK_HOOK,
		A_ATTACK_JUMPING, A_ATTACK_FLYSTAMP, A_ATTACK_COMBO,
		A_DEATH, A_FINCH, A_GROGGY, A_DOWN, A_END
	};

	static double duration[NUM_ANIMATION] =
	{
		19200, 6400, 12800, 9600, 17600, 17600, 16000, 22400, 30560, 22400, 4800, 12800, 33120
	};

	constexpr int A_ATTACK_RIGHT_START_TICK     = 40;
	constexpr int A_ATTACK_LEFT_START_TICK      = 30;
	constexpr int A_ATTACK_STAMP_START_TICK     = 61;
	constexpr int A_ATTACK_HOOK_START_TICK      = 53;
	constexpr int A_ATTACK_JUMPING_START_TICK   = 65;
	constexpr int A_ATTACK_FLYSTAMP_START_TICK  = 57;
	constexpr int A_ATTACK_COMBO_START_TICK     = 15 + 61;
	constexpr int A_ATTACK_COMBO_END_START_TICK = 82 + 61;
	constexpr int A_DOWN_START_TICK				= 5;
}

namespace CraftyArachne
{
	constexpr int ATTACK_POUND		= 2;
	constexpr int ATTACK_NORMAL		= 3;
	constexpr int ATTACK_SPIN		= 4;
	constexpr int ATTACK_FLYSTAMP	= 5;
	constexpr int DEATH				= 6;

	constexpr int NUM_ANIMATION		= 7;

	enum ANIM_CRAFTYARACHNE {
		A_WAIT, A_WALK,
		A_ATTACK_POUND, A_ATTACK_NORMAL,
		A_ATTACK_SPIN, A_ATTACK_FLYSTAMP,
		A_DEATH, A_END
	};

	static double duration[NUM_ANIMATION] =
	{
		27200, 4800, 12000, 12000, 22400, 32000, 14400
	};

	constexpr int ATTACK_POUND_START_TICK    = 36;
	constexpr int ATTACK_NORMAL_START_TICK   = 52;
	constexpr int ATTACK_SPIN_START_TICK     = 66;
	constexpr int ATTACK_FLYSTAMP_START_TICK = 134;
}