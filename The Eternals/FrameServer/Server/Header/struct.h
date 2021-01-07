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
struct client_info
{
	
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
};