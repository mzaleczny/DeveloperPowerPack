#include <iostream>
#include <string>
#include <sstream>
#include "conncpp.hpp"

constexpr const char* HOST{ "192.168.56.2" };
constexpr const char* DB{ "testdb" };
constexpr const char* USER{ "testuser" };
constexpr const char* PASSWD{ "testpasswd" };

typedef std::unique_ptr<sql::Connection> Connection;
typedef std::unique_ptr<sql::PreparedStatement> PreparedStatement;
typedef std::unique_ptr<sql::ParameterMetaData> ParameterMetaData;
typedef std::unique_ptr<sql::Statement> Statement;
typedef std::unique_ptr<sql::ResultSet> ResultSet;

sql::Connection* GetConnection(sql::Driver* driver, sql::ConnectOptionsMap* additional_opts = nullptr);
void ShowWarningsForStatement(Statement& stmt);

int main(int argc, char* argv[])
{
    sql::Driver* DBDriver = sql::mariadb::get_driver_instance();
    Connection Conn(GetConnection(DBDriver));
    if (Conn)
    {
        try
        {
            Statement stmt;
            PreparedStatement pstmt;
            ResultSet res;

            stmt.reset(Conn->createStatement());
            stmt->executeQuery("DROP TABLE IF EXISTS products");
            ShowWarningsForStatement(stmt);
            stmt->execute("CREATE TABLE products(id INT AUTO_INCREMENT PRIMARY KEY, name VARCHAR(128), price DECIMAL(10,2), description TEXT)");
            ShowWarningsForStatement(stmt);

            pstmt.reset(Conn->prepareStatement("INSERT INTO products(name, price, description) VALUES (?,?,?)"));
            for (int i = 1; i <= 9; ++i)
            {
                std::string ProductName = "Prduct" + std::to_string(i);
                pstmt->setString(1, ProductName);
                pstmt->setFloat(2, 111.11 * i);
                pstmt->setString(3, std::format("Krótki opis dla produktu: {}\nLinijka1\nLinijka 2", ProductName));
                pstmt->executeUpdate();
            }
            pstmt.reset(Conn->prepareStatement("SELECT * FROM products"));
            res.reset(pstmt->executeQuery());

            while (res->next())
            {
                std::cout << res->getString(1) << " | " << res->getString(2) << " | " << res->getString(3) << std::endl;
                std::cout << "Description:" << std::endl;
                std::cout << res->getString(4) << std::endl << std::endl;
            }
            res->close();

            stmt->executeQuery("DROP TABLE IF EXISTS products");
            ShowWarningsForStatement(stmt);
        }
        catch (sql::SQLException& e)
        {
            std::cerr << "SQL error: " << e.what() << std::endl;
            std::cerr << "SQLState: " + std::string(e.getSQLState()) << std::endl;
        }
    }
    return 0;
}



sql::Connection* GetConnection(sql::Driver* driver, sql::ConnectOptionsMap* additional_options)
{
    if (driver == nullptr)
    {
        driver = sql::mariadb::get_driver_instance();
    }

    sql::ConnectOptionsMap connection_properties;
    connection_properties["user"] = USER;
    connection_properties["password"] = PASSWD;

    connection_properties["useTls"] = "false";
    //connection_properties["metadataUseInfoSchema"] = "1";

    if (additional_options != nullptr)
    {
        for (sql::ConnectOptionsMap::const_iterator cit = additional_options->cbegin(); cit != additional_options->cend(); ++cit)
        {
            connection_properties[cit->first] = cit->second;
        }
    }

    std::string url = std::string("jdbc:mariadb") + "://" + HOST + ":3306/" + DB;
    std::cout << url << std::endl;
    sql::Connection* Conn{};
    try
    {
        Conn = sql::DriverManager::getConnection(url.c_str(), connection_properties);
    }
    catch (sql::SQLWarning& e)
    {
        std::cerr << "SQL warning: " << e.getMessage().c_str() << std::endl;
        std::cerr << "SQLState: " + std::string(e.getSQLState()) << std::endl;
    }
    catch (sql::InvalidArgumentException& e)
    {
        std::cerr << "Invalid argument: " << e.what() << std::endl;
    }
    catch (sql::SQLException& e)
    {
        std::cerr << "SQL error: " << e.what() << std::endl;
        std::cerr << "SQLState: " + std::string(e.getSQLState()) << std::endl;
    }
    catch (std::exception& e)
    {
        std::cerr << "Connection error: " << e.what() << std::endl;
    }
    return Conn;
}

void ShowWarningsForStatement(Statement& stmt)
{
    int count = 0;
    std::stringstream msg;
    for (const sql::SQLWarning* warn = stmt->getWarnings(); warn; warn = warn->getNextWarning())
    {
        ++count;
        msg.str("");
        msg << "... ErrorCode = '" << warn->getErrorCode() << "', ";
        msg << "SQLState = '" << warn->getSQLState() << "', ";
        msg << "ErrorMessage = '" << warn->getMessage() << "'";
        std::cerr << msg.str();
    }
}
