#pragma once
#include "Engine_Typedef.h"

namespace Engine
{
	constexpr _int	DEAD_OBJ   = 0;
	constexpr _int	NO_EVENT   = 1;
	constexpr _int	RETURN_OBJ = 2;

	constexpr _int	WINCX				= 1'600;
	constexpr _int	WINCY				= 900;

	constexpr _int	MIN_STR				= 64;
	constexpr _int	MAX_STR				= 256;

	constexpr _float SHADOWTEX_WIDTH	= 6000.0f;
	constexpr _float SHADOWTEX_HEIGHT	= 6000.0f;

	constexpr _int	MAX_PALETTE			= 128;

	constexpr _uint TRAIL_SIZE = (3 * 7) + 1;
}