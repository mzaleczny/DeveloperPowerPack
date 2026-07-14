#include "Tilc/Data/DbMySQL.h"
#include "Tilc/Utils/ExtString.h"
#include "Tilc/Utils/Log.h"
#include <iostream>
#include <sstream>

Tilc::Data::TDBMySQL::TDBMySQL(const Tilc::TExtString& DbHost, const Tilc::TExtString& DbName, const Tilc::TExtString& DbUser, const Tilc::TExtString& DbPasswd)
	: Tilc::Data::TDB()
{
    // Open database
    m_Conn.reset(GetConnection(nullptr, DbHost.c_str(), DbName.c_str(), DbUser.c_str(), DbPasswd.c_str()));
    if (m_Conn)
    {
        IsOpenDB = true;
    }
}

Tilc::Data::TDBMySQL::~TDBMySQL()
{
    if (IsOpenDB)
    {
        m_Conn.reset();
        IsOpenDB = false;
    }
}

sql::Connection* Tilc::Data::TDBMySQL::GetConnection(sql::Driver* driver,
    const char* Host,
    const char* DbName,
    const char* User,
    const char* Passwd,
    sql::ConnectOptionsMap* additional_options)
{
    if (driver == nullptr)
    {
        driver = sql::mariadb::get_driver_instance();
    }

    sql::ConnectOptionsMap connection_properties;
    connection_properties["user"] = User;
    connection_properties["password"] = Passwd;

    connection_properties["useTls"] = "false";
    //connection_properties["metadataUseInfoSchema"] = "1";

    if (additional_options != nullptr)
    {
        for (sql::ConnectOptionsMap::const_iterator cit = additional_options->cbegin(); cit != additional_options->cend(); ++cit)
        {
            connection_properties[cit->first] = cit->second;
        }
    }

    std::string url = std::string("jdbc:mariadb") + "://" + Host + ":3306/" + DbName;
    std::cout << url << std::endl;
    sql::Connection* Conn{};
    try
    {
        Conn = sql::DriverManager::getConnection(url.c_str(), connection_properties);
    }
    catch (sql::SQLWarning& e)
    {
        std::cerr << "SQL warning: " << e.getMessage().c_str() << std::endl;
        std::cerr << "SQLState: " + std::string(e.getSQLState()) << std::endl;
    }
    catch (sql::InvalidArgumentException& e)
    {
        std::cerr << "Invalid argument: " << e.what() << std::endl;
    }
    catch (sql::SQLException& e)
    {
        std::cerr << "SQL error: " << e.what() << std::endl;
        std::cerr << "SQLState: " + std::string(e.getSQLState()) << std::endl;
    }
    catch (std::exception& e)
    {
        std::cerr << "Connection error: " << e.what() << std::endl;
    }
    return Conn;
}

void Tilc::Data::TDBMySQL::ShowWarningsForStatement(Tilc::Data::TDBMySQL::Statement& stmt)
{
    int count = 0;
    std::stringstream msg;
    for (const sql::SQLWarning* warn = stmt->getWarnings(); warn; warn = warn->getNextWarning())
    {
        ++count;
        msg.str("");
        msg << "... ErrorCode = '" << warn->getErrorCode() << "', ";
        msg << "SQLState = '" << warn->getSQLState() << "', ";
        msg << "ErrorMessage = '" << warn->getMessage() << "'";
        std::cerr << msg.str();
    }
}

int Tilc::Data::TDBMySQL::ExecuteQuery(ResultSet& res, Statement& stmt, const char* query)
{
    try
    {
        res.reset(stmt->executeQuery(query));
        return 0;
    }
    catch (sql::SQLWarning& e)
    {
        std::cerr << "SQL warning: " << e.getMessage().c_str() << std::endl;
        std::cerr << "SQLState: " + std::string(e.getSQLState()) << std::endl;
        return e.getErrorCode();
    }
    catch (sql::InvalidArgumentException& e)
    {
        std::cerr << "Invalid argument: " << e.what() << std::endl;
        return e.getErrorCode();
    }
    catch (sql::SQLException& e)
    {
        std::cerr << "SQL error: " << e.what() << std::endl;
        std::cerr << "SQLState: " + std::string(e.getSQLState()) << std::endl;
        return e.getErrorCode();
    }
    catch (std::exception& e)
    {
        std::cerr << "Connection error: " << e.what() << std::endl;
        return -1;
    }
}

