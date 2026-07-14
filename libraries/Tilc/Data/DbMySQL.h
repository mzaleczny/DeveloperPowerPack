#pragma once
#include "Tilc/DllGlobals.h"
#include "Tilc/Data/Db.h"
#include "conncpp.hpp"

namespace Tilc {
    namespace Data {
        class DECLSPEC TDBMySQL : public TDB
        {
        public:
            using Connection = std::unique_ptr<sql::Connection>;
            using PreparedStatement = std::unique_ptr<sql::PreparedStatement>;
            using ParameterMetaData = std::unique_ptr<sql::ParameterMetaData>;
            using Statement = std::unique_ptr<sql::Statement>;
            using ResultSet = std::unique_ptr<sql::ResultSet>;

            TDBMySQL(const Tilc::TExtString& DbHost, const Tilc::TExtString& DbName, const Tilc::TExtString& DbUser, const Tilc::TExtString& DbPasswd);
            virtual ~TDBMySQL();

            virtual void* GetDb() override { return reinterpret_cast<void*>(&(*m_Conn)); };
            virtual int CreateTable(const char* CreateQuery) override;
            virtual int Select(const char* Sql, TDBDataRows& DataRows) override;
            // Values are passed to MySQL/MariaDB as strings so we can pass empty initializer list {} as
            // FieldTypes. This apply three functions below.
            virtual int Select(const char* Sql, const TDBFieldTypes& FieldTypes, const TStringVector& FieldValues, TDBDataRows& DataRows) override;
            virtual int ExecQuery(const char* Sql, const TDBFieldTypes& FieldTypes, const TStringVector& FieldValues) override;
            virtual int Insert(const char* InsertSql, const TDBFieldTypes& FieldTypes, const TStringVector& FieldValues) override;
            virtual int Delete(const char* DeleteSql, int Id) override
            {
                return ExecQuery(DeleteSql, Tilc::Data::TDBFieldTypes({ Tilc::Data::EDBFieldType::EDBFT_INT }),
                    Tilc::TStringVector({ std::to_string(Id) }));
            }
            virtual int Update(const char* UpdateSql, const TDBFieldTypes& FieldTypes, const TStringVector& FieldValues) override {
                return ExecQuery(UpdateSql, FieldTypes, FieldValues);
            }

        protected:
            Connection m_Conn{ nullptr };
            sql::Connection* GetConnection(sql::Driver* driver,
                const char* Host,
                const char* DbName,
                const char* User,
                const char* Passwd,
                sql::ConnectOptionsMap* additional_options = nullptr);
            void ShowWarningsForStatement(Statement& stmt);
            int ExecuteQuery(ResultSet& res, Statement& stmt, const char* query);
        };

    }
}
