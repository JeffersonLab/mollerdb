# Development Dockerfile for mollerdb
# This container provides all dependencies needed for C++ and Python development,
# including tools to build and test both native and Python wheels.
#
# SECURITY NOTE: This Dockerfile includes SSL verification workarounds (--no-check-certificate,
# --strict-ssl=false, --trusted-host) to handle certificate issues in certain build environments.
# These workarounds are necessary for builds in environments with self-signed certificates but
# should be removed for production deployments with proper SSL/TLS configuration.
# Only use this Dockerfile in trusted network environments.

FROM ubuntu:24.04

# Prevent interactive prompts during package installation
ENV DEBIAN_FRONTEND=noninteractive

# Install basic build tools and system dependencies
RUN apt-get update && apt-get install -y --no-install-recommends \
    build-essential \
    cmake \
    git \
    wget \
    curl \
    ca-certificates \
    gnupg \
    software-properties-common \
    lsb-release \
    apt-transport-https \
    && rm -rf /var/lib/apt/lists/*

# Install GCC 14 for C++23 support
# GCC 14 is necessary because sqlpp23 requires the "deducing this" feature from C++23
# GCC 14 is available in Ubuntu 24.04 universe repository
RUN apt-get update && apt-get install -y --no-install-recommends \
    gcc-14 \
    g++-14 \
    && rm -rf /var/lib/apt/lists/*

# Set GCC 14 as the default compiler
RUN update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-14 100 && \
    update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-14 100 && \
    update-alternatives --install /usr/bin/cc cc /usr/bin/gcc-14 100 && \
    update-alternatives --install /usr/bin/c++ c++ /usr/bin/g++-14 100

# Install Node.js and npm (for dbml-cli)
# Using Node.js from Ubuntu repositories for simplicity
RUN apt-get update && apt-get install -y --no-install-recommends \
    nodejs \
    npm \
    && rm -rf /var/lib/apt/lists/*

# Install dbml-cli for database schema generation
# SECURITY NOTE: SSL verification is temporarily disabled to handle certificate issues
# in build environments. For production, ensure proper certificate configuration.
# This workaround is safe when using trusted network infrastructure.
RUN npm config set strict-ssl false && \
    npm install -g @dbml/cli && \
    npm config set strict-ssl true

# Add Apache Arrow APT repository
# Arrow is used for efficient zero-copy data transfer between C++ and Python
# Note: In restricted network environments or CI systems with self-signed certificates,
# this step may fail. For production use with proper network access, remove --no-check-certificate.
# SECURITY: --no-check-certificate is used here to handle certificate issues in build environments.
# In production, ensure proper SSL/TLS configuration and remove this flag.
RUN wget --no-check-certificate https://packages.apache.org/artifactory/arrow/$(lsb_release --id --short | tr 'A-Z' 'a-z')/apache-arrow-apt-source-latest-$(lsb_release --codename --short).deb || \
    (echo "WARNING: Failed to download Arrow APT source. Arrow packages may not be available." && touch /tmp/arrow-download-failed)

# Install PostgreSQL client libraries and Apache Arrow development libraries
# If Arrow download failed, skip Arrow installation
RUN apt-get update && \
    apt-get install -y --no-install-recommends \
    libpq-dev \
    && \
    if [ ! -f /tmp/arrow-download-failed ] && [ -f apache-arrow-apt-source-latest-$(lsb_release --codename --short).deb ]; then \
        apt-get install -y ./apache-arrow-apt-source-latest-$(lsb_release --codename --short).deb && \
        rm apache-arrow-apt-source-latest-$(lsb_release --codename --short).deb && \
        apt-get update && \
        apt-get install -y --no-install-recommends libarrow-dev liblz4-dev libre2-dev; \
    else \
        echo "WARNING: Skipping Arrow installation due to network restrictions."; \
        echo "To build mollerdb, Arrow must be installed manually or network access restored."; \
    fi && \
    rm -rf /var/lib/apt/lists/*

# Install Python 3 and pip
RUN apt-get update && apt-get install -y --no-install-recommends \
    python3 \
    python3-pip \
    python3-dev \
    && rm -rf /var/lib/apt/lists/*

# Create a symbolic link for python (python3 -> python)
RUN ln -s /usr/bin/python3 /usr/bin/python

# Install Python build tools and testing dependencies
# These are required for building the Python package and running tests
# Using --break-system-packages is safe in Docker containers
# SECURITY NOTE: --trusted-host flags bypass SSL verification to handle certificate issues
# in build environments. For production, ensure proper SSL/TLS configuration and remove these flags.
# Only use these workarounds in environments with trusted network infrastructure.
RUN pip3 install --no-cache-dir --break-system-packages \
    --trusted-host pypi.org --trusted-host files.pythonhosted.org \
    scikit-build-core>=0.7.0 \
    pybind11 \
    pyparsing \
    pytest \
    wheel \
    build

# Create a non-root user for development
# Using UID 1001 to avoid conflicts with existing system users
RUN useradd -m -u 1001 -s /bin/bash developer && \
    mkdir -p /workspace && \
    chown -R developer:developer /workspace

# Set the working directory
WORKDIR /workspace

# Switch to non-root user
USER developer

# Set environment variables for the compiler
ENV CC=gcc-14
ENV CXX=g++-14

# Set helpful environment variables
ENV PYTHONUNBUFFERED=1

# Default command: start an interactive bash shell
CMD ["/bin/bash"]

# Usage Instructions:
# 
# Build the Docker image:
#   docker build -t mollerdb-dev .
#
# Note: The build requires network access to download Apache Arrow packages.
# In restricted environments, Arrow installation may be skipped with a warning.
#
# Start a development shell:
#   docker run -it --rm -v $(pwd):/workspace mollerdb-dev
#
# Build the project inside the container:
#   docker run -it --rm -v $(pwd):/workspace mollerdb-dev bash -c "
#     git submodule update --init --recursive &&
#     pip install -e . --verbose --break-system-packages
#   "
#
# Run tests inside the container:
#   docker run -it --rm -v $(pwd):/workspace mollerdb-dev bash -c "
#     git submodule update --init --recursive &&
#     pip install -e . --verbose --break-system-packages &&
#     pytest tests/
#   "
#
# For iterative development:
#   docker run -it --rm -v $(pwd):/workspace mollerdb-dev
#   # Inside container:
#   git submodule update --init --recursive
#   pip install -e . --verbose --break-system-packages
#   # Make code changes on host, rebuild as needed
#   pip install -e . --verbose --break-system-packages
