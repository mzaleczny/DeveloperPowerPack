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
            ResultSet res;

            stmt.reset(Conn->createStatement());

            stmt->executeQuery("DROP TABLE IF EXISTS products");
            ShowWarningsForStatement(stmt);
            stmt->execute("CREATE TABLE products(id INT AUTO_INCREMENT PRIMARY KEY, name VARCHAR(128), price DECIMAL(10,2))");
            ShowWarningsForStatement(stmt);
            stmt->execute("INSERT INTO products(name, price) VALUES ('Product1', 111.11)");
            ShowWarningsForStatement(stmt);
            stmt->execute("INSERT INTO products(name, price) VALUES ('Product2', 222.22)");
            ShowWarningsForStatement(stmt);
            stmt->execute("INSERT INTO products(name, price) VALUES ('Product3', 333.33)");
            ShowWarningsForStatement(stmt);
            stmt->execute("INSERT INTO products(name, price) VALUES ('Product4', 444.44)");
            ShowWarningsForStatement(stmt);
            stmt->execute("INSERT INTO products(name, price) VALUES ('Product5', 555.55)");
            ShowWarningsForStatement(stmt);
            stmt->execute("INSERT INTO products(name, price) VALUES ('Product6', 666.66)");
            ShowWarningsForStatement(stmt);
            stmt->execute("INSERT INTO products(name, price) VALUES ('Product7', 777.77)");
            ShowWarningsForStatement(stmt);
            stmt->execute("INSERT INTO products(name, price) VALUES ('Product8', 888.88)");
            ShowWarningsForStatement(stmt);
            stmt->execute("INSERT INTO products(name, price) VALUES ('Product9', 999.99)");
            ShowWarningsForStatement(stmt);

            res.reset(stmt->executeQuery("SELECT * FROM products"));
            ShowWarningsForStatement(stmt);
            while (res->next())
            {
                std::cout << res->getString(1) << " | " << res->getString(2) << " | " << res->getString(3) << std::endl;
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
