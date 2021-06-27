#pragma once

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
void send_suggest_party(int to_client, int id);
void send_enter_party(int to_client, int id, const int& hp, const int& maxHp, const int& mp, const int& maxMp, const char* ID, const char& job);
void send_reject_party(int to_client, int id);
void send_join_party(int to_client, int id);
void send_leave_party(int to_client, int id);
void send_update_party(const int& to_client, const int& id, const int& hp, const int& maxHp, const int& mp, const int& maxMp);
void send_chat(const int& to_client, const int& id, const char* name, const char* buffer, const int len);
void send_update_inventory(const int& id, const char& chItemType, const char& chName, const int& count, const bool& isPushItem);
void send_update_equipment(const int& to_client, const char& chItemType, const bool& isPushItem);
void send_load_equipment(const int& to_client, const char* chItemType, const char* chName);

void process_move(int id, const _vec3& _vDir, const _vec3& _vPos);										// ������ ó�� �Լ�
void process_move_stop(int id, const _vec3& _vPos, const _vec3& _vDir);
void process_collide(int id, int colID, int damage);													// �ٸ� OBJECT�� �浹���� ���
void process_attack(int id, const _vec3& _vDir, const _vec3& _vPos, int aniIdx, float end_angleY);		// ���� ó��
void process_attack_stop(int id, const _vec3& _vDir, const _vec3& _vPos, int aniIdx);					// ���� �ߴ� ó��
void process_stance_change(int id, const bool& stance);
void process_stage_change(int id, const char& stage_id);
void process_suggest_party(const int& suggester_id, const int& others_id);
void process_respond_party(const bool& result, const int& suggester_id, const int& responder_id);
void process_join_party(const int& joinner_id, const int& others_id);
void process_decide_party(const bool& result, const int& joinner_id, const int& responder_id);
void process_leave_party(const int& id);
void process_disconnect(const int& id);
void process_chat(const int& id, const char* buffer);
void process_add_item(const int& id, const char& chItemType, const char& chName);
void process_delete_item(const int& id, const char& chItemType, const char& chName);
void process_equip_item(const int& id, const char& chItemType, const char& chName);
void process_unequip_item(const int& id, const char& chItemType, const char& chName);

/* ��Ŷ ���� �� ó�� �Լ� */
void process_packet(int id);													// ��Ŷ ó�� �Լ� (��� ������ ó��)
void process_recv(int id, DWORD iosize);										// ��Ŷ ������ �Լ� (Ring Buffer ���)

/* =========================FUNC========================== */
void add_timer(int obj_id, OPMODE ev_type, system_clock::time_point t);
bool CAS(atomic<STATUS>* addr, STATUS* old_v, STATUS new_v);
void Check_Korean(const char* text, int* len);