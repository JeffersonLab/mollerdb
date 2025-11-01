#include "mollerdb/Database.h"
#include <sqlpp23/postgresql/postgresql.h>
#include <iostream>
#include <memory>
#include <stdexcept>

namespace sql = sqlpp::postgresql;

class moller::db::Database::Impl {
private:
    static std::string trim(const std::string& str) {
        auto first = str.find_first_not_of(" \t");
        auto last = str.find_last_not_of(" \t");
        if (first != std::string::npos && last != std::string::npos) {
            return str.substr(first, last - first + 1);
        }
        return "";
    }

    static std::shared_ptr<sql::connection_config> parse_connection_string(const std::string& conn_string) {
        auto config = std::make_shared<sql::connection_config>();
        
        // Parse connection string (PostgreSQL libpq format)
        // Supported format: "key1=value1 key2=value2 ..."
        // Supported keys: host, dbname, user, password, port
        // 
        // NOTE: This parser does not handle quoted values (e.g., password='my pass').
        // Values containing spaces or special characters will be incorrectly parsed.
        // This limitation is acceptable for the keys currently parsed (host, dbname, 
        // user, password, port) as spaces are unlikely to appear in these values.
        // TODO: Create GitHub issue to implement proper quote handling for full 
        // PostgreSQL connection string support (see libpq documentation).
        size_t pos = 0;
        std::string str = conn_string;
        while (pos < str.length()) {
            size_t eq_pos = str.find('=', pos);
            if (eq_pos == std::string::npos) break;
            
            size_t space_pos = str.find(' ', eq_pos);
            if (space_pos == std::string::npos) space_pos = str.length();
            
            std::string key = trim(str.substr(pos, eq_pos - pos));
            std::string value = trim(str.substr(eq_pos + 1, space_pos - eq_pos - 1));
            
            if (key == "host") config->host = value;
            else if (key == "dbname") config->dbname = value;
            else if (key == "user") config->user = value;
            else if (key == "password") config->password = value;
            else if (key == "port") {
                try {
                    config->port = std::stoul(value);
                } catch (const std::exception& e) {
                    throw std::invalid_argument("Invalid port value: " + value);
                }
            }
            
            pos = space_pos + 1;
        }
        
        return config;
    }

public:
    explicit Impl(const std::string& conn_string) {
        config = parse_connection_string(conn_string);
        
        try {
            conn = std::make_unique<sql::connection>(config);
        } catch (const std::exception& e) {
            throw std::runtime_error("Failed to connect to database: " + std::string(e.what()));
        }
    }
    
    std::shared_ptr<sql::connection_config> config;
    std::unique_ptr<sql::connection> conn;
};

moller::db::Database::Database(const std::string& conn_string) : pimpl(std::make_unique<Impl>(conn_string)) {}
moller::db::Database::~Database() = default;
std::shared_ptr<arrow::Table> moller::db::Database::get_main_detector_results_arrow(int, int, const std::string&) {
    return nullptr;
}
