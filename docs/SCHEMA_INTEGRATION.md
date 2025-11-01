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
- SQL is generated during CMake build in `schema/qwparity_schema.sql`
- Custom type mappings in `schema/custom_types.csv` handle PostgreSQL ENUMs
- Generated SQL is excluded from version control

### 3. C++ Header Generation  
- Added `thirdparty/sqlpp23` as a git submodule for type-safe SQL in C++
- Use `sqlpp23-ddl2cpp` to generate C++ headers from SQL during CMake build
- Generated header is `include/mollerdb/schema/qwparity_schema.h`
- Provides compile-time type safety for database queries
- Generated header is excluded from version control

### 4. Build-time Generation
- CMake automatically generates both SQL and C++ headers during the build process
- No manual script execution needed for normal development
- Files are generated in the source tree for IDE integration
- `scripts/generate_schema.sh` retained for manual testing if needed

## Architecture Benefits

1. **Single Source of Truth**: Schema is defined once in MOLLER-parity-schema
2. **Version Control**: Submodule tracks specific schema versions
3. **Type Safety**: sqlpp23 provides compile-time SQL validation
4. **Easy Updates**: Simple workflow to pull schema changes
5. **Build-time Generation**: Schema artifacts generated automatically during build
6. **Clean Repository**: Generated files excluded from version control

## Update Workflow

When the schema changes in MOLLER-parity-schema:

```bash
# 1. Update submodule
cd thirdparty/MOLLER-parity-schema
git pull origin main
cd ../..

# 2. Commit submodule update
git add thirdparty/MOLLER-parity-schema
git commit -m "Update database schema to version X.Y.Z"

# 3. Rebuild to regenerate schema files
pip install . --verbose
```

## Alternative Approaches Considered

1. **Build-time Generation**: Chosen approach - generates during CMake build
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
- `.gitignore`: Excludes generated schema files
- `thirdparty/MOLLER-parity-schema/`: Schema source (submodule)
- `thirdparty/sqlpp23/`: SQL library (submodule)
- `schema/custom_types.csv`: Type mappings for sqlpp23
- `schema/README.md`: Schema integration documentation
- `scripts/generate_schema.sh`: Manual generation script (optional)
- `docs/SCHEMA_INTEGRATION.md`: This documentation file
- `CMakeLists.txt`: Schema generation during build
- `.github/workflows/build.yml`: Updated to checkout submodules
- `README.md`: Added schema integration overview
- `AGENTS.md`: Updated with documentation structure guidance

## Generated Files (not in repository)

- `schema/qwparity_schema.sql`: Generated PostgreSQL schema
- `include/mollerdb/schema/qwparity_schema.h`: Generated C++ headers

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
