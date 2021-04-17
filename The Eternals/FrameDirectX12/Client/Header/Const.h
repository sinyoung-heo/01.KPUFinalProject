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

// Stage Offset
const _float STAGE_VELIKA_OFFSET_X = 0.0f;
const _float STAGE_VELIKA_OFFSET_Z = 0.0f;
const _float STAGE_BEACH_OFFSET_X  = 256.0f;
const _float STAGE_BEACH_OFFSET_Z  = 0.0f;
const _float STAGE_TEMP_OFFSET_X   = 256.0f;
const _float STAGE_TEMP_OFFSET_Z   = 256.0f;