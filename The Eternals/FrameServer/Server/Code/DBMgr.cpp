#include "stdafx.h"
#include "DBMgr.h"
#include "Player.h"

IMPLEMENT_SINGLETON(CDBMgr)

CDBMgr::CDBMgr(void)
{
}

CDBMgr::~CDBMgr(void)
{
}

HRESULT CDBMgr::Ready_DB()
{
	// Allocate environment handle  
	m_retcode = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &m_henv);

	// Set the ODBC version environment attribute  
	if (m_retcode == SQL_SUCCESS || m_retcode == SQL_SUCCESS_WITH_INFO)
	{
		m_retcode = SQLSetEnvAttr(m_henv, SQL_ATTR_ODBC_VERSION, (void*)SQL_OV_ODBC3, 0);

		// Allocate connection handle  
		if (m_retcode == SQL_SUCCESS || m_retcode == SQL_SUCCESS_WITH_INFO)
		{
			m_retcode = SQLAllocHandle(SQL_HANDLE_DBC, m_henv, &m_hdbc);

			// Set login timeout to 5 seconds  
			if (m_retcode == SQL_SUCCESS || m_retcode == SQL_SUCCESS_WITH_INFO)
			{
				SQLSetConnectAttr(m_hdbc, SQL_LOGIN_TIMEOUT, (SQLPOINTER)5, 0);

				// Connect to data source  
				m_retcode = SQLConnect(m_hdbc, (SQLWCHAR*)L"Eternals_DBServer", SQL_NTS, (SQLWCHAR*)NULL, 0, NULL, 0);

				// Allocate statement handle  
				if (m_retcode == SQL_SUCCESS || m_retcode == SQL_SUCCESS_WITH_INFO)
				{
					cout << "DATABASE Connected" << endl;
					m_retcode = SQLAllocHandle(SQL_HANDLE_STMT, m_hdbc, &m_hstmt);
				}
				else
				{
					db_show_error(m_hdbc, SQL_HANDLE_DBC, m_retcode);
					return E_FAIL;
				}
			}
		}
	}

	return S_OK;
}

void CDBMgr::Disconnect_DB()
{
	SQLFreeHandle(SQL_HANDLE_STMT, m_hstmt);
	SQLDisconnect(m_hdbc);
	SQLFreeHandle(SQL_HANDLE_DBC, m_hdbc);
	SQLFreeHandle(SQL_HANDLE_ENV, m_henv);
}

HRESULT CDBMgr::Ready_GameItem(map<int, GAMEITEM>& mapGameItems)
{
	SQLINTEGER u_itemType, u_itemName, u_itemNumber, u_itemAtt, u_itemHP, u_itemMP, u_itemCost;

	SQLLEN cbLen = 0;

	/* 유효한 ID인지 검사하는 쿼리문 */
	std::string str_order
		= "SELECT * FROM GAME_ITEM";

	std::wstring wsr_order = L"";
	wsr_order.assign(str_order.begin(), str_order.end());

	m_retcode = SQLExecDirect(m_hstmt, (SQLWCHAR*)wsr_order.c_str(), SQL_NTS);

	// 데이터 처리 시작 - 유효한 ID일 경우
	if (m_retcode == SQL_SUCCESS || m_retcode == SQL_SUCCESS_WITH_INFO)
	{
		/* Load Data on player info */
		m_retcode = SQLBindCol(m_hstmt, 1, SQL_C_LONG, &u_itemNumber, 100, &cbLen);
		m_retcode = SQLBindCol(m_hstmt, 2, SQL_C_LONG, &u_itemType, 100, &cbLen);
		m_retcode = SQLBindCol(m_hstmt, 3, SQL_C_LONG, &u_itemName, 100, &cbLen);
		m_retcode = SQLBindCol(m_hstmt, 4, SQL_C_LONG, &u_itemAtt, 100, &cbLen);
		m_retcode = SQLBindCol(m_hstmt, 5, SQL_C_LONG, &u_itemHP, 100, &cbLen);
		m_retcode = SQLBindCol(m_hstmt, 6, SQL_C_LONG, &u_itemMP, 100, &cbLen);
		m_retcode = SQLBindCol(m_hstmt, 7, SQL_C_LONG, &u_itemCost, 100, &cbLen);

		for (int i = 0; ; i++)
		{
			m_retcode = SQLFetch(m_hstmt);
			if (m_retcode == SQL_ERROR || m_retcode == SQL_SUCCESS_WITH_INFO)
				db_show_error(m_hstmt, SQL_HANDLE_STMT, m_retcode);

			/* player info를 올바르게 로드했다면 정보 저장 */
			if (m_retcode == SQL_SUCCESS || m_retcode == SQL_SUCCESS_WITH_INFO)
			{		
				mapGameItems[u_itemNumber] = GAMEITEM(static_cast<char>(u_itemType), static_cast<char>(u_itemName), u_itemNumber, u_itemAtt, u_itemHP, u_itemMP, u_itemCost, 1);
			}
			else
				break;
		}
		SQLCloseCursor(m_hstmt);
		return S_OK;
	}
	else db_show_error(m_hstmt, SQL_HANDLE_STMT, m_retcode);

	SQLCloseCursor(m_hstmt);

	return E_FAIL;
}

