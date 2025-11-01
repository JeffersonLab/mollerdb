# GitHub Copilot Instructions for mollerdb

## Project Overview

The **mollerdb** project is a high-performance, dual-language Software Development Kit (SDK) for accessing the MOLLER experiment's analysis database. MOLLER (Measurement Of a Lepton Lepton Electroweak Reaction) is a precision physics experiment at Jefferson Lab that will measure parity-violating asymmetry in electron-electron scattering. This SDK provides convenient access to the experiment's database for collaborators who may not be proficient in SQL.

## Architecture

### Core Design
- **C++ Core Library (`libmollerdb`)**: A central, high-performance C++ library that contains all database interaction logic
- **Python Bindings**: A thin wrapper around the C++ core, exposing its functionality to Python using pybind11

### Key Technologies
- **Build System**: CMake + scikit-build-core (PEP 517 build backend)
- **C++ Database Driver**: sqlpp23 (header-only library, included as git submodule in `thirdparty/`)
- **Python Bindings**: pybind11
- **Data Interchange**: Apache Arrow for efficient zero-copy data transfer between C++ and Python
- **Database**: PostgreSQL

## Project Structure

```
mollerdb/
├── include/           # C++ header files
├── src/              # C++ source files
│   └── Database.cpp  # Core database interaction logic
├── python/           # Python package
│   ├── mollerdb/     # Python package directory
│   │   └── __init__.py
│   └── bindings.cpp  # pybind11 bindings
├── thirdparty/       # Third-party dependencies (git submodules)
├── CMakeLists.txt    # CMake build configuration
└── pyproject.toml    # Python package configuration
```

## Development Guidelines

### Building the Project

**Python Package:**
```bash
pip install -e .
```

**C++ Library (via CMake):**
```bash
mkdir build && cd build
cmake ..
make
```

### Git Submodules
The project uses git submodules for dependencies like sqlpp23. Always ensure submodules are initialized:
```bash
git submodule update --init --recursive
```

### Code Style
- Follow the existing code style in each file
- C++ code uses C++23 standard (as specified in CMakeLists.txt)
- Python code should follow PEP 8 guidelines
- Keep the C++ core focused on database operations
- Keep Python bindings thin and delegate to C++ core

### Data Flow
1. C++ functions query the database using sqlpp23
2. Results are converted to Apache Arrow `Table` objects
3. Arrow tables are passed to Python with zero-copy
4. Python users can convert Arrow tables to Pandas DataFrames

### Naming Conventions
- **Python Package**: `mollerdb`
- **Compiled Module**: `mollerdb`
- **C++ Namespace**: `mollerdb`
- Use snake_case for Python code
- Use camelCase for C++ code (following existing conventions)

## Database Schema Context

The SDK interacts with a PostgreSQL database designed for the MOLLER experiment. Key concepts:
- **Unified Results Table**: Single, flexible table for detector data
- **Sensitivities Table**: Stores linear correlations between quantities
- **Versioning**: Tables support versioning with `valid_from_run` and `valid_to_run` fields
- **Quantity Lookup**: Master table for all measured/calculated quantities

## Dependencies

### C++ Dependencies
- sqlpp23 (git submodule)
- Apache Arrow C++ library
- PostgreSQL client library (libpq)
- pybind11

### Python Dependencies
- scikit-build-core (build)
- pybind11 (build)
- pyarrow (runtime - for Arrow table interaction)
- pandas (optional - for DataFrame conversion in user code and examples)

## CI/CD

The project uses GitHub Actions for continuous integration (see `.github/workflows/`). When modifying code:
- Ensure builds pass on supported platforms
- Git submodules must be checked out in CI workflows
- Both C++ and Python components should be tested

## Common Tasks

### Adding a New Database Query Function
1. Implement the C++ function in `src/Database.cpp`
2. Use sqlpp23 for database interaction
3. Return results as Arrow `Table` objects
4. Expose the function in `python/bindings.cpp` using pybind11
5. Update Python package `__init__.py` if needed

### Modifying Build Configuration
- C++ build: Edit `CMakeLists.txt`
- Python package: Edit `pyproject.toml`
- Dependencies: Update both files as needed

### Working with Arrow Tables
- Construct Arrow tables in C++ using the Arrow C++ API
- Pass table pointers through pybind11
- Python receives Arrow tables that can be converted to Pandas DataFrames

## Important Notes

- **Minimal Changes**: Make the smallest possible changes to achieve goals
- **Testing**: Run existing tests before and after changes
- **Dependencies**: Avoid adding new dependencies unless absolutely necessary
- **Platform Independence**: Keep code portable (Linux, macOS, Windows)
- **Performance**: The C++ core is designed for high performance; maintain this in all changes
- **Documentation**: Update README.md and docstrings when changing public APIs

## Security Considerations

- Never commit database credentials or connection strings
- Use environment variables for sensitive configuration
- Validate all database inputs to prevent SQL injection (sqlpp23 provides protection)
- Review dependencies for known vulnerabilities

## Contact

For questions about design decisions or project direction, refer to `AGENTS.md` or contact the maintainers at wdconinc@jlab.org.
