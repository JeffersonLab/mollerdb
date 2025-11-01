# MOLLER DB SDK

This repository contains the official C++ and Python Software Development Kit (SDK) for accessing the MOLLER experiment's analysis database.

## Installation

The package is available on PyPI and can be installed using pip:

```bash
pip install mollerdb
```

## Database Schema

The database schema is maintained in the separate [MOLLER-parity-schema](https://github.com/JeffersonLab/MOLLER-parity-schema) repository as a DBML file. This repository includes it as a git submodule and generates C++ headers for type-safe database access using sqlpp23.

For details on the schema integration, see [schema/README.md](schema/README.md).

## Development

For local development, clone the repository and install in editable mode:

```bash
git clone https://github.com/JeffersonLab/mollerdb.git
cd mollerdb
git submodule update --init --recursive
pip install -e .
```

## Releasing

To create a new release and publish to PyPI:

1. Update the version number in `pyproject.toml`
2. Commit the version change: `git commit -am "Bump version to X.Y.Z"`
3. Create and push a version tag: `git tag vX.Y.Z && git push origin vX.Y.Z`
4. The GitHub Actions workflow will automatically build and publish the package to PyPI

**Note:** PyPI publishing uses [Trusted Publishing](https://docs.pypi.org/trusted-publishers/) for secure authentication. Repository maintainers need to configure the PyPI project to trust the GitHub Actions workflow before the first release. See [docs/PYPI_SETUP.md](docs/PYPI_SETUP.md) for detailed setup instructions.
