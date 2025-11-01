# MOLLER DB SDK

The MOLLER DB SDK provides convenient access to the MOLLER experiment's analysis database through both C++ and Python interfaces.

## Overview

This SDK is designed to provide high-performance, dual-language access to the MOLLER experiment's analysis database. It features:

- **C++ Core Library (`libmollerdb`)**: A high-performance C++ library for database interaction
- **Python Bindings**: A thin wrapper around the C++ core for Python users
- **Apache Arrow Integration**: Efficient, zero-copy data transfer between C++ and Python
- **Cross-Platform Support**: Works on Linux and macOS

## Installation

### Prerequisites

#### Linux (Ubuntu/Debian)

```bash
# Install system dependencies
sudo apt-get update
sudo apt-get install -y build-essential cmake git

# Install GCC 14 for C++23 support (required for sqlpp23)
sudo add-apt-repository -y ppa:ubuntu-toolchain-r/test
sudo apt-get update
sudo apt-get install -y gcc-14 g++-14

# Install PostgreSQL and Apache Arrow development libraries
sudo apt-get install -y libpq-dev libarrow-dev
```

#### macOS

```bash
# Install dependencies via Homebrew
brew install cmake git libpq apache-arrow
```

### Python Installation

The easiest way to install the Python package is via pip:

```bash
pip install mollerdb
```

For development installation:

```bash
# Clone the repository with submodules
git clone --recursive https://github.com/JeffersonLab/mollerdb.git
cd mollerdb

# Install in editable mode
pip install -e .
```

### C++ Installation

To use the C++ library directly:

```bash
# Clone the repository with submodules
git clone --recursive https://github.com/JeffersonLab/mollerdb.git
cd mollerdb

# Build and install
mkdir build && cd build
cmake ..
cmake --build .
sudo cmake --install .
```

## Usage

### Python Usage

Basic example of connecting to the database and querying data:

```python
import mollerdb

# Connect to the database
db = mollerdb.Database("host=localhost dbname=moller user=myuser password=mypass")

# Query data (returns pandas DataFrame via Apache Arrow)
results = db.query("SELECT * FROM runs WHERE run_number > 1000")
print(results)
```

### C++ Usage

Basic example of using the C++ library:

```cpp
#include <mollerdb/Database.h>
#include <iostream>

int main() {
    // Connect to the database
    mollerdb::Database db("host=localhost dbname=moller user=myuser password=mypass");
    
    // Query data
    auto results = db.query("SELECT * FROM runs WHERE run_number > 1000");
    
    // Process results...
    
    return 0;
}
```

Compile with:

```bash
g++ -std=c++23 -o my_program my_program.cpp -lmollerdb -larrow
```

## API Reference

### Database Class

#### Constructor

- `Database(const std::string& connection_string)` - Create a database connection

#### Methods

- `query(const std::string& sql)` - Execute a SQL query and return results as an Apache Arrow table

## Architecture

### Design Decisions

The SDK architecture follows these key principles:

1. **C++ Core**: All database logic resides in a central C++ library (`libmollerdb`)
2. **Python Bindings**: Python interface is a thin wrapper using pybind11
3. **Apache Arrow**: Data interchange format for efficient zero-copy transfer
4. **sqlpp23**: Header-only C++ database library for PostgreSQL interaction

### Build System

- **CMake**: C++ build configuration
- **scikit-build-core**: Modern PEP 517 build backend for Python packaging
- **pybind11**: C++/Python binding generation

## Contributing

Contributions are welcome! Please:

1. Fork the repository
2. Create a feature branch
3. Make your changes with tests
4. Submit a pull request

## License

This project is licensed under the terms specified in the LICENSE file.

## Support

For questions or issues:
- Open an issue on [GitHub](https://github.com/JeffersonLab/mollerdb/issues)
- Contact the MOLLER collaboration

## Related Projects

- [MOLLER Parity Database Schema](https://github.com/JeffersonLab/MOLLER-parity-schema) - Database schema definition
