#pragma once

/* =========================PLAYER======================== */
/* [���� �������� ��Ŷ] */
void send_packet(int id, void* p);												// Packet ���� �Լ�
void send_login_ok(int id);														// �α��� ���� ��Ŷ
void send_enter_packet(int to_client, int new_id);								// ���� ��Ŷ
void send_leave_packet(int to_client, int leave_id);							// ���� ��Ŷ
void send_move_packet(int to_client, int id);									// ������ ��Ŷ
void send_move_stop_packet(int to_client, int id);								// ������ ��Ŷ

void process_move(int id, const _vec3& _vDir, const _vec3& _vPos);				// ������ ó�� �Լ�
void process_move_stop(int id, const _vec3& _vPos, const _vec3& _vDir);

/* ��Ŷ ���� �� ó�� �Լ� */
void process_packet(int id);													// ��Ŷ ó�� �Լ� (��� ������ ó��)
void process_recv(int id, DWORD iosize);										// ��Ŷ ������ �Լ� (Ring Buffer ���)

/* ==========================NPC========================== */
void send_NPC_enter_packet(int to_client, int new_id);							// NPC ���� ��Ŷ
void send_NPC_move_packet(int to_client, int id);								// ������ ��Ŷ

void random_move_npc(int id);													// NPC ���� ������
void active_npc(int id);														// �ش� NPC�� STATUS = ST_ACTIVE

/* ==========================MONSTER========================== */
void send_Monster_enter_packet(int to_client, int new_id);						// Monster ���� ��Ŷ

/* =========================FUNC========================== */
void add_timer(int obj_id, OPMODE ev_type, system_clock::time_point t);
bool CAS(atomic<STATUS>* addr, STATUS* old_v, STATUS new_v);