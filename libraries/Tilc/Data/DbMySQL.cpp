#include "Tilc/Data/DbMySQL.h"
#include "Tilc/Utils/ExtString.h"
#include "Tilc/Utils/Log.h"

Tilc::TExtString Tilc::Data::TDBMySQLField::toString()
{
    return Tilc::TExtString("  m_Type: ") + std::to_string(m_Type) +
        Tilc::TExtString("  m_Integer: ") + std::to_string(m_Integer) +
        Tilc::TExtString("  m_Float: ") + std::to_string(m_Float) +
        Tilc::TExtString("  m_Double: ") + std::to_string(m_Double) +
        Tilc::TExtString("  m_Length: ") + std::to_string(m_Length) +
        Tilc::TExtString("  m_ReadLength: ") + std::to_string(m_ReadLength) +
        Tilc::TExtString("  m_String: ") + m_String;
}

Tilc::Data::TDBMySQL::TDBMySQL(const Tilc::TExtString& DbHost, const Tilc::TExtString& DbName, const Tilc::TExtString& DbUser, const Tilc::TExtString& DbPasswd)
	: TDB()
{
    /* Open database */
    Conn = mysql_init(NULL);
    if (!mysql_real_connect(Conn, DbHost.c_str(), DbUser.c_str(), DbPasswd.c_str(), DbName.c_str(), 0, NULL, 0))
    {
        LastError = mysql_error(Conn);
        return;
    }

    IsOpenDB = true;
}

Tilc::Data::TDBMySQL::~TDBMySQL()
{
    if (IsOpenDB)
    {
        mysql_close(Conn);
        IsOpenDB = false;
    }
}

int Tilc::Data::TDBMySQL::CreateTable(const char* CreateQuery)
{
    int Result = mysql_query(Conn, CreateQuery);
    LastError = mysql_error(Conn);
    return Result;
}

int Tilc::Data::TDBMySQL::Select(const char* Sql, TDBDataRows& DataRows)
{
    MYSQL_RES *res;
    MYSQL_ROW row;

    DataRows.clear();

    mysql_query(Conn, Sql);
    LastError = mysql_error(Conn);
    if (LastError != "") return -1;
    res = mysql_use_result(Conn);
    while ((row = mysql_fetch_row(res)) != NULL)
    {
        TDBDataRow Row;
        for (int i = 0; i < mysql_num_fields(res); ++i)
        {
            Row.push_back(row[i]);
        }
        DataRows.push_back(std::move(Row));
    }
    mysql_free_result(res);
    return static_cast<int>(DataRows.size());
}

int Tilc::Data::TDBMySQL::Select(const char* Sql, const TDBFieldTypes& FieldTypes, const TStringVector& FieldValues, TDBDataRows& DataRows)
{
    MYSQL_STMT* Stmt;
    MYSQL_RES *res;
    MYSQL_FIELD *Fields;
    int Result;
    int ColumnCount;

    /* Execute SQL statement */
    DataRows.clear();

    Stmt = mysql_stmt_init(Conn);
    if (!Stmt)
    {
        LastError = mysql_error(Conn);
        return -1;
    }

    Result = mysql_stmt_prepare(Stmt, Sql, strlen(Sql));
    if (Result != 0)
    {
        LastError = mysql_error(Conn);
        mysql_stmt_close(Stmt);
        return Result;
    }

    res = mysql_stmt_result_metadata(Stmt);
    if (!res)
    {
        LastError = mysql_error(Conn);
        mysql_stmt_close(Stmt);
        return Result;
    }
    ColumnCount = mysql_num_fields(res);
    Fields = mysql_fetch_fields(res);
    if (!Fields || ColumnCount < 1)
    {
        LastError = mysql_error(Conn);
        mysql_stmt_close(Stmt);
        return Result;
    }
    std::vector<Tilc::Data::TDBMySQLField> OutputBindValues;
    for (int i = 0; i < ColumnCount; ++i)
    {
        Tilc::Data::TDBMySQLField OutputBindValue;
        OutputBindValue.m_Type = Fields[i].type;
        OutputBindValue.m_Length = Fields[i].length;
        OutputBindValue.m_String.reserve(Fields[i].length);
        memcpy(OutputBindValue.m_String.data(), 0, OutputBindValue.m_String.size());
        OutputBindValues.push_back(std::move(OutputBindValue));
    }

    // We prepare input bindings. For that are required vector of MYSQL_BINDS which stores configuration
    // of bindings and vector of TDBMySQLField which stores actual values for configured above bindings.
    std::vector<MYSQL_BIND> InputBinds;
    std::vector<Tilc::Data::TDBMySQLField> InputBindValues;
    PrepareInputBindings(Stmt, FieldTypes, FieldValues, InputBinds, InputBindValues);
    Result = mysql_stmt_bind_param(Stmt, InputBinds.data());
    if (Result != 0)
    {
        LastError = mysql_error(Conn);
        mysql_stmt_close(Stmt);
        return Result;
    }

    Result = mysql_stmt_execute(Stmt);
    if (Result != 0)
    {
        LastError = mysql_error(Conn);
        mysql_stmt_close(Stmt);
        return Result;
    }

    // We prepare output bindings
    std::vector<MYSQL_BIND> OutputBinds;
    OutputBinds.reserve(OutputBindValues.size());
    for (int i = 0; i < OutputBindValues.size(); ++i)
    {
        MYSQL_BIND Bind{};
        Bind.buffer_type = MYSQL_TYPE_STRING;
        Bind.buffer = OutputBindValues[i].m_String.data();
        Bind.buffer_length = OutputBindValues[i].m_Length;
        Bind.length = &OutputBindValues[i].m_ReadLength;
        OutputBinds.push_back(Bind);
    }
    mysql_stmt_bind_result(Stmt, OutputBinds.data());

    while (mysql_stmt_fetch(Stmt) == 0)
    {
        TDBDataRow Row;
        for (int i = 0; i < OutputBindValues.size(); ++i)
        {
            unsigned long Len = OutputBindValues[i].m_ReadLength;
            OutputBindValues[i].m_String[Len] = 0;
            Row.push_back(OutputBindValues[i].m_String);
        }
        DataRows.push_back(std::move(Row));
    }
    mysql_free_result(res);
    mysql_stmt_close(Stmt);

    return static_cast<int>(DataRows.size());
}

