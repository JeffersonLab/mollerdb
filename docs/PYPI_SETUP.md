# PyPI Publishing Setup

This document describes how to set up PyPI trusted publishing for the `mollerdb` package.

## Prerequisites

Before the release workflow can publish to PyPI, repository maintainers need to configure PyPI trusted publishing.

## Setup Steps

### 1. Create the PyPI Project (First-time only)

If the package doesn't exist on PyPI yet, you have two options:

**Option A: Manual package upload (for first release)**
```bash
# Build the package locally
python -m pip install build
python -m build

# Upload to PyPI (you'll need PyPI credentials)
python -m pip install twine
python -m twine upload dist/*
```

**Option B: Use TestPyPI first**
Configure the workflow to publish to TestPyPI first, then copy to production PyPI.

### 2. Configure PyPI Trusted Publishing

1. Go to https://pypi.org/manage/account/publishing/
2. Log in with your PyPI account (must be a project owner/maintainer)
3. Scroll to "Add a new pending publisher"
4. Fill in the following details:
   - **PyPI Project Name**: `mollerdb`
   - **Owner**: `JeffersonLab`
   - **Repository name**: `mollerdb`
   - **Workflow name**: `release.yml`
   - **Environment name**: `pypi`
5. Click "Add"

### 3. Configure GitHub Environment (Recommended)

For additional protection, configure a GitHub environment:

1. Go to the repository settings: https://github.com/JeffersonLab/mollerdb/settings/environments
2. Click "New environment"
3. Name it `pypi`
4. Add protection rules:
   - **Required reviewers**: Add trusted maintainers who must approve releases
   - **Deployment branches**: Limit to tags matching `v*` pattern

## Making a Release

Once configured, releases are automatic:

1. Update version in `pyproject.toml`
2. Commit: `git commit -am "Bump version to X.Y.Z"`
3. Tag: `git tag vX.Y.Z`
4. Push: `git push origin vX.Y.Z`

The workflow will automatically:
- Build wheels for Linux, macOS, and Windows
- Build for Python 3.10-3.13
- Publish to PyPI using trusted publishing

## Testing

To test the workflow without publishing:

1. Use `workflow_dispatch` trigger to manually run the workflow
2. Or publish to TestPyPI first by modifying the workflow temporarily

## Troubleshooting

### "Publishing is currently disabled"

This means trusted publishing is not configured. Follow setup step 2 above.

### "Workflow does not have permission to publish"

Check that:
- The workflow has `id-token: write` permission ✅ (already configured)
- The environment name in the workflow matches PyPI configuration ✅ (both are `pypi`)
- The workflow name matches PyPI configuration ✅ (both are `release.yml`)

### Windows build failures

Windows builds may require additional configuration for C++ dependencies. The workflow includes a basic Windows setup, but it may need refinement based on testing.

## References

- [PyPI Trusted Publishers Documentation](https://docs.pypi.org/trusted-publishers/)
- [GitHub Actions PyPI Publish Action](https://github.com/marketplace/actions/pypi-publish)
- [scikit-build-core Documentation](https://scikit-build-core.readthedocs.io/)
