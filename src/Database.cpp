#include "mollerdb/Database.h"
#include <sqlpp23/postgresql/postgresql.h>
#include <iostream>
#include <memory>
#include <stdexcept>

namespace sql = sqlpp::postgresql;

class moller::db::Database::Impl {
private:
    static std::shared_ptr<sql::connection_config> parse_connection_string(const std::string& conn_string) {
        auto config = std::make_shared<sql::connection_config>();
        
        // Parse connection string (basic parsing, could be enhanced)
        // Format: "host=... dbname=... user=... password=..."
        size_t pos = 0;
        std::string str = conn_string;
        while (pos < str.length()) {
            size_t eq_pos = str.find('=', pos);
            if (eq_pos == std::string::npos) break;
            
            size_t space_pos = str.find(' ', eq_pos);
            if (space_pos == std::string::npos) space_pos = str.length();
            
            std::string key = str.substr(pos, eq_pos - pos);
            std::string value = str.substr(eq_pos + 1, space_pos - eq_pos - 1);
            
            // Trim whitespace
            key.erase(0, key.find_first_not_of(" \t"));
            key.erase(key.find_last_not_of(" \t") + 1);
            value.erase(0, value.find_first_not_of(" \t"));
            value.erase(value.find_last_not_of(" \t") + 1);
            
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
