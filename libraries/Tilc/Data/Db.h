#pragma once
#include "Tilc/DllGlobals.h"
#include "Tilc/Globals.h"

namespace Tilc::Data
{
	enum EDBFieldType
	{
		EDBFT_TYPE_DATE,
		EDBFT_TYPE_DATETIME,
		EDBFT_TINYINT,
		EDBFT_INT,
		EDBFT_FLOAT,
		EDBFT_DOUBLE,
		EDBFT_DECIMAL,
		EDBFT_MEDIUMTEXT,
		EDBFT_TEXT,
		EDBFT_STRING,
		EDBFT_LONGTEXT,
		EDBFT_CHAR,
		EDBFT_VARCHAR,
		EDBFT_TINY_BLOB,
		EDBFT_BLOB,
        EDBFT_MEDIUM_BLOB,
        EDBFT_LONG_BLOB,
        EDBFT_VAR_STRING
	};

	using TDBFieldTypes = std::vector<EDBFieldType>;
	using TDBDataRow = TStringVector;
	using TDBDataRows = std::vector<TDBDataRow>;

	class DECLSPEC TDB
	{
	public:
		TDB() {}
		virtual ~TDB() {}

		virtual void* GetDb() { return nullptr; }
		inline bool IsOpen() const { return IsOpenDB; }
		virtual int CreateTable(const char* CreateQuery) { return 0; }
		virtual int Select(const char* Sql, TDBDataRows& DataRows) { return 0; }
		virtual int Select(const char* Sql, const TDBFieldTypes& FieldTypes, const TStringVector& FieldValues, TDBDataRows& DataRows) { return 0; }
		virtual int ExecQuery(const char* Sql, const TDBFieldTypes& FieldTypes, const TStringVector& FieldValues) { return 0; }
		virtual int Insert(const char* InsertSql, const TDBFieldTypes& FieldTypes, const TStringVector& FieldValues) { return 0; }
		virtual int Update(const char* UpdateSql, const TDBFieldTypes& FieldTypes, const TStringVector& FieldValues) { return 0; }
		virtual int Delete(const char* DeleteSql, int Id) { return 0; }
		virtual Tilc::TExtString GetErrorMessage() { return LastError; }
	protected:
        char Buffer[2048]{};
		bool IsOpenDB = false;
		Tilc::TExtString LastError;

		// Ponizsze dwie zmienne wskazuja na wartosci podane w funkcji Select
		TStringVector* FieldNames;
		std::vector<TStringVector>* DataRows;
	};
}
