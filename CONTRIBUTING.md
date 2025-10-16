# Contributing to CoronaLogger

Thanks for taking the time to contribute! This document describes how to propose changes and get them merged.

## Development workflow
- Fork this repo and create a feature branch from `main`.
- Keep changes focused and incremental; one PR per logical change.
- Add/update documentation when behavior or public APIs change.
- Ensure the project builds and the example runs.

## Coding standards
- C++20. Prefer modern C++ features and RAII.
- Keep the façade header clean: avoid leaking backend-specific types in `include/corona_logger.h`.
- Avoid exceptions across the boundary; spdlog is configured to not throw.
- Small, readable functions. Comment intent where it isn’t obvious.

## CMake
- Library target name: `CoronaLogger` with alias `Corona::Logger`.
- Examples are guarded by `CORONA_LOGGER_BUILD_EXAMPLES` (ON for top-level builds).

## Commit messages
- Use imperative mood: "Add X", "Fix Y".
- Reference issues like: `Fixes #123` if applicable.

## Pull requests
- Describe the motivation, approach, and any trade-offs.
- Include before/after notes or screenshots for UX-facing changes.
- CI (if present) must pass.

## Reporting issues
- Provide OS, compiler version, and CMake version.
- Share a minimal repro if possible (CMakeLists + small code snippet).

## License
If/when a LICENSE file is provided, your contributions will be licensed under the same terms.
