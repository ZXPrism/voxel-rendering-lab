#include <logger.h>

#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <vector>

namespace vox {

std::shared_ptr<spdlog::logger> g_logger;

auto init_logger = []() {
	auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
	// console_sink->set_level(spdlog::level::info);
	console_sink->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] [%s:%#] %v");

	auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>("log.txt", true);  // true: trunc prev logs
	// file_sink->set_level(spdlog::level::debug);
	file_sink->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%l] [%s:%#] [thread %t] %v");

	std::vector<spdlog::sink_ptr> sinks{ console_sink, file_sink };
	g_logger = std::make_shared<spdlog::logger>("g_logger", sinks.begin(), sinks.end());
	// g_logger->set_level(spdlog::level::debug);
	// g_logger->flush_on(spdlog::level::warn);

	spdlog::register_logger(g_logger);
	spdlog::set_default_logger(g_logger);

	return 0;
}();

}  // namespace vox
