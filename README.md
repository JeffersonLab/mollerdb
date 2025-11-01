# MOLLER DB SDK

This repository contains the official C++ and Python Software Development Kit (SDK) for accessing the MOLLER experiment's analysis database.

## Database Schema

The database schema is maintained in the separate [MOLLER-parity-schema](https://github.com/JeffersonLab/MOLLER-parity-schema) repository as a DBML file. This repository includes it as a git submodule and generates C++ headers for type-safe database access using sqlpp23.

For details on the schema integration, see [schema/README.md](schema/README.md).
