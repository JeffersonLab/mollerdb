#!/usr/bin/env bash
set -euo pipefail

# Script to generate C++ schema headers from DBML

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO_ROOT="$(cd "${SCRIPT_DIR}/.." && pwd)"

cd "${REPO_ROOT}"

# Check if required tools are available
if ! command -v dbml2sql &> /dev/null; then
    echo "Error: dbml2sql not found. Install with: npm install -g @dbml/cli"
    exit 1
fi

if ! command -v python3 &> /dev/null; then
    echo "Error: python3 not found. Please install Python 3."
    exit 1
fi

SQLPP23_SCRIPT="thirdparty/sqlpp23/scripts/sqlpp23-ddl2cpp"
if [ ! -f "${SQLPP23_SCRIPT}" ]; then
    echo "Error: sqlpp23-ddl2cpp script not found at ${SQLPP23_SCRIPT}"
    echo "Make sure git submodules are initialized: git submodule update --init --recursive"
    exit 1
fi

echo "==> Generating PostgreSQL SQL from DBML..."
dbml2sql --postgresql \
  -o schema/qwparity_schema.sql \
  thirdparty/MOLLER-parity-schema/qwparity_schema.dbml

echo "==> Generating C++ headers from SQL..."
python3 "${SQLPP23_SCRIPT}" \
  --path-to-ddl schema/qwparity_schema.sql \
  --namespace moller::db::schema \
  --path-to-header include/mollerdb/schema/qwparity_schema.h \
  --path-to-custom-types schema/custom_types.csv

echo "==> Schema generation complete!"
echo "    SQL: schema/qwparity_schema.sql"
echo "    C++: include/mollerdb/schema/qwparity_schema.h"
