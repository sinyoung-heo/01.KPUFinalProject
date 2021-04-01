#pragma once
/* NORMAL MONSTER */
namespace Monster_Normal
{
	constexpr int WAIT		= 0;
	constexpr int WALK		= 1;
	constexpr int RUN		= 2;
	constexpr int ATTACK	= 3;
}
namespace Crab
{
	constexpr int DEATH		= 4;
	
	enum ANIM_CRAB { A_WAIT, A_WALK, A_RUN, A_ATTACK, A_DEATH, A_END };
}

namespace Monkey
{
	constexpr int ATTACK_THROW	= 4;
	constexpr int DEATH			= 5;

	enum ANIM_CRAB { A_WAIT, A_WALK, A_RUN, A_ATTACK, A_ATTACK_THROW, A_DEATH, A_END };
}

/* EPIC MONSTER */
namespace Cloder
{
	constexpr int ATTACK_POKE	= 4;
	constexpr int ATTACK_SWING	= 5;
	constexpr int ATTACK_DEATH	= 6;

	enum ANIM_CLODER { A_WAIT, A_WALK, A_RUN, A_ATTACK, A_ATTACK_POKE, A_ATTACK_SWING, A_DEATH, A_END };
}