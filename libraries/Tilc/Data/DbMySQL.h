#pragma once
#include "Tilc/DllGlobals.h"
#include "Tilc/Data/Db.h"
#include "mysql.h"

namespace Tilc {
    namespace Data {
        class DECLSPEC TDBMySQL : public TDB
        {
        public:
            TDBMySQL(const Tilc::TExtString& DbHost, const Tilc::TExtString& DbName, const Tilc::TExtString& DbUser, const Tilc::TExtString& DbPasswd, const Tilc::TExtString& Socket = "");
            virtual ~TDBMySQL();

            virtual void* GetDb() override { return reinterpret_cast<void*>(&(*m_Conn)); };
            virtual int CreateTable(const char* CreateQuery) override;
            virtual int Select(const char* Sql, TDBDataRows& DataRows) override;
            // Values are passed to MySQL/MariaDB as strings so we can pass empty initializer list {} as
            // FieldTypes. This apply three functions below.
            virtual int Select(const char* Sql, const TDBFieldTypes& FieldTypes, const TStringVector& FieldValues, TDBDataRows& DataRows) override;
            virtual int ExecQuery(const char* Sql, const TDBFieldTypes& FieldTypes, const TStringVector& FieldValues) override;
            virtual int Insert(const char* InsertSql, const TDBFieldTypes& FieldTypes, const TStringVector& FieldValues) override
            {
                return ExecQuery(InsertSql, FieldTypes, FieldValues);
            }
            virtual int Delete(const char* DeleteSql, int Id) override
            {
                return ExecQuery(DeleteSql, Tilc::Data::TDBFieldTypes({ Tilc::Data::EDBFieldType::EDBFT_INT }),
                    Tilc::TStringVector({ std::to_string(Id) }));
            }
            virtual int Update(const char* UpdateSql, const TDBFieldTypes& FieldTypes, const TStringVector& FieldValues) override {
                return ExecQuery(UpdateSql, FieldTypes, FieldValues);
            }
            void PrintError();
            void PrintStmtError(MYSQL_STMT* stmt);

            static void LoadSharedMariaDbLibrary();
            static void CloseSharedMariaDbLibrary();

        protected:
            static void* m_MariaDbHandle;
            MYSQL* m_Conn{ nullptr };

            int Select(MYSQL_STMT* stmt, TDBDataRows& DataRows);
        };
    }
}
