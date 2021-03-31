#pragma once
/* Buffer Size */
constexpr int MAX_BUFFER		= 4096;
constexpr int MIN_BUFFER		= 1024;

/* IOCP KEY */
constexpr int KEY_SERVER		= 1000000;

/* SYSTEM DEFINE */
#define NO_EVENT 0
#define DEAD_OBJ 1
#define NO_INIT -1
#define PI 3.141592
constexpr int SERVER_CORE		= 4;

/* GAME CONTENTS */
//constexpr int ZERO_HP			= 0;
//constexpr int INIT_HP			= 100;			// 플레이어 초기 HP
//constexpr int INIT_EXP		= 0;			// 플레이어 초기 EXP
//constexpr int INIT_MAXEXP		= 100;			// 플레이어 초기 EXP
//constexpr int INIT_LEV		= 1;			// 플레이어 초기 LEVEL
//constexpr int INIT_ATT		= 5;			// 플레이어 초기 ATTACK
//constexpr int INIT_SPEED		= 5;			// 플레이어 초기 SPEED

constexpr int MOVE_LIMIT				= 7;			// MONSTER MOVE OFFSET
constexpr int CHASE_RANGE				= 8;			// MONSTER 추적 범위

constexpr int ATTACK_RANGE_CRAB			= 2;			// MONSTER CRAB 공격 범위
constexpr int ATTACK_RANGE_MONKEY		= 3;			// MONSTER MONKEY NORMAL 공격 범위
constexpr int THROW_RANGE_MONKEY_START	= 10;			// MONSTER MONKEY 원거리 공격 범위
constexpr int THROW_RANGE_MONKEY_END	= 15;			// MONSTER MONKEY 원거리 공격 범위
constexpr int ATTACK_RANGE_EPIC			= 4;

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