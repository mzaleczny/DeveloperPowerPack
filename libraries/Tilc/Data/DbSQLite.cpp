#include "Tilc/Data/DbSQLite.h"
#include "Tilc/Utils/ExtString.h"
#include "Tilc/Utils/SystemUtils.h"
#include "SDL3/SDL.h"

Tilc::Data::TDBSQLite::TDBSQLite(const char* Filename)
	: TDB()
{
    /* Open database */
    RC = sqlite3_open(Filename, &DB);
    if (RC)
    {
        DB = nullptr;
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Błąd otwarcia bazy danych: (%s)", Filename);
        return;
    }

    IsOpenDB = true;
}

Tilc::Data::TDBSQLite::~TDBSQLite()
{
    if (IsOpenDB && DB)
    {
        sqlite3_close(DB);
        IsOpenDB = false;
    }
}

int Tilc::Data::TDBSQLite::CreateTable(const char* CreateQuery)
{
    int Result = 0;
    sqlite3_stmt* CreateStmt;
    if (!DB) return -1;
    sqlite3_prepare_v2(DB, CreateQuery, static_cast<int>(strlen(CreateQuery)), &CreateStmt, NULL);
    Result = sqlite3_step(CreateStmt);
    sqlite3_finalize(CreateStmt);
    return Result;
}

int Tilc::Data::TDBSQLite::Select(const char* Sql, TDBDataRows& DataRows)
{
    /* Execute SQL statement */
    TStringVector FieldNames;
    if (!DB) return 0;
    DataRows.clear();
    this->FieldNames = &FieldNames;
    this->DataRows = &DataRows;
    RC = sqlite3_exec(DB, Sql, DBCallback, reinterpret_cast<void*>(this), &ErrMsg);
    if (RC != SQLITE_OK)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SQL error: (%s)", ErrMsg);
        sqlite3_free(ErrMsg);
    }
    this->FieldNames = nullptr;
    this->DataRows = nullptr;

    return static_cast<int>(DataRows.size());
}

int Tilc::Data::TDBSQLite::Select(const char* Sql, const TDBFieldTypes& FieldTypes, const TStringVector& FieldValues, TDBDataRows& DataRows)
{
    sqlite3_stmt* Stmt;

    /* Execute SQL statement */
    TStringVector FieldNames;
    if (!DB) return 0;
    DataRows.clear();
    this->FieldNames = &FieldNames;
    this->DataRows = &DataRows;
    RC = sqlite3_prepare_v2(DB, Sql, -1, &Stmt, NULL);
    if (RC != SQLITE_OK)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SQL error: (%s)", ErrMsg);
        sqlite3_free(ErrMsg);
        return -1;
    }
    for (int i = 0; i < FieldValues.size(); ++i)
    {
        if (FieldTypes[i] == EDBFieldType::EDBFT_TEXT || FieldTypes[i] == EDBFieldType::EDBFT_MEDIUMTEXT || FieldTypes[i] == EDBFieldType::EDBFT_LONGTEXT)
        {
			sqlite3_bind_text(Stmt, (i + 1), FieldValues[i].c_str(), -1, SQLITE_STATIC);
		}
        else if (FieldTypes[i] == EDBFieldType::EDBFT_TINYINT || FieldTypes[i] == EDBFieldType::EDBFT_INT)
        {
			sqlite3_bind_int(Stmt, (i + 1), std::stoi(FieldValues[i]));
		}
        else if (FieldTypes[i] == EDBFieldType::EDBFT_BLOB)
        {
			sqlite3_bind_blob(Stmt, (i + 1), FieldValues[i].c_str(), static_cast<int>(FieldValues[i].size()), SQLITE_STATIC);
		}
	}
    TDBDataRow Row;
    while (sqlite3_step(Stmt) == SQLITE_ROW)
    {
        Row.clear();
        for (int i = 0; i < sqlite3_column_count(Stmt); i++)
        {
            if (FieldNames.size() < sqlite3_column_count(Stmt))
            {
				FieldNames.push_back(sqlite3_column_name(Stmt, i));
            }
            int ColumnType = sqlite3_column_type(Stmt, i);
            switch (ColumnType)
            {
                case SQLITE_INTEGER:
					Row.push_back(std::to_string(sqlite3_column_int(Stmt, i)));
					break;
				case SQLITE_FLOAT:
                	Row.push_back(std::to_string(sqlite3_column_double(Stmt, i)));
                    break;
                case SQLITE_TEXT:
                    Row.push_back(Tilc::TExtString(reinterpret_cast<const char*>(sqlite3_column_text(Stmt, i))));
					break;
                case SQLITE_BLOB:
                    Row.push_back(Tilc::TExtString(reinterpret_cast<const char*>(sqlite3_column_blob(Stmt, i)), sqlite3_column_bytes(Stmt, i)));
					break;
				case SQLITE_NULL:
					Row.push_back("NULL");
                    break;
            }
		}
        DataRows.push_back(Row);
    }
    sqlite3_finalize(Stmt);

    this->FieldNames = nullptr;
    this->DataRows = nullptr;

    return static_cast<int>(DataRows.size());
}