bool CDBMgr::Check_ID(int id, char* pw)
{
	CPlayer* pPlayer = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", id));
	
	SQLCHAR name[MAX_STR_LEN], password[MAX_STR_LEN];
	SQLINTEGER u_type, u_level, u_Hp, u_maxHp, u_Exp, u_maxExp, u_Mp, u_maxMp, u_maxAtt, u_minAtt, u_money, u_StageID;
	SQLFLOAT u_posX, u_posY, u_posZ;

	SQLLEN cbLen = 0;

	/* 유효한 ID인지 검사하는 쿼리문 */
	std::string str_order
		= "SELECT * FROM USER_DATA WHERE user_ID = '";

	str_order += pPlayer->m_ID;
	str_order += "' and user_password = '";
	str_order += pw;
	str_order += "'";

	std::wstring wsr_order = L"";
	wsr_order.assign(str_order.begin(), str_order.end());

	m_retcode = SQLExecDirect(m_hstmt, (SQLWCHAR*)wsr_order.c_str(), SQL_NTS);

	// 데이터 처리 시작 - 유효한 ID일 경우
	if (m_retcode == SQL_SUCCESS || m_retcode == SQL_SUCCESS_WITH_INFO)
	{
		/* Load Data on player info */
		m_retcode = SQLBindCol(m_hstmt, 1, SQL_C_CHAR, &name, 100, &cbLen);
		m_retcode = SQLBindCol(m_hstmt, 2, SQL_C_CHAR, &password, 100, &cbLen);
		m_retcode = SQLBindCol(m_hstmt, 3, SQL_C_DOUBLE, &u_posX, 100, &cbLen);
		m_retcode = SQLBindCol(m_hstmt, 4, SQL_C_DOUBLE, &u_posY, 100, &cbLen);
		m_retcode = SQLBindCol(m_hstmt, 5, SQL_C_DOUBLE, &u_posZ, 100, &cbLen);
		m_retcode = SQLBindCol(m_hstmt, 6, SQL_C_LONG, &u_type, 100, &cbLen);
		m_retcode = SQLBindCol(m_hstmt, 7, SQL_C_LONG, &u_level, 100, &cbLen);
		m_retcode = SQLBindCol(m_hstmt, 8, SQL_C_LONG, &u_Hp, 100, &cbLen);
		m_retcode = SQLBindCol(m_hstmt, 9, SQL_C_LONG, &u_maxHp, 100, &cbLen);
		m_retcode = SQLBindCol(m_hstmt, 10, SQL_C_LONG, &u_Exp, 100, &cbLen);
		m_retcode = SQLBindCol(m_hstmt, 11, SQL_C_LONG, &u_maxExp, 100, &cbLen);
		m_retcode = SQLBindCol(m_hstmt, 12, SQL_C_LONG, &u_Mp, 100, &cbLen);
		m_retcode = SQLBindCol(m_hstmt, 13, SQL_C_LONG, &u_maxMp, 100, &cbLen);
		m_retcode = SQLBindCol(m_hstmt, 14, SQL_C_LONG, &u_maxAtt, 100, &cbLen);
		m_retcode = SQLBindCol(m_hstmt, 15, SQL_C_LONG, &u_minAtt, 100, &cbLen);
		m_retcode = SQLBindCol(m_hstmt, 16, SQL_C_LONG, &u_money, 100, &cbLen);
		m_retcode = SQLBindCol(m_hstmt, 17, SQL_C_LONG, &u_StageID, 100, &cbLen);

		for (int i = 0; ; i++)
		{
			m_retcode = SQLFetch(m_hstmt);
			if (m_retcode == SQL_ERROR || m_retcode == SQL_SUCCESS_WITH_INFO)
				db_show_error(m_hstmt, SQL_HANDLE_STMT, m_retcode);

			/* player info를 올바르게 로드했다면 정보 저장 */
			if (m_retcode == SQL_SUCCESS || m_retcode == SQL_SUCCESS_WITH_INFO)
			{
				pPlayer->m_vPos			= _vec3((float)u_posX, (float)u_posY, (float)u_posZ);
				pPlayer->m_type			= (char)u_type;
				pPlayer->m_iLevel		= u_level;
				pPlayer->m_iHp			= u_Hp;
				pPlayer->m_iMaxHp		= u_maxHp;
				pPlayer->m_iExp			= u_Exp;
				pPlayer->m_iMaxExp		= u_maxExp;
				pPlayer->m_iMp			= u_Mp;
				pPlayer->m_iMaxMp		= u_maxMp;
				pPlayer->m_iMaxAtt		= u_maxAtt;
				pPlayer->m_iMinAtt		= u_minAtt;
				pPlayer->m_iMoney		= u_money;
				pPlayer->m_chStageId	= (char)u_StageID;

				SQLCloseCursor(m_hstmt);
				return true;
			}
			else
				break;
		}
	}
	else db_show_error(m_hstmt, SQL_HANDLE_STMT, m_retcode);

	SQLCloseCursor(m_hstmt);

	return false;
}

