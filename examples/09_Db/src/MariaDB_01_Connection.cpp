#include <iostream>
#include <string>
#include "conncpp.hpp"

constexpr const char* HOST{ "192.168.56.2" };
constexpr const char* DB{ "testdb" };
constexpr const char* USER{ "testuser" };
constexpr const char* PASSWD{ "testpasswd" };

typedef std::unique_ptr<sql::Connection> Connection;
sql::Connection* GetConnection(sql::Driver* driver, sql::ConnectOptionsMap* additional_opts = nullptr);

int main(int argc, char* argv[])
{
    sql::Driver* DBDriver = sql::mariadb::get_driver_instance();
    Connection Conn(GetConnection(DBDriver));
    if (Conn)
    {
        std::cout << "Successfully connected with mariadb server!" << std::endl;
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
    }
    catch (sql::InvalidArgumentException& e)
    {
        std::cerr << "Invalid argument: " << e.what() << std::endl;
    }
    catch (sql::SQLException& e)
    {
        std::cerr << "SQL error: " << e.what() << std::endl;
    }
    catch (std::exception& e)
    {
        std::cerr << "Connection error: " << e.what() << std::endl;
    }
    return Conn;
}
