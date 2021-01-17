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
constexpr int NPC_NUM_START		= 1000;


/* GAME CONTENTS */
constexpr int ZERO_HP			= 0;
constexpr int ZERO_EXP			= 0;
constexpr int LEVEL_UP_EXP		= 100;
constexpr int ATTCK_RANGE		= 1;
constexpr int ORC_TARGET_RANGE	= 5;
constexpr int ORC_RAOMING_DIST	= 20;

constexpr int INIT_HP			= 100;			// 플레이어 초기 HP
constexpr int ORC_HP			= 70;			// ORC HP
constexpr int ELF_HP			= 80;			// ELF HP
constexpr int BOSS_HP			= 150;			// BOSS HP

constexpr int INIT_ATT			= 5;			// 플레이어 초기 공격력
constexpr int ORC_ATT			= 7;			// ORC 공격력
constexpr int ELF_ATT			= 3;			// ELF 공격력
constexpr int BOSS_ATT			= 10;			// BOSS 공격력

constexpr int MASTER			= 99999;

constexpr int SECTOR_SIZE		= 400;





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