bool CDBMgr::Load_Equipment(const int& id)
{
	CPlayer* pPlayer = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", id));

	SQLCHAR name[MAX_STR_LEN];
	SQLINTEGER u_weapon, u_armor, u_helmet, u_shoes;

	SQLLEN cbLen = 0;

	/* 유효한 ID인지 검사하는 쿼리문 */
	std::string str_order
		= "SELECT * FROM USER_EQUIPMENT WHERE user_ID = '";

	str_order += pPlayer->m_ID;
	str_order += "'";

	std::wstring wsr_order = L"";
	wsr_order.assign(str_order.begin(), str_order.end());

	m_retcode = SQLExecDirect(m_hstmt, (SQLWCHAR*)wsr_order.c_str(), SQL_NTS);

	// 데이터 처리 시작 - 유효한 ID일 경우
	if (m_retcode == SQL_SUCCESS || m_retcode == SQL_SUCCESS_WITH_INFO)
	{
		/* Load Data on player info */
		m_retcode = SQLBindCol(m_hstmt, 1, SQL_C_CHAR, &name, 100, &cbLen);
		m_retcode = SQLBindCol(m_hstmt, 2, SQL_C_LONG, &u_weapon, 100, &cbLen);
		m_retcode = SQLBindCol(m_hstmt, 3, SQL_C_LONG, &u_armor, 100, &cbLen);
		m_retcode = SQLBindCol(m_hstmt, 4, SQL_C_LONG, &u_helmet, 100, &cbLen);
		m_retcode = SQLBindCol(m_hstmt, 5, SQL_C_LONG, &u_shoes, 100, &cbLen);
	
		for (int i = 0; ; i++)
		{
			m_retcode = SQLFetch(m_hstmt);
			if (m_retcode == SQL_ERROR || m_retcode == SQL_SUCCESS_WITH_INFO)
				db_show_error(m_hstmt, SQL_HANDLE_STMT, m_retcode);

			/* player info를 올바르게 로드했다면 정보 저장 */
			if (m_retcode == SQL_SUCCESS || m_retcode == SQL_SUCCESS_WITH_INFO)
			{
				pPlayer->Load_EquipmentFromDB(u_weapon, u_helmet, u_armor, u_shoes);
				
				SQLCloseCursor(m_hstmt);
				return true;
			}
			else
				break;
		}
	}
	else db_show_error(m_hstmt, SQL_HANDLE_STMT, m_retcode);

	SQLCloseCursor(m_hstmt);

	return false;
}

