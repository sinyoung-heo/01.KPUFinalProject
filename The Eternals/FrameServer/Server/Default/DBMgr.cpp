#include "stdafx.h"
#include "DBMgr.h"

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
				m_retcode = SQLConnect(m_hdbc, (SQLWCHAR*)L"gs_2015182030", SQL_NTS, (SQLWCHAR*)NULL, 0, NULL, 0);

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

bool CDBMgr::Check_ID(int id)
{
	return false;
}

void CDBMgr::Insert_NewPlayer_DB(int id)
{
}

void CDBMgr::Update_stat_DB(int id)
{
}

void CDBMgr::Update_move_DB(int id)
{
}

void CDBMgr::Release()
{
	Disconnect_DB();
}
