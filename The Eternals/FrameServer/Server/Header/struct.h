#pragma once
/* _______________________________________확장 OVERAPPED 구조체________________________________________ */
struct OVER_EX
{
	WSAOVERLAPPED wsa_over;
	OPMODE	op_mode;
	WSABUF	wsa_buf;
	unsigned char iocp_buf[MAX_BUFFER];

	int object_id;
};

/* _____________________________________________USER 구조체_____________________________________________ */
typedef struct tagSectorInfo
{
	int m_iSectorNum = 0;
	int m_iMinX = 0;
	int m_iMinY = 0;
	int m_iMaxX = 0;
	int m_iMaxY = 0;
}SECINFO, * PSECINFO;

template <>
struct hash<pair<int, int>> 
{
	size_t operator()(const pair<int, int>& t) const 
	{
		// 해시 계산
		return t.first ^ t.second;
	}
};

/* _____________________________________________TIMER THERAD - NPC AI STRUCT_____________________________________________ */
struct event_type
{
	int obj_id;
	system_clock::time_point wakeup_time;
	OPMODE event_id;
	int target_id;
	constexpr bool operator < (const event_type& _Left) const
	{
		return (wakeup_time > _Left.wakeup_time);
	}

	event_type(int id, system_clock::time_point t, OPMODE eid)
		:obj_id(id), wakeup_time(t), event_id(eid)
	{
		target_id = -1;
	}
};

typedef struct tagEquipment
{
	char weapon = -1;
	char armor	= -1;
	char helmet = -1;
	char shoes	= -1;
}EQUIPMENT;