void CDBMgr::update_Equipment(const int& id)
{
	CPlayer* pPlayer = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", id));

	// ID
	string name{ "'" };
	name += pPlayer->m_ID;
	name += "'";
	
	std::string str_order
		= "EXEC update_equipment " + name + ", "
		+ to_string(static_cast<int>(pPlayer->Get_All_Equipment().weapon)) + ", "
		+ to_string(static_cast<int>(pPlayer->Get_All_Equipment().armor)) + ", "
		+ to_string(static_cast<int>(pPlayer->Get_All_Equipment().helmet)) + ", "
		+ to_string(static_cast<int>(pPlayer->Get_All_Equipment().shoes));

	std::wstring wstr_order = L"";
	wstr_order.assign(str_order.begin(), str_order.end());
	m_retcode = SQLExecDirect(m_hstmt, (SQLWCHAR*)wstr_order.c_str(), SQL_NTS);

	if (!(m_retcode == SQL_SUCCESS || m_retcode == SQL_SUCCESS_WITH_INFO))
		db_show_error(m_hstmt, SQL_HANDLE_STMT, m_retcode);

#ifdef TEST
	cout << "인벤토리 저장 완료" << endl;
#endif // TEST

	SQLCloseCursor(m_hstmt);
	SQLCancel(m_hstmt);
}

