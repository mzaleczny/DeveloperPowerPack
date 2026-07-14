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

    DB.Select("DROP TABLE IF EXISTS products", Rows);
    DB.Select("CREATE TABLE products(id INT AUTO_INCREMENT PRIMARY KEY, name VARCHAR(128), price DECIMAL(10,2), description TEXT)", Rows);
    
    for (int i = 1; i <= 9; ++i)
    {
        std::string ProductName = "Prduct" + std::to_string(i);
        std::string ProductDescription = std::format("Krótki opis dla produktu: {}\nLinijka1\nLinijka 2", ProductName);
        DB.Insert("INSERT INTO products(name, price, description) VALUES (?,?,?)", {},
            { ProductName, std::to_string(111.11 * i), ProductDescription }
        );
    }

    DB.Update("UPDATE products set price=price/2", {}, {});
    DB.Update("UPDATE products SET price=? WHERE id=?", {}, {std::to_string(99999.99), std::to_string(5) });
    DB.Delete("DELETE FROM products WHERE id=?", 6);

    DB.Select("SELECT * FROM products", Rows);
    for (size_t i = 0; i < Rows.size(); ++i)
    {
        std::cout << Rows[i][0] << " | " << Rows[i][1] << " | " << Rows[i][2] << std::endl;
        std::cout << "Description:" << std::endl;
        std::cout << Rows[i][3] << std::endl << std::endl;
    }

    DB.Select("DROP TABLE IF EXISTS products", Rows);

    return 0;
}
