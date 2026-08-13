#include "Tilc/Data/DbMySQL.h"
#include "Tilc/Data/TilcMariaDbLoader.h"
#include "Tilc/Utils/ExtString.h"
#include "Tilc/Utils/Log.h"
#include <iostream>
#include <sstream>
#ifdef __linux__
    #include <dlfcn.h>
#endif

void* Tilc::Data::TDBMySQL::m_MariaDbHandle{};
void Tilc::Data::TDBMySQL::LoadSharedMariaDbLibrary()
{
    if (!m_MariaDbHandle)
    {
#ifdef __linux__
        m_MariaDbHandle = TilcLoadMariaDb("/usr/local/lib/libmariadb.so.3");
#endif
    }
    if (!m_MariaDbHandle)
    {
        std::cerr << "ERROR: Could not load libmariadb.so.3" << std::endl;
    }
}
void Tilc::Data::TDBMySQL::CloseSharedMariaDbLibrary()
{
    if (m_MariaDbHandle)
    {
#ifdef __linux__
        dlclose(m_MariaDbHandle);
#endif
        m_MariaDbHandle = nullptr;
    }
}

Tilc::Data::TDBMySQL::TDBMySQL(const Tilc::TExtString& DbHost, const Tilc::TExtString& DbName, const Tilc::TExtString& DbUser, const Tilc::TExtString& DbPasswd)
	: Tilc::Data::TDB()
{

    if (m_MariaDbHandle)
    {
        // Open database
        m_Conn = tilc_mysql_init(NULL);
        if (m_Conn)
        {
            tilc_mysql_real_connect(m_Conn, DbHost.c_str(), DbUser.c_str(), DbPasswd.c_str(), DbName.c_str(), 0, NULL, 0);
            IsOpenDB = true;
        }
    }
}

Tilc::Data::TDBMySQL::~TDBMySQL()
{
    if (IsOpenDB)
    {
        tilc_mysql_close(m_Conn);
        IsOpenDB = false;
    }
}

int Tilc::Data::TDBMySQL::CreateTable(const char* CreateQuery)
{
    int Result = tilc_mysql_query(m_Conn, CreateQuery);
    return Result;
}

int Tilc::Data::TDBMySQL::Select(const char* Sql, TDBDataRows& DataRows)
{
    MYSQL_STMT* stmt = tilc_mysql_stmt_init(m_Conn);
    tilc_mysql_stmt_prepare(stmt, Sql, strlen(Sql));
    tilc_mysql_stmt_execute(stmt);

    unsigned int field_count = tilc_mysql_stmt_field_count(stmt);
    std::vector<MYSQL_BIND> OutputBind(field_count);
    memset(&OutputBind[0], 0, sizeof(MYSQL_BIND) * OutputBind.size());

    MYSQL_RES* meta = tilc_mysql_stmt_result_metadata(stmt);
    field_count = tilc_mysql_num_fields(meta);
    MYSQL_FIELD* fields = tilc_mysql_fetch_fields(meta);
    std::vector<unsigned long> lengths(field_count);
    std::vector<my_bool> is_null(field_count);
    std::vector<my_bool> error(field_count);

    for (unsigned int i = 0; i < field_count; i++)
    {
        OutputBind[i].buffer_type = fields[i].type;
        OutputBind[i].buffer = malloc(256);          // domyslny bufor
        OutputBind[i].buffer_length = 256;           // 256
        OutputBind[i].length = &lengths[i];
        OutputBind[i].is_null = &is_null[i];
        OutputBind[i].error = &error[i];
    }
    tilc_mysql_stmt_execute(stmt);
    tilc_mysql_stmt_bind_result(stmt, OutputBind.data());

    int status = tilc_mysql_stmt_fetch(stmt);
    if (status == MYSQL_DATA_TRUNCATED)
    {
        // przynajmniej jedno pole jest za duże
        for (unsigned int i = 0; i < field_count; ++i)
        {
            if (error[i])
            {
                // pole zostało obcięte -> zwolnij i zaalokuj wiekszy bufor
                free(OutputBind[i].buffer);
                OutputBind[i].buffer = malloc(lengths[i] + 1);
                OutputBind[i].buffer_length = lengths[i] + 1;
            }
        }
        tilc_mysql_stmt_fetch(stmt);
    }

    DataRows.clear();
    for (unsigned int i = 0; i < field_count; i++)
    {
        Tilc::Data::TDBDataRow Row;
        if (!is_null[i])
        {
            Row.push_back((char*)OutputBind[i].buffer);
        }
        else
        {
            Row.push_back("");
        }
        DataRows.push_back(std::move(Row));
    }

    for (unsigned int i = 0; i < field_count; ++i)
    {
        free(OutputBind[i].buffer);
    }

    tilc_mysql_stmt_close(stmt);
    return static_cast<int>(DataRows.size());
}

