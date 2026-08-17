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
        DB.ExecQuery("DROP TABLE IF EXISTS products", {}, {});
        DB.CreateTable("CREATE TABLE products(id INT AUTO_INCREMENT PRIMARY KEY, name VARCHAR(128), price DECIMAL(10,2))");
        DB.ExecQuery("INSERT INTO products(name, price) VALUES ('Product1', 111.11)", {}, {});
        DB.ExecQuery("INSERT INTO products(name, price) VALUES ('Product2', 222.22)", {}, {});
        DB.ExecQuery("INSERT INTO products(name, price) VALUES ('Product3', 333.33)", {}, {});
        DB.ExecQuery("INSERT INTO products(name, price) VALUES ('Product4', 444.44)", {}, {});
        DB.ExecQuery("INSERT INTO products(name, price) VALUES ('Product5', 555.55)", {}, {});
        DB.ExecQuery("INSERT INTO products(name, price) VALUES ('Product6', 666.66)", {}, {});
        DB.ExecQuery("INSERT INTO products(name, price) VALUES ('Product7', 777.77)", {}, {});
        DB.ExecQuery("INSERT INTO products(name, price) VALUES ('Product8', 888.88)", {}, {});
        DB.ExecQuery("INSERT INTO products(name, price) VALUES ('Product9', 999.99)", {}, {});

        DB.Select("SELECT * FROM products", Rows);
        for (size_t i = 0; i < Rows.size(); ++i)
        {
            std::cout << Rows[i][0] << " | " << Rows[i][1] << " | " << Rows[i][2] << std::endl;
        }
    }

    Tilc::CleanupTilc();
    return 0;
}
