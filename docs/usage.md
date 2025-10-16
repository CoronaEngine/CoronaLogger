# CoronaLogger Usage Guide

This guide explains how to integrate and use CoronaLogger in your project, including configuration options, macros, and troubleshooting.

## 1. Integration and dependencies
- C++20 compiler (MSVC/Clang/GCC).
- The repository fetches `spdlog` automatically via CMake FetchContent (no manual installation required).

See the README for integration via subdirectory or FetchContent.

## 2. Quick start
```cpp
#include <corona_logger.h>

int main() {
    Corona::Logger::init();
    CE_LOG_INFO("Hello from CoronaLogger");
    Corona::Logger::shutdown();
    return 0;
}
```

## 3. LogConfig options
Customize initialization via `Corona::Logger::init(const LogConfig&)`:

- Outputs
    - `enable_console_`: colored console output (default true).
    - `enable_file_`: file output (default false).
    - `file_path_`: file path (default `logs/Corona.log`).
    - `max_file_size_bytes_`: per-file rollover size (default 5MB).
    - `max_files_`: number of rotated files (default 3).
- Format and level
    - `pattern_`: spdlog-style pattern string; includes timestamp, logger name, level, and file:line by default.
    - `level_`: initial log level (default `kDebug`).
- Performance
    - `async_`: enable async logging to reduce caller blocking (default false).

Example:
```cpp
Corona::LogConfig cfg;
cfg.enable_console_ = true;
cfg.enable_file_ = true;
cfg.file_path_ = "logs/app.log";
cfg.level_ = Corona::LogLevel::kInfo;
cfg.async_ = true;
Corona::Logger::init(cfg);
```

## 4. API and logging macros
- Core methods:
    - `Logger::init(const LogConfig& = {})`
    - `Logger::shutdown()`
    - `Logger::set_level(LogLevel)` / `Logger::get_level()`
    - `Logger::log(LogLevel, std::string_view [, std::source_location])`
    - `Logger::flush()`
- Macros (auto-inject `std::source_location`):
    - `CE_LOG_TRACE/DEBUG/INFO/WARN/ERROR/CRITICAL("message {}", arg)`

Macro activation is controlled at compile time via:
- Global switch: `LOG_LEVEL=0..5` (0=TRACE enabled; 5=CRITICAL only).
- Per-level flags: `CE_LOG_LEVEL_TRACE/DEBUG/INFO/WARN/ERROR/CRITICAL=0/1`.

## 5. Example program
With `CORONA_LOGGER_BUILD_EXAMPLES=ON`, the `MinimalExample` will be built showing:
- Default init/shutdown
- Async + file output
- All log levels

## 6. Performance and threading
- Async mode uses a thread pool and suits high-frequency logging; on crashes, tail logs may be lost—call `Logger::flush()` on critical paths.
- Sync mode offers predictability but may block the caller under heavy IO.
- The backend is process-wide; thread safety is provided by spdlog.

## 7. Troubleshooting FAQ
- No output? Ensure `Logger::init()` was called; verify `set_level()` and compile-time `LOG_LEVEL` allow the message.
- File write fails on Windows? Ensure the directory exists and is writable; prefer relative paths to a writable folder.
- Garbled console output? On Windows PowerShell, use UTF-8 encoding (e.g., `$OutputEncoding = [Console]::OutputEncoding = [System.Text.Encoding]::UTF8`).

## 8. Migration and swapability
CoronaLogger acts as a façade; you can change the backend implementation (currently spdlog) without touching business code.
