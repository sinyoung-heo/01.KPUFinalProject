#pragma once
/* ���� �������� ��Ŷ */
void send_packet(int id, void* p);												// Packet ���� �Լ�
void send_login_ok(int id);														// �α��� ���� ��Ŷ
void send_chat_packet(int to, int id, char* message);							// ä�� ��Ŷ
void send_move_packet(int to_client, int id);									// ������ ��Ŷ
void send_enter_packet(int to_client, int new_id);								// ���� ��Ŷ
void send_leave_packet(int to_client, int new_id);								// ���� ��Ŷ
void send_stat_change_packet(int to, int id);									// ��������(HP,LV,EXP) ���� ��Ŷ

void process_move(int id, char dir);											// ������ ó�� �Լ�
void process_attck(int id);														// ���� ó�� �Լ�
void update_view_leave(int id);													// NPC�� ���� �� ������ �þ� ����					

/* ��Ŷ ���� �� ó�� �Լ� */
void process_packet(int id);													// ��Ŷ ó�� �Լ� (��� ������ ó��)
void process_recv(int id, DWORD iosize);										// ��Ŷ ������ �Լ�
