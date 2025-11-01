cmake_minimum_required(VERSION 3.18)
project(mollerdb LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --- Find Dependencies ---
# scikit-build-core helps CMake find the correct Python
find_package(Python 3.7 COMPONENTS Interpreter Development REQUIRED)
find_package(libpqxx REQUIRED)
find_package(Arrow REQUIRED)
find_package(pybind11 CONFIG REQUIRED)

# --- Core C++ Library ---
add_library(mollerdb_core SHARED
    src/Database.cpp
)
target_include_directories(mollerdb_core PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
)
target_link_libraries(mollerdb_core PRIVATE
    pqxx::pqxx
    arrow_shared
)
set_target_properties(mollerdb_core PROPERTIES POSITION_INDEPENDENT_CODE ON)


# --- Python Bindings ---
# The module is named `mollerdb` and will be placed inside the `mollerdb` package.
pybind11_add_module(mollerdb python/bindings.cpp)

target_link_libraries(mollerdb PRIVATE mollerdb_core)

# --- Installation ---
# The destination directory is now `mollerdb`.
install(TARGETS mollerdb DESTINATION mollerdb)