bool CDBMgr::Load_Inventory(const int& id)
{
	CPlayer* pPlayer = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", id));

	SQLINTEGER u_itemType, u_itemName, u_itemNumber, u_itemAtt, u_itemHP, u_itemMP, u_itemCost, u_itemCount;

	SQLLEN cbLen = 0;

	/* 유저의 인벤토리를 탐색하는 쿼리문 */
	std::string str_order
		= "SELECT ITEM.item_type,ITEM.item_name,ITEM.item_number,ITEM.item_att,ITEM.item_hp,ITEM.item_mp,ITEM.item_cost,INVEN.item_count\
			FROM USER_INVENTORY AS INVEN\
			JOIN GAME_ITEM AS ITEM\
			ON INVEN.item_number = ITEM.item_number\
			WHERE user_ID = '";

	str_order += pPlayer->m_ID;
	str_order += "'";

	std::wstring wsr_order = L"";
	wsr_order.assign(str_order.begin(), str_order.end());

	m_retcode = SQLExecDirect(m_hstmt, (SQLWCHAR*)wsr_order.c_str(), SQL_NTS);

	// 데이터 처리 시작 - 유효한 ID일 경우
	if (m_retcode == SQL_SUCCESS || m_retcode == SQL_SUCCESS_WITH_INFO)
	{
		/* Load Data on player info */
		m_retcode = SQLBindCol(m_hstmt, 1, SQL_C_LONG, &u_itemType, 100, &cbLen);
		m_retcode = SQLBindCol(m_hstmt, 2, SQL_C_LONG, &u_itemName, 100, &cbLen);
		m_retcode = SQLBindCol(m_hstmt, 3, SQL_C_LONG, &u_itemNumber, 100, &cbLen);
		m_retcode = SQLBindCol(m_hstmt, 4, SQL_C_LONG, &u_itemAtt, 100, &cbLen);
		m_retcode = SQLBindCol(m_hstmt, 5, SQL_C_LONG, &u_itemHP, 100, &cbLen);
		m_retcode = SQLBindCol(m_hstmt, 6, SQL_C_LONG, &u_itemMP, 100, &cbLen);
		m_retcode = SQLBindCol(m_hstmt, 7, SQL_C_LONG, &u_itemCost, 100, &cbLen);
		m_retcode = SQLBindCol(m_hstmt, 8, SQL_C_LONG, &u_itemCount, 100, &cbLen);

		for (int i = 0; ; i++)
		{
			m_retcode = SQLFetch(m_hstmt);
			if (m_retcode == SQL_ERROR || m_retcode == SQL_SUCCESS_WITH_INFO)
				db_show_error(m_hstmt, SQL_HANDLE_STMT, m_retcode);

			/* player info를 올바르게 로드했다면 정보 저장 */
			if (m_retcode == SQL_SUCCESS || m_retcode == SQL_SUCCESS_WITH_INFO)
			{
				pPlayer->Load_InventoryFromDB(GAMEITEM(static_cast<char>(u_itemType), 
											  static_cast<char>(u_itemName),
											  u_itemNumber,
											  u_itemAtt, u_itemHP, u_itemMP, u_itemCost,
											  u_itemCount),
											  u_itemNumber, u_itemCount);
			}
			else
				break;
		}

		SQLCloseCursor(m_hstmt);
		return true;
	}
	else db_show_error(m_hstmt, SQL_HANDLE_STMT, m_retcode);

	SQLCloseCursor(m_hstmt);

	return false;
}

void CDBMgr::Insert_NewPlayer_DB(int id, char* pw)
{
	CPlayer* pPlayer = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", id));

	// ID
	string name{ "'" };
	name += pPlayer->m_ID;
	name += "'";
	// password
	string password{ "'" };
	password += pw;
	password += "'";

	/* 회원 가입 */
	std::string str_order
		= "EXEC insert_user " + name + ", " + password + ", "
		+ to_string(pPlayer->m_vPos.x) + ", "
		+ to_string(pPlayer->m_vPos.y) + ", "
		+ to_string(pPlayer->m_vPos.z) + ", "
		+ to_string(pPlayer->m_type) + ", "
		+ to_string(pPlayer->m_iLevel) + ", "
		+ to_string(pPlayer->m_iHp) + ", "
		+ to_string(pPlayer->m_iMaxHp) + ", "
		+ to_string(pPlayer->m_iExp) + ", "
		+ to_string(pPlayer->m_iMaxExp) + ", "
		+ to_string(pPlayer->m_iMp) + ", "
		+ to_string(pPlayer->m_iMaxMp) + ", "
		+ to_string(pPlayer->m_iMaxAtt) + ", "
		+ to_string(pPlayer->m_iMinAtt) + ", "
		+ to_string(pPlayer->m_iMoney) + ", "
		+ to_string((int)pPlayer->m_chStageId);

	std::wstring wstr_order = L"";
	wstr_order.assign(str_order.begin(), str_order.end());
	m_retcode = SQLExecDirect(m_hstmt, (SQLWCHAR*)wstr_order.c_str(), SQL_NTS);

	if (!(m_retcode == SQL_SUCCESS || m_retcode == SQL_SUCCESS_WITH_INFO))
		db_show_error(m_hstmt, SQL_HANDLE_STMT, m_retcode);

#ifdef TEST
	cout << "회원 가입 완료" << endl;
#endif // TEST

	SQLCloseCursor(m_hstmt);
	SQLCancel(m_hstmt);
}

