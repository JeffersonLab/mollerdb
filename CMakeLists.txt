cmake_minimum_required(VERSION 3.18)
project(mollerdb LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --- sqlpp23 (header-only library as submodule) ---
add_subdirectory(thirdparty/sqlpp23 EXCLUDE_FROM_ALL)

# --- Find Dependencies ---
# scikit-build-core helps CMake find the correct Python
find_package(Python 3.7 COMPONENTS Interpreter Development REQUIRED)
find_package(PostgreSQL REQUIRED)
find_package(Arrow REQUIRED)
find_package(pybind11 CONFIG REQUIRED)

# --- sqlpp23 setup ---
# Add sqlpp23 as a subdirectory with the PostgreSQL connector enabled
set(BUILD_POSTGRESQL_CONNECTOR ON CACHE BOOL "Build PostgreSQL Connector" FORCE)
set(BUILD_MYSQL_CONNECTOR OFF CACHE BOOL "Build MySQL Connector" FORCE)
set(BUILD_MARIADB_CONNECTOR OFF CACHE BOOL "Build MariaDB Connector" FORCE)
set(BUILD_SQLITE3_CONNECTOR OFF CACHE BOOL "Build SQLite3 Connector" FORCE)
set(BUILD_SQLCIPHER_CONNECTOR OFF CACHE BOOL "Build SQLCipher Connector" FORCE)
set(BUILD_TESTING OFF CACHE BOOL "Build tests" FORCE)
add_subdirectory(thirdparty/sqlpp23)

# --- Core C++ Library ---
add_library(mollerdb_core SHARED
    src/Database.cpp
)
target_include_directories(mollerdb_core PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/sqlpp23/include"
)
target_link_libraries(mollerdb_core PRIVATE
    sqlpp23::sqlpp23
    sqlpp23::postgresql
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
