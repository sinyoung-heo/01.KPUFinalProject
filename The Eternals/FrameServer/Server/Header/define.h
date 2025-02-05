#pragma once
/* Buffer Size */
constexpr int MAX_BUFFER		= 4096;
constexpr int MIN_BUFFER		= 1024;

/* IOCP KEY */
constexpr int KEY_SERVER		= 1000000;

/* SYSTEM DEFINE */
#define NO_EVENT			0
#define DEAD_OBJ			1
#define NO_INIT				-1
#define PI					3.141592f
#define _3DMAX_FPS			30.0f
#define ANIMA_INTERPOLATION	1.f
#define MAX_ANI				15
#define MAX_ANI_AI			30

/* GAME CONTENTS */
constexpr int	SERVER_CORE					= 4;
constexpr int	NEAR_SECTOR					= 5;

constexpr int	ZERO_HP						= 0;
constexpr int	INIT_HP						= 100'000;		// 초기 HPw1
constexpr int	INIT_MP						= 100'000;		// 초기 MP
constexpr int	INIT_EXP					= 0;			// 초기 EXP
constexpr int	INIT_MAXEXP					= 100;			// 초기 MAXEXP
constexpr int	INIT_LEV					= 1;			// 초기 LEVEL
constexpr int	INIT_MINATT					= 1'000;		// 초기 ATTACK
constexpr int	INIT_MAXATT					= 2'000;		// 초기 ATTACK
constexpr int	INIT_MONEY					= 100'000;
constexpr float	INIT_SPEED					= 5.f;			// 초기 SPEED
constexpr int	INCREASE_EXP				= 50;
constexpr int	INCREASE_HP					= 50;
constexpr int	INCREASE_MP					= 20;
constexpr int	INCREASE_ATT				= 5;
constexpr float	NUCKBACK_DIST				= 6.f;
constexpr int	INIT_PARTY_NUMBER			= -1;
constexpr int	MAX_ITEMSIZE				= 80;
constexpr float	PERCENT						= 100.f;

constexpr int	RAID_PARTY					= 99;
constexpr int	RAID_MINIMUM				= 2;
constexpr int	AI_HP						= 100'000;
constexpr int	AI_MP						= 100'000;
constexpr int	AI_MAXATT					= 50'000;
constexpr int	AI_MINATT					= 10'000;

/*____________________________________________________________________
Monster Attack & Chase Dist
______________________________________________________________________*/
constexpr int	MOVE_LIMIT					= 7;			// MONSTER MOVE OFFSET
constexpr int	CHASE_RANGE					= 15;			// MONSTER 추적 범위

constexpr int	ATTACK_RANGE_CRAB			= 2;			// MONSTER CRAB 공격 범위
constexpr int	ATTACK_RANGE_MONKEY			= 3;			// MONSTER MONKEY 근거리 공격 범위
constexpr int	THROW_RANGE_MONKEY_START	= 10;			// MONSTER MONKEY 원거리 공격 범위
constexpr int	THROW_RANGE_MONKEY_END		= 15;			// MONSTER MONKEY 원거리 공격 범위
constexpr int	ATTACK_RANGE_CLODER			= 4;			// MONSTER CLODER 공격 범위
constexpr float ATTACK_RANGE_SAILOR			= 2.5f;			// MONSTER SAILOR 공격 범위
constexpr int	ATTACK_RANGE_GBEETEL		= 5;			// MONSTER GIANT BEETLE 공격 범위
constexpr int	ATTACK_RANGE_ARACHNE		= 13;			// MONSTER ARACHNE 공격 범위
constexpr int	RUSH_RANGE_GBEETEL_START	= 9;			// MONSTER GIANT BEETLE 돌진 범위
constexpr int	RUSH_RANGE_GBEETEL_END		= 10;			// MONSTER GIANT BEETLE 돌진 범위
constexpr int	RUSH_RANGE_GMONKEY_START	= 7;			// MONSTER GIANT MONKEY 돌진 범위
constexpr int	RUSH_RANGE_GMONKEY_END		= 8;			// MONSTER GIANT MONKEY 돌진 범위

constexpr int	CRAB_HP						= 8'000'000;
constexpr int	CRAB_ATT					= 1'600;
constexpr int	CRAB_EXP					= 500;
constexpr float	CRAB_SPD					= 0.5f;
constexpr float	CRAB_CHASE_SPD				= 2.f;

