# Database Schema Integration Solution

## Problem Statement

Determine the best way to provide the database schema to the mollerdb repository. The schema is versioned as DBML in the `JeffersonLab/MOLLER-parity-schema` repository, and a CI workflow there exports it to SQL commands that sqlpp23 tools understand.

## Solution

The implemented solution uses a **git submodule approach with generated artifacts**:

### 1. Schema Source (MOLLER-parity-schema submodule)
- Added `thirdparty/MOLLER-parity-schema` as a git submodule
- This provides the authoritative DBML schema source
- Allows version tracking and easy schema updates

### 2. SQL Generation
- Convert DBML to PostgreSQL SQL using `dbml2sql` tool from `@dbml/cli`
- Generated SQL is stored in `schema/qwparity_schema.sql`
- Custom type mappings in `schema/custom_types.csv` handle PostgreSQL ENUMs

### 3. C++ Header Generation  
- Added `thirdparty/sqlpp23` as a git submodule for type-safe SQL in C++
- Use `sqlpp23-ddl2cpp` to generate C++ headers from SQL
- Generated header is `include/mollerdb/schema/qwparity_schema.h`
- Provides compile-time type safety for database queries

### 4. Automation
- Created `scripts/generate_schema.sh` to automate the entire process
- Run this script whenever the schema submodule is updated

## Architecture Benefits

1. **Single Source of Truth**: Schema is defined once in MOLLER-parity-schema
2. **Version Control**: Submodule tracks specific schema versions
3. **Type Safety**: sqlpp23 provides compile-time SQL validation
4. **Easy Updates**: Simple workflow to pull schema changes
5. **Build Independence**: Generated files are committed, so builds don't require Node.js/Python

## Update Workflow

When the schema changes in MOLLER-parity-schema:

```bash
# 1. Update submodule
cd thirdparty/MOLLER-parity-schema
git pull origin main
cd ../..

# 2. Regenerate files
./scripts/generate_schema.sh

# 3. Commit changes
git add thirdparty/MOLLER-parity-schema schema/ include/mollerdb/schema/
git commit -m "Update database schema to version X.Y.Z"
```

## Alternative Approaches Considered

1. **Git Submodule Only**: Would require generating SQL/C++ headers during build (complex)
2. **Download from Artifacts**: Artifacts expire after 90 days (unreliable)
3. **Manual Copy**: No version tracking, easy to get out of sync
4. **Schema in mollerdb**: Duplication, harder to maintain

## Dependencies

### For Schema Generation (Development Only)
- Node.js and npm (for `@dbml/cli`)
- Python 3 (for `sqlpp23-ddl2cpp`)

### For Building mollerdb
- C++23 compiler with good support (GCC 14+, Clang 18+, MSVC 2022)
- sqlpp23 (included as submodule)
- libpq (PostgreSQL client library)
- Apache Arrow

## Files Added/Modified

- `.gitmodules`: Submodule configurations
- `thirdparty/MOLLER-parity-schema/`: Schema source (submodule)
- `thirdparty/sqlpp23/`: SQL library (submodule)
- `schema/qwparity_schema.sql`: Generated PostgreSQL schema
- `schema/custom_types.csv`: Type mappings for sqlpp23
- `schema/README.md`: Schema integration documentation
- `include/mollerdb/schema/qwparity_schema.h`: Generated C++ headers
- `scripts/generate_schema.sh`: Automation script
- `CMakeLists.txt`: Updated to include sqlpp23
- `.github/workflows/build.yml`: Updated to checkout submodules
- `README.md`: Added schema integration overview

## Known Issues

The DBML schema in MOLLER-parity-schema currently uses some non-standard SQL types that may need to be addressed:

1. **DATETIME**: Used in `run`, `runlet`, and `analysis` tables. PostgreSQL uses `TIMESTAMP` or `TIMESTAMPTZ` instead.
2. **TINYINT**: Used in `error_code` table. PostgreSQL uses `SMALLINT` as the equivalent.

These issues exist in the upstream DBML and should be fixed in the MOLLER-parity-schema repository. The dbml-cli tool directly converts these types, and they may cause errors when creating the actual database schema.

**Recommendation**: Open an issue in MOLLER-parity-schema to update the DBML file to use PostgreSQL-compatible types.

## Next Steps

The schema integration is complete. Future work includes:

1. Implement actual database queries in `Database.cpp` using sqlpp23
2. Convert query results to Apache Arrow tables
3. Add Python bindings for the query functions
4. Add tests for database operations
