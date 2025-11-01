"""
Tests for the mollerdb Database class.

Tests Database class instantiation, methods, and error handling.
"""

import pytest


def test_database_constructor_with_connection_string(mock_connection_string):
    """Test that Database can be instantiated with a connection string."""
    import mollerdb
    
    # This will throw a RuntimeError since we can't connect to a real database
    # We're testing that the constructor accepts the string and attempts connection
    with pytest.raises(RuntimeError):
        db = mollerdb.Database(mock_connection_string)


def test_database_constructor_with_invalid_connection(invalid_connection_string):
    """Test that Database throws an error with an invalid connection string."""
    import mollerdb
    
    # Should raise RuntimeError for connection failure
    with pytest.raises(RuntimeError):
        db = mollerdb.Database(invalid_connection_string)


def test_database_has_get_main_detector_results_method():
    """Test that the Database class has the get_main_detector_results method."""
    import mollerdb
    
    # Check that the method exists (even though we can't call it without a connection)
    assert hasattr(mollerdb.Database, 'get_main_detector_results')


def test_database_constructor_with_invalid_port():
    """Test that Database throws an error with an invalid port."""
    import mollerdb
    
    # Should raise an exception for invalid port
    with pytest.raises(Exception):  # Could be ValueError or RuntimeError
        db = mollerdb.Database("host=localhost dbname=test user=test password=test port=invalid")
