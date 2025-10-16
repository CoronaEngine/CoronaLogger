// CoronaEngine - 日志实现（spdlog 后端）

#include <corona_logger.h>

#include <atomic>
#include <mutex>
#include <source_location>
#include <vector>

// 只有该实现文件包含 spdlog，避免在公共头中暴露依赖。
#include <spdlog/async.h>
#include <spdlog/sinks/base_sink.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

// 类单例存储
static std::mutex g_mutex{};
static std::shared_ptr<Corona::ILogBackend> g_backend{nullptr};
static std::atomic<bool> g_inited{false};

namespace Corona {
// Backend interface hidden within translation unit
class ILogBackend {
   public:
    virtual ~ILogBackend() = default;
    virtual void configure(const LogConfig& cfg) = 0;
    virtual void set_level(LogLevel level) = 0;
    virtual LogLevel get_level() const = 0;
    virtual void log(LogLevel level, std::string_view msg) = 0;
    virtual void log(LogLevel level, std::string_view msg, const std::source_location& loc) = 0;
    virtual void flush() = 0;
};

static spdlog::level::level_enum to_spd(LogLevel lvl) {
    switch (lvl) {
        case LogLevel::kTrace:
            return spdlog::level::trace;
        case LogLevel::kDebug:
            return spdlog::level::debug;
        case LogLevel::kInfo:
            return spdlog::level::info;
        case LogLevel::kWarn:
            return spdlog::level::warn;
        case LogLevel::kError:
            return spdlog::level::err;
        case LogLevel::kCritical:
            return spdlog::level::critical;
        case LogLevel::kOff:
            return spdlog::level::off;
    }
    return spdlog::level::info;
}

static LogLevel from_spd(spdlog::level::level_enum lvl) {
    switch (lvl) {
        case spdlog::level::trace:
            return LogLevel::kTrace;
        case spdlog::level::debug:
            return LogLevel::kDebug;
        case spdlog::level::info:
            return LogLevel::kInfo;
        case spdlog::level::warn:
            return LogLevel::kWarn;
        case spdlog::level::err:
            return LogLevel::kError;
        case spdlog::level::critical:
            return LogLevel::kCritical;
        case spdlog::level::off:
            return LogLevel::kOff;
        default:
            return LogLevel::kInfo;
    }
}

class SpdLogBackend final : public ILogBackend {
   public:
    SpdLogBackend() {
        // Ensure spdlog nothrow; avoid exceptions across engine
        spdlog::init_thread_pool(8192, 1);  // small queue by default, will enable when async
    }

    ~SpdLogBackend() override {
        try {
            if (logger_) {
                logger_->flush();
            }
        } catch (...) { /* swallow */
        }
    }

    void configure(const LogConfig& cfg) override {
        std::vector<spdlog::sink_ptr> sinks;
        if (cfg.enable_console_) {
            auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
            sinks.emplace_back(console_sink);
        }
        if (cfg.enable_file_) {
            auto file_sink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(cfg.file_path_, cfg.max_file_size_bytes_, cfg.max_files_);
            sinks.emplace_back(file_sink);
        }

        if (cfg.async_) {
            logger_ = std::make_shared<spdlog::async_logger>("Corona", begin(sinks), end(sinks), spdlog::thread_pool(), spdlog::async_overflow_policy::block);
            spdlog::register_logger(logger_);
        } else {
            logger_ = std::make_shared<spdlog::logger>("Corona", begin(sinks), end(sinks));
            spdlog::register_logger(logger_);
        }

        logger_->set_pattern(cfg.pattern_);
        logger_->set_level(to_spd(cfg.level_));
        logger_->flush_on(spdlog::level::warn);
    }

    void set_level(LogLevel level) override {
        if (logger_) {
            logger_->set_level(to_spd(level));
        }
    }

    LogLevel get_level() const override {
        return logger_ ? from_spd(logger_->level()) : LogLevel::kInfo;
    }

    void log(LogLevel level, std::string_view msg) override {
        if (!logger_) {
            return;
        }
        switch (level) {
            case LogLevel::kTrace:
                logger_->trace("{}", msg);
                break;
            case LogLevel::kDebug:
                logger_->debug("{}", msg);
                break;
            case LogLevel::kInfo:
                logger_->info("{}", msg);
                break;
            case LogLevel::kWarn:
                logger_->warn("{}", msg);
                break;
            case LogLevel::kError:
                logger_->error("{}", msg);
                break;
            case LogLevel::kCritical:
                logger_->critical("{}", msg);
                break;
            case LogLevel::kOff: /* no-op */
                break;
        }
    }

    void log(LogLevel level, std::string_view msg, const std::source_location& loc) override {
        if (!logger_) {
            return;
        }
        // Map to spdlog::source_loc to drive [%s:%#] and function formatting
        spdlog::source_loc sloc{loc.file_name(), static_cast<int>(loc.line()), loc.function_name()};
        switch (level) {
            case LogLevel::kTrace:
                logger_->log(sloc, spdlog::level::trace, "{}", msg);
                break;
            case LogLevel::kDebug:
                logger_->log(sloc, spdlog::level::debug, "{}", msg);
                break;
            case LogLevel::kInfo:
                logger_->log(sloc, spdlog::level::info, "{}", msg);
                break;
            case LogLevel::kWarn:
                logger_->log(sloc, spdlog::level::warn, "{}", msg);
                break;
            case LogLevel::kError:
                logger_->log(sloc, spdlog::level::err, "{}", msg);
                break;
            case LogLevel::kCritical:
                logger_->log(sloc, spdlog::level::critical, "{}", msg);
                break;
            case LogLevel::kOff:
                break;
        }
    }

    void flush() override {
        if (logger_) {
            logger_->flush();
        }
    }

   private:
    std::shared_ptr<spdlog::logger> logger_;
};

static std::shared_ptr<ILogBackend> create_backend() {
    return std::make_shared<SpdLogBackend>();
}

void Logger::init(const LogConfig& config) {
    if (g_inited.load(std::memory_order_acquire)) {
        return;
    }
    std::lock_guard<std::mutex> lock(g_mutex);
    if (!g_backend) {
        g_backend = create_backend();
        g_backend->configure(config);
    }
    g_inited.store(true, std::memory_order_release);
}

void Logger::shutdown() {
    std::lock_guard<std::mutex> lock(g_mutex);
    if (g_backend) {
        g_backend->flush();
        g_backend.reset();
    }
    g_inited.store(false, std::memory_order_release);
    spdlog::shutdown();
}

void Logger::set_level(LogLevel level) {
    auto backend = get_or_create_backend();
    backend->set_level(level);
}

LogLevel Logger::get_level() {
    auto backend = get_or_create_backend();
    return backend->get_level();
}

void Logger::log(LogLevel level, std::string_view message) {
    auto backend = get_or_create_backend();
    backend->log(level, message);
}

void Logger::log(LogLevel level, std::string_view message, const std::source_location& loc) {
    auto backend = get_or_create_backend();
    backend->log(level, message, loc);
}

void Logger::flush() {
    auto backend = get_or_create_backend();
    backend->flush();
}

std::shared_ptr<ILogBackend> Logger::get_or_create_backend() {
    if (g_backend) {
        return g_backend;
    }
    std::lock_guard<std::mutex> lock(g_mutex);
    if (!g_backend) {
        g_backend = create_backend();
        LogConfig default_config{};
        g_backend->configure(default_config);
        g_inited.store(true, std::memory_order_release);
    }
    return g_backend;
}
}  // namespace Corona
