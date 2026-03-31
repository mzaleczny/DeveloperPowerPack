#pragma once
#include "Db.h"
#include "sqlite3.h"
#include "Tilc/DllGlobals.h"

namespace Tilc::Data
{

	class DECLSPEC TDBSQLite : public TDB
	{
	public:
		TDBSQLite(const char* Filename);
		virtual ~TDBSQLite();

		virtual void* GetDb() override { return reinterpret_cast<void*>(DB); };
		virtual int CreateTable(const char* CreateQuery) override;
		virtual int Select(const char* Sql, TDBDataRows& DataRows) override;
		virtual int Select(const char* Sql, const TDBFieldTypes& FieldTypes, const TStringVector& FieldValues, TDBDataRows& DataRows) override;
		virtual int ExecQuery(const char* Sql, const TDBFieldTypes& FieldTypes, const TStringVector& FieldValues) override;
		virtual int Insert(const char* InsertSql, const TDBFieldTypes& FieldTypes, const TStringVector& FieldValues) override;
		virtual int Update(const char* UpdateSql, const TDBFieldTypes& FieldTypes, const TStringVector& FieldValues) override
		{
			return ExecQuery(UpdateSql, FieldTypes, FieldValues);
		};
		virtual int Delete(const char* DeleteSql, int Id) override;

	private:
		sqlite3* DB;
		char* ErrMsg = 0;
		int RC;

		static int DBCallback(void* DbInstance, int Argc, char** Argv, char** ColName);
	};

}