# GitHub Copilot Instructions for mollerdb

## About This Project

The **mollerdb** project is a high-performance, dual-language Software Development Kit (SDK) for accessing the MOLLER experiment's analysis database. MOLLER (Measurement Of a Lepton Lepton Electroweak Reaction) is a precision physics experiment at Jefferson Lab that will measure parity-violating asymmetry in electron-electron scattering.

## For AI Agents

**The primary source of instructions and context for AI agents working on this project is the `AGENTS.md` file in the repository root.**

Please consult `AGENTS.md` for:
- Complete project overview and architecture
- Design decisions and core technologies
- Development guidelines and code style
- Build instructions and dependencies
- Common tasks and workflows
- Security considerations
- Project status and next steps

When making changes to project documentation or updating context for AI agents, **modify `AGENTS.md`** rather than this file to maintain a single source of truth.

## Quick Reference

- **Language**: C++ (core library) + Python (bindings via pybind11)
- **Build System**: CMake + scikit-build-core
- **Database**: PostgreSQL via sqlpp23
- **Data Transfer**: Apache Arrow for zero-copy C++ ↔ Python
- **C++ Standard**: C++23

For complete details, see `AGENTS.md`.