void CDBMgr::Update_stat_DB(int id)
{
	CPlayer* pPlayer = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", id));

	if (false == pPlayer->Get_IsConnected())
		return;

	// ID
	string name{ "'" };
	name += pPlayer->m_ID;
	name += "'";

	/* Player Stat 갱신 */
	std::string str_order
		= "EXEC update_user_info "
		+ to_string(pPlayer->m_vPos.x) + ", "
		+ to_string(pPlayer->m_vPos.y) + ", "
		+ to_string(pPlayer->m_vPos.z) + ", "
		+ to_string(pPlayer->m_iLevel) + ", "
		+ to_string(pPlayer->m_iHp) + ", "
		+ to_string(pPlayer->m_iMaxHp) + ", "
		+ to_string(pPlayer->m_iExp) + ", "
		+ to_string(pPlayer->m_iMaxExp) + ", "
		+ to_string(pPlayer->m_iMp) + ", "
		+ to_string(pPlayer->m_iMaxMp) + ", "
		+ to_string(pPlayer->m_iMaxAtt) + ", "
		+ to_string(pPlayer->m_iMinAtt) + ", "
		+ to_string(pPlayer->m_iMoney) + ", "
		+ to_string((int)pPlayer->m_chStageId) + ", "
		+ name;
		
	std::wstring wstr_order = L"";
	wstr_order.assign(str_order.begin(), str_order.end());
	m_retcode = SQLExecDirect(m_hstmt, (SQLWCHAR*)wstr_order.c_str(), SQL_NTS);

	if (!(m_retcode == SQL_SUCCESS || m_retcode == SQL_SUCCESS_WITH_INFO))
		db_show_error(m_hstmt, SQL_HANDLE_STMT, m_retcode);

	SQLCloseCursor(m_hstmt);
	SQLCancel(m_hstmt);	
}

void CDBMgr::update_Inventory(const int& id, const int& itemNumber, const int& itemCount)
{
	CPlayer* pPlayer = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", id));

	// ID
	string name{ "'" };
	name += pPlayer->m_ID;
	name += "'";

	std::string str_order
		= "EXEC update_inventory " + name + ", "
		+ to_string(itemNumber) + ", "
		+ to_string(itemCount);

	std::wstring wstr_order = L"";
	wstr_order.assign(str_order.begin(), str_order.end());
	m_retcode = SQLExecDirect(m_hstmt, (SQLWCHAR*)wstr_order.c_str(), SQL_NTS);

	if (!(m_retcode == SQL_SUCCESS || m_retcode == SQL_SUCCESS_WITH_INFO))
		db_show_error(m_hstmt, SQL_HANDLE_STMT, m_retcode);

#ifdef TEST
	cout << "인벤토리 저장 완료" << endl;
#endif // TEST

	SQLCloseCursor(m_hstmt);
	SQLCancel(m_hstmt);
}

void CDBMgr::db_show_error(SQLHANDLE hHandle, SQLSMALLINT hType, RETCODE RetCode)
{
	SQLSMALLINT iRec = 0;
	SQLINTEGER iError;
	WCHAR wszMessage[1000];
	WCHAR wszState[SQL_SQLSTATE_SIZE + 1];
	if (RetCode == SQL_INVALID_HANDLE) {
		//wcout << L"Invalid handle!\n";
		return;
	}
	while (SQLGetDiagRec(hType, hHandle, ++iRec, wszState, &iError, wszMessage,
		(SQLSMALLINT)(sizeof(wszMessage) / sizeof(WCHAR)), (SQLSMALLINT*)NULL) == SQL_SUCCESS) {
		// Hide data truncated..
		if (wcsncmp(wszState, L"01004", 5)) {
			wcout << L"[" << wszState << L"]" << wszMessage << L"(" << iError << L")\n";
		}
	}
}

void CDBMgr::Release()
{
	Disconnect_DB();
}
