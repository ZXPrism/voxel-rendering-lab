#pragma once

#include <spdlog/spdlog.h>
#include <memory>

namespace vox {

extern std::shared_ptr<spdlog::logger> g_logger;

}  // namespace vox
