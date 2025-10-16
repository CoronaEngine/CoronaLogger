# CoronaLogger

CoronaLogger is a C++20 logging façade built on top of [spdlog](https://github.com/gabime/spdlog). It provides a unified, type‑safe, and swappable logging interface for CoronaEngine and standalone projects.

## ✨ Features
- Minimal surface: expose only `Corona::Logger` and `CE_LOG_*` macros; hide backend details.
- Type‑safe formatting and source info: `{fmt}`-style formatting; macros inject `std::source_location` automatically.
- Compile‑time elision: compile out lower-level logs to reduce runtime overhead.
- Flexible configuration: colored console, rotating file sinks, and async logging.

## 📦 Integration

This project fetches `spdlog` via CMake FetchContent automatically; no manual install required.

### A. Add as subdirectory
```cmake
# Assuming the repo lives at extern/CoronaLogger
add_subdirectory(extern/CoronaLogger)
target_link_libraries(MyTarget PRIVATE Corona::Logger)
```

### B. Use FetchContent
```cmake
include(FetchContent)

FetchContent_Declare(
	CoronaLogger
	GIT_REPOSITORY https://github.com/CoronaEngine/CoronaLogger.git
	GIT_TAG        main # or a release tag
)

FetchContent_MakeAvailable(CoronaLogger)
target_link_libraries(MyTarget PRIVATE Corona::Logger)
```

> Note: A stable install + `find_package` flow is not finalized yet. Prefer the two approaches above for now.

## ⚙️ CMake options
- `CORONA_LOGGER_BUILD_EXAMPLES`: build `examples/` (ON when building this repo as the top-level project).

## 🚀 Quick start
```cpp
#include <corona_logger.h>

int main() {
	Corona::Logger::init();
	CE_LOG_INFO("CoronaLogger ready");
	Corona::Logger::shutdown();
	return 0;
}
```

## 🧰 API at a glance
- Class: `Corona::Logger`
	- `init(const LogConfig& = {})` / `shutdown()`
	- `set_level(LogLevel)` / `get_level()`
	- `log(LogLevel, std::string_view[, std::source_location])`
	- `flush()`
- Struct: `Corona::LogConfig`
	- `enable_console_`, `enable_file_`
	- `file_path_`, `max_file_size_bytes_`, `max_files_`
	- `async_`, `pattern_`, `level_`
- Macros: `CE_LOG_TRACE/DEBUG/INFO/WARN/ERROR/CRITICAL(...)` (auto-inject `std::source_location`)

See `docs/usage.md` for more details.

## 🧪 Build and run the example
Enable examples:
```cmake
# in your top-level CMake project
set(CORONA_LOGGER_BUILD_EXAMPLES ON)
```
You will get the `MinimalExample` executable demonstrating sync/async and file output.

## ⛏️ Compile-time log switches
Default `LOG_LEVEL=0` (all levels enabled). You can adjust via compile definitions:
- `LOG_LEVEL=0/1/2/3/4/5` enables up to TRACE/DEBUG/INFO/WARN/ERROR/CRITICAL;
- or control `CE_LOG_LEVEL_TRACE/DEBUG/...` individually.

Example (in your app's CMakeLists.txt):
```cmake
target_compile_definitions(MyTarget PRIVATE LOG_LEVEL=2) # enable up to INFO only
```

## 💡 FAQ
- No output? Ensure `Corona::Logger::init()` is called and the log level allows the message.
- File output not working? Ensure `enable_file_ = true` and `file_path_` is writable (on Windows, prefer an existing or relative path).
- Logs lost in async mode? Crashes may skip flush; call `Corona::Logger::flush()` before exit on critical paths.

## 🤝 Contributing
- See `CONTRIBUTING.md`
- Code of Conduct: `CODE_OF_CONDUCT.md`
- Changelog: `CHANGELOG.md`
- Security: `SECURITY.md`

## 📄 License
This repository currently does not include a LICENSE file. If you plan to use it in production, please open an issue to clarify licensing terms with the maintainers.
