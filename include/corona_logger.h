// CoronaEngine - Logging API (Facade)
// Exposes a stable interface while hiding the concrete backend, enabling easy backend swaps.

#pragma once

#include <cstdint>
#include <memory>
#include <source_location>
#include <string>
#include <string_view>

// Use spdlog's bundled fmt (header-only) for type-safe formatting without leaking spdlog types in the public API.
#include <spdlog/fmt/bundled/core.h>
#include <spdlog/fmt/bundled/format.h>

namespace Corona {
enum class LogLevel : uint8_t {
    kTrace = 0,
    kDebug = 1,
    kInfo = 2,
    kWarn = 3,
    kError = 4,
    kCritical = 5,
    kOff = 6
};

struct LogConfig {
    bool enable_console_ = true;
    bool enable_file_ = false;
    // Rotating file sink configuration (effective when enable_file_ = true)
    std::string file_path_ = "logs/Corona.log";
    std::size_t max_file_size_bytes_ = 5 * 1024 * 1024;  // 5MB
    std::size_t max_files_ = 3;

    // Async logging (when supported by the backend)
    bool async_ = false;

    // Log pattern string (see backend syntax, e.g., spdlog):
    // Example: [timestamp][logger][level][file:line] message
    std::string pattern_ = "%^[%Y-%m-%d %H:%M:%S.%e][%n][%-5!l][%g:%#] %v%$";

    // Initial log level
    LogLevel level_ = LogLevel::kDebug;
};

// Backend interface (hidden from users; forward-declared only in header)
class ILogBackend;

class Logger {
   public:
    // Initialize the global logger with config (idempotent)
    static void init(const LogConfig& config = {});
    static void shutdown();

    static void set_level(LogLevel level);
    static LogLevel get_level();

    // Formatted logging (fmt-style), without exposing backend types
    template <typename... Args>
    static void trace(fmt::format_string<Args...> fmtStr, Args&&... args) {
        log_formatted(LogLevel::kTrace, fmtStr, std::forward<Args>(args)...);
    }

    // Overloads with source_location (used by macros to inject call site info)
    template <typename... Args>
    static void trace(const std::source_location& loc, fmt::format_string<Args...> fmtStr, Args&&... args) {
        log_formatted(LogLevel::kTrace, loc, fmtStr, std::forward<Args>(args)...);
    }

    template <typename... Args>
    static void debug(fmt::format_string<Args...> fmtStr, Args&&... args) {
        log_formatted(LogLevel::kDebug, fmtStr, std::forward<Args>(args)...);
    }

    template <typename... Args>
    static void debug(const std::source_location& loc, fmt::format_string<Args...> fmtStr, Args&&... args) {
        log_formatted(LogLevel::kDebug, loc, fmtStr, std::forward<Args>(args)...);
    }

    template <typename... Args>
    static void info(fmt::format_string<Args...> fmtStr, Args&&... args) {
        log_formatted(LogLevel::kInfo, fmtStr, std::forward<Args>(args)...);
    }

    template <typename... Args>
    static void info(const std::source_location& loc, fmt::format_string<Args...> fmtStr, Args&&... args) {
        log_formatted(LogLevel::kInfo, loc, fmtStr, std::forward<Args>(args)...);
    }

    template <typename... Args>
    static void warn(fmt::format_string<Args...> fmtStr, Args&&... args) {
        log_formatted(LogLevel::kWarn, fmtStr, std::forward<Args>(args)...);
    }

    template <typename... Args>
    static void warn(const std::source_location& loc, fmt::format_string<Args...> fmtStr, Args&&... args) {
        log_formatted(LogLevel::kWarn, loc, fmtStr, std::forward<Args>(args)...);
    }

    template <typename... Args>
    static void error(fmt::format_string<Args...> fmtStr, Args&&... args) {
        log_formatted(LogLevel::kError, fmtStr, std::forward<Args>(args)...);
    }

    template <typename... Args>
    static void error(const std::source_location& loc, fmt::format_string<Args...> fmtStr, Args&&... args) {
        log_formatted(LogLevel::kError, loc, fmtStr, std::forward<Args>(args)...);
    }

    template <typename... Args>
    static void critical(fmt::format_string<Args...> fmtStr, Args&&... args) {
        log_formatted(LogLevel::kCritical, fmtStr, std::forward<Args>(args)...);
    }

