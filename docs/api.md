# API Overview

This document summarizes the public API from `include/corona_logger.h`.

## Namespace `Corona`

### enum class LogLevel : uint8_t
- kTrace, kDebug, kInfo, kWarn, kError, kCritical, kOff

### struct LogConfig
- enable_console_ : bool (default true)
- enable_file_ : bool (default false)
- file_path_ : std::string (default "logs/Corona.log")
- max_file_size_bytes_ : std::size_t (default 5MB)
- max_files_ : std::size_t (default 3)
- async_ : bool (default false)
- pattern_ : std::string (default "%^" pattern with time, logger, level, file:line)
- level_ : LogLevel (default kDebug)

### class Logger (static façade)
- static void init(const LogConfig& = {})
- static void shutdown()
- static void set_level(LogLevel)
- static LogLevel get_level()
- static void log(LogLevel, std::string_view)
- static void log(LogLevel, std::string_view, const std::source_location&)
- static void flush()

### Macros
- CE_LOG_TRACE/DEBUG/INFO/WARN/ERROR/CRITICAL(...)
  - Injects `std::source_location` automatically
  - Compiled out based on `LOG_LEVEL` or per-level flags

For usage details and configuration examples, see `docs/usage.md`.
