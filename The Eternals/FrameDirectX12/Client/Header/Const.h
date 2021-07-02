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
constexpr _float STAGE_WINTER_OFFSET_X = 256.0f;
constexpr _float STAGE_WINTER_OFFSET_Z = 256.0f;

// Portal Pos
constexpr _float PORTAL_FROM_VELIKA_TO_BEACH_X = 130.0f;
constexpr _float PORTAL_FROM_VELIKA_TO_BEACH_Z = 10.0f;

constexpr _float PORTAL_FROM_BEACH_TO_VELIKA_X = 325.0f;
constexpr _float PORTAL_FROM_BEACH_TO_VELIKA_Z = 65.0f;

// ItemInventorySlot && QuickSlot
constexpr char NO_ITEM    = -1;
constexpr char EMPTY_SLOT = -1;

constexpr char QUCKSLOT_POTION_HP = 0;
constexpr char QUCKSLOT_POTION_MP = 1;

constexpr char QUCKSLOT_SKILL_STINGER_BLADE = 2;
constexpr char QUCKSLOT_SKILL_CUTTING_SLASH = 3;
constexpr char QUCKSLOT_SKILL_JAW_BREAKER   = 4;
constexpr char QUCKSLOT_SKILL_GAIA_CRUSH    = 5;
constexpr char QUCKSLOT_SKILL_DRAW_SWORD    = 6;

constexpr char QUCKSLOT_SKILL_RAPID_SHOT    = 7;
constexpr char QUCKSLOT_SKILL_ARROW_SHOWER  = 8;
constexpr char QUCKSLOT_SKILL_ESCAPING_BOMB = 9;
constexpr char QUCKSLOT_SKILL_ARROW_FALL    = 10;
constexpr char QUCKSLOT_SKILL_CHARGE_ARROW  = 11;

constexpr char QUCKSLOT_SKILL_AURA_ON       = 12;
constexpr char QUCKSLOT_SKILL_PURIFY        = 13;
constexpr char QUCKSLOT_SKILL_HEAL          = 14;
constexpr char QUCKSLOT_SKILL_MP_CHARGE     = 15;