    template <typename... Args>
    static void critical(const std::source_location& loc, fmt::format_string<Args...> fmtStr, Args&&... args) {
        log_formatted(LogLevel::kCritical, loc, fmtStr, std::forward<Args>(args)...);
    }

    static void flush();

    // Pre-formatted raw string logging
    static void log(LogLevel level, std::string_view message);
    static void log(LogLevel level, std::string_view message, const std::source_location& loc);

   private:
    static std::shared_ptr<ILogBackend> get_or_create_backend();

    template <typename... Args>
    static inline void log_formatted(LogLevel level, fmt::format_string<Args...> fmtStr, Args&&... args) {
        log(level, fmt::vformat(fmtStr, fmt::make_format_args(args...)));
    }

    template <typename... Args>
    static inline void log_formatted(LogLevel level, const std::source_location& loc, fmt::format_string<Args...> fmtStr, Args&&... args) {
        log(level, fmt::vformat(fmtStr, fmt::make_format_args(args...)), loc);
    }
};
}  // namespace Corona

// Compile-time logging switches:
// - Per-level flags CE_LOG_LEVEL_TRACE/DEBUG/INFO/WARN/ERROR/CRITICAL (1 on / 0 off)
#ifndef LOG_LEVEL
#define LOG_LEVEL 0
#endif

#ifndef CE_LOG_LEVEL_TRACE
#if LOG_LEVEL <= 0
#define CE_LOG_LEVEL_TRACE 1
#else
#define CE_LOG_LEVEL_TRACE 0
#endif
#endif

#ifndef CE_LOG_LEVEL_DEBUG
#if LOG_LEVEL <= 1
#define CE_LOG_LEVEL_DEBUG 1
#else
#define CE_LOG_LEVEL_DEBUG 0
#endif
#endif

#ifndef CE_LOG_LEVEL_INFO
#if LOG_LEVEL <= 2
#define CE_LOG_LEVEL_INFO 1
#else
#define CE_LOG_LEVEL_INFO 0
#endif
#endif

#ifndef CE_LOG_LEVEL_WARN
#if LOG_LEVEL <= 3
#define CE_LOG_LEVEL_WARN 1
#else
#define CE_LOG_LEVEL_WARN 0
#endif
#endif

#ifndef CE_LOG_LEVEL_ERROR
#if LOG_LEVEL <= 4
#define CE_LOG_LEVEL_ERROR 1
#else
#define CE_LOG_LEVEL_ERROR 0
#endif
#endif

#ifndef CE_LOG_LEVEL_CRITICAL
#if LOG_LEVEL <= 5
#define CE_LOG_LEVEL_CRITICAL 1
#else
#define CE_LOG_LEVEL_CRITICAL 0
#endif
#endif

// Convenience macros (compiled out when disabled)
#if CE_LOG_LEVEL_TRACE
#define CE_LOG_TRACE(...) ::Corona::Logger::trace(std::source_location::current(), __VA_ARGS__)
#else
#define CE_LOG_TRACE(...) (void)0
#endif

#if CE_LOG_LEVEL_DEBUG
#define CE_LOG_DEBUG(...) ::Corona::Logger::debug(std::source_location::current(), __VA_ARGS__)
#else
#define CE_LOG_DEBUG(...) (void)0
#endif

#if CE_LOG_LEVEL_INFO
#define CE_LOG_INFO(...) ::Corona::Logger::info(std::source_location::current(), __VA_ARGS__)
#else
#define CE_LOG_INFO(...) (void)0
#endif

#if CE_LOG_LEVEL_WARN
#define CE_LOG_WARN(...) ::Corona::Logger::warn(std::source_location::current(), __VA_ARGS__)
#else
#define CE_LOG_WARN(...) (void)0
#endif

#if CE_LOG_LEVEL_ERROR
#define CE_LOG_ERROR(...) ::Corona::Logger::error(std::source_location::current(), __VA_ARGS__)
#else
#define CE_LOG_ERROR(...) (void)0
#endif

#if CE_LOG_LEVEL_CRITICAL
#define CE_LOG_CRITICAL(...) ::Corona::Logger::critical(std::source_location::current(), __VA_ARGS__)
#else
#define CE_LOG_CRITICAL(...) (void)0
#endif
