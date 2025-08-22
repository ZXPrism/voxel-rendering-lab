#include "logger.h"
#include "spdlog/spdlog.h"

#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <memory>

namespace vrl {

std::shared_ptr<spdlog::logger> g_logger;

namespace internal {

auto init_logger = []() {
	auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
	auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>("log.txt", true);
	std::vector<spdlog::sink_ptr> sinks{ console_sink, file_sink };

	g_logger = std::make_shared<spdlog::logger>("logger", sinks.begin(), sinks.end());
	g_logger->set_pattern("[%Y/%m/%d %H:%M:%S.%e] [%l] %v");

	return 0;
}();

}  // namespace internal

}  // namespace vrl
