#pragma once
#include "Typedef.h"

const _int WINCX	= 1'600;
const _int WINCY	= 900;
const _int WINPOS_X	= 0;
const _int WINPOS_Y	= 0;


const _int DEAD_OBJ   = 0;
const _int NO_EVENT   = 1;
const _int RETURN_OBJ = 2;


const _int MIN_STR	= 64;
const _int MAX_STR	= 256;


const _uint VTXCNTX	= 129;
const _uint VTXCNTZ	= 129;
const _uint VTXITV	= 1;

const _float PI		= 3.141592f;

// Animation Tick Per Seconds
const _float TPS	= 4'800.0f;

const _float TIME_OFFSET = 1.0f;

/* SERVER */
constexpr int MAX_BUF_SIZE = 4096;
constexpr int MIN_BUF_SIZE = 1024;

// Away Position
const _float AWAY_FROM_STAGE = -10'000.0f;

// Stage Offset
const _float STAGE_VELIKA_OFFSET_X = 0.0f;
const _float STAGE_VELIKA_OFFSET_Z = 0.0f;
const _float STAGE_BEACH_OFFSET_X  = 256.0f;
const _float STAGE_BEACH_OFFSET_Z  = 0.0f;
const _float STAGE_TEMP_OFFSET_X   = 256.0f;
const _float STAGE_TEMP_OFFSET_Z   = 256.0f;

// Portal Pos
const _float PORTAL_FROM_VELIKA_TO_BEACH_X = 130.0f;
const _float PORTAL_FROM_VELIKA_TO_BEACH_Z = 10.0f;

const _float PORTAL_FROM_BEACH_TO_VELIKA_X = 325.0f;
const _float PORTAL_FROM_BEACH_TO_VELIKA_Z = 65.0f;