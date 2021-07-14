#pragma once

/* [게임 컨텐츠용 패킷] */
void send_packet(int id, void* p);												// Packet 전송 함수

void send_leave_packet(const int& to_client, const int& leave_id);				
void send_suggest_party(const int& to_client, const int& id);
void send_reject_party(const int& to_client, const int& id);
void send_join_party(const int& to_client, const int& id);


void process_move(int id, const _vec3& _vDir, const _vec3& _vPos);										// 움직임 처리 함수
void process_move_stop(int id, const _vec3& _vPos, const _vec3& _vDir);
void process_collide(int id, int colID, int damage);													// 다른 OBJECT와 충돌했을 경우
void process_attack(int id, const _vec3& _vDir, const _vec3& _vPos, int aniIdx, float end_angleY);		// 공격 처리
void process_attack_stop(int id, const _vec3& _vDir, const _vec3& _vPos, int aniIdx);					// 공격 중단 처리
void process_buff(const int& id, cs_packet_attack* p);
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
void process_equip_item(const int& id, const char& chItemSlotType, const char& chItemType, const char& chName);
void process_unequip_item(const int& id, const char& chItemSlotType, const char& chItemType, const char& chName);
void process_shopping(const int& id, cs_packet_shop* p);
void process_load_equipment(const int& id, const char& chItemSlotType, const char& chItemType, const char& chName);
void process_logoutForEquipment(const int& id, const char& chItemSlotType, const char& chItemType, const char& chName);
void process_use_potion(const int& id, const bool& bIsPotionHP);
void process_cinema_end(const int& id);

/* 패킷 관리 및 처리 함수 */
void process_packet(int id);													// 패킷 처리 함수 (모든 컨텐츠 처리)
void process_recv(int id, DWORD iosize);										// 패킷 재조립 함수 (Ring Buffer 사용)

/* =========================FUNC========================== */
void add_timer(int obj_id, OPMODE ev_type, system_clock::time_point t);
bool CAS(atomic<STATUS>* addr, STATUS* old_v, STATUS new_v);
void Check_Korean(const char* text, int* len);