"""
Basic tests for the mollerdb Python module.

Tests module import, initialization, and basic functionality.
"""

import pytest


def test_import_mollerdb():
    """Test that the mollerdb module can be imported."""
    import mollerdb
    assert mollerdb is not None


def test_module_has_database_class():
    """Test that the Database class is available in the module."""
    import mollerdb
    assert hasattr(mollerdb, 'Database')


def test_database_class_is_callable():
    """Test that the Database class can be instantiated (constructor exists)."""
    import mollerdb
    assert callable(mollerdb.Database)
