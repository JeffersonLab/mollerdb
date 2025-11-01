# mollerdb Tests

This directory contains the test suites for the mollerdb project, covering both C++ core functionality and Python bindings.

## Structure

```
tests/
├── cpp/                    # C++ tests using Google Test
│   ├── CMakeLists.txt     # CMake configuration for C++ tests
│   └── test_database.cpp  # Tests for Database class
└── python/                # Python tests using pytest
    ├── __init__.py        # Makes tests a Python package
    ├── conftest.py        # pytest fixtures and configuration
    ├── test_basic.py      # Basic import and initialization tests
    ├── test_database.py   # Database class functionality tests
    └── test_integration.py # Integration tests (C++/Python interchange)
```

## Running Tests

### C++ Tests

Build and run C++ tests using CMake and CTest:

```bash
# From repository root
mkdir build && cd build
cmake .. -DBUILD_MOLLERDB_TESTS=ON
cmake --build .
ctest --output-on-failure
```

Or run the test executable directly:

```bash
./test_database
```

### Python Tests

Run Python tests using pytest:

```bash
# From repository root
pytest tests/python/ -v

# Run specific test file
pytest tests/python/test_database.py -v

# Run with coverage
pytest tests/python/ --cov=mollerdb
```

## Test Coverage

### C++ Tests (`tests/cpp/test_database.cpp`)

- Database constructor with valid connection string
- Database constructor with invalid connection string
- Connection string parsing (various formats)
- Invalid port handling
- Error handling and exceptions

### Python Tests

**`test_basic.py`**: Module and class availability
- Module import
- Database class existence
- Database class instantiation

**`test_database.py`**: Database class functionality
- Constructor with connection string
- Constructor with invalid connection
- Method availability
- Invalid port handling

**`test_integration.py`**: Integration tests
- Type checking
- Module attributes
- Placeholders for Arrow/pandas integration (future)

## CI Integration

All tests are automatically run in the CI pipeline on every pull request and push to main:

1. **C++ tests** are built and run via CTest
2. **Python tests** are run via pytest

See `.github/workflows/build.yml` for the complete CI configuration.

## Writing New Tests

### Adding C++ Tests

1. Add new test cases to `tests/cpp/test_database.cpp` or create new test files
2. If creating new test files, update `tests/cpp/CMakeLists.txt`
3. Follow Google Test conventions (TEST_F for fixtures, EXPECT_*/ASSERT_* macros)

Example:
```cpp
TEST_F(DatabaseTest, YourNewTest) {
    // Test implementation
    EXPECT_TRUE(some_condition);
}
```

### Adding Python Tests

1. Add test functions to existing files or create new test files in `tests/python/`
2. Follow pytest conventions (test functions start with `test_`)
3. Use fixtures from `conftest.py` for common setup

Example:
```python
def test_your_new_feature():
    import mollerdb
    # Test implementation
    assert some_condition
```

## Test Dependencies

- **C++ Tests**: Google Test v1.15.2 (fetched automatically via CMake)
- **Python Tests**: pytest (installed via `pip install -e .[test]`)

## Notes

- Tests expect database connection attempts to fail (no database available in test environment)
- Tests validate parsing, error handling, and API structure
- Mock database connections can be added via pytest fixtures in `conftest.py`
- Integration tests with actual database connections should be added to `test_integration.py`
