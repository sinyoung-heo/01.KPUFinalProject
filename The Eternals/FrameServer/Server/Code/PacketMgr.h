#pragma once

/* =========================PLAYER======================== */
/* [���� �������� ��Ŷ] */
void send_packet(int id, void* p);												// Packet ���� �Լ�
void send_login_ok(int id);														// �α��� ���� ��Ŷ
void send_enter_packet(int to_client, int new_id);								// ���� ��Ŷ
void send_leave_packet(int to_client, int leave_id);							// ���� ��Ŷ
void send_move_packet(int to_client, int id);									// ������ ��Ŷ
void send_chat_packet(int to, int id, char* message);							// ä�� ��Ŷ
void send_stat_change_packet(int to, int id);									// ��������(HP,LV,EXP) ���� ��Ŷ

void process_move(int id, cs_packet_move* info);								// ������ ó�� �Լ�
void process_attck(int id);														// ���� ó�� �Լ�

/* ��Ŷ ���� �� ó�� �Լ� */
void process_packet(int id);													// ��Ŷ ó�� �Լ� (��� ������ ó��)
void process_recv(int id, DWORD iosize);										// ��Ŷ ������ �Լ� (Ring Buffer ���)


/* ==========================NPC========================== */
void send_NPC_enter_packet(int to_client, int new_id);							// NPC���� ��Ŷ
void send_NPC_move_packet(int to_client, int id);								// ������ ��Ŷ

void random_move_npc(int id);													// NPC ���� ������

void active_npc(int id);														// �ش� NPC�� STATUS = ST_ACTIVE

/* =========================FUNC========================== */
void add_timer(int obj_id, OPMODE ev_type, system_clock::time_point t);
bool CAS(atomic<STATUS>* addr, STATUS* old_v, STATUS new_v);