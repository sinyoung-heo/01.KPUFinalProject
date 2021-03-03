#pragma once

/* =========================PLAYER======================== */
/* [게임 컨텐츠용 패킷] */
void send_packet(int id, void* p);												// Packet 전송 함수
void send_login_ok(int id);														// 로그인 수락 패킷
void send_enter_packet(int to_client, int new_id);								// 등장 패킷
void send_leave_packet(int to_client, int leave_id);							// 떠남 패킷
void send_move_packet(int to_client, int id);									// 움직임 패킷
void send_move_stop_packet(int to_client, int id);								// 움직임 패킷

void process_move(int id, const _vec3& _vDir, const _vec3& _vPos);				// 움직임 처리 함수
void process_move_stop(int id, const _vec3& _vPos, const _vec3& _vDir);

/* 패킷 관리 및 처리 함수 */
void process_packet(int id);													// 패킷 처리 함수 (모든 컨텐츠 처리)
void process_recv(int id, DWORD iosize);										// 패킷 재조립 함수 (Ring Buffer 사용)

/* ==========================NPC========================== */
void send_NPC_enter_packet(int to_client, int new_id);							// NPC 등장 패킷
void send_NPC_move_packet(int to_client, int id);								// NPC 움직임 패킷

void active_npc(int id);														// 해당 NPC의 STATUS = ST_ACTIVE
void random_move_npc(int id);													// NPC 랜덤 움직임

/* ==========================MONSTER========================== */
void send_Monster_enter_packet(int to_client, int new_id);						// Monster 등장 패킷
void send_Monster_move_packet(int to_client, int id);							// Monster 움직임 패킷

void active_monster(int id);													// 해당 Monster의 STATUS = ST_ACTIVE
void nonActive_monster(int id);													// 해당 Monster의 STATUS = ST_NONACTIVE
void random_move_monster(int id);												// Normal Monster 랜덤 움직임
void chase_move_monster(int id);												// Chase Monster 움직임

/* =========================FUNC========================== */
void add_timer(int obj_id, OPMODE ev_type, system_clock::time_point t);
bool CAS(atomic<STATUS>* addr, STATUS* old_v, STATUS new_v);