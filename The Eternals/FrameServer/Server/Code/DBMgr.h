#pragma once
class CDBMgr
{
	DECLARE_SINGLETON(CDBMgr)

private:
	explicit CDBMgr(void);
	virtual ~CDBMgr(void);

public:
	HRESULT Ready_DB();								// Connect to Database Server 
	void Disconnect_DB();							// Disconnect Database Server

	HRESULT Ready_GameItem(map<int, GAMEITEM>& mapGameItems);

public:
	/* 장비창 이벤트 */
	bool Load_Equipment(const int& id);
	void update_Equipment(const int& id);

	/* 인벤토리 이벤트 */
	bool Load_Inventory(const int& id);
	void insert_Inventory(const int& id, const int& itemNumber, const int& itemCount);
	void update_Inventory(const int& id, const int& itemNumber, const int& itemCount);
	void delete_Inventory(const int& id, const int& itemNumber);

	/* 유저 이벤트 */
	bool Check_ID(int id, char* pw);				// Check Id in Login Server
	void Insert_NewPlayer_DB(int id, char* pw);		// Insert New Player in to Database Server
	void Update_stat_DB(int id);					// Update User Stat in to Database Server

private:
	void db_show_error(SQLHANDLE hHandle, SQLSMALLINT hType, RETCODE RetCode);
public:
	void Release();

private:
	/* DATABASE 관련 변수 */
	SQLHDBC m_hdbc;
	SQLHSTMT m_hstmt;
	SQLRETURN m_retcode;
	SQLHENV m_henv;
};

