#include <pybind11/pybind11.h>
#include <arrow/api.h>
#include "mollerdb/Database.h"
namespace py = pybind11;
PYBIND11_MODULE(mollerdb, m) {
    py::class_<moller::db::Database>(m, "Database")
        .def(py::init<const std::string&>())
        .def("get_main_detector_results", &moller::db::Database::get_main_detector_results_arrow);
}
