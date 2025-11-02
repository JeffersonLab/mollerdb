#include "mollerdb/Database.h"
#include <sqlpp23/postgresql/postgresql.h>
#include <algorithm>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>

#include "mollerdb/schema/qwparity_schema.h"

namespace sql = sqlpp::postgresql;

// Note: Full sqlpp23 PostgreSQL integration will be implemented in future commits.
// This currently provides a placeholder implementation with schema headers available.

class moller::db::Database::Impl {
private:
    // Helper function to parse boolean values according to PostgreSQL conventions
    // Accepts: true/false, on/off, yes/no, 1/0 (case-insensitive)
    static bool parse_bool(const std::string& value) {
        std::string lower_val = value;
        std::transform(lower_val.begin(), lower_val.end(), lower_val.begin(), 
                      [](unsigned char c) { return std::tolower(c); });
        
        if (lower_val == "1" || lower_val == "true" || lower_val == "on" || lower_val == "yes") {
            return true;
        } else if (lower_val == "0" || lower_val == "false" || lower_val == "off" || lower_val == "no") {
            return false;
        }
        
        throw std::invalid_argument("Invalid boolean value: " + value);
    }
    
    static std::shared_ptr<sql::connection_config> parse_connection_string(const std::string& conn_string) {
        auto config = std::make_shared<sql::connection_config>();
        
        // Parse connection string (PostgreSQL libpq format)
        // Supported format: "key1=value1 key2=value2 ..."
        // Values can be quoted with single quotes to include spaces or special characters
        // Single quotes in values must be escaped as \'
        
        auto trim = [](const std::string& str) -> std::string {
            auto first = str.find_first_not_of(" \t");
            auto last = str.find_last_not_of(" \t");
            if (first != std::string::npos && last != std::string::npos) {
                return str.substr(first, last - first + 1);
            }
            return "";
        };
        
        size_t pos = 0;
        const std::string& str = conn_string;
        
        while (pos < str.length()) {
            // Skip whitespace
            while (pos < str.length() && (str[pos] == ' ' || str[pos] == '\t')) {
                ++pos;
            }
            if (pos >= str.length()) break;
            
            // Find the key (everything before '=')
            size_t eq_pos = str.find('=', pos);
            if (eq_pos == std::string::npos) break;
            
            std::string key = trim(str.substr(pos, eq_pos - pos));
            pos = eq_pos + 1;
            
            // Skip whitespace after '='
            while (pos < str.length() && (str[pos] == ' ' || str[pos] == '\t')) {
                ++pos;
            }
            if (pos >= str.length()) break;
            
            // Parse the value (which may be quoted)
            std::string value;
            
            if (str[pos] == '\'') {
                // Quoted value - parse until closing quote, handling escaped quotes
                ++pos; // Skip opening quote
                while (pos < str.length()) {
                    if (str[pos] == '\\' && pos + 1 < str.length() && str[pos + 1] == '\'') {
                        // Escaped quote
                        value += '\'';
                        pos += 2;
                    } else if (str[pos] == '\'') {
                        // End of quoted value
                        ++pos;
                        break;
                    } else {
                        value += str[pos];
                        ++pos;
                    }
                }
            } else {
                // Unquoted value - read until space or end of string
                while (pos < str.length() && str[pos] != ' ' && str[pos] != '\t') {
                    value += str[pos];
                    ++pos;
                }
            }
            
            // Process the key-value pair
            try {
                if (key == "host") {
                    config->host = value;
                } else if (key == "hostaddr") {
                    config->hostaddr = value;
                } else if (key == "port") {
                    config->port = std::stoul(value);
                } else if (key == "dbname") {
                    config->dbname = value;
                } else if (key == "user") {
                    config->user = value;
                } else if (key == "password") {
                    config->password = value;
                } else if (key == "connect_timeout") {
                    config->connect_timeout = std::stoul(value);
                } else if (key == "client_encoding") {
                    config->client_encoding = value;
                } else if (key == "options") {
                    config->options = value;
                } else if (key == "application_name") {
                    config->application_name = value;
                } else if (key == "fallback_application_name") {
                    config->fallback_application_name = value;
                } else if (key == "keepalives") {
                    config->keepalives = parse_bool(value);
                } else if (key == "keepalives_idle") {
                    config->keepalives_idle = std::stoul(value);
                } else if (key == "keepalives_interval") {
                    config->keepalives_interval = std::stoul(value);
                } else if (key == "keepalives_count") {
                    config->keepalives_count = std::stoul(value);
                } else if (key == "sslmode") {
                    if (value == "disable") {
                        config->sslmode = sql::connection_config::sslmode_t::disable;
                    } else if (value == "allow") {
                        config->sslmode = sql::connection_config::sslmode_t::allow;
                    } else if (value == "prefer") {
                        config->sslmode = sql::connection_config::sslmode_t::prefer;
                    } else if (value == "require") {
                        config->sslmode = sql::connection_config::sslmode_t::require;
                    } else if (value == "verify-ca") {
                        config->sslmode = sql::connection_config::sslmode_t::verify_ca;
                    } else if (value == "verify-full") {
                        config->sslmode = sql::connection_config::sslmode_t::verify_full;
                    }
                } else if (key == "sslcompression") {
                    config->sslcompression = parse_bool(value);
                } else if (key == "sslcert") {
                    config->sslcert = value;
                } else if (key == "sslkey") {
                    config->sslkey = value;
                } else if (key == "sslrootcert") {
                    config->sslrootcert = value;
                } else if (key == "sslcrl") {
                    config->sslcrl = value;
                } else if (key == "requirepeer") {
                    config->requirepeer = value;
                } else if (key == "krbsrvname") {
                    config->krbsrvname = value;
                } else if (key == "service") {
                    config->service = value;
                }
                // Silently ignore unknown keys for forward compatibility
            } catch (const std::invalid_argument& e) {
                throw std::invalid_argument("Invalid value for '" + key + "': " + std::string(e.what()));
            } catch (const std::out_of_range& e) {
                throw std::invalid_argument("Value out of range for '" + key + "': " + value);
            }
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
    // TODO: Implement actual query using sqlpp23 and convert results to Arrow Table
    return nullptr;
}
