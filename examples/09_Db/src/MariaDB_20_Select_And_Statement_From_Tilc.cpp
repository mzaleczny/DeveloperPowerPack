#include <iostream>
#include "Tilc/Utils/ExtString.h"
#include "Tilc/Data/DbMySQL.h"

constexpr const char* HOST_NAME{ "192.168.56.2" };
constexpr const char* DB_NAME{ "testdb" };
constexpr const char* USER_NAME{ "testuser" };
constexpr const char* PASSWD_NAME{ "testpasswd" };

int main(int argc, char* argv[])
{
    Tilc::Data::TDBMySQL DB(HOST_NAME, DB_NAME, USER_NAME, PASSWD_NAME);
    Tilc::Data::TDBDataRows Rows;
    DB.Select("SELECT id, name, age FROM users", Rows);
    for (size_t i = 0; i < Rows.size(); ++i)
    {
        std::cout << Rows[i][0] << " | " << Rows[i][1] << " | " << Rows[i][2] << std::endl;
    }

    return 0;
}
