#pragma once
#include "Typedef.h"

constexpr _int WINCX	= 1'600;
constexpr _int WINCY	= 900;
constexpr _int WINPOS_X	= 0;
constexpr _int WINPOS_Y	= 0;


constexpr _int DEAD_OBJ   = 0;
constexpr _int NO_EVENT   = 1;
constexpr _int RETURN_OBJ = 2;


constexpr _int MIN_STR	= 64;
constexpr _int MAX_STR	= 256;


constexpr _uint VTXCNTX	= 129;
constexpr _uint VTXCNTZ	= 129;
constexpr _uint VTXITV	= 1;

constexpr _float PI		= 3.141592f;

// Animation Tick Per Seconds
constexpr _float TPS	= 4'800.0f;

constexpr _float TIME_OFFSET = 1.0f;

/* SERVER */
constexpr int MAX_BUF_SIZE = 4096;
constexpr int MIN_BUF_SIZE = 1024;

// Away Position
constexpr _float AWAY_FROM_STAGE = -10'000.0f;

// Stage Offset
constexpr _float STAGE_VELIKA_OFFSET_X = 0.0f;
constexpr _float STAGE_VELIKA_OFFSET_Z = 0.0f;
constexpr _float STAGE_BEACH_OFFSET_X  = 256.0f;
constexpr _float STAGE_BEACH_OFFSET_Z  = 0.0f;
constexpr _float STAGE_TEMP_OFFSET_X   = 256.0f;
constexpr _float STAGE_TEMP_OFFSET_Z   = 256.0f;

// Portal Pos
constexpr _float PORTAL_FROM_VELIKA_TO_BEACH_X = 130.0f;
constexpr _float PORTAL_FROM_VELIKA_TO_BEACH_Z = 10.0f;

constexpr _float PORTAL_FROM_BEACH_TO_VELIKA_X = 325.0f;
constexpr _float PORTAL_FROM_BEACH_TO_VELIKA_Z = 65.0f;

constexpr char NO_ITEM    = -1;
constexpr char EMPTY_SLOT = -1;