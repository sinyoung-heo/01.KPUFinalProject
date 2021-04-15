#pragma once
/* NPC */
namespace NPC
{
	// cat, chicken, Aman_boy, Baraka_M_Extractor, NPC_Villagers, Popori_boy
	constexpr int WAIT		= 0;
	constexpr int IDLE		= 1;
	constexpr int WALK		= 2;

	constexpr double TPS	= 4800.0;
}

namespace NPC_NORMAL
{
	// Highelf_girl, Human_boy, Human_girl
	constexpr int TALK		= 3;
	constexpr int LAUGH		= 4;
}

namespace NPC_MERCHANT
{
	// Baraka_M_Merchant, Baraka_M_Mystellium, Popori_M_Merchant
	constexpr int GREET		= 1;
	constexpr int TALK		= 2;
}

namespace NPC_QUEST
{
	// Castanic_M_Lsmith
	constexpr int GREET		= 1;
	constexpr int TALK		= 2;
}

/* ____________________________________________________________________________________*/

/* MOVE BACKGROUND NPC */
namespace Aman_boy
{
	constexpr int NUM_ANIMATION = 3;

	enum ANIM_CLODER { A_WAIT, A_IDLE, A_WALK, A_END };

	static double duration[NUM_ANIMATION] =
	{
		8000, 16000, 4800
	};
}
namespace Cat
{
	constexpr int NUM_ANIMATION = 3;

	enum ANIM_CLODER { A_WAIT, A_IDLE, A_WALK, A_END };

	static double duration[NUM_ANIMATION] =
	{
		19200, 20480, 4800
	};
}

namespace Chicken
{
	constexpr int NUM_ANIMATION = 3;

	enum ANIM_CLODER { A_WAIT, A_IDLE, A_WALK, A_END };

	static double duration[NUM_ANIMATION] =
	{
		19200, 27200, 6400
	};
}

namespace Highelf_girl
{
	constexpr int NUM_ANIMATION = 5;

	enum ANIM_CLODER { A_WAIT, A_IDLE, A_WALK, A_TALK, A_LAUGH, A_END };

	static double duration[NUM_ANIMATION] =
	{
		11200, 16480, 5440, 16800, 10880
	};
}

namespace Human_boy
{
	constexpr int NUM_ANIMATION = 5;

	enum ANIM_CLODER { A_WAIT, A_IDLE, A_WALK, A_TALK, A_LAUGH, A_END };

	static double duration[NUM_ANIMATION] =
	{
		9600, 19200, 4480, 12800, 17600
	};
}

namespace Human_girl
{
	constexpr int NUM_ANIMATION = 5;

	enum ANIM_CLODER { A_WAIT, A_IDLE, A_WALK, A_TALK, A_LAUGH, A_END };

	static double duration[NUM_ANIMATION] =
	{
		9600, 28800, 5440, 27200, 28800
	};
}

/* NON MOVE BACKGROUND NPC */
namespace Popori_boy
{
	constexpr int NUM_ANIMATION = 2;

	enum ANIM_CLODER { A_WAIT, A_IDLE, A_END };

	static double duration[NUM_ANIMATION] =
	{
		7200, 22400
	};
}

namespace NPC_Villagers
{
	constexpr int NUM_ANIMATION = 1;

	enum ANIM_CLODER { A_WAIT, A_END };

	static double duration[NUM_ANIMATION] =
	{
		12800
	};
}

namespace Baraka_M_Extractor
{
	constexpr int NUM_ANIMATION = 1;

	enum ANIM_CLODER { A_WAIT, A_END };

	static double duration[NUM_ANIMATION] =
	{
		10880
	};
}

/* MERCHANT */
namespace Baraka_M_Merchant
{
	constexpr int NUM_ANIMATION = 3;

	enum ANIM_CLODER { A_WAIT, A_GREET, A_TALK, A_END };

	static double duration[NUM_ANIMATION] =
	{
		14080, 9920, 17920
	};
}

namespace Baraka_M_Mystellium
{
	constexpr int NUM_ANIMATION = 3;

	enum ANIM_CLODER { A_WAIT, A_GREET, A_TALK, A_END };

	static double duration[NUM_ANIMATION] =
	{
		9600, 5120, 7680
	};
}

namespace Popori_M_Merchant
{
	constexpr int NUM_ANIMATION = 3;

	enum ANIM_CLODER { A_WAIT, A_GREET, A_TALK, A_END };

	static double duration[NUM_ANIMATION] =
	{
		5600, 8000, 19200
	};
}

/* QUEST */
namespace Castanic_M_Lsmith
{
	constexpr int NUM_ANIMATION = 3;

	enum ANIM_CLODER { A_WAIT, A_GREET, A_TALK, A_END };

	static double duration[NUM_ANIMATION] =
	{
		11200, 5280, 16000
	};
}