int Tilc::Data::TDBSQLite::DBCallback(void* DbInstance, int Argc, char** Argv, char** ColName)
{
    int i;
    TDBSQLite* DB = reinterpret_cast<TDBSQLite*>(DbInstance);
    if (!DB) return 0;

    std::vector<Tilc::TExtString> Row;
    for (i = 0; i < Argc; i++)
    {
        if (DB->FieldNames->size() < Argc)
        {
            DB->FieldNames->push_back(ColName[i]);
        }
        Row.push_back(Argv[i] ? Argv[i] : "NULL");
    }
    DB->DataRows->push_back(Row);

    return 0;
}

int Tilc::Data::TDBSQLite::ExecQuery(const char* Sql, const TDBFieldTypes& FieldTypes, const TStringVector& FieldValues)
{
    sqlite3_stmt* Stmt;
    Tilc::TExtString TypeStr, ValueStr;
    int Result;

    if (!DB) return -2;

    RC = sqlite3_prepare_v2(DB, Sql, -1, &Stmt, 0);
    if (RC != SQLITE_OK)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SQL error: (%s)", sqlite3_errmsg(DB));
        return -1;
    }
    for (int i = 0; i < FieldValues.size(); i++)
    {
        if (FieldTypes[i] == EDBFieldType::EDBFT_MEDIUMTEXT || FieldTypes[i] == EDBFieldType::EDBFT_TEXT || FieldTypes[i] == EDBFieldType::EDBFT_LONGTEXT || FieldTypes[i] == EDBFieldType::EDBFT_VARCHAR)
        {
            sqlite3_bind_text(Stmt, (i + 1), FieldValues[i].c_str(), -1, SQLITE_STATIC);
        }
        else if (FieldTypes[i] == EDBFieldType::EDBFT_TINYINT || FieldTypes[i] == EDBFieldType::EDBFT_INT)
        {
            sqlite3_bind_int(Stmt, (i + 1), std::stoi(FieldValues[i]));
        }
        else if (FieldTypes[i] == EDBFieldType::EDBFT_BLOB)
        {
            sqlite3_bind_blob(Stmt, (i + 1), FieldValues[i].c_str(), static_cast<int>(FieldValues[i].size()), SQLITE_STATIC);
        }
    }

    Result = sqlite3_step(Stmt);
    sqlite3_finalize(Stmt);
    return Result;
}

int Tilc::Data::TDBSQLite::Insert(const char* InsertSql, const TDBFieldTypes& FieldTypes, const TStringVector& FieldValues)
{
    int Result = -1;
    if (!DB) return Result;
    if ((Result = ExecQuery(InsertSql, FieldTypes, FieldValues)) >= 0)
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
        TDBDataRows Rows;
        Select(Sql.c_str(), Rows);
        if (Rows.size() != 1)
        {
            return -2;
        }
        return std::stoi(Rows[0][0]);
    }
    return Result;
};

int Tilc::Data::TDBSQLite::Delete(const char* DeleteSql, int Id)
{
    sqlite3_stmt* stmt;
    if (!DB) return -3;
    RC = sqlite3_prepare_v2(DB, DeleteSql, -1, &stmt, 0);
    if (RC != SQLITE_OK)
    {
        return -1;
    }
    sqlite3_bind_int(stmt, 1, Id);
    RC = sqlite3_step(stmt);

    sqlite3_finalize(stmt);

    if (RC != SQLITE_DONE)
    {
        return -2;
    }

    return 0;
}