int Tilc::Data::TDBMySQL::ExecQuery(const char* Sql, const TDBFieldTypes& FieldTypes, const TStringVector& FieldValues)
{
    MYSQL_STMT* Stmt;
    int Result;

    /* Execute SQL statement */
    Stmt = mysql_stmt_init(Conn);
    if (!Stmt)
    {
        LastError = mysql_error(Conn);
        return -1;
    }

    Result = mysql_stmt_prepare(Stmt, Sql, strlen(Sql));
    if (Result != 0)
    {
        LastError = mysql_error(Conn);
        mysql_stmt_close(Stmt);
        return -2;
    }

    // We prepare input bindings. For that are required vector of MYSQL_BINDS which stores configuration
    // of bindings and vector of TDBMySQLField which stores actual values for configured above bindings.
    std::vector<MYSQL_BIND> InputBinds;
    std::vector<Tilc::Data::TDBMySQLField> InputBindValues;
    PrepareInputBindings(Stmt, FieldTypes, FieldValues, InputBinds, InputBindValues);
    Result = mysql_stmt_bind_param(Stmt, InputBinds.data());
    if (Result != 0)
    {
        LastError = mysql_error(Conn);
        mysql_stmt_close(Stmt);
        return -3;
    }

    Result = mysql_stmt_execute(Stmt);
    if (Result != 0)
    {
        LastError = mysql_error(Conn);
        mysql_stmt_close(Stmt);
        return -4;
    }

    return 0;
}

int Tilc::Data::TDBMySQL::Insert(const char* InsertSql, const TDBFieldTypes& FieldTypes, const TStringVector& FieldValues)
{
    int Result = 0;
    LastError = mysql_error(Conn);
    if (LastError != "")
    {
        LastError = mysql_error(Conn);
        return -5;
    }
    if ((Result = ExecQuery(InsertSql, FieldTypes, FieldValues)) == 0)
    {
        Tilc::TExtString TableName = InsertSql;
        size_t pos = 0;
        // get table name from sql of form "INSERT INTO TableName..."
        // Find first space after "INSERT"
        if ((pos = TableName.find(" ", pos)) != Tilc::TExtString::npos)
        {
            // Find first space after "INTO"
            if ((pos = TableName.find(" ", pos + 1)) != Tilc::TExtString::npos)
            {
                // Copy table name
                TableName = TableName.substr(pos+1);
                pos = 0;
                // Find first space after table name
                if ((pos = TableName.find(" ", pos)) != Tilc::TExtString::npos)
                {
                    // And Truncate string to table name
                    TableName = TableName.substr(0, pos);
                }
            }
        }

        // get id of newly inserted row
        Tilc::TExtString Sql = "SELECT id FROM " + TableName + " ORDER BY id DESC LIMIT 1";
        Tilc::Data::TDBDataRows Rows;
        Select(Sql.c_str(), Rows);
        Result = 0;
        if (Rows.size() != 1)
        {
            Result = -6;
        }
        if (Result == 0)
        {
            return std::stoi(Rows[0][0]);
        }
        return Result;
    }
    return Result;
};

void Tilc::Data::TDBMySQL::PrepareInputBindings(MYSQL_STMT* Stmt, const TDBFieldTypes& FieldTypes, const TStringVector& FieldValues,
    std::vector<MYSQL_BIND>& InputBinds, std::vector<Tilc::Data::TDBMySQLField>& InputBindValues)
{
    InputBinds.reserve(FieldValues.size());
    InputBinds.clear();
    InputBindValues.reserve(FieldValues.size());
    InputBindValues.clear();
    for (int i = 0; i < FieldValues.size(); ++i)
    {
        MYSQL_BIND Bind{};
        Tilc::Data::TDBMySQLField v{};

        v.m_Type = EDBFT_STRING;
        v.m_String = FieldValues[i];
        v.m_Length = FieldValues[i].size();
        InputBindValues.push_back(std::move(v));
        Bind.buffer_type = MYSQL_TYPE_STRING;
        Bind.buffer = InputBindValues[InputBindValues.size()-1].m_String.data();
        Bind.buffer_length = FieldValues[i].size();

        InputBinds.push_back(Bind);
	}
}