int Tilc::Data::TDBMySQL::CreateTable(const char* CreateQuery)
{
    Statement stmt;
    ResultSet res;
    stmt.reset(m_Conn->createStatement());
    int Result = ExecuteQuery(res, stmt, CreateQuery);
    res->close();
    return Result;
}

int Tilc::Data::TDBMySQL::Select(const char* Sql, TDBDataRows& DataRows)
{
    Statement stmt;
    ResultSet res;
    stmt.reset(m_Conn->createStatement());

    DataRows.clear();

    int Result = ExecuteQuery(res, stmt, Sql);
    if (Result)
    {
        return Result;
    }

    sql::ResultSetMetaData* meta = res->getMetaData();
    while (res->next())
    {
        Tilc::Data::TDBDataRow Row;
        for (int i = 1; i <= meta->getColumnCount(); ++i)
        {
            Row.push_back(res->getString(i).c_str());
        }
        DataRows.push_back(std::move(Row));
    }
    res->close();
    return static_cast<int>(DataRows.size());
}

int Tilc::Data::TDBMySQL::Select(const char* Sql, const TDBFieldTypes& FieldTypes, const TStringVector& FieldValues, TDBDataRows& DataRows)
{
    PreparedStatement pstmt;
    ResultSet res;
    int Result;
    int ColumnCount;

    // Execute SQL statement
    DataRows.clear();

    std::string StringSql{ Sql };
    ColumnCount = std::count(StringSql.begin(), StringSql.end(), '?');

    pstmt.reset(m_Conn->prepareStatement(Sql));
    for (int i = 1; i <= ColumnCount; ++i)
    {
        pstmt->setString(i, FieldValues[i-1]);
    }
    res.reset(pstmt->executeQuery());
    sql::ResultSetMetaData* meta = res->getMetaData();
    while (res->next())
    {
        Tilc::Data::TDBDataRow Row;
        for (int i = 1; i <= meta->getColumnCount(); ++i)
        {
            Row.push_back(res->getString(i).c_str());
        }
        DataRows.push_back(std::move(Row));
    }
    res->close();

    return static_cast<int>(DataRows.size());
}

int Tilc::Data::TDBMySQL::ExecQuery(const char* Sql, const TDBFieldTypes& FieldTypes, const TStringVector& FieldValues)
{
    PreparedStatement pstmt;
    int Result;
    int ColumnCount;

    Tilc::TExtString StringSql{ Sql };
    ColumnCount = std::count(StringSql.begin(), StringSql.end(), '?');

    pstmt.reset(m_Conn->prepareStatement(Sql));
    for (int i = 1; i <= ColumnCount; ++i)
    {
        pstmt->setString(i, FieldValues[i - 1]);
    }
    if (StringSql.StartsWith("SELECT"))
    {
        pstmt->executeQuery();
    }
    else
    {
        pstmt->executeUpdate();
    }

    return 0;
}

int Tilc::Data::TDBMySQL::Insert(const char* InsertSql, const TDBFieldTypes& FieldTypes, const TStringVector& FieldValues)
{
    PreparedStatement pstmt;
    int Result;
    int ColumnCount;

    std::string StringSql{ InsertSql };
    ColumnCount = std::count(StringSql.begin(), StringSql.end(), '?');

    pstmt.reset(m_Conn->prepareStatement(InsertSql));
    for (int i = 1; i <= ColumnCount; ++i)
    {
        pstmt->setString(i, FieldValues[i - 1]);
    }
    pstmt->executeUpdate();

    return 0;
};
