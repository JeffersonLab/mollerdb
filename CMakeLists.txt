cmake_minimum_required(VERSION 3.18)
project(mollerdb LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --- Generate Schema Files from DBML ---
set(DBML_FILE "${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/MOLLER-parity-schema/qwparity_schema.dbml")
set(SQL_FILE "${CMAKE_CURRENT_SOURCE_DIR}/schema/qwparity_schema.sql")
set(SCHEMA_HEADER "${CMAKE_CURRENT_SOURCE_DIR}/include/mollerdb/schema/qwparity_schema.h")
set(CUSTOM_TYPES_FILE "${CMAKE_CURRENT_SOURCE_DIR}/schema/custom_types.csv")

# Find dbml2sql and python3
find_program(DBML2SQL dbml2sql)
find_package(Python3 COMPONENTS Interpreter REQUIRED)

# Check if sqlpp23-ddl2cpp script exists
set(SQLPP23_DDL2CPP "${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/sqlpp23/scripts/sqlpp23-ddl2cpp")

# Only proceed with schema generation if both tools are available
if(DBML2SQL AND EXISTS ${SQLPP23_DDL2CPP})
    # Generate SQL from DBML
    add_custom_command(
        OUTPUT ${SQL_FILE}
        COMMAND ${DBML2SQL} --postgresql -o ${SQL_FILE} ${DBML_FILE}
        DEPENDS ${DBML_FILE}
        COMMENT "Generating PostgreSQL SQL from DBML schema..."
    )

    # Generate C++ headers from SQL
    add_custom_command(
        OUTPUT ${SCHEMA_HEADER}
        COMMAND ${Python3_EXECUTABLE} ${SQLPP23_DDL2CPP}
                --path-to-ddl ${SQL_FILE}
                --namespace moller::db::schema
                --path-to-header ${SCHEMA_HEADER}
                --path-to-custom-types ${CUSTOM_TYPES_FILE}
        DEPENDS ${SQL_FILE} ${CUSTOM_TYPES_FILE}
        COMMENT "Generating C++ headers from SQL schema..."
    )

    # Create a custom target for schema generation
    add_custom_target(generate_schema ALL DEPENDS ${SCHEMA_HEADER})
else()
    if(NOT DBML2SQL)
        message(FATAL_ERROR "dbml2sql not found. Install with: npm install -g @dbml/cli")
    endif()
    if(NOT EXISTS ${SQLPP23_DDL2CPP})
        message(FATAL_ERROR "sqlpp23-ddl2cpp script not found at ${SQLPP23_DDL2CPP}. Make sure git submodules are initialized: git submodule update --init --recursive")
    endif()
endif()

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
add_dependencies(mollerdb_core generate_schema)
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
