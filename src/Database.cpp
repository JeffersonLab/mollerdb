#include "mollerdb/Database.h"
#include <pqxx/pqxx>
#include <iostream>

class moller::db::Database::Impl {
public:
    explicit Impl(const std::string& conn_string) : conn(conn_string) {
        if (!conn.is_open()) { throw std::runtime_error("Failed to connect to database."); }
    }
    pqxx::connection conn;
};

moller::db::Database::Database(const std::string& conn_string) : pimpl(std::make_unique<Impl>(conn_string)) {}
moller::db::Database::~Database() = default;
std::shared_ptr<arrow::Table> moller::db::Database::get_main_detector_results_arrow(int, int, const std::string&) {
    return nullptr;
}
