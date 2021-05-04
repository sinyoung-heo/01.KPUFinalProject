#pragma once

/* =========================PLAYER======================== */
/* [���� �������� ��Ŷ] */
void send_packet(int id, void* p);												// Packet ���� �Լ�
void send_login_ok(int id);														// �α��� ���� ��Ŷ
void send_enter_packet(int to_client, int new_id);								// ���� ��Ŷ
void send_leave_packet(int to_client, int leave_id);							// ���� ��Ŷ
void send_move_packet(int to_client, int id);									// ������ ��Ŷ
void send_move_stop_packet(int to_client, int id);								// ������ ��Ŷ
void send_attack_packet(int to_client, int id, int animIdx, float end_angleY);
void send_attack_stop_packet(int to_client, int id, int animIdx);
void send_player_stat(int to_client, int id);
void send_player_stance_change(int to_client, int id, const bool& st);
void send_player_stage_change(int to_client, int id);

void process_move(int id, const _vec3& _vDir, const _vec3& _vPos);					// ������ ó�� �Լ�
void process_move_stop(int id, const _vec3& _vPos, const _vec3& _vDir);
void process_collide(int id, int colID);											// �ٸ� OBJECT�� �浹���� ���
void process_attack(int id, const _vec3& _vDir, const _vec3& _vPos, int aniIdx, float end_angleY);	// ���� ó��
void process_attack_stop(int id, const _vec3& _vDir, const _vec3& _vPos, int aniIdx);	// ���� �ߴ� ó��
void process_stance_change(int id, const bool& stance);
void process_stage_change(int id, const char& stage_id);

/* ��Ŷ ���� �� ó�� �Լ� */
void process_packet(int id);													// ��Ŷ ó�� �Լ� (��� ������ ó��)
void process_recv(int id, DWORD iosize);										// ��Ŷ ������ �Լ� (Ring Buffer ���)

/* ==========================MONSTER========================== */
//void active_monster(int id);													// �ش� Monster�� STATUS = ST_ACTIVE
//void nonActive_monster(int id);													// �ش� Monster�� STATUS = ST_NONACTIVE

/* =========================FUNC========================== */
void add_timer(int obj_id, OPMODE ev_type, system_clock::time_point t);
bool CAS(atomic<STATUS>* addr, STATUS* old_v, STATUS new_v);