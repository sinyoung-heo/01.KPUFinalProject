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
#define PI					3.141592
#define _3DMAX_FPS			30.0f
#define ANIMA_INTERPOLATION	1.f
#define MAX_ANI				10

/* GAME CONTENTS */
//constexpr int ZERO_HP			= 0;
//constexpr int INIT_HP			= 100;			// 플레이어 초기 HP
//constexpr int INIT_EXP		= 0;			// 플레이어 초기 EXP
//constexpr int INIT_MAXEXP		= 100;			// 플레이어 초기 EXP
//constexpr int INIT_LEV		= 1;			// 플레이어 초기 LEVEL
//constexpr int INIT_ATT		= 5;			// 플레이어 초기 ATTACK
//constexpr int INIT_SPEED		= 5;			// 플레이어 초기 SPEED
constexpr int	SERVER_CORE					= 4;

constexpr int	MOVE_LIMIT					= 7;			// MONSTER MOVE OFFSET
constexpr int	CHASE_RANGE					= 8;			// MONSTER 추적 범위

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

// Player StartPos
constexpr float STAGE_VELIKA_X = 130.0f;
constexpr float STAGE_VELIKA_Z = 70.0f;

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