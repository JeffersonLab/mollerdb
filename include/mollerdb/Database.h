#pragma once
#include <string>
#include <memory>
namespace arrow { class Table; }
namespace moller::db {
class Database {
public:
    Database(const std::string& conn_string);
    ~Database();
    std::shared_ptr<arrow::Table> get_main_detector_results_arrow(int start_run, int end_run, const std::string& detector_name);
private:
    class Impl;
    std::unique_ptr<Impl> pimpl;
};
}
