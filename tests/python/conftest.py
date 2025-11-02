# Pytest configuration and fixtures for mollerdb tests

import pytest

@pytest.fixture
def mock_connection_string():
    """Provide a mock database connection string for testing."""
    return "host=localhost dbname=test user=test password=test"

@pytest.fixture
def invalid_connection_string():
    """Provide an invalid connection string for testing error handling."""
    return "host=invalid_host dbname=nonexistent user=nobody password=wrong"
