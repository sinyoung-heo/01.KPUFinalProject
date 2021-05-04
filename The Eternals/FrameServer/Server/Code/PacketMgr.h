#pragma once

/* =========================PLAYER======================== */
/* [게임 컨텐츠용 패킷] */
void send_packet(int id, void* p);												// Packet 전송 함수
void send_login_ok(int id);														// 로그인 수락 패킷
void send_enter_packet(int to_client, int new_id);								// 등장 패킷
void send_leave_packet(int to_client, int leave_id);							// 떠남 패킷
void send_move_packet(int to_client, int id);									// 움직임 패킷
void send_move_stop_packet(int to_client, int id);								// 움직임 패킷
void send_attack_packet(int to_client, int id, int animIdx, float end_angleY);
void send_attack_stop_packet(int to_client, int id, int animIdx);
void send_player_stat(int to_client, int id);
void send_player_stance_change(int to_client, int id, const bool& st);
void send_player_stage_change(int to_client, int id);

void process_move(int id, const _vec3& _vDir, const _vec3& _vPos);					// 움직임 처리 함수
void process_move_stop(int id, const _vec3& _vPos, const _vec3& _vDir);
void process_collide(int id, int colID);											// 다른 OBJECT와 충돌했을 경우
void process_attack(int id, const _vec3& _vDir, const _vec3& _vPos, int aniIdx, float end_angleY);	// 공격 처리
void process_attack_stop(int id, const _vec3& _vDir, const _vec3& _vPos, int aniIdx);	// 공격 중단 처리
void process_stance_change(int id, const bool& stance);
void process_stage_change(int id, const char& stage_id);

/* 패킷 관리 및 처리 함수 */
void process_packet(int id);													// 패킷 처리 함수 (모든 컨텐츠 처리)
void process_recv(int id, DWORD iosize);										// 패킷 재조립 함수 (Ring Buffer 사용)

/* ==========================MONSTER========================== */
//void active_monster(int id);													// 해당 Monster의 STATUS = ST_ACTIVE
//void nonActive_monster(int id);													// 해당 Monster의 STATUS = ST_NONACTIVE

/* =========================FUNC========================== */
void add_timer(int obj_id, OPMODE ev_type, system_clock::time_point t);
bool CAS(atomic<STATUS>* addr, STATUS* old_v, STATUS new_v);