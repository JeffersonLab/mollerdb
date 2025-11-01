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

### Local Development (Native)

For local development, clone the repository and install in editable mode:

```bash
git clone https://github.com/JeffersonLab/mollerdb.git
cd mollerdb
git submodule update --init --recursive
pip install -e .
```

### Docker Development Environment

A complete Docker development environment is provided with all dependencies pre-installed. This is the easiest way to get started.

**Using pre-built image from GitHub Container Registry:**
```bash
# Pull the latest image
docker pull ghcr.io/jeffersonlab/mollerdb:latest

# Start a development shell
docker run -it --rm -v $(pwd):/workspace ghcr.io/jeffersonlab/mollerdb:latest
```

**Building locally:**
```bash
docker build -t mollerdb-dev .
```

**Start a development shell:**
```bash
docker run -it --rm -v $(pwd):/workspace mollerdb-dev
```

Inside the container, you can build and test the project:
```bash
# Initialize submodules (if not already done on host)
git submodule update --init --recursive

# Build and install the package
# Note: Use --break-system-packages in the container environment
pip install -e . --verbose --break-system-packages

# Run tests (when available)
pytest tests/
```

**Run a one-off build command:**
```bash
docker run -it --rm -v $(pwd):/workspace mollerdb-dev bash -c "
  git submodule update --init --recursive &&
  pip install -e . --verbose --break-system-packages
"
```

The Docker image includes:
- Ubuntu 24.04 base
- GCC 14 with C++23 support
- CMake 3.28+
- Node.js and dbml-cli for schema generation
- PostgreSQL client libraries (libpq)
- Python 3.12 with pip
- All Python build dependencies (scikit-build-core, pybind11, pyparsing, pytest, wheel, build)

For more details, see the comments in the `Dockerfile`. The Docker image is automatically built and published to the GitHub Container Registry on pushes to main and on releases.

## Releasing

To create a new release and publish to PyPI:

1. Update the version number in `pyproject.toml`
2. Commit the version change: `git commit -am "Bump version to X.Y.Z"`
3. Create and push a version tag: `git tag vX.Y.Z && git push origin vX.Y.Z`
4. The GitHub Actions workflow will automatically build and publish the package to PyPI

**Note:** PyPI publishing uses [Trusted Publishing](https://docs.pypi.org/trusted-publishers/) for secure authentication. Repository maintainers need to configure the PyPI project to trust the GitHub Actions workflow before the first release. See [docs/PYPI_SETUP.md](docs/PYPI_SETUP.md) for detailed setup instructions.