constexpr int	SAILOR_HP					= 10'000'000;
constexpr int	SAILOR_ATT					= 3'200;
constexpr int	SAILOR_EXP					= 800;
constexpr float	SAILOR_SPD					= 1.f;
constexpr float	SAILOR_CHASE_SPD			= 4.f;
constexpr int	SAILOR_RUSH_TICK_START		= 30;
constexpr int	SAILOR_RUSH_TICK_END		= 55;

constexpr int	ARCHNE_HP					= 10000;
constexpr int	ARCHNE_ATT					= 150;
constexpr int	ARCHNE_EXP					= 100;
constexpr float	ARCHNE_SPD					= 1.f;
constexpr float	ARCHNE_CHASE_SPD			= 6.f;

constexpr int	MONKEY_HP					= 10'000'000;
constexpr int	MONKEY_ATT					= 4'000;
constexpr int	MONKEY_EXP					= 900;
constexpr float	MONKEY_SPD					= 1.f;
constexpr float	MONKEY_CHASE_SPD			= 3.f;

constexpr int	CLODER_HP					= 1000;
constexpr int	CLODER_ATT					= 150;
constexpr int	CLODER_EXP					= 100;
constexpr float	CLODER_SPD					= 1.f;
constexpr float	CLODER_CHASE_SPD			= 4.f;

constexpr int	GIANTMONKEY_HP				= 50'000'000;
constexpr int	GIANTMONKEY_ATT				= 7'000;
constexpr int	GIANTMONKEY_EXP				= 1'000;
constexpr float	GIANTMONKEY_SPD				= 1.f;
constexpr float	GIANTMONKEY_CHASE_SPD		= 4.f;

constexpr int	GIANTBETTLE_HP				= 1000;
constexpr int	GIANTBETTLE_ATT				= 150;
constexpr int	GIANTBETTLE_EXP				= 100;
constexpr float	GIANTBETTLE_SPD				= 1.f;
constexpr float	GIANTBETTLE_CHASE_SPD		= 4.f;

constexpr int	VERGOS_HP					= 200'000'000;
constexpr int	VERGOS_ATT					= 8'000;
constexpr int	VERGOS_EXP					= 10'000;
constexpr float	VERGOS_SPD					= 1.f;
constexpr float	VERGOS_CHASE_SPD			= 4.f;
constexpr int	VERGOS_PATTERN				= 7;

constexpr float	GOBACK_SPD					= 6.f;

/*____________________________________________________________________
Player Direction
______________________________________________________________________*/
constexpr float	ANGLE_FRONT			= 0.f;
constexpr float	ANGLE_RIGHT			= 90.f;
constexpr float	ANGLE_RIGHT_UP		= 45.f;
constexpr float	ANGLE_RIGHT_DOWN	= 135.f;
constexpr float	ANGLE_LEFT			= 270.f;
constexpr float	ANGLE_LEFT_UP		= 315.f;
constexpr float	ANGLE_LEFT_DOWN		= 225.f;
constexpr float	ANGLE_BACK			= 180.f;

/*____________________________________________________________________
Stage Offset
______________________________________________________________________*/
const float STAGE_VELIKA_OFFSET_X	= 0.0f;
const float STAGE_VELIKA_OFFSET_Z	= 0.0f;
const float STAGE_BEACH_OFFSET_X	= 256.0f;
const float STAGE_BEACH_OFFSET_Z	= 0.0f;
const float STAGE_WINTER_OFFSET_X	= 256.0f;
const float STAGE_WINTER_OFFSET_Z	= 256.0f;

/* TYPEDEF */
typedef	unsigned long		_ulong;
typedef	unsigned int		_uint;

/* MACRO */
#define NO_COPY(CLASSNAME)							\
	private:										\
	CLASSNAME(const CLASSNAME&);					\
	CLASSNAME& operator = (const CLASSNAME&);		

#define DECLARE_SINGLETON(CLASSNAME)				\
	NO_COPY(CLASSNAME)								\
	private:										\
	static CLASSNAME*	m_pInstance;				\
	public:											\
	static CLASSNAME*	GetInstance( void );		\
	static void DestroyInstance( void );			

#define IMPLEMENT_SINGLETON(CLASSNAME)				\
	CLASSNAME*	CLASSNAME::m_pInstance = NULL;		\
	CLASSNAME*	CLASSNAME::GetInstance( void )	{	\
		if(NULL == m_pInstance) {					\
			m_pInstance = new CLASSNAME;			\
		}											\
		return m_pInstance;							\
	}												\
	void CLASSNAME::DestroyInstance( void ) {		\
		if(NULL != m_pInstance)	{					\
			m_pInstance->Release();					\
			m_pInstance = NULL;						\
		}											\
	}