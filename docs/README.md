# MOLLER DB SDK

The MOLLER DB SDK provides convenient access to the MOLLER experiment's analysis database through both C++ and Python interfaces.

## Overview

This SDK is designed to provide high-performance, dual-language access to the MOLLER experiment's analysis database. It features:

- **C++ Core Library (`libmollerdb`)**: A high-performance C++ library for database interaction
- **Python Bindings**: A thin wrapper around the C++ core for Python users
- **Apache Arrow Integration**: Efficient, zero-copy data transfer between C++ and Python
- **Cross-Platform Support**: Works on Linux and macOS

## Installation

### Docker Development Environment (Recommended for Quick Start)

The easiest way to get started with development is using the provided Docker environment:

```bash
# Clone the repository
git clone --recursive https://github.com/JeffersonLab/mollerdb.git
cd mollerdb

# Build the Docker development image
docker build -t mollerdb-dev .

# Start an interactive development shell
docker run -it --rm -v $(pwd):/workspace mollerdb-dev
```

Inside the container, all dependencies are pre-installed:

```bash
# Initialize git submodules (if not already done)
git submodule update --init --recursive

# Build and install the package in editable mode
# Note: Use --break-system-packages in the container environment
pip install -e . --verbose --break-system-packages

# Run tests (when available)
pytest tests/

# Or work with C++ directly
mkdir build && cd build
cmake ..
make
```

**Important Notes:**
- The Docker image requires network access during build to download Apache Arrow packages from `packages.apache.org`.
- In restricted network environments, the Arrow installation may be skipped with a warning. To manually install Arrow later:
  ```bash
  # Inside the container
  wget https://packages.apache.org/artifactory/arrow/ubuntu/apache-arrow-apt-source-latest-$(lsb_release --codename --short).deb
  sudo dpkg -i apache-arrow-apt-source-latest-$(lsb_release --codename --short).deb
  sudo apt-get update
  sudo apt-get install libarrow-dev liblz4-dev libre2-dev
  ```
- When using `pip install` inside the container, add the `--break-system-packages` flag as shown above (this is safe in container environments).
- **Security Note**: The Dockerfile includes SSL verification workarounds for environments with certificate issues. For production deployments, ensure proper SSL/TLS configuration and remove these workarounds.

**What's included in the Docker image:**
- Ubuntu 24.04 base system
- GCC 14 with full C++23 support (required for sqlpp23)
- CMake 3.28+
- Node.js 18 and dbml-cli for database schema generation
- PostgreSQL client libraries (libpq-dev)
- Python 3.12 with pip
- Python build tools: scikit-build-core, pybind11, pyparsing
- Testing tools: pytest, wheel, build
- A non-root `developer` user (UID 1001)

**Common Docker workflows:**

Run a one-off build:
```bash
docker run -it --rm -v $(pwd):/workspace mollerdb-dev bash -c "
  git submodule update --init --recursive &&
  pip install -e . --verbose --break-system-packages
"
```

Run tests:
```bash
docker run -it --rm -v $(pwd):/workspace mollerdb-dev bash -c "
  git submodule update --init --recursive &&
  pip install -e . --verbose --break-system-packages &&
  pytest tests/
"
```

### Prerequisites (Native Installation)

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
