#include <iostream>
#include "Tilc/Tilc.h"
#include "Tilc/Utils/ExtString.h"
#include "Tilc/Data/DbMySQL.h"

constexpr const char* HOST_NAME{ "192.168.56.102" };
constexpr const char* DB_NAME{ "testdb" };
constexpr const char* USER_NAME{ "testuser" };
constexpr const char* PASSWD_NAME{ "testpasswd" };

int main(int argc, char* argv[])
{
    Tilc::InitTilc();

    // IMPORTANT!!! Variable Tilc::Data::TDBMySQL DB must be in inner scope, to do proper close db. Because if it would be in main scope, function Tilc::Cleanup
    // would be called before DB destructor. And in destructor would be call to unloaded DLL.
    {
        Tilc::Data::TDBMySQL DB(HOST_NAME, DB_NAME, USER_NAME, PASSWD_NAME);
        Tilc::Data::TDBDataRows Rows;
        DB.ExecQuery("DROP TABLE IF EXISTS users", {}, {});
        DB.ExecQuery("CREATE TABLE users (id INT NOT NULL AUTO_INCREMENT PRIMARY KEY, name VARCHAR(255), age INTEGER, dat DATE, tim TIME, dattim DATETIME, fract FLOAT, dbl DOUBLE, decim DECIMAL(10,2))", {}, {});
        DB.Insert("INSERT INTO users (name, age, dat, tim, dattim, fract, dbl, decim) VALUES (?,?,?,?,?,?,?,?)",
            { Tilc::Data::EDBFT_VARCHAR, Tilc::Data::EDBFT_INT, Tilc::Data::EDBFT_DATE, Tilc::Data::EDBFT_TIME, Tilc::Data::EDBFT_DATETIME, Tilc::Data::EDBFT_FLOAT, Tilc::Data::EDBFT_DOUBLE, Tilc::Data::EDBFT_DECIMAL},
            { "TestowyUser #1", "11", "2026-01-01", "12:11:11", "2026-01-01 12:11:11", "11.11", "11.11111", "11111.11"}
        );
        DB.Insert("INSERT INTO users (name, age, dat, tim, dattim, fract, dbl, decim) VALUES (?,?,?,?,?,?,?,?)",
            { Tilc::Data::EDBFT_VARCHAR, Tilc::Data::EDBFT_INT, Tilc::Data::EDBFT_DATE, Tilc::Data::EDBFT_TIME, Tilc::Data::EDBFT_DATETIME, Tilc::Data::EDBFT_FLOAT, Tilc::Data::EDBFT_DOUBLE, Tilc::Data::EDBFT_DECIMAL},
            { "TestowyUser #2", "22", "2026-02-02", "12:22:22", "2026-01-02 12:22:22", "22.22", "22.22222", "22222.22"}
        );
        DB.Insert("INSERT INTO users (name, age, dat, tim, dattim, fract, dbl, decim) VALUES (?,?,?,?,?,?,?,?)",
            { Tilc::Data::EDBFT_VARCHAR, Tilc::Data::EDBFT_INT, Tilc::Data::EDBFT_DATE, Tilc::Data::EDBFT_TIME, Tilc::Data::EDBFT_DATETIME, Tilc::Data::EDBFT_FLOAT, Tilc::Data::EDBFT_DOUBLE, Tilc::Data::EDBFT_DECIMAL},
            { "TestowyUser #3", "33", "2026-01-03", "12:33:33", "2026-01-03 12:33:33", "33.33", "33.33333", "33333.33"}
        );
        DB.Select("SELECT * FROM users", Rows);
        for (size_t i = 0; i < Rows.size(); ++i)
        {
            for (size_t j = 0; j < Rows[i].size(); ++j)
            {
                std::cout << Rows[i][j] << " | ";
            }
            std::cout << std::endl;
        }
    }

    Tilc::CleanupTilc();
    return 0;
}
