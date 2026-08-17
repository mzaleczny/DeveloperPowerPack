#include "Tilc/Data/DbMySQL.h"
#include "Tilc/Data/TilcMariaDbLoader.h"
#include "Tilc/Utils/ExtString.h"
#include "Tilc/Utils/Log.h"
#include <iostream>
#include <sstream>
#ifdef __linux__
    #include <dlfcn.h>
#elifdef _WIN32
    #include <windows.h>
#endif

void* Tilc::Data::TDBMySQL::m_MariaDbHandle{};
void Tilc::Data::TDBMySQL::LoadSharedMariaDbLibrary()
{
    if (!m_MariaDbHandle)
    {
#ifdef __linux__
        m_MariaDbHandle = TilcLoadMariaDb("/usr/local/lib/libmariadb.so.3");
        if (!m_MariaDbHandle)
        {
            std::cerr << "ERROR: Could not load libmariadb.so.3" << std::endl;
        }
#elifdef _WIN32
        m_MariaDbHandle = TilcLoadMariaDb("libmariadb.dll");
        if (!m_MariaDbHandle)
        {
            std::cerr << "ERROR: Could not load libmariadb.dll" << std::endl;
        }
#endif
    }
}
void Tilc::Data::TDBMySQL::CloseSharedMariaDbLibrary()
{
    if (m_MariaDbHandle)
    {
#ifdef __linux__
        dlclose(m_MariaDbHandle);
#elifdef _WIN32
        FreeLibrary((HMODULE)m_MariaDbHandle);
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
            std::cerr << tilc_mysql_error(m_Conn) << std::endl;
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

int Tilc::Data::TDBMySQL::Select(MYSQL_STMT* stmt, TDBDataRows& DataRows)
{
    DataRows.clear();

    unsigned int field_count = tilc_mysql_stmt_field_count(stmt);
    if (field_count < 1) return 0;
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
    tilc_mysql_stmt_store_result(stmt);
    tilc_mysql_stmt_bind_result(stmt, OutputBind.data());

    while (true)
    {
        int status = tilc_mysql_stmt_fetch(stmt);
        if (status == 0 || status == MYSQL_DATA_TRUNCATED)
        {
            Tilc::Data::TDBDataRow Row;

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
                status = tilc_mysql_stmt_fetch(stmt);
            }

            for (unsigned int i = 0; i < field_count; ++i)
            {
                if (is_null[i])
                {
                    Row.push_back("");
                }
                else
                {
                    // UWAGA: używaj lengths[i], a nie gołego buffer!
                    std::string val((char*)OutputBind[i].buffer, lengths[i]);
                    switch (OutputBind[i].buffer_type)
                    {
                    case MYSQL_TYPE_DATE:
                    {
                        const MYSQL_TIME* t = (MYSQL_TIME*)val.data();
                        char buf[11];
                        snprintf(buf, sizeof(buf), "%04u-%02u-%02u", t->year, t->month, t->day);
                        Row.push_back(buf);
                        break;
                    }
                    case MYSQL_TYPE_TIME:
                    {
                        const MYSQL_TIME* t = (MYSQL_TIME*)val.data();
                        char buf[9];
                        snprintf(buf, sizeof(buf), "%02u:%02u:%02u", t->hour, t->minute, t->second);
                        Row.push_back(buf);
                        break;
                    }
                    case MYSQL_TYPE_DATETIME:
                    {
                        const MYSQL_TIME* t = (MYSQL_TIME*)val.data();
                        char buf[22];
                        snprintf(buf, sizeof(buf), "%04u-%02u-%02u %02u:%02u:%02u", t->year, t->month, t->day, t->hour, t->minute, t->second);
                        Row.push_back(buf);
                        break;
                    }
                    case MYSQL_TYPE_BLOB:
                    case MYSQL_TYPE_TINY_BLOB:
                    case MYSQL_TYPE_MEDIUM_BLOB:
                    case MYSQL_TYPE_NEWDECIMAL:
                    case MYSQL_TYPE_STRING:
                    case MYSQL_TYPE_VAR_STRING:
                        Row.push_back(std::move(val));
                        break;
                    case MYSQL_TYPE_TINY:
                    case MYSQL_TYPE_LONG:
                    case MYSQL_TYPE_LONGLONG:
                        Row.push_back(std::to_string(*(int*)val.data()));
                        break;
                    case MYSQL_TYPE_FLOAT:
                        Row.push_back(std::to_string(*(float*)val.data()));
                        break;
                    case MYSQL_TYPE_DOUBLE:
                        Row.push_back(std::to_string(*(double*)val.data()));
                        break;
                    default:
                        break;
                    }
                }
            }

            DataRows.push_back(std::move(Row));
        }
        else if (status == MYSQL_NO_DATA)
        {
            break; // koniec wyników
        }
        else
        {
            // błąd
            break;
        }
    }

    for (unsigned int i = 0; i < field_count; ++i)
    {
        free(OutputBind[i].buffer);
    }

    tilc_mysql_stmt_close(stmt);
    return static_cast<int>(DataRows.size());
}

int Tilc::Data::TDBMySQL::Select(const char* Sql, TDBDataRows& DataRows)
{
    MYSQL_STMT* stmt = tilc_mysql_stmt_init(m_Conn);
    tilc_mysql_stmt_prepare(stmt, Sql, strlen(Sql));
    return Select(stmt, DataRows);
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
    return Select(stmt, DataRows);
}

int Tilc::Data::TDBMySQL::ExecQuery(const char* Sql, const TDBFieldTypes& FieldTypes, const TStringVector& FieldValues)
{
    if (FieldValues.size() > 0)
    {
        TStringVector Input = FieldValues;
        std::vector<int> IntInputs;
        std::vector<float> FloatInputs;
        std::vector<double> DoubleInputs;
        MYSQL_STMT* stmt = tilc_mysql_stmt_init(m_Conn);
        tilc_mysql_stmt_prepare(stmt, Sql, strlen(Sql));
        std::vector<MYSQL_BIND> InputBind(FieldValues.size());
        if (InputBind.size() > 0)
        {
            memset(&InputBind[0], 0, sizeof(MYSQL_BIND) * InputBind.size());
            for (size_t i = 0; i < Input.size(); ++i)
            {
                switch (FieldTypes[i])
                {
                case Tilc::Data::EDBFT_DECIMAL:
                case Tilc::Data::EDBFT_DATE:
                case Tilc::Data::EDBFT_DATETIME:
                case Tilc::Data::EDBFT_TIME:
                case Tilc::Data::EDBFT_VARCHAR:
                case Tilc::Data::EDBFT_STRING:
                case Tilc::Data::EDBFT_TEXT:
                case Tilc::Data::EDBFT_MEDIUMTEXT:
                case Tilc::Data::EDBFT_LONGTEXT:
                case Tilc::Data::EDBFT_TINY_BLOB:
                case Tilc::Data::EDBFT_MEDIUM_BLOB:
                case Tilc::Data::EDBFT_LONG_BLOB:
                case Tilc::Data::EDBFT_BLOB:
                    InputBind[i].buffer_type = MYSQL_TYPE_STRING;
                    InputBind[i].buffer = Input[i].data();
                    InputBind[i].buffer_length = Input[i].length();
                    break;
                case Tilc::Data::EDBFT_TINYINT:
                case Tilc::Data::EDBFT_INT:
                    InputBind[i].buffer_type = MYSQL_TYPE_LONG;
                    IntInputs.push_back(std::atoi(Input[i].data()));
                    InputBind[i].buffer = &IntInputs[IntInputs.size() - 1];
                    break;
                case Tilc::Data::EDBFT_FLOAT:
                    InputBind[i].buffer_type = MYSQL_TYPE_FLOAT;
                    FloatInputs.push_back(std::atof(Input[i].data()));
                    InputBind[i].buffer = &FloatInputs[FloatInputs.size() - 1];
                    break;
                case Tilc::Data::EDBFT_DOUBLE:
                    InputBind[i].buffer_type = MYSQL_TYPE_DOUBLE;
                    DoubleInputs.push_back(std::atof(Input[i].data()));
                    InputBind[i].buffer = &DoubleInputs[DoubleInputs.size() - 1];
                    break;
                default:
                    break;
                }
            }
            tilc_mysql_stmt_bind_param(stmt, &InputBind[0]);
        }
        tilc_mysql_stmt_execute(stmt);
        if (mysql_stmt_field_count(stmt) > 0)
        {
            tilc_mysql_stmt_store_result(stmt);
        }
        tilc_mysql_stmt_close(stmt);
    }
    else
    {
        tilc_mysql_query(m_Conn, Sql);
        PrintError();
    }
    return 0;
}

void Tilc::Data::TDBMySQL::PrintError()
{
    const char* error = tilc_mysql_error(m_Conn);
    if (std::strlen(error) > 0)
    {
        std::cerr << error << std::endl;
    }
}

void Tilc::Data::TDBMySQL::PrintStmtError(MYSQL_STMT* stmt)
{
    const char* error = tilc_mysql_stmt_error(stmt);
    if (std::strlen(error) > 0)
    {
        std::cerr << error << std::endl;
    }
}
