#!/usr/bin/env bash
set -euo pipefail

# Script to generate C++ schema headers from DBML

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO_ROOT="$(cd "${SCRIPT_DIR}/.." && pwd)"

cd "${REPO_ROOT}"

echo "==> Generating PostgreSQL SQL from DBML..."
dbml2sql --postgresql \
  -o schema/qwparity_schema.sql \
  thirdparty/MOLLER-parity-schema/qwparity_schema.dbml

echo "==> Generating C++ headers from SQL..."
python3 thirdparty/sqlpp23/scripts/sqlpp23-ddl2cpp \
  --path-to-ddl schema/qwparity_schema.sql \
  --namespace moller::db::schema \
  --path-to-header include/mollerdb/schema/qwparity_schema.h \
  --path-to-custom-types schema/custom_types.csv

echo "==> Schema generation complete!"
echo "    SQL: schema/qwparity_schema.sql"
echo "    C++: include/mollerdb/schema/qwparity_schema.h"
