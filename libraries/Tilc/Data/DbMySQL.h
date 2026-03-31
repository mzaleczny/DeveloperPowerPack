#pragma once
#include "Db.h"
#include "mariadb/mysql.h"
#include "Tilc/DllGlobals.h"

namespace Tilc::Data
{

	struct DECLSPEC TDBMySQLField
	{
		int m_Type;
		long m_Integer;
		float m_Float;
		float m_Double;
		unsigned long m_Length;
		unsigned long m_ReadLength;
		std::string m_String;

		TDBMySQLField() = default;
		Tilc::TExtString toString();
	};

	class DECLSPEC TDBMySQL : public TDB
	{
	public:
		TDBMySQL(const Tilc::TExtString& DbHost, const Tilc::TExtString& DbName, const Tilc::TExtString& DbUser, const Tilc::TExtString& DbPasswd);
		virtual ~TDBMySQL();

		virtual void* GetDb() override { return reinterpret_cast<void*>(Conn); };
		virtual int CreateTable(const char* CreateQuery);
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
		MYSQL *Conn;
		void PrepareInputBindings(MYSQL_STMT* Stmt, const TDBFieldTypes& FieldTypes, const TStringVector& FieldValues,
		    std::vector<MYSQL_BIND>& InputBinds, std::vector<Tilc::Data::TDBMySQLField>& InputBindValues);
	};

}