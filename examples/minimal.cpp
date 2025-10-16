#include <corona_logger.h>

void print_log()
{
    CE_LOG_TRACE("This is a trace message.");
    CE_LOG_DEBUG("This is a debug message.");
    CE_LOG_INFO("This is an info message.");
    CE_LOG_WARN("This is a warning message.");
    CE_LOG_ERROR("This is an error message.");
    CE_LOG_CRITICAL("This is a critical message.");
}

int main() {
    // 1) Initialize with default configuration
    Corona::Logger::init();
    CE_LOG_INFO("Default logger initialized.");
    print_log();
    Corona::Logger::shutdown();

    // 2) Custom configuration: async + file sink
    Corona::LogConfig config;
    config.async_ = true;
    config.enable_file_ = true;
    config.file_path_ = "example.log";
    config.level_ = Corona::LogLevel::kTrace;
    Corona::Logger::init(config);
    CE_LOG_INFO("Async file logger initialized. Check 'example.log'.");
    print_log();
    Corona::Logger::shutdown();

    return 0;
}
