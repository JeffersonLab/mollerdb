#include "mollerdb/Database.h"
#include <sqlpp23/postgresql/postgresql.h>
#include <libpq-fe.h>
#include <algorithm>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>

namespace sql = sqlpp::postgresql;

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
        
        // Use libpq's PQconninfoParse to properly parse the connection string
        // This handles all PostgreSQL connection string features including:
        // - Quoted values (e.g., password='my pass')
        // - Escaped characters
        // - All libpq connection parameters
        char* errmsg = nullptr;
        PQconninfoOption* opts = PQconninfoParse(conn_string.c_str(), &errmsg);
        
        if (opts == nullptr) {
            std::string error_message = errmsg ? std::string(errmsg) : "unknown error";
            if (errmsg) PQfreemem(errmsg);
            throw std::invalid_argument("Invalid connection string: " + error_message);
        }
        
        // Use RAII to ensure opts is always freed
        struct OptsGuard {
            PQconninfoOption* opts;
            ~OptsGuard() { if (opts) PQconninfoFree(opts); }
        } guard{opts};
        
        // Extract parsed values into sqlpp23's connection_config
        for (PQconninfoOption* opt = opts; opt->keyword != nullptr; ++opt) {
            if (opt->val == nullptr) continue;
            
            std::string keyword = opt->keyword;
            std::string value = opt->val;
            
            try {
                if (keyword == "host") {
                    config->host = value;
                } else if (keyword == "hostaddr") {
                    config->hostaddr = value;
                } else if (keyword == "port") {
                    config->port = std::stoul(value);
                } else if (keyword == "dbname") {
                    config->dbname = value;
                } else if (keyword == "user") {
                    config->user = value;
                } else if (keyword == "password") {
                    config->password = value;
                } else if (keyword == "connect_timeout") {
                    config->connect_timeout = std::stoul(value);
                } else if (keyword == "client_encoding") {
                    config->client_encoding = value;
                } else if (keyword == "options") {
                    config->options = value;
                } else if (keyword == "application_name") {
                    config->application_name = value;
                } else if (keyword == "fallback_application_name") {
                    config->fallback_application_name = value;
                } else if (keyword == "keepalives") {
                    config->keepalives = parse_bool(value);
                } else if (keyword == "keepalives_idle") {
                    config->keepalives_idle = std::stoul(value);
                } else if (keyword == "keepalives_interval") {
                    config->keepalives_interval = std::stoul(value);
                } else if (keyword == "keepalives_count") {
                    config->keepalives_count = std::stoul(value);
                } else if (keyword == "sslmode") {
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
                } else if (keyword == "sslcompression") {
                    config->sslcompression = parse_bool(value);
                } else if (keyword == "sslcert") {
                    config->sslcert = value;
                } else if (keyword == "sslkey") {
                    config->sslkey = value;
                } else if (keyword == "sslrootcert") {
                    config->sslrootcert = value;
                } else if (keyword == "sslcrl") {
                    config->sslcrl = value;
                } else if (keyword == "requirepeer") {
                    config->requirepeer = value;
                } else if (keyword == "krbsrvname") {
                    config->krbsrvname = value;
                } else if (keyword == "service") {
                    config->service = value;
                }
            } catch (const std::invalid_argument& e) {
                throw std::invalid_argument("Invalid value for '" + keyword + "': " + std::string(e.what()));
            } catch (const std::out_of_range& e) {
                throw std::invalid_argument("Value out of range for '" + keyword + "': " + value);
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
    return nullptr;
}
