#include "mollerdb/Database.h"
#include <sqlpp23/postgresql/postgresql.h>
#include <libpq-fe.h>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>

namespace sql = sqlpp::postgresql;

class moller::db::Database::Impl {
private:
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
        
        // Extract parsed values into sqlpp23's connection_config
        for (PQconninfoOption* opt = opts; opt->keyword != nullptr; ++opt) {
            if (opt->val == nullptr) continue;
            
            std::string keyword = opt->keyword;
            std::string value = opt->val;
            
            if (keyword == "host") {
                config->host = value;
            } else if (keyword == "hostaddr") {
                config->hostaddr = value;
            } else if (keyword == "port") {
                try {
                    config->port = std::stoul(value);
                } catch (const std::exception& e) {
                    PQconninfoFree(opts);
                    throw std::invalid_argument("Invalid port value: " + value);
                }
            } else if (keyword == "dbname") {
                config->dbname = value;
            } else if (keyword == "user") {
                config->user = value;
            } else if (keyword == "password") {
                config->password = value;
            } else if (keyword == "connect_timeout") {
                try {
                    config->connect_timeout = std::stoul(value);
                } catch (const std::exception&) {
                    // Ignore invalid timeout values
                }
            } else if (keyword == "client_encoding") {
                config->client_encoding = value;
            } else if (keyword == "options") {
                config->options = value;
            } else if (keyword == "application_name") {
                config->application_name = value;
            } else if (keyword == "fallback_application_name") {
                config->fallback_application_name = value;
            } else if (keyword == "keepalives") {
                config->keepalives = (value == "1");
            } else if (keyword == "keepalives_idle") {
                try {
                    config->keepalives_idle = std::stoul(value);
                } catch (const std::exception&) {
                    // Ignore invalid values
                }
            } else if (keyword == "keepalives_interval") {
                try {
                    config->keepalives_interval = std::stoul(value);
                } catch (const std::exception&) {
                    // Ignore invalid values
                }
            } else if (keyword == "keepalives_count") {
                try {
                    config->keepalives_count = std::stoul(value);
                } catch (const std::exception&) {
                    // Ignore invalid values
                }
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
                config->sslcompression = (value == "1");
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
        }
        
        PQconninfoFree(opts);
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