int Tilc::Data::TDBMySQL::Select(const char* Sql, const TDBFieldTypes& FieldTypes, const TStringVector& FieldValues, TDBDataRows& DataRows)
{
    TStringVector Input = FieldValues;
    MYSQL_STMT* stmt = tilc_mysql_stmt_init(m_Conn);
    tilc_mysql_stmt_prepare(stmt, Sql, strlen(Sql));
    std::vector<MYSQL_BIND> InputBind(FieldValues.size());
    memset(&InputBind[0], 0, sizeof(MYSQL_BIND) * InputBind.size());
    for (size_t i = 0; i < Input.size(); ++i)
    {
        InputBind[i].buffer_type = MYSQL_TYPE_STRING;
        InputBind[i].buffer = Input[i].data();
        tilc_mysql_stmt_bind_param(stmt, &InputBind[i]);
    }
    tilc_mysql_stmt_execute(stmt);

    unsigned int field_count = tilc_mysql_stmt_field_count(stmt);
    std::vector<MYSQL_BIND> OutputBind(field_count);
    memset(&OutputBind[0], 0, sizeof(OutputBind));
    std::vector<unsigned long> lengths(field_count);
    std::vector<my_bool> is_null(field_count);
    std::vector<my_bool> error(field_count);

    MYSQL_RES* meta = tilc_mysql_stmt_result_metadata(stmt);
    field_count = tilc_mysql_num_fields(meta);
    MYSQL_FIELD* fields = tilc_mysql_fetch_fields(meta);

    for (unsigned int i = 0; i < field_count; i++)
    {
        OutputBind[i].buffer_type = fields[i].type;
        OutputBind[i].buffer = malloc(256);          // domyslny bufor
        OutputBind[i].buffer_length = 256;           // 256
        OutputBind[i].length = &lengths[i];
        OutputBind[i].is_null = &is_null[i];
        OutputBind[i].error = &error[i];
    }
    tilc_mysql_stmt_execute(stmt);
    tilc_mysql_stmt_bind_result(stmt, OutputBind.data());

    int status = tilc_mysql_stmt_fetch(stmt);
    if (status == MYSQL_DATA_TRUNCATED)
    {
        // przynajmniej jedno pole jest za duże
        for (unsigned int i = 0; i < field_count; ++i)
        {
            if (error[i])
            {
                // pole zostało obcięte -> zwolnij i zaalokuj wiekszy bufor
                free(OutputBind[i].buffer);
                OutputBind[i].buffer = malloc(lengths[i] + 1);
                OutputBind[i].buffer_length = lengths[i] + 1;
            }
        }
        tilc_mysql_stmt_fetch(stmt);
    }

    DataRows.clear();
    for (unsigned int i = 0; i < field_count; i++)
    {
        Tilc::Data::TDBDataRow Row;
        if (!is_null[i])
        {
            Row.push_back((char*)OutputBind[i].buffer);
        }
        else
        {
            Row.push_back("");
        }
        DataRows.push_back(std::move(Row));
    }

    for (unsigned int i = 0; i < field_count; ++i)
    {
        free(OutputBind[i].buffer);
    }

    tilc_mysql_stmt_close(stmt);
    return static_cast<int>(DataRows.size());
}

int Tilc::Data::TDBMySQL::ExecQuery(const char* Sql, const TDBFieldTypes& FieldTypes, const TStringVector& FieldValues)
{
    TStringVector Input = FieldValues;
    MYSQL_STMT* stmt = tilc_mysql_stmt_init(m_Conn);
    tilc_mysql_stmt_prepare(stmt, Sql, strlen(Sql));
    std::vector<MYSQL_BIND> InputBind(FieldValues.size());
    memset(&InputBind[0], 0, sizeof(MYSQL_BIND) * InputBind.size());
    for (size_t i = 0; i < Input.size(); ++i)
    {
        InputBind[i].buffer_type = MYSQL_TYPE_STRING;
        InputBind[i].buffer = Input[i].data();
        tilc_mysql_stmt_bind_param(stmt, &InputBind[i]);
    }
    tilc_mysql_stmt_execute(stmt);

    return 0;
}

int Tilc::Data::TDBMySQL::Insert(const char* InsertSql, const TDBFieldTypes& FieldTypes, const TStringVector& FieldValues)
{
    TStringVector Input = FieldValues;
    std::string StringSql{ InsertSql };
    size_t ColumnCount = std::count(StringSql.begin(), StringSql.end(), '?');

    MYSQL_STMT* stmt = tilc_mysql_stmt_init(m_Conn);
    tilc_mysql_stmt_prepare(stmt, InsertSql, strlen(InsertSql));
    std::vector<MYSQL_BIND> InputBind(ColumnCount);
    memset(&InputBind[0], 0, sizeof(InputBind));
    for (size_t i = 1; i <= ColumnCount; ++i)
    {
        InputBind[i].buffer_type = MYSQL_TYPE_STRING;
        InputBind[i].buffer = Input[i - 1].data();
        tilc_mysql_stmt_bind_param(stmt, &InputBind[i]);
    }
    tilc_mysql_stmt_execute(stmt);

    return 0;